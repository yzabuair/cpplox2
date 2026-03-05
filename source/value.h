// Copyright 2026, Yasser Zabuair.  See LICENSE for details.
#pragma once
#include <vector>

namespace cpplox2 {

using Value = double;

struct Values {
    std::vector<Value> constants;
    
    int add_value(Value val) {
        constants.push_back(val);
        return constants.size()-1;
    }
};

} // namespace cpplox2
