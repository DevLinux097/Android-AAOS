#include <iostream>
#include <fstream>
#include <string>

int main() {
    fstream file("/data/selinux.txt", ios::in | ios::out);
    if (!file.is_open()) {
        cout << "Unable to open file!" << endl;
        return 1;
    }

    file << "Hello Selinux\n";
    file.close();
    std::cout << "File reading and writing completed!" << endl;

    return 0;
}