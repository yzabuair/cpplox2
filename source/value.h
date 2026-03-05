#pragma once
#include <vector>

namespace cpplox2 {

using Value = double;

struct Values {
    std::vector<Value> value_data;
    int add_value(Value val) {
        value_data.push_back(val);
        return value_data.size()-1;
    }
};

} // namespace cpplox2
