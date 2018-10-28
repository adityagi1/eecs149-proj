import socket
import re
import time

ip = "0.0.0.0"
port = 80
bufsize = 1024

#(([0-9a-fA-F]{2}:?){6})
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((ip, port))
s.listen(5)
counter = 0;
while 1:
    conn, addr = s.accept()
    data = conn.recv(bufsize)
    if not data:
        conn.close()
        continue
    print(data)
#    data_list = data.split(" ")
#    if (data_list[0] == "mac:"):
#        print('valid macs: ')
#    for mac_addr in data_list[1:]:
#        if re.match('(([0-9a-fA-F]{2}:?){6})', mac_addr):
#            print(mac_addr)
    conn.close()
    counter += 1

