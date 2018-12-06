#!/usr/bin/env python
# Copyright (c) 2016, Universal Robots A/S,
# All rights reserved.
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
#    * Neither the name of the Universal Robots A/S nor the names of its
#      contributors may be used to endorse or promote products derived
#      from this software without specific prior written permission.
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL UNIVERSAL ROBOTS A/S BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import argparse
import logging
import sys
import rtde.rtde as rtde
import rtde.rtde_config as rtde_config
import rtde.csv_writer as csv_writer
import json
import requests as req
import math
# import socket

ROBOT_IP =  '192.168.0.21'
ROBOT_PORT =  30004
SERVER_IP = '127.0.0.1'
SERVER_PORT = 3001

def apertusNode(name, value):
	item = {}
	item['type'] = "node"
	item['name'] = name
	item['properties'] = {}
	item['properties']['orientation'] = {}
	item['properties']['orientation']['axis'] = {}
	item['properties']['orientation']['axis']['x'] = 0
	item['properties']['orientation']['axis']['y'] = 0
	item['properties']['orientation']['axis']['z'] = 1
	item['properties']['orientation']['angle'] = value
	return item
	
def apertusGripper(name, value):
	item = {}
	item['type'] = "gripper"
	item['name'] = name
	item['properties'] = {}
	item['properties']['value'] = value
	return item

def apertusText(name, value):
	item = {}
	item['type'] = "text"
	item['name'] = name
	item['properties'] = {}
	item['properties']['caption'] = value
	return item


#parameters
parser = argparse.ArgumentParser()
parser.add_argument('--robothost', default=ROBOT_IP, help='ip of robot')
parser.add_argument('--robotport', type=int, default=ROBOT_PORT, help='port number (30004)')
parser.add_argument('--serverhost', default=SERVER_IP, help='IP of server')
parser.add_argument('--serverport', type=int, default=SERVER_PORT, help='port number of server')
parser.add_argument('--samples', type=int, default=0, help='number of samples to record')
parser.add_argument('--config', default='record_configuration.xml', help='data configuration file to use (record_configuration.xml)')
parser.add_argument('--output', default='robot_data.csv', help='data output file to write to (robot_data.csv)')
parser.add_argument("--verbose", help="increase output verbosity", action="store_true")
args = parser.parse_args()

if args.verbose:
    logging.basicConfig(level=logging.INFO)

RTDE_PROTOCOL_VERSION = 1

conf = rtde_config.ConfigFile(args.config)
output_names, output_types = conf.get_recipe('out')

con = rtde.RTDE(args.robothost, args.robotport)
con.connect()

# get controller version
con.get_controller_version()

# set protocol version
if not con.negotiate_protocol_version(RTDE_PROTOCOL_VERSION):
    logging.error('Unable to negotiate protocol version')
    sys.exit()

# setup recipes
if not con.send_output_setup(output_names, output_types):
    logging.error('Unable to configure output')
    sys.exit()

#start data synchronization
if not con.send_start():
    logging.error('Unable to start synchronization')
    sys.exit()

# s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# s.settimeout(5)
# s.connect((args.serverhost, args.serverport))	

# with open(args.output, 'w') as csvfile:
    # writer = csv_writer.CSVWriter(csvfile, output_names, output_types)
    # writer.writeheader()
    
i = 1
keep_running = True
while keep_running:
	if i%125 == 0:
		if args.samples > 0:
			sys.stdout.write("\r")
			sys.stdout.write("{:.2%} done.".format(float(i)/float(args.samples))) 
			sys.stdout.flush()
		else:
			sys.stdout.write("\r")
			sys.stdout.write("{:3d} samples.".format(i)) 
			sys.stdout.flush()
	if args.samples > 0 and i >= args.samples:
		keep_running = False
	try:
		state = con.receive()
		if state is not None:
			# writer.writerow(state)
			if i%20 == 0:				
				url = 'http://{}:{}/api/v1/setproperties'.format(args.serverhost,  args.serverport)
				headers = {'Content-type': 'application/json', 'Access-Control-Allow-Origin': '*'}

				data = {}
				data['items'] = []
				data['items'].append(apertusNode("JOINT(Rotational)(q1)13melecsCell", state.actual_q[0]))				
				data['items'].append(apertusNode("JOINT(Rotational)(q2)14melecsCell", state.actual_q[1]+math.pi/2))
				data['items'].append(apertusNode("JOINT(Rotational)(q3)15melecsCell", state.actual_q[2]))
				data['items'].append(apertusNode("JOINT(Rotational)(q4)16melecsCell", state.actual_q[3]+math.pi/2))
				data['items'].append(apertusNode("JOINT(Rotational)(q5)17melecsCell", state.actual_q[4]))
				data['items'].append(apertusNode("JOINT(Rotational)(q6)18melecsCell", state.actual_q[5]))
				data['items'].append(apertusGripper("ur10Gripper", state.output_int_register_0))
				data['items'].append(apertusText("stateText", "Connection OK"))

				payload =  {}
				payload['data'] = data
				
				#print payload
				
				try:
					r = req.post(url, headers=headers, json=payload)
					if r.status_code == req.codes.ok:
						pass # print "Update OK"
					else:
						print r.status_code #"Error with ApertusVR request."
				except req.ConnectionError:
					print "ApertusVR Connection Error"
					
		else:
			sys.exit()
	except KeyboardInterrupt:
		keep_running = False
	i += 1

# s.close()
sys.stdout.write("\rComplete!            \n")

con.send_pause()
con.disconnect()


