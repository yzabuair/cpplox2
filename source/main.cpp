// Copyright 2026, Yasser Zabuair.  See LICENSE for details.
#include "Chunk.h"
#include "Common.h"
#include "Compiler.h"
#include "Debug.h"
#include "RuntimeError.h"
#include "Scanner.h"
#include "Value.h"
#include "Vm.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <source_location>
#include <sstream>
#include <string>


namespace {

struct RuntimeOptions {
    bool debug{false};
    bool run_repl{false};
    std::string script_file;
};


RuntimeOptions global_runtime_options = {};

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
        } else if (cmd_args[0] == "help") {
            std::cout << "help: This command.\n";
            std::cout << "quit: Quits the program.\n";
        } else {
            std::cout << "Error: Unknown command\n";
        }
        std::cout << "===> ";
    }
}

std::string read_file(const std::string& file) {
    std::fstream file_stream{file};
    if (!file_stream.is_open()) {
        std::stringstream stream;
        stream << "Could not open file: " << file;
        cpplox2::RuntimeError{stream.str()};
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
    
    cpplox2::Vm vm{chunk, true};
    
    auto result = vm.run();

    return result;
}

int run_file(const std::string& file) {
    auto script = read_file(file);
    
    auto result = interpret(script,
                            file);
    
    return 0;
}


bool parse_command_line(int argc, char* argv[]) {
    if (argc == 1) {
        global_runtime_options.run_repl = true;
    } else if (argc == 2) {
        global_runtime_options.script_file = argv[1];
    } else {
        std::cout << "Usage: cpplox2 [file_path]\n";
        return false;
    }
    
    return true;
}

int execute() {
    if (global_runtime_options.run_repl == true) {
        repl();
    } else if (!global_runtime_options.script_file.empty()) {
        return run_file(global_runtime_options.script_file);
    }
    
    return 0;
}


} // inline namespace

int main(int argc, char* argv[]) {
    std::cout << "cpplox2, (c) 2026 Yasser Zabuair.  See LICENSE for details.\n";
    std::cout << "Version 0.0.1\n";
    std::cout << "Built on " << __DATE__ << " at " << __TIME__ << "\n";
    std::cout << "---\n\n";
    
    try {
        bool keep_truckin = parse_command_line(argc, argv);
        if (keep_truckin) {
            return execute();
        }
    } catch (const std::exception& exc) {
        std::cout << exc.what() << "\n";
        return 64;
    }
    
    return 0;
}
