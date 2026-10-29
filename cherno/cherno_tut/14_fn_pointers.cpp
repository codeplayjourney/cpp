#include <iostream>
#include <vector>

void HelloWorld()
{
    std::cout << "Hello World!" << std::endl;
}

void HelloWorld2(int a) {
    std::cout << "Hello World! " << a << std::endl;
}

void PrintValue(int value) {
    std::cout << value << std::endl;
}

void ForEach(const std::vector<int> &values, void(*function)(int)) {
    for (int value: values) {
        function(value);
    }
}

int main() {
    void(*cherno)() = HelloWorld;
    /*
    Read it piece by piece:

    void (*cherno)()
    │     │       │
    │     │       └── the parameter list: () means "takes no arguments"
    │     └────────── (*cherno) means "cherno is a pointer"
    └──────────────── void is the return type: "returns nothing"

    Put together: "cherno is a pointer to a function that takes no arguments and returns void."
    */
    cherno();
    typedef void(*HelloFunction)();
    HelloFunction function2 = HelloWorld;
    function2();
    auto function = HelloWorld;
    function();

    typedef void(*HelloFunction2)(int);
    HelloFunction2 function3 = HelloWorld2;
    function3(5);


    std::vector<int> values = {1, 2, 3, 4, 5};
    ForEach(values, PrintValue);
    return 0;
}