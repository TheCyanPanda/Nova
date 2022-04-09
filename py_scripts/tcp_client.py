#!/usr/bin/python3
import socket
import time

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 1234  # The port used by the server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(b"Hello, world")

    t0 = time.time()
    t1 = 0
    while t1 < 20:  # Run for X seconds
        print(f'time: {t1}')
        try:
            s.sendall(b'Testing testing\n')
            time.sleep(1)
            t1 = time.time() - t0
            #data = s.recv(4)
            #print(f"Received {data!r}")
        except BrokenPipeError: # Server shut down
            print("Error, server shut down")
            break
    data = s.recv(1024)
    s.sendall(b"Hello, world")
    print(f"Received {data!r}")
    s.close()
