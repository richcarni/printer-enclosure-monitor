#include "serial.hpp"

/*void Serial::write(std::string message) {
    // std::cout<<"here "<<message;
    std::iostream os(&wbuf_);
    os << message;
    // std::cout<<message.size()<<", "<<wbuf_.size()<<"\n";
    if (!writeBusy) {
        // todo: see websocket_session for ideas on implementing a write queue, as is this will block
        writeBusy = true;
        boost::asio::async_write(sp_, wbuf_, boost::bind(&Serial::writeHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
}*/
void Serial::write(std::shared_ptr<std::string const> const& ss) {
    // Always add to queue
    queue_.push_back(ss);

    // Are we already writing?
    if(queue_.size() > 1)
        return;

    // We are not currently writing, so send this immediately
    boost::asio::async_write(sp_, boost::asio::buffer(*queue_.front()), boost::bind(&Serial::writeHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));


    /*ws_.async_write(
        net::buffer(*queue_.front()),
        [sp = shared_from_this()](
            error_code ec, std::size_t bytes)
        {
            sp->on_write(ec, bytes);
        });*/
}

boost::posix_time::seconds Serial::interval = boost::posix_time::seconds(3); // 3 seconds