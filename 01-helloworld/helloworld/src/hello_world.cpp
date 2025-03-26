#include <iostream>
#include "../include/mmath.h"

int main() {
    int a = 10;
    int b = 5;

    std::cout << "Hello, World!" << std::endl;
    std::cout << "Addition: " << add(a, b) << std::endl;
    std::cout << "Subtraction: " << subtract(a, b) << std::endl;
    std::cout << "Multiplication: " << multiply(a, b) << std::endl;
    std::cout << "Division: " << divide(a, b) << std::endl;

    return 0;
}
