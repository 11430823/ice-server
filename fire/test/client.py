#!/usr/bin/env python

import socket

s = socket.socket()
s.connect(("192.168.0.104", 9001))
print s.recv(1024)
