#coding=utf-8
import socket
import pickle
from datetime import datetime
HOST = '47.93.204.170'
PORT = 8080

if __name__ == '__main__':
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	data = {
		"head":"measure",
		"contents":[datetime.utcnow(),0,0]
	}
	req = pickle.dumps(data)
	sock.sendto(req + "\n", (HOST, PORT))
	print "sent data to "+HOST+" "+str(PORT)
	print "waiting..."
	received = sock.recv(1024)
	t = datetime.utcnow()
	res = pickle.loads(received)
	res[2] = t
	delay0 = res[1]-res[0]
	delay0 = delay0.total_seconds()
	delay1 = res[2]-res[1]
	delay1 = delay1.total_seconds()
	
	print str(delay0),str(delay1)
