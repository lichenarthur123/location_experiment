#coding=utf-8

import SocketServer
import time
import pickle
from datetime import datetime

Myhost = ''
Port = 8080

class MyHandler(SocketServer.BaseRequestHandler):

	
	def handle(self):
		data = self.request[0].strip()
        #socket = self.request[1]
		print "received a request."
		self.route(data)
		
	#route requests	
	def route(self,data):
		route_table = {
			'task':"test",
			'test_alive':self.test_alive,
			'challenge':self.challenge,
			'measure':self.measure
		}
		req = pickle.loads(data)
		route_table[req["head"]](req["contents"])
		
	# as a V to generate a challenge and send to server
	def challenge(self):
		pass
	
	# query be challenge
	def query_challenge(self):
		pass
	
	#server generate a proof and send to V
	def proof(self):
		pass
	
	#V to check proof
	def check_proof(self):
		pass
	
	#master test if machine alive 
	def test_alive(self,content):
		print "received."
		socket = self.request[1]
		socket.sendto(content,self.client_address)
		print "sent to client:"+self.client_address[0]
		
	def measure(self,content):
		socket = self.request[1]
		content[1] = datetime.utcnow()
		content = pickle.dumps(content)
		socket.sendto(content,self.client_address)
		print "sent to client:"+self.client_address[0]
		
	#master assign a task
	def task(self):
		pass

if __name__ == "__main__":
	s = SocketServer.UDPServer((Myhost,Port),MyHandler)
	s.serve_forever()