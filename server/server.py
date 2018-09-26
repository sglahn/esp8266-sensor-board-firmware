#!/usr/bin/env python

'''

 Copyright 2018 Sebastian Glahn

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

'''

import os
import argparse
import hashlib
import glob
import logging
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer

LOG_LEVEL = 'ERROR'
LOG_FORMAT = '%(asctime)-15s %(levelname)s %(ip)s --- %(message)s'
PORT_NUMBER = 8000
FIRMWARE_DIRECTORY = os.environ['HOME'] + os.sep + "firmware"

class HttpHandler(BaseHTTPRequestHandler):

    def getLatestFirmwareVersion(self, flavor):
        for firmware in os.listdir(FIRMWARE_DIRECTORY):
            if firmware.startswith(flavor):
                return firmware[firmware.index("-") +1:firmware.index('.bin')]
        return -1     

    def validRequest(self, flavor):
        return glob.glob(FIRMWARE_DIRECTORY + os.sep + flavor + '*') and 'x-ESP8266-version' in self.headers

    def buildHtmlResponse(self, status, text=''):
        self.send_response(status)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write(text)

    def buildStreamResponse(self, flavor, latest):
        filename = FIRMWARE_DIRECTORY + os.sep + flavor + '-' + latest + ".bin"
        self.send_response(200)
        self.send_header('Content-type', 'application/octet-stream')
        self.send_header('Content-Disposition', 'attachment; filename=' + filename)
        self.send_header('Content-Length', os.path.getsize(filename))
        self.send_header('x-MD5', hashlib.md5(open(filename, 'rb').read()).hexdigest())
        self.end_headers()
        binary = open(filename) 
        self.wfile.write(binary.read())
        binary.close()

    def do_GET(self):
        log_stat = { 'ip' : self.client_address[0] }
        flavor = self.path.rsplit('/', 1)[-1]

        if not self.validRequest(flavor):
            logging.error('Invalid request', extra = log_stat)
            self.buildHtmlResponse(400, 'Invalid request')
            return
        
        latest = self.getLatestFirmwareVersion(flavor)
        firmware_version = self.headers.get('x-ESP8266-version')
        if float(latest) > float(firmware_version):
            logging.info('Sending firmware update for ' + flavor + ' from ' + firmware_version + ' to ' + latest + '.', extra = log_stat)
            self.buildStreamResponse(flavor, latest)
            return
        else:
            logging.debug('No update available', extra = log_stat)
            self.buildHtmlResponse(304)
            return

parser = argparse.ArgumentParser(description='HTTP Server which delivers firmware binaries for Arduino OTA updates.')
parser.add_argument('--dir', help='Directory containing the firmware binaries to serve. Default: ~/firmware')
parser.add_argument('--port', help='Server port. Default: 8000.')
parser.add_argument('--log', help='Log level. Default ERROR')
args = parser.parse_args()

if args.dir:
    FIRMWARE_DIRECTORY = args.dir
if args.port:
    PORT_NUMBER = int(args.port)
if args.log:
    LOG_LEVEL = args.log.upper()
logging.basicConfig(level=LOG_LEVEL, format=LOG_FORMAT)

try:
    server = HTTPServer(('', PORT_NUMBER), HttpHandler)
    print 'Started httpserver on port ' + str(PORT_NUMBER) + ', firmware directory: ' + FIRMWARE_DIRECTORY
    server.serve_forever()

except KeyboardInterrupt:
    print 'Shutting down httpserver'
    server.socket.close()
