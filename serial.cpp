#include "serial.hpp"

void Serial::write(std::string message) {
    // std::cout<<"here "<<message;
    std::iostream os(&wbuf_);
    os << message;
    // std::cout<<message.size()<<", "<<wbuf_.size()<<"\n";
    if (!writeBusy) {
        // todo: see websocket_session for ideas on implementing a write queue, as is this will block
        writeBusy = true;
        boost::asio::async_write(sp_, wbuf_, boost::bind(&Serial::writeHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
}

boost::posix_time::seconds Serial::interval = boost::posix_time::seconds(3); // 3 seconds