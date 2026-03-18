#pragma once

namespace cpplox2 {

/// Detailed error about what went wrong during a parse.
struct ParseError: public std::exception {
    std::string details;
    std::string script_file;
    int script_line{0};
    std::source_location location;
    mutable std::string msg;
    
    
    ParseError(const std::string& details,
               const std::string& script_file,
               int script_line,
               const std::source_location& location = std::source_location::current()):
    details{details},
    script_file{script_file},
    script_line{script_line},
    location{location} {
    }
    
    const char* what() const noexcept override {
        if (msg.empty()) {
            std::stringstream stream;
            stream << "ParseError: " << details << "\n";
            stream << "   in: " << script_file << " at line: " << script_line << "\n";
            stream << "   at: [" << location.file_name() << " : " << location.line() << "]\n";
            msg = stream.str();
        }
        
        return msg.c_str();
    }
};

} // namespace cpplox2
