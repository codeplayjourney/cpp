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
    /*
        A lambda is just a function you write inline, right where you need it, without giving it a name or declaring it separately up top. Let me connect it to what you already did.

    The before and after

    Previously you passed a named function:

        void PrintValue(int value) {        // defined separately, up at file scope
            std::cout << value << std::endl;
        }

        ForEach(values, PrintValue);        // pass its address

    Now you pass a lambda instead:

        ForEach(values, [](int value) {
            std::cout << value << std::endl;
        });

    Both do the exact same thing. The lambda is just PrintValue's body written directly at the call site, with no name. You didn't have to go define a whole separate function somewhere else — you defined the behavior right where you hand it in.

    Anatomy of the lambda

        [](int value) { std::cout << value << std::endl; }
        │ │           │                                  │
        │ │           └── body: what it does (same as any function body)
        │ └────────────── parameter list: takes an int called value
        └──────────────── capture list: what outside variables it needs (empty here)

    Compare that to a normal function and you'll see it's the same three pieces, just reordered and nameless:

        void PrintValue (int value) { ...body... }   // normal: return-type NAME (params) { body }
                    { ...body... }   // lambda:  [capture] (params) { body }

    The only genuinely new part is the [] at the front — the capture list. A normal function can't see local variables from main; a lambda can, if you list them in []. Here it's empty because the body only uses value (which comes in as a parameter), so there's nothing from the outside to grab.

    Why this works with ForEach

    Remember ForEach's second parameter is void(*function)(int) — a pointer to a function taking an int, returning void. A lambda with an empty capture list [] converts automatically into exactly that kind of function pointer. So it slots right in where PrintValue went.
    */
    ForEach(values, [](int value) { // [] is the capture method, which is how we pass parameters to the lambda function from the outside world. In this case, we don't need to capture anything, so we leave it empty.
        std::cout << value << std::endl;
    });
    return 0;
}