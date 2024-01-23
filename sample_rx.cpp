#include <iostream>
#include <array>
#include <string>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/bind/bind.hpp>

#include "nfiniity_cube_radio.pb.h"
using namespace std;

namespace
{
    string cube_ip = "192.168.1.45";
}

class CubeEvk
{
    private:
        boost::asio::io_service &io;
        boost::asio::ip::udp::socket tx_socket;
        boost::asio::ip::udp::socket rx_socket;

        std::array<uint8_t, 4096> received_data;
        boost::asio::ip::udp::endpoint host_endpoint;

    public:
        CubeEvk(boost::asio::io_service& io);

        void send_data(string payload);
        void handle_packet_received(const boost::system::error_code &ec, size_t bytes);
};

CubeEvk::CubeEvk(boost::asio::io_service& io)
    : io(io), tx_socket(io), rx_socket(io)
{
    static constexpr unsigned int cube_evk_radio_port_tx = 33210;
    static constexpr unsigned int cube_evk_radio_port_rx = 33211;

    boost::asio::ip::address radio_ip = boost::asio::ip::address::from_string(cube_ip);

    const boost::asio::ip::udp::endpoint radio_endpoint_tx(radio_ip, cube_evk_radio_port_tx);
    tx_socket.connect(radio_endpoint_tx);

    boost::asio::ip::udp::endpoint radio_endpoint_rx(boost::asio::ip::udp::v4(), cube_evk_radio_port_rx);
    rx_socket.open(radio_endpoint_rx.protocol());
    rx_socket.bind(radio_endpoint_rx);

     rx_socket.async_receive_from(
        boost::asio::buffer(received_data), host_endpoint,
        boost::bind(&CubeEvk::handle_packet_received, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
        );
}

void CubeEvk::handle_packet_received(const boost::system::error_code &ec, size_t bytes)
{
    GossipMessage gossipMessage;
    gossipMessage.ParseFromArray(received_data.data(), received_data.size());

    switch (gossipMessage.kind_case()) 
    {
        case GossipMessage::KindCase::kCbr:
        {
            // got CBR; use this for your DCC
            // const ChannelBusyRatio& cbr = gossipMessage.cbr();
            // -> cbr.busy() -> cbr.total()
            break;
        }
        case GossipMessage::KindCase::kLinklayerRx:
        {
            cout << "LinkLayerReception:" << endl;
            std::unique_ptr<LinkLayerReception> linkLayerRx{gossipMessage.release_linklayer_rx()};

            cout << "\t Data: " << linkLayerRx->payload() << endl;
            cout << "\t Power: " << linkLayerRx->power_cbm() / 10 << " dbm" << endl;
            break;
        }
        default:
        {
            cerr << "Received GossipMessage of unknown kind " << gossipMessage.kind_case() << std::endl;
        }
    }

    rx_socket.async_receive_from(
        boost::asio::buffer(received_data), host_endpoint,
        boost::bind(&CubeEvk::handle_packet_received, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
        );

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

int main()
{
    boost::asio::io_service io;
    CubeEvk cube(io); 

    // the evk needs one inital packet from its host
    // in order to know where to forward received packages
    cube.send_data("init");  

    io.run();    
    return 0;
}
