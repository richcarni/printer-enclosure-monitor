//
// Copyright (c) 2018 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/vinniefalco/CppCon2018
//

#ifndef CPPCON2018_SHARED_STATE_HPP
#define CPPCON2018_SHARED_STATE_HPP

#include <memory>
#include <string>
#include <unordered_set>
#include <optional>
//#include "serial.hpp"
// #include <boost/asio.hpp>

// Forward declaration
class websocket_session;
class Serial;

// Represents the shared server state
class shared_state
{
    std::string doc_root_;

    // This simple method of tracking
    // sessions only works with an implicit
    // strand (i.e. a single-threaded server)
    std::unordered_set<websocket_session*> sessions_;

    // Manage a single usb serial connection
    std::optional<Serial*> serial_;

    // Reached temperature alert
    bool alertEnabled_ = false;
    double alertTemp_ = 0.0;
    double previousTemp_ = 0.0;

public:
    explicit
    shared_state(std::string doc_root);

    std::string const&
    doc_root() const noexcept
    {
        return doc_root_;
    }

    void join  (websocket_session& session);
    void leave (websocket_session& session);
    void send  (std::string message);

    int sessionCount() {
        return sessions_.size();
    }

    void addSerial (Serial* sp) {
        serial_.emplace(sp);
    }

    void sendSerial (std::string message); 

    void setAlertTemp(double level) {
        alertTemp_ = level;
        alertEnabled_ = true;
    }

    void disableAlert() {
        alertEnabled_ = false;
    }

    void storePreviousTemp(double temp) {
        previousTemp_ = temp;
    }

    bool alertEnabled() {
        return alertEnabled_;
    }

    double previousTemp() {
        return previousTemp_;
    }

    double alertTemp() {
        return alertTemp_;
    }

};

#endif