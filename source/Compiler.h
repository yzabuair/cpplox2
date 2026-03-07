// Copyright 2026 Yasser Zabuair.  See LICENSE for details.
#pragma once

#include <string>

namespace cpplox2 {

class Compiler {
    std::string source_;
public:
    Compiler(const std::string& source): source_{source} {
    }
    ~Compiler() = default;
    
    
};

} // namespace cpplox2
