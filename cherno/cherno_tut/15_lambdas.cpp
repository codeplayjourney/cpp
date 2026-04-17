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
    // switch to lanbda
    ForEach(values, [](int value) { // [] is the capture method, which is how we pass parameters to the lambda function from the outside world. In this case, we don't need to capture anything, so we leave it empty.
        std::cout << value << std::endl;
    });
    return 0;
}