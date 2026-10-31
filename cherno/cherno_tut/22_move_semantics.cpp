#include <iostream>
#include <cstring>   // strlen, memcpy
#include <utility>   // std::move

// =====================================================================
// MOVE SEMANTICS  (C++11, builds on lvalues/rvalues from lesson 21)
//
// Problem: passing/returning heavy objects forces COPIES. Copying a
// string means a brand-new heap allocation + copying every byte. When
// the source is a TEMPORARY (an rvalue) that copy is pure waste -- the
// temporary is about to die anyway.
//
// Solution: a MOVE constructor takes an rvalue reference (T&&) and
// STEALS the temporary's resources (just reassigns the pointer) instead
// of deep-copying. The gutted source is left in a safe "hollow" state.
// =====================================================================

class String {
public:
    // Normal constructor: allocates and deep-copies the incoming text.
    String(const char* string) {
        printf("Created!\n");
        m_Size = strlen(string);
        m_Data = new char[m_Size];
        memcpy(m_Data, string, m_Size);
    }

    // COPY constructor: deep copy -> a SECOND heap allocation.
    String(const String& other) {
        printf("Copied!\n");
        m_Size = other.m_Size;
        m_Data = new char[m_Size];
        memcpy(m_Data, other.m_Data, m_Size);
    }

    // MOVE constructor: takes an rvalue ref (a temporary). No allocation
    // and no byte copy -- just take over the other's pointer, then null
    // out the source so its destructor doesn't free data we now own.
    // 'noexcept' is expected on move ctors (they shouldn't throw).
    String(String&& other) noexcept {
        printf("Moved!\n");
        m_Size = other.m_Size;
        m_Data = other.m_Data;    // steal the pointer (shallow)

        other.m_Size = 0;         // leave source in a valid empty state
        other.m_Data = nullptr;   // so its delete[] on nullptr is a no-op
    }

    ~String() {
        printf("Destroyed!\n");
        delete[] m_Data;          // delete[] nullptr is safe -> no-op
    }

    void Print() {
        // Our toy string has no null terminator, so print char by char.
        for (uint32_t i = 0; i < m_Size; i++)
            printf("%c", m_Data[i]);
        printf("\n");
    }

private:
    char* m_Data;
    uint32_t m_Size;
};

class Entity {
public:
    // lvalue overload: copies the passed-in String.
    Entity(const String& name) : m_Name(name) {
        // m_Name(name): 'name' is an lvalue -> calls String's COPY ctor
    }

    // rvalue overload: receives a temporary String.
    // IMPORTANT: 'name' is itself a NAMED variable here, so it is an
    // LVALUE inside this body. Without std::move it would call the COPY
    // ctor. std::move casts it back to an rvalue so the MOVE ctor runs.
    Entity(String&& name) : m_Name(std::move(name)) {}

    void PrintName() { m_Name.Print(); }

private:
    String m_Name;
};

int main() {
    // "Cherno" builds a temporary String (an rvalue), which is passed to
    // Entity's String&& overload, which MOVES it into m_Name.
    // Result: Created! then Moved! -- only ONE allocation, no Copied!.
    Entity entity(String("Cherno"));
    entity.PrintName();

    return 0;
}
