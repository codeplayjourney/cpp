#include <iostream>
#include <string>

int main() {
    const char name[8] = "Che\0rno"; // need 8 to include null terminator and the unused space
    std::cout << name << std::endl; // Output: Che
    std::cout << strlen(name) << std::endl; // Output: 3 (strlen counts characters until the null terminator)
    std::cout << sizeof(name) << std::endl; // Output: 8 (sizeof counts the total size of the array, including the null terminator and any unused space)
    const char* name2 = "Che\0rno";
    std::cout << name2 << std::endl; // Output: Che
    std::cout << strlen(name2) << std::endl; // Output: 3
    std::cout << sizeof(name2) << std::endl; // Output: 8
    return 0;
}