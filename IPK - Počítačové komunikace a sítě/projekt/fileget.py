#!/usr/bin/env python3
"""
Juraj Sloboda, xslobo07
IPK 
March 2021

"""

import sys
import socket as s
import re
import os

"""make request for downloading file"""
def MakeRequest(file, server):
    request = b"GET " + file.encode() + b" FSP/1.0\r\n"
    request += b"Hostname: " + server.encode() + b"\r\n"
    request += b"Agent: xslobo07"
    request += b"\r\n\r\n"
    return request

"""make request to find server port (WHERE IS request)"""
def MakeWhereIsRequest(server):
    request = b"WHEREIS " + server + b"\r\n"
    return request

"""parse response from nameserver"""
def ParseNameServerResponse(response):
    tmpResponse = response.decode("ISO-8859-1")
    response = tmpResponse.split()
    if response[0] != "OK":
        sys.stderr.write("ERR fail in server response")
        sys.exit("ERR response isnt OK ")
    ip, host = ParseHost(response[1])
    return ip, host

"""parse response from server to find lenght of file"""
def ParseResponse(response):
    tmpResponse = response.split(b"\r\n\r\n")
    return tmpResponse[1]

def ParsePath(path):
    file = path.split("/")
    i = 0
    tmp = ""
    if len(file) == 2:
        tmp = file[0]
        return tmp , 1
    tmp = file[0]
    while i < len(file)-2:
        tmp += '/' + file[i+1]
        i += 1
    return tmp, i

"""udp for nameserver"""
def Udp(nshost, nsport, server):
    try:
        nameServerSocket = s.socket(s.AF_INET, s.SOCK_DGRAM)
        request = MakeWhereIsRequest(server)
        nameServerSocket.sendto(request, (nshost, nsport))
        response = nameServerSocket.recv(4096)
    except:
        sys.exit("ERR connecting to nameserver")
        sys.stderr.write("ERR connecting to nameserever")
    ip, port = ParseNameServerResponse(response)
    return ip, port

"""tcp to ddownload files"""
def Tcp(host, port, path, server):
    try:
        serverSocket = s.socket(s.AF_INET, s.SOCK_STREAM)
        serverSocket.connect((host,int(port)))
    except:
        sys.exit("error connecting to server")
        sys.stderr.write("ERR connecting to server")
    
    if path == "*":
        Tcp(host, port, "index", server)
        with open("index") as f:
            line = f.readline()
            cnt = 1
            while line:
                Tmpfile = line.split("\n")
                line = f.readline()
                cnt += 1
                Tcp(host,port,Tmpfile[0],server)
        return True

    request = MakeRequest(path, server)
    try:
        serverSocket.sendall(request)
        response = serverSocket.recv(1024)
    except: 
        sys.exit("ERR problem to send request to server")
        sys.stderr.write("ERR problem to send or recieve adta from servre")

    
    afterHeader = ParseResponse(response)
    
    pathToFile , sizeOfPath = ParsePath(path)
    if sizeOfPath != 0:
        if not os.path.exists(pathToFile):
            os.makedirs(pathToFile, mode=0o755)


    file = open(path, "wb")
    file.write(afterHeader)
    while True:
        try:
            response = serverSocket.recv(1024)
        except:
            sys.exit("ERR closed connection to server")
            sys.stderr.write("ERR problem in recieving data from serevr")
        if response == b"":
            break
        file.write(response)
    file.close()
    serverSocket.close()

    return True

"""parse host to find ip and port of nameserver"""
def ParseHost(nameServer):
    server = nameServer.split(":", 1)
    return server[0], server[1]

"""parse SURL to find protocol, name of server and dfile to download"""
def ParseSURL(SURL):
    tmp = SURL.split("/", 3)
    return tmp[0], tmp[2], tmp[3]

"""check and parse arguments"""
def GetAndCheckArguments():
    if len(sys.argv) != 5:
        sys.exit("ERR Not right amount of args")
        sys.stderr.write("ERR not right amount of args")

    nameServer = ""
    SURL = ""
    if sys.argv[1] == "-n":
        nameServer = sys.argv[2]
    if sys.argv[3] == "-n":
        nameServer = sys.argv[4]
    if sys.argv[1] == "-f":
        SURL = sys.argv[2]
    if sys.argv[3] == "-f":
        SURL = sys.argv[4]
    NShost, NSport = ParseHost(nameServer)
    protocol, serverName, path = ParseSURL(SURL)
    try:
        s.inet_aton(NShost)
    except s.error:
        sys.exit("ERR not valid ip")
        sys.stderr.write("ERR not valid ip")
    return NShost, NSport, protocol, serverName, path


NSHost, NSPort, protocol, serverName, path  = GetAndCheckArguments()

"""print("nshost    " + NSHost )
print("nspost    " + NSPort )
print("protocol  " + protocol)
print("server    " + serverName)
print("path      " + path)
print("..........................")
"""

host, port = Udp(NSHost.encode(), 3333, serverName.encode())

"""
print("host      " + host)
print("port      " + port)
"""

Tcp(host, port, path, serverName)
