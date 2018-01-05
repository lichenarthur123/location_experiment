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
			'measure':self.measure
		}
		req = pickle.loads(data)
		route_table[req["head"]](req["contents"])
	
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
		print self.client_address
		socket.sendto(content,self.client_address)
		
		#socket.sendto(content,('61.164,37.139',self.client_address[1]))
		print "sent to client:"+self.client_address[0]
		
			

if __name__ == "__main__":
	s = SocketServer.UDPServer((Myhost,Port),MyHandler)
	s.serve_forever()
