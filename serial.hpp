#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <iostream>
#include <curl/curl.h>
#include "shared_state.hpp"
#include <curl/curl.h>

class Serial : public std::enable_shared_from_this<Serial> {

    void readHandler(const boost::system::error_code& ec, std::size_t bytesRead) {
        if (!ec) {
            std::istream is(&buf_);
            std::string line_str;
            std::getline(is, line_str);
            
            // std::cout << "read-serial - " << bytesRead<<" - "<<line_str.size()<<" - "<<line_str<< "\n";
            
            if (line_str.rfind("TMP", 0) == 0) {
                double currentTemp = std::stod(line_str.substr(3, line_str.find(",")));
                // std::cout << state_->alertEnabled() <<  " - " << state_->alertTemp() << " - " << state_->previousTemp() <<" - "<< currentTemp << "\n";
                
                if (state_->alertEnabled()) {
                    if ( (state_->previousTemp() - state_->alertTemp()) * ( currentTemp - state_->alertTemp() ) <= 0 ) {
                        // send alert
                        CURL *curl;
                        struct curl_slist *list = NULL;
                        curl = curl_easy_init();
                        if (curl) {
                            curl_easy_setopt(curl, CURLOPT_URL, "ntfy.sh/Highett-cetus");
                            list = curl_slist_append(list, "Tags: warning");
                            list = curl_slist_append(list, "Title: Enclosure at temperature");
                            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
                            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, std::to_string(currentTemp).c_str());
                            curl_easy_perform(curl);
                            curl_slist_free_all(list);
                            curl_easy_cleanup(curl);
                        }
                        state_->disableAlert();
                        state_->send("ALE");
                    }
                }
                state_->storePreviousTemp(currentTemp);
            }

            /*// handle alert
            if (state_->alertEnabled() && (line_str.rfind("TMP", 0) == 0)) {
                double currentTemp = std::stod(line_str.substr(3, line_str.find(",")));
                
                // test if we have crossed temp threshold
                if ( (state_->previousTemp() - state_->alertTemp()) * ( currentTemp - state_->alertTemp() ) < 0 ) {
                    // send alert
                    CURL *curl;
                    struct curl_slist *list = NULL;
                    curl = curl_easy_init();
                    if (curl) {
                        curl_easy_setopt(curl, CURLOPT_URL, "ntfy.sh/Highett-cetus");
                        list = curl_slist_append(list, "Tags: warning");
                        list = curl_slist_append(list, "Title: Enclosure at temperature");
                        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
                        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, std::to_string(currentTemp).c_str());
                        curl_easy_perform(curl);
                        curl_slist_free_all(list);
                        curl_easy_cleanup(curl);
                    }
                    state_->disableAlert();
                    state_->send("ALE");
                    // todo: send notification to frontend

                }
            }*/
            
            state_->send(line_str);
            boost::asio::async_read_until(sp_, buf_, '\n', boost::bind(&Serial::readHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }
        else {
            std::cout<<"readHandler errored - " << ec << "\n";
            sp_.close();
            open();
        }
    }

    void writeHandler(const boost::system::error_code& ec, std::size_t bytesRead) {
        // wbuf_.consume(wbuf_.size());
        // writeBusy = false;
        // Handle the error, if any
        if(ec) {
            if( ec == boost::asio::error::operation_aborted )
                return;

            std::cerr << "write" << ": " << ec.message() << "\n";
        }

        // Remove the string from the queue
        queue_.erase(queue_.begin());

        // Send the next message if any
        if(! queue_.empty())
            boost::asio::async_write(sp_, boost::asio::buffer(*queue_.front()), boost::bind(&Serial::writeHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

    }

    void tick(const boost::system::error_code& /*e*/) {//, boost::asio::deadline_timer* t, const std::shared_ptr<shared_state>& state) {
        std::cout<<"in serial tick\n";
        // Attempt serial connection/re-connection
        open();
    }

    void open() {
        boost::system::error_code ec;
        sp_.open(path_, ec);
        if (!ec) {
            state_->addSerial(this);
            sp_.set_option(boost::asio::serial_port_base::baud_rate(9600));
            boost::asio::async_read_until(sp_, buf_, '\n', boost::bind(&Serial::readHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }
        else {
            // try again after delay
            timer_.expires_from_now(interval);
            timer_.async_wait(boost::bind(&Serial::tick, this, boost::asio::placeholders::error));
        }
    }

    boost::asio::serial_port sp_;
    std::string path_;
    boost::asio::streambuf buf_;
    boost::asio::streambuf wbuf_;
    std::shared_ptr<shared_state> state_;
    boost::asio::deadline_timer timer_;
    static boost::posix_time::seconds interval;
    bool writeBusy;
    std::vector<std::shared_ptr<std::string const>> queue_;
    

public:
    Serial(boost::asio::io_context& ioc, const char* path, std::shared_ptr<shared_state> state) 
        : sp_(ioc), path_(path), state_(state), timer_(ioc, interval), writeBusy(false) {
        open();
    }

    //void write(std::string);
    void write (std::shared_ptr<std::string const> const&);

    /*void onRead(const boost::system::error_code& ec, std::size_t bytesRead) {
    }

    void run() {
        boost::asio::async_read_until(
            *serial_, 
            buf3, 
            '\n',
            [self = shared_from_this()](const boost::system::error_code& ec, std::size_t bytesRead) {
                
                boost::asio::async_read_until(*serial_, buf3, '\n', self->onRead);
            }
        );
    }*/
};




#endif