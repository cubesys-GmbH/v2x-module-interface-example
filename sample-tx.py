import sys
sys.path.append(".")
sys.path.append("build")

import socket
from nfiniity_cube_radio_pb2 import *

class CubeEvk:
    def __init__(self, cube_ip):
        self.cube_ip = cube_ip
        self.cube_port_tx = 33210
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def send_data(self, data):
        msg = LinkLayerTransmission()
        msg.source = bytes([0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
        msg.destination = bytes([0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF])
        msg.priority = BEST_EFFORT
        msg.payload = data

        cmd = CommandRequest()
        cmd.linklayer_tx.CopyFrom(msg)
        
        self.sock.sendto(cmd.SerializeToString(), (self.cube_ip, self.cube_port_tx))

    def close(self):
        self.sock.close()

def main():
    cube_ip = "127.0.0.1"
    cube = CubeEvk(cube_ip)

    try:
        data = "hello v2x" # we need minimum data of 2 bytes
        cube.send_data(data.encode())

    finally:
        cube.close()

if __name__ == "__main__":
    main()