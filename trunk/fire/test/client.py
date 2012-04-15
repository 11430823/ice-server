#!/usr/bin/python

import socket

for i in range (1, 10000):
	s = socket.socket()
	s.connect(("10.1.1.57", 9001))
	#print 'connect:', i
