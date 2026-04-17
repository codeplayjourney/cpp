#include <iostream>

class String {
private:
    char* m_Buffer;
    int m_Size;
public:
    String(const char* string) {
        m_Size = strlen(string);
        m_Buffer = new char[m_Size + 1]; // include space for null terminator
        memcpy(m_Buffer, string, m_Size);
        m_Buffer[m_Size] = 0; // add termination character
    }
    // copy constructor
    String(const String& other) {
        std::cout << "copy constructor" << std::endl;
        m_Size = other.m_Size;
        m_Buffer = new char[m_Size + 1];
        memcpy(m_Buffer, other.m_Buffer, m_Size);
        m_Buffer[m_Size] = 0;
    }
    // move an r value object into another object
    String(String&& other) noexcept {
        std::cout << "move constructor" << std::endl;
        m_Size = other.m_Size;
        m_Buffer = other.m_Buffer;
        other.m_Size = 0;
        other.m_Buffer = nullptr;
    }
    ~String() {
        std::cout << "delete" << std::endl;
        // if (m_Buffer) {
        // According to the C++ standard:

        // - `delete ptr` and `delete[] ptr` are __no-ops__ (do nothing) when `ptr` is `nullptr`

            std::cout << "deleting buffer" << std::endl;
            delete[] m_Buffer;
        // }
    }

    // move assignment operator
    String& operator=(String&& other) {
        std::cout << "move assignment" << std::endl;
        if (this != &other) {
            delete[] m_Buffer; // free existing resource
            m_Size = other.m_Size;
            m_Buffer = other.m_Buffer; // make m_Buffer point to the resource owned by other
            other.m_Size = 0;
            other.m_Buffer = nullptr; // make other point to null so that it doesn't free the resource in its destructor
        }
        return *this;
    }

    void Print() {
        for (int i = 0; i < m_Size; i++) {
            std::cout << m_Buffer[i] << ',';
        }
        std::cout << std::endl;
    }
};


class Entity {
private:
    String m_Name;
public:
    Entity(String&& name) : m_Name(std::move(name)) {}
    void PrintName() {
        m_Name.Print();
    }
};

int main() {
    Entity entity("Cherno");
    /*
    ### Why the Temporary Dies

    The temporary String object has a very short lifetime:

    - It's created for the duration of the function call
    - Once the Entity constructor finishes, the temporary is no longer needed
    - C++ automatically calls its destructor when it goes out of scope
    */
    entity.PrintName();
    return 0;
}