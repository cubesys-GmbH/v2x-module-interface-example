import sys
sys.path.append("build")

import socket
from nfiniity_cube_radio_pb2 import *

class CubeEvk:
    def __init__(self, cube_ip):
        self.cube_port_tx = 33210
        self.cube_port_rx = 33211
        
        self.cube_ip = cube_ip
        self.sock_tx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock_rx = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock_rx.bind(("127.0.0.1", self.cube_port_rx))

    def send_data(self, data):
        msg = LinkLayerTransmission()
        msg.source = bytes([0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
        msg.destination = bytes([0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF])
        msg.priority = BEST_EFFORT
        msg.payload = data

        cmd = CommandRequest()
        cmd.linklayer_tx.CopyFrom(msg)
        
        self.sock_tx.sendto(cmd.SerializeToString(), (self.cube_ip, self.cube_port_tx))

    def run_reception(self):
        while True:
            data, address = self.sock_rx.recvfrom(1024)
            gossip = GossipMessage()
            gossip.ParseFromString(data)

            match gossip.WhichOneof("kind"):
                case "cbr":
                    # got CBR; use this for your DCC
                    # cbr.busy and cbr.total
                    cbr = gossip.cbr
                case "linklayer_rx":
                    # got a packet
                    print(f"Payload: {gossip.linklayer_rx.payload.decode()}")
                    print(f"Power: {gossip.linklayer_rx.power_cbm / 10} dbm")


    def close(self):
        self.sock_tx.close()
        self.sock_rx.close()

def main():
    cube_ip = "127.0.0.1"
    cube = CubeEvk(cube_ip)

    try:
        # the evk needs one inital packet from its host
        # in order to know where to forward received packages
        cube.send_data(b"init")
        cube.run_reception()

    finally:
        cube.close()

if __name__ == "__main__":
    main()
