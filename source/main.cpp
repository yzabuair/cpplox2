#include "Chunk.h"
#include "Common.h"
#include "Compiler.h"
#include "Debug.h"
#include "Scanner.h"
#include "Vm.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <source_location>
#include <sstream>
#include <string>


namespace {
struct RuntimeError: public std::exception {
    std::string msg;
    std::source_location location;
    mutable std::string resolved;
    
    RuntimeError(const std::string& message,
                 const std::source_location& curr_location = std::source_location::current()):
        msg{message},
        location{curr_location} {
    }
    
    const char* what() const noexcept {
        if (resolved.empty()) {
            std::stringstream stream;
            stream << "Error: " << msg << "\n";
            stream << "At: [" << location.file_name() << ":" << location.line() << "]\n";
            resolved = stream.str();
        }
        
        return resolved.c_str();
    }
    
    static void throw_exc(const std::string& what) {
        throw RuntimeError(what);
    }
};
std::vector<std::string> parse_line(const std::string& inp_line) {
    std::vector<std::string> command_args;
    
    std::string curr_arg;
    std::stringstream stream(inp_line);
    while(std::getline(stream, curr_arg, ' ')) {
        command_args.push_back(curr_arg);
    }
    
    return command_args;
}

void repl() {
    std::string line;
    std::cout << "===> ";
    while(std::getline(std::cin, line)) {
        auto cmd_args = parse_line(line);
        if (cmd_args[0] == "quit") {
            std::cout << "*** Bye\n";
            break;
        }
        std::cout << "===> ";
    }
}

std::string read_file(const std::string& file) {
    std::fstream file_stream{file};
    if (!file_stream.is_open()) {
        std::stringstream stream;
        stream << "Could not open file: " << file;
        RuntimeError::throw_exc(stream.str());
    }
    
    std::string file_contents;
    std::string curr_line;
    while(std::getline(file_stream, curr_line)) {
        file_contents += curr_line;
        file_contents += "\n";
    }
    
    return file_contents;
}


cpplox2::InterpretResult interpret(const std::string& script,
                                   const std::string& file_name = "<stdin>") {
    cpplox2::Compiler compiler{script,
                               file_name};
    
    auto chunk = compiler.compile();
    cpplox2::disassemble_chunk(chunk, file_name);
    
    return cpplox2::InterpretResult::INTERPRET_OK;
}

int run_file(const std::string& file) {
    auto script = read_file(file);
    
    auto result = interpret(script,
                            file);
    
    return 0;
}

} // inline namespace

int main(int argc, char* argv[]) {
    std::cout << "cpplox2, (c) 2026 Yasser Zabuair.  See LICENSE for details.\n";
    std::cout << "Version 0.0.1\n";
    std::cout << "Built on " << __DATE__ << " at " << __TIME__ << "\n";
    std::cout << "---\n";
    
    try {
        if (argc == 1) {
            repl();
        } else if (argc == 2) {
            return run_file(argv[1]);
        } else {
            std::cout << "Usage: cpplox2 [file_path]\n";
            return 64;
        }
    } catch (const std::exception& exc) {
        std::cout << exc.what() << "\n";
        return 64;
    }
    
    return 0;
}
