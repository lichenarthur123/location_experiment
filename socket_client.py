#coding=utf-8
import socket

HOST = '47.93.204.170'
PORT = 8080

if __name__ == '__main__':
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	data = {
		"head":"test_alive",
		"contents":"hello"
	}
	sock.sendto(data + "\n", (HOST, PORT))
	print "sent data to "+HOST+" "+str(PORT)
	print "waiting..."
	received = sock.recv(1024)
	print received
