import socket
import time


class TcpClient:

    def __init__(self, host: str = 'localhost', port: int = 1234):
        self.host = host
        self.port = port
        self.server = None

    def connect(self, host: str = 'localhost', port: int = 1234):
        self.host = host
        self.port = port
        self.server = socket.socket()
        self.server.connect((self.host, self.port))

    def close(self):
        self.server.close()

    def send_cmd(self, msg: str, recv_bytes=1024, end='\n'):
        msg += end
        self.server.sendall(msg.encode('utf-8'))
        data = self.server.recv(recv_bytes)
        print('geh')
        return data


