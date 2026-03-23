// Copyright 2026, Yasser Zabuair.  See LICENSE for details.
#pragma once

#include <iostream>
#include <string>
#include <variant>
#include <vector>

namespace cpplox2 {

using Value = std::variant<std::monostate, bool, double, std::string>;

struct Values {
    std::vector<Value> constants;
    
    int add_value(Value val) {
        constants.push_back(val);
        return constants.size()-1;
    }
    
    void clear() {
        constants.clear();
    }
};

inline std::ostream& operator<<(std::ostream& stream, const Value& value) {
    switch (value.index()) {
        case 0:
            stream << "empty";
            break;
            
        case 1:
            if (std::get<bool>(value) == true) {
                stream << "true";
            } else {
                stream << "false";
            }
            break;
            
        case 2:
            stream << std::get<double>(value);
            break;
            
        case 3:
            stream << std::get<std::string>(value);
            break;
            
        default:
            stream << "unknown-type";
            break;
    }
    return stream;
}

} // namespace cpplox2
