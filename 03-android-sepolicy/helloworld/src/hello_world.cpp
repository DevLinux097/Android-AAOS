#include <iostream>
#include <fstream>
#include "../include/mmath.h"

int main() {
    int a = 10;
    int b = 5;

    std::cout << "Hello, World!" << std::endl;
    std::cout << "Addition: " << add(a, b) << std::endl;
    std::cout << "Subtraction: " << subtract(a, b) << std::endl;
    std::cout << "Multiplication: " << multiply(a, b) << std::endl;
    std::cout << "Division: " << divide(a, b) << std::endl;

    std::ofstream file("/data/selinux.txt");

    if (file.is_open()) {
        file << "Hello SELinux";
        std::cout << "Write to file selinux.txt Successfully" << std::endl;
        file.close();
    } else {
        std::cerr << "Cannot open file!" << std::endl;
    }

    return 0;
}
