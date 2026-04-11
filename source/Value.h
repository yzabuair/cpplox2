// Copyright 2026, Yasser Zabuair.  See LICENSE for details.
#pragma once

#include <iostream>
#include <string>
#include <variant>
#include <vector>

namespace cpplox2 {

/// This are the value types we understand in the VM.
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

/// Checks if values are equal to each other.
inline bool values_equal(const Value& value1, const Value& value2) {
    // Damnit, different types.
    if (value1.index() != value2.index()) {
        return false;
    }
    
    switch (value1.index()) {
        case 0: return true;
        case 1: return std::get<bool>(value1) == std::get<bool>(value2);
        case 2: return std::get<double>(value1) == std::get<double>(value2);
        case 3: return std::get<std::string>(value1) == std::get<std::string>(value2);
        default:
            return false;
    }
    return false;
}

/// Dumps out data to an output stream.
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
