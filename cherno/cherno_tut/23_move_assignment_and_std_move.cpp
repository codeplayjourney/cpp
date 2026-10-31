#include <iostream>
#include <cstring>   // strlen, memcpy
#include <utility>   // std::move

// =====================================================================
// std::move AND THE MOVE ASSIGNMENT OPERATOR  (continues lesson 22)
//
// Two new pieces on top of the move constructor:
//
// 1. std::move -- a utility that casts an existing (named) variable to
//    an rvalue reference, i.e. "you may steal from this." It does NOT
//    move anything itself; it just marks the object as movable so a
//    move constructor / move assignment gets selected.
//
// 2. Move ASSIGNMENT operator (operator=) -- used when moving into an
//    ALREADY-EXISTING object, as opposed to the move CONSTRUCTOR which
//    builds a brand-new object.
//        String dest = std::move(src);   // construction -> move CTOR
//        dest = std::move(src);          // assignment   -> move operator=
// =====================================================================

class String {
public:
    String() = default;   // see note at bottom of file

    String(const char* string) {
        printf("Created!\n");
        m_Size = strlen(string);
        m_Data = new char[m_Size];
        memcpy(m_Data, string, m_Size);
    }

    // COPY constructor (deep copy).
    String(const String& other) {
        printf("Copied!\n");
        m_Size = other.m_Size;
        m_Data = new char[m_Size];
        memcpy(m_Data, other.m_Data, m_Size);
    }

    // MOVE constructor: steal resources from a temporary, build NEW object.
    String(String&& other) noexcept {
        printf("Moved!\n");
        m_Size = other.m_Size;
        m_Data = other.m_Data;    // steal pointer
        other.m_Size = 0;
        other.m_Data = nullptr;   // leave source hollow but valid
    }

    // MOVE ASSIGNMENT operator: move into an object that ALREADY EXISTS.
    // Returns String& so you can chain (a = b = c). Key differences from
    // the move constructor:
    //   * self-assignment guard: moving an object into itself must be a
    //     no-op, or we'd delete our own data and lose it.
    //   * we must delete[] our OWN current data first, or reassigning
    //     m_Data leaks the old allocation.
    String& operator=(String&& other) noexcept {
        printf("Moved (assignment)!\n");
        if (this != &other) {         // skip if moving into self
            delete[] m_Data;          // free what we currently own

            m_Size = other.m_Size;
            m_Data = other.m_Data;    // steal pointer

            other.m_Size = 0;
            other.m_Data = nullptr;   // hollow out the source
        }
        return *this;                 // return reference to self
    }

    ~String() {
        printf("Destroyed!\n");
        delete[] m_Data;              // delete[] nullptr is a safe no-op
    }

    void Print() {
        for (uint32_t i = 0; i < m_Size; i++)
            printf("%c", m_Data[i]);
        printf("\n");
    }

private:
    char* m_Data = nullptr;   // default-initialised so default ctor is safe
    uint32_t m_Size = 0;
};

int main() {
    String apple = "Apple";
    String dest;              // existing empty object (uses default ctor)

    std::cout << "apple: "; apple.Print();
    std::cout << "dest:  "; dest.Print();

    // dest already exists, so '=' calls the MOVE ASSIGNMENT operator.
    // std::move casts 'apple' (a named lvalue) to an rvalue so the move
    // path is chosen instead of a copy. Ownership transfers to dest.
    dest = std::move(apple);

    std::cout << "After move:" << std::endl;
    std::cout << "apple: "; apple.Print();   // now empty (hollow)
    std::cout << "dest:  "; dest.Print();    // now "Apple"

    return 0;
}
