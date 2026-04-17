#include <iostream>

// strings are const char *

int main() {
    const char* name = "Cherno";
    std::cout << name << std::endl; // Output: Cherno
    char name2[6] = {
        'C', 'h', 'e', 'r', 'n', 'o',
    };
    std::cout << name2 << std::endl; // Output: Cherno
    name2[5] = '\0'; // Null terminator to indicate the end of the string
    std::cout << name2 << std::endl; // Output: Chern
    // std::string name3 = "Cherno" + " is the best!"; // This will not work because "Cherno" is a const char* and cannot be concatenated with a const char*
    std::string name4 = std::string("Cherno") + " is the best!"; // This will work because we are creating a std::string from the const char* and then concatenating it with another const char*
    std::cout << name4 << std::endl; // Output: Cherno is the best!
    return 0;
}