#include <iostream>
#include <string>

struct Vector {
    int x, y;
};

class String {
private:
    char* m_Buffer;
    unsigned int m_Size;
public:
    String(const char* string) {
        m_Size = strlen(string);
        m_Buffer = new char[m_Size + 1];
        memcpy(m_Buffer, string, m_Size);
        m_Buffer[m_Size] = '\0';
    }

    ~String() {
        delete[] m_Buffer;
    }

    friend std::ostream& operator<<(std::ostream& stream, const String& string);

    char& operator[](unsigned int index) {
        return m_Buffer[index];
    }

    // copy constructor (performs a deep copy)
    String(const String& other) {
        m_Size = other.m_Size;
        m_Buffer = new char[m_Size + 1];
        memcpy(m_Buffer, other.m_Buffer, m_Size);
        m_Buffer[m_Size] = '\0';
    }
    // copy assignment operator (performs a deep copy)
    String& operator=(const String& other) {
        if (this != &other) { // check for self-assignment
            delete[] m_Buffer; // free existing resource
            m_Size = other.m_Size;
            m_Buffer = new char[m_Size + 1];
            memcpy(m_Buffer, other.m_Buffer, m_Size);
            m_Buffer[m_Size] = '\0';
        }
        return *this;
    }
};

// print string
std::ostream& operator<<(std::ostream& stream, const String& string) {
    stream << string.m_Buffer;
    return stream;
}

int main() {
    Vector a = {1, 2};
    Vector b = a; // copy constructor is called here
    b.x = 3; // a.x is still 1, b.x is 3
    String string = "Cherno";
    String string2 = string; // did a shallow copy here, string2.m_Buffer points to the same memory as string.m_Buffer
    string[2] = 'a'; // string.m_Buffer is now "Charno", string2.m_Buffer is also "Charno" as they point to the same memory
    // before adding copy constructor: crashes as try to delete the same memory twice when string and string2 go out of scope and their destructors are called
    std::cout << string << std::endl; // prints "Cherno"
    std::cout << string2 << std::endl; // prints "Cherno"
    
    // after adding copy constructor: no longer crashes as we have a copy constructor that performs a deep copy, so string and string2 point to different memory locations
    std::cout << string << std::endl; // prints "Cherno"
    std::cout << string2 << std::endl; // prints "Charno"

    return 0;
}