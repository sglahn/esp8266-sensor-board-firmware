#!/usr/bin/env python3

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

import argparse
import datetime
import configparser
import simplejson as json
import paho.mqtt.client as mqtt
from uuid import getnode as get_mac
from influxdb import InfluxDBClient

class Mqtt2InfluxDb:
    mqtt = None
    influx = None
    topic = None

    def __init__(self, configFile):
        config = configparser.ConfigParser()
        config.read(configFile)

        self.__initMqttClient(config)
        self.__initInfluxDbClient(config)
    

    def connect(self):
        self.mqtt.loop_forever(timeout=1.0, max_packets=1, retry_first_connection=False)

    def disconnect(self):
        self.mqtt.disconnect()

    
    def __initMqttClient(self, config):
        self.topic = config['mqtt']['topic']
        self.mqtt = mqtt.Client(client_id='Mqtt2InfluxDB-Client-' + str(hex(get_mac())))
        self.mqtt.username_pw_set(config['mqtt']['username'], config['mqtt']['password'])
        self.mqtt.connect(config['mqtt']['host'], int(config['mqtt']['port']), 60)
        self.mqtt.on_connect = self.__onConnect
        self.mqtt.on_message = self.__onMessage

    def __onConnect(self, client, userdata, flags, rc):
        self.mqtt.subscribe(self.topic, 2)

    def __onMessage(self, client, userdata, msg):
        print('Received: ' + msg.payload.decode('utf-8') + ' on ' + msg.topic)
        data = self.__createDataSet(msg)
        self.influx.write_points(data)

    def __initInfluxDbClient(self, config):
        database = config['influxdb']['database']
        self.influx = InfluxDBClient(config['influxdb']['host'], config['influxdb']['port'], config['influxdb']['username'], config['influxdb']['password'], database)
        dbAlreadyExists =  next((item for item in self.influx.get_list_database() if item['name'] == database), False)
        if not dbAlreadyExists:
            print('Creating database: ' + database)
            self.influx.create_database(database)

    def __createDataSet(self, msg):
        payload = json.loads(msg.payload.decode('utf-8'))
        time = payload['timestamp'] 
        messurement = payload['key'] 
        value = payload['value']
        return [
            {
                'measurement': messurement,
                'time': time,
                'fields': {
                    'value': value
                }
            }
        ]


def parseArgs():
    parser = argparse.ArgumentParser(description='MQTT client which saves received messages in an InfluxDB instance.')
    parser.add_argument('--config', help='File containing configuration. Default: mqtt2influx.ini', default='mqtt2influx.ini')
    return parser.parse_args()


if __name__ == '__main__':
    args = parseArgs()
    mqtt_client = Mqtt2InfluxDb(args.config)
    try:
        print('Started mqtt2influx client')
        mqtt_client.connect()
    except KeyboardInterrupt:
        print('Stopping mqtt2influx client')
        mqtt_client.disconnect()

