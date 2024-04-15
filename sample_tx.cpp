#include <iostream>
#include <array>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind/bind.hpp>

#include "nfiniity_cube_radio.pb.h"
using namespace std;

namespace
{
    string cube_ip = "127.0.0.1";
}

class CubeEvk
{
    private:
        boost::asio::io_service &io;
        boost::asio::ip::udp::socket tx_socket;

    public:
        CubeEvk(boost::asio::io_service& io);
        void send_data(string payload);
};

CubeEvk::CubeEvk(boost::asio::io_service& io)
    : io(io), tx_socket(io)
{
    static constexpr unsigned int cube_evk_radio_port_tx = 33210;
    boost::asio::ip::address radio_ip = boost::asio::ip::address::from_string(cube_ip);

    const boost::asio::ip::udp::endpoint radio_endpoint_tx(radio_ip, cube_evk_radio_port_tx);
    tx_socket.connect(radio_endpoint_tx);    
}

void CubeEvk::send_data(string payload)
{
    unique_ptr<LinkLayerTransmission> transmission{new LinkLayerTransmission()};

    array<uint8_t, 6> source_addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    array<uint8_t, 6> destination_addr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    transmission->set_source(source_addr.data(), source_addr.size());
    transmission->set_destination(destination_addr.data(), destination_addr.size());
    transmission->set_priority(LinkLayerPriority::BEST_EFFORT);

    transmission->set_payload(payload);

    CommandRequest command;
    command.set_allocated_linklayer_tx(transmission.release());
    
    std::string serializedTransmission;
    command.SerializeToString(&serializedTransmission);
    tx_socket.send(boost::asio::buffer(serializedTransmission));
}

int main(int argc, char* argv[])
{
    boost::asio::io_service io;
    CubeEvk cube(io); 

    string data = "";
    for(int idx = 1; idx < argc; ++idx)
        data += string(argv[idx]) + " ";

    cout << "Sending: " << data << endl;
    cube.send_data(data);
    
    return 0;
}
