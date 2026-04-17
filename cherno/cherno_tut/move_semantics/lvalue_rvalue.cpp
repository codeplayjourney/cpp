#include <iostream>

int GetValue() {
    return 5;
}

int& GetValueRef() {
    static int value = 5;
    return value;
}

void SetValue(const int& value) { // accepts both rvalues and lvalues
    // dummy function to demonstrate accepting both rvalues and lvalues
}

void SetValueAcceptingRValue(int&& value) { // accepts only rvalues
    // dummy function to demonstrate accepting only rvalues
}

int main() {
    int x = GetValue(); // rvalue
    int& y = GetValueRef(); // lvalue reference
    GetValueRef() = 10;
    std::cout << "x: " << x << std::endl; // x: 5
    std::cout << "y: " << y << std::endl; // y:

    /*
    cant have an l value reference of an r value
    int &a = 10
    */
   // but can have l value const reference of an r value
   /*
   behind the scenes does smth like this:
   int temp = 10;
   const int &a = temp;
   */
    const int &a = 10;
    SetValue(10); // rvalue
    int z = 20;
    SetValue(z); // lvalue
    SetValueAcceptingRValue(30); // rvalue
    // SetValueAcceptingRValue(z); // error: cannot bind rvalue reference of type 'int&&' to lvalue of type 'int'
    return 0;
}