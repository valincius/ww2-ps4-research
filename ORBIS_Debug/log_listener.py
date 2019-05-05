import socket
import select
import sys
import threading

def connect():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    server_address = ('192.168.1.21', 4455)
    while True:
        try:
            sock.connect(server_address)
            break
        except:
            continue

    return sock

def listener(sock):
    while True:
        data = sock.recv(1100)
        if not data: continue
        print(str(data, 'utf-8'))

def sender(sock):
    while True:
        command = input()
        sock.send(command.encode())

if __name__ == '__main__':
    sock = connect()
    print('Connected to PS4')

    t1 = threading.Thread(target=listener, args=(sock,))
    t2 = threading.Thread(target=sender, args=(sock,))
    t1.start()
    t2.start()

    while True:
        pass
