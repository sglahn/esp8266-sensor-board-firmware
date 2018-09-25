#!/user/bin/python

from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import os
import hashlib

PORT_NUMBER = 8000
FIRMWARE_DIRECTORY = os.environ['HOME'] + "/firmware"

def getLatestFirmwareVersion(prefix):
     for firmware in os.listdir(FIRMWARE_DIRECTORY):
         if firmware.startswith(prefix):
             return firmware[firmware.index("-") +1:firmware.index('.bin')]
     return -1        

class HttpHandler(BaseHTTPRequestHandler):

    def do_GET(self):
        if self.path == "/sensorboard":
            firmware_version = self.headers.get('x-ESP8266-version')
            latest = getLatestFirmwareVersion("sensorboard")
            if not firmware_version:
                self.send_response(500)
                self.send_header('Content-type', 'text/html')
                self.end_headers()
                return
            if float(latest) > float(firmware_version):
                filename = FIRMWARE_DIRECTORY + "/sensorboard-" + latest + ".bin"

                self.send_response(200)
                self.send_header('Content-type', 'application/octet-stream')
                self.send_header('Content-Disposition', 'attachment; filename=' + filename)
                self.send_header('Content-Length', os.path.getsize(filename))
                self.send_header('x-MD5', hashlib.md5(open(filename, 'rb').read()).hexdigest())
                self.end_headers()
                
                binary = open(filename) 
                self.wfile.write(binary.read())
                binary.close()
            else:
                self.send_response(304)
                self.send_header('Content-type', 'text/html')
                self.end_headers()
        return

try:
    server = HTTPServer(('', PORT_NUMBER), HttpHandler)
    print 'Started httpserver on port ' , PORT_NUMBER
    server.serve_forever()

except KeyboardInterrupt:
    print 'Shutting down web server'
    server.socket.close()
