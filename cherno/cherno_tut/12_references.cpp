// references is a pointer in disguise, they are an alias for another variable
#include <iostream>

void Increment(int& value) { // accepts an lvalue reference
    value++;
}
// same as
void IncrementValueByPtr(int* value) { // accepts a pointer
    (*value)++;
}

int main() {
    int a = 5;
    int& ref = a;
    ref = 10;
    std::cout << "a: " << a << std::endl; // a should be 10, because ref is an alias for a
    // note: you cannot have a reference that is not initialized, and you cannot change what a reference refers to after it has been initialized:
    int b = 20;
    ref = b;
    std::cout << "a: " << a << std::endl; // a should be 20, because ref is an alias for a
    b = 30;
    std::cout << "a: " << a << std::endl; // a should still be 20, because ref is an alias for a, and changing b does not change a

    // to change what a reference refers to, you can use a pointer instead:
    int* ptr = &a; // ptr is a pointer to a
    std::cout << "a: " << a << std::endl; // a should be 20, because ptr is a pointer to a
    *ptr = 40; // change the value of a through the pointer
    std::cout << "a: " << a << std::endl; // a should be 40, because we changed the value of a through the pointer

    return 0;
}