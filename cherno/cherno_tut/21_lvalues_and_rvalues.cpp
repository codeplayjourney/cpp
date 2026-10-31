#include <iostream>
#include <string>

// =====================================================================
// LVALUES AND RVALUES  (plus lvalue references and rvalue references)
// Prerequisite for move semantics.
//
//   LVALUE  -- has a location in memory; a real, named, storable thing.
//              Often (not always) the LEFT of '='.  e.g. a variable.
//   RVALUE  -- a temporary value with no storage of its own.
//              Often the RIGHT of '='. e.g. a literal, or a function's
//              returned-by-value result.
//
// Rules to remember:
//   * You can assign TO an lvalue, not to an rvalue:  10 = i;  is illegal.
//   * A non-const lvalue reference (T&)  binds ONLY to lvalues.
//   * A const lvalue reference (const T&) binds to BOTH lvalues & rvalues.
//   * An rvalue reference (T&&) binds ONLY to rvalues (temporaries).
// =====================================================================

// Returns an RVALUE: a temporary int, no storage you can address.
int GetValue() { return 10; }

// Returns an LVALUE: a reference to something with real storage (the
// static int lives for the whole program). Because it's an lvalue, you
// can assign to the call:  GetValueRef() = 5;  is legal.
int& GetValueRef() {
    static int value = 10;
    return value;
}

// const lvalue reference parameter: accepts BOTH lvalues and rvalues.
// This is why you see 'const T&' everywhere -- it's maximally compatible.
void PrintName(const std::string& name) {
    std::cout << "[const&] " << name << std::endl;
}

// -----------------------------------------------------------------
// OVERLOAD PAIR: detect lvalue vs rvalue.
//   T&   -> picked for lvalues (named, storable things)
//   T&&  -> picked for rvalues (temporaries)
// With both present, the compiler routes each argument to the right one.
// -----------------------------------------------------------------
void Describe(std::string& name) {       // lvalue overload
    std::cout << "[lvalue]  " << name << std::endl;
}
void Describe(std::string&& name) {      // rvalue overload (note the &&)
    std::cout << "[rvalue]  " << name << std::endl;
}

int main() {
    // --- basic lvalue / rvalue ---
    int i = 10;      // i is an LVALUE (has storage); 10 is an RVALUE (literal)
    int a = i;       // both sides are lvalues here -- so "left/right" is only a hint
    // 10 = i;       // ERROR: cannot assign to an rvalue (10 has no location)

    int b = GetValue();   // store the returned RVALUE into lvalue b
    // GetValue() = 5;    // ERROR: "expression must be a modifiable lvalue"

    GetValueRef() = 5;    // OK: the function returns an LVALUE reference
    std::cout << "GetValueRef() is now " << GetValueRef() << std::endl;

    // --- lvalue references ---
    int& ref = i;         // OK: non-const lvalue ref bound to an lvalue
    // int& bad = 10;     // ERROR: can't bind non-const lvalue ref to an rvalue
    const int& cref = 10; // OK: const lvalue ref CAN bind an rvalue
                          // (compiler makes a hidden temporary to point at)
    std::cout << "ref=" << ref << " cref=" << cref << std::endl;

    // --- const& accepts both lvalue and rvalue ---
    std::string first = "Yan";
    std::string last  = "Chernikov";
    std::string fullName = first + last;  // first+last is an RVALUE (temporary string)

    PrintName(fullName);        // lvalue  -> fine
    PrintName(first + last);    // rvalue  -> fine, because the param is const&

    // --- lvalue vs rvalue overload resolution ---
    Describe(fullName);         // picks [lvalue]  (fullName is a named variable)
    Describe(first + last);     // picks [rvalue]  (the sum is a temporary)
    Describe("literal here");   // picks [rvalue]  (temporary std::string built)

    // Why rvalue references matter: a temporary won't live long, so a
    // function taking T&& is free to STEAL its resources instead of
    // copying them. That's the basis of move semantics (next lesson).

    std::cin.get();
    return 0;
}
