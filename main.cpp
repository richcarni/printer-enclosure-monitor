#include "net.hpp"
#include "listener.hpp"
#include "shared_state.hpp"
#include "serial.hpp"
#include <boost/asio/signal_set.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>

//oost::posix_time::seconds interval(3); // 3 second
char buf[100];

// void tick(const boost::system::error_code& /*e*/, boost::asio::deadline_timer* t, const std::shared_ptr<shared_state>& state) {

//     std::cout << "tick - " << state->sessionCount() << std::endl;
//     if (state->sessionCount()>0) {
//         state->send("tick");
//     }

//     // Reschedule the timer for 1 second in the future:
//     t->expires_at(t->expires_at() + interval);
//     // Posts the timer event
//     t->async_wait(boost::bind(tick, boost::asio::placeholders::error, t, state));
// }

int main(int argc, char* argv[])
{
    // Check command line arguments.
    if (argc != 4)
    {
        std::cerr <<
            "Usage: websocket-chat-server <address> <port> <doc_root>\n" <<
            "Example:\n" <<
            "    websocket-chat-server 0.0.0.0 8080 .\n";
        return EXIT_FAILURE;
    }

    auto address = net::ip::make_address(argv[1]);
    auto port = static_cast<unsigned short>(std::atoi(argv[2]));
    auto doc_root = argv[3];

    // The io_context is required for all I/O
    net::io_context ioc;

    auto state = std::make_shared<shared_state>(doc_root);

    // Create and launch a listening port
    std::make_shared<listener>(
        ioc,
        tcp::endpoint{address, port},
        state)->run();

    // Add periodic event
    // boost::asio::deadline_timer timer(ioc, interval);
    // timer.async_wait(boost::bind(tick, boost::asio::placeholders::error, &timer, state));
    
    // Serial IO
    Serial serial(
        ioc, 
        "/dev/serial/by-id/usb-STMicroelectronics_BLACKPILL_F103C8_CDC_in_FS_Mode_6D9012685650-if00",
        state);
 
    //serial.write("GMO\n");

    // Capture SIGINT and SIGTERM to perform a clean shutdown
    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(
        [&ioc](boost::system::error_code const&, int)
        {
            // Stop the io_context. This will cause run()
            // to return immediately, eventually destroying the
            // io_context and any remaining handlers in it.
            ioc.stop();
        });

    // Run the I/O service on the main thread
    ioc.run();

    return EXIT_SUCCESS;
}