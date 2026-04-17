#include <iostream>
#include <string>

class Entity {
private:
    int* m_X, *m_Y;
public:
    const int* const GetX() const { return m_X; }
    int* GetY() const { return m_Y; }
};

void PrintEntity(const Entity& e) {
    std::cout << *e.GetX() << ", " << *e.GetY() << std::endl;
}

int main() {
    
    return 0;
}

// ## 1. `const int*` → `int*`

// ❌ __FORBIDDEN__ - Compilation error

// ```cpp
// const int* constPtr = ...;
// int* ptr = constPtr;  // ❌ Error: invalid conversion from 'const int*' to 'int*'
// ```

// __Why forbidden?__ This removes const-ness from the pointed-to data, which could allow modification of data that was meant to be read-only.

// ## 2. `int* const` → `int*`

// ✅ __ALLOWED__ - Works perfectly!

// ```cpp
// int* const constPtr = ...;
// int* ptr = constPtr;  // ✅ Compiles fine
// ```
