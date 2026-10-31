#include <iostream>

// =====================================================================
// STATIC IN A LOCAL SCOPE
// Two things matter when you declare a variable:
//   LIFETIME -- how long it stays in memory before being destroyed
//   SCOPE    -- where in the code it can be accessed
//
// A STATIC LOCAL variable splits these apart:
//   * LIFETIME = the entire program (like a global)
//   * SCOPE    = only the block it was declared in (like a local)
//
// It is initialised ONCE, the first time control reaches it. On every
// later call the same variable is reused -- it is NOT recreated.
// =====================================================================

// ---------------------------------------------------------------------
// 1. THE COUNTER EXAMPLE
// Without 'static', i would be created and set to 0 on every call,
// so this would print 1 every time. WITH 'static', i is created once,
// keeps its value between calls, and counts up: 1 2 3 4 5.
// ---------------------------------------------------------------------
void Function() {
    static int i = 0;   // initialised only on the FIRST call
    i++;
    std::cout << i << std::endl;
}

// For contrast: a plain local that proves the point. This ALWAYS prints 1
// because j is destroyed and recreated each call.
void FunctionNoStatic() {
    int j = 0;
    j++;
    std::cout << j << std::endl;
}

// ---------------------------------------------------------------------
// 2. THE SINGLETON EXAMPLE -- the real payoff
// A Singleton is a class that should have exactly ONE instance.
//
// The OLD way needs a static member pointer plus an external definition
// (shown commented below). The CLEAN way uses a static local variable
// inside Get(): the instance is constructed the first time Get() is
// called and lives forever, so we can safely return it by reference.
// ---------------------------------------------------------------------
class Singleton {
public:
    static Singleton& Get() {
        static Singleton instance;  // created once, lives for the whole program
        return instance;            // safe to return by reference: it never dies
    }

    void Hello() {
        std::cout << "Hello from the one and only Singleton" << std::endl;
    }
};

// WHY 'static' is essential here:
// Without it, 'instance' would be an ordinary local on the stack and
// would be DESTROYED the moment Get() returns. Returning a reference to
// a destroyed object is a serious bug (dangling reference). 'static'
// extends its lifetime to the whole program, so the reference stays valid.

// OBJECT vs POINTER -- why this ONE line already creates the object:
//     static Singleton instance;   // type is Singleton  -> this IS an object;
//                                  // reaching this line runs the constructor
//                                  // and builds a concrete Singleton right here.
// Compare with the pointer in the old way below:
//     static SingletonOld* s_Instance;  // type is SingletonOld* (note the '*')
//                                       // this is only a POINTER -- an empty slot
//                                       // that can hold an object's address. It
//                                       // points at NOTHING until you do
//                                       // s_Instance = new SingletonOld();
// Rule of thumb:
//     Type  x;   // declaring this CONSTRUCTS a concrete object
//     Type* x;   // declaring this makes an empty pointer -- no object exists
//                // until you 'new' one and assign its address.
// That is why the object version needs no 'new', no external definition,
// and no manual nullptr: being an object, the declaration builds it.

// ---- The old, verbose way for comparison (kept commented) ----
// class SingletonOld {
// private:
//     static SingletonOld* s_Instance;   // DECLARATION of an (empty) pointer -- no object yet
// public:
//     static SingletonOld& Get() { return *s_Instance; }
//     void Hello() { std::cout << "hello\n"; }
// };
// SingletonOld* SingletonOld::s_Instance = nullptr;  // external DEFINITION; still points at nothing
// // ...and somewhere you would STILL need:  s_Instance = new SingletonOld();
// // before any real object exists. Three steps vs the one line above.

int main() {
    std::cout << "--- static local counter (expect 1 2 3 4 5) ---" << std::endl;
    Function();
    Function();
    Function();
    Function();
    Function();

    std::cout << "--- plain local (expect 1 1 1 1 1) ---" << std::endl;
    FunctionNoStatic();
    FunctionNoStatic();
    FunctionNoStatic();
    FunctionNoStatic();
    FunctionNoStatic();

    std::cout << "--- singleton via static local ---" << std::endl;
    Singleton::Get().Hello();   // first call constructs the instance
    Singleton::Get().Hello();   // same instance reused

    return 0;
}
