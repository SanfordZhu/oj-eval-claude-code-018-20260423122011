#include "value.hpp"
#include <iostream>

int main() {
    Assoc env = empty();
    
    // Test extend
    env = extend("x", IntegerV(5), env);
    
    // Test find
    Value v = find("x", env);
    if (v.get() != nullptr) {
        std::cout << "Found x: ";
        v->show(std::cout);
        std::cout << std::endl;
    } else {
        std::cout << "x not found" << std::endl;
    }
    
    return 0;
}
