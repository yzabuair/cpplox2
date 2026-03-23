// Copyright 2026, Yasser Zabuair.  See LICENSE for details.
#pragma once

#include <exception>
#include <source_location>
#include <sstream>
#include <string>

namespace cpplox2 {

struct RuntimeError: public std::exception {
    mutable std::string msg;
    std::string details;
    std::source_location location;
    
    RuntimeError(const std::string& details,
                 const std::source_location& location = std::source_location::current()):
        details(details),
        location(location) {
    }
    
    const char* what() const noexcept override {
        if (msg.empty()) {
            std::stringstream stream;
            stream << "RuntimeError: " << details << "\n";
            stream << "          at: " << location.file_name() << ", line: " << location.line() << "\n";
            msg = stream.str();
        }
        
        return msg.c_str();
    }
};

} // namespace cpplox2
