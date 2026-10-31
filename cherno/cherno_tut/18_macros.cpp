#include <iostream>

// =====================================================================
// MACROS IN C++
// A macro is a PREPROCESSOR instruction. Before the compiler ever sees
// your code, the preprocessor does a pure find-and-replace pass on every
// line starting with '#'. Macros let you define that find-and-replace.
// It happens BEFORE compilation and is just text substitution -- the
// compiler never knows the macro existed.
// =====================================================================

// ---- Controlling the build: define this as 1 (debug) or 0 (release) ----
// Doing it as a VALUE (1/0) instead of just #define PR_DEBUG is clearer:
// you flip it to 0 to disable instead of deleting/commenting the whole line.
#define PR_DEBUG 1

// ---------------------------------------------------------------------
// 1. SIMPLE TEXT REPLACEMENT (the "please never actually do this" example)
// WAIT -> std::cin.get()   The preprocessor pastes the code in verbatim.
// We leave the ';' OUT of the macro so we can write it at the call site.
// This is BAD style: a reader seeing WAIT; has no idea what it means.
// ---------------------------------------------------------------------
#define WAIT std::cin.get()

// ---------------------------------------------------------------------
// 2. MACRO WITH A PARAMETER
// LOG(x) -> std::cout << x << std::endl
// Whatever you pass as x is pasted literally where the 'x' appears.
// LOG("Hello")  becomes  std::cout << "Hello" << std::endl
// ---------------------------------------------------------------------
#define LOG(x) std::cout << x << std::endl

// ---------------------------------------------------------------------
// 3. CONDITIONAL COMPILATION -- the GENUINELY useful use case.
// In debug builds we want logging; in release builds we want it GONE
// (no performance cost, no leaking info). Depending on PR_DEBUG we
// define DLOG either as a real log OR as literally nothing, so the
// calls vanish from the release build entirely.
// ---------------------------------------------------------------------
#if PR_DEBUG == 1
    #define DLOG(x) std::cout << "[DEBUG] " << x << std::endl
#else
    #define DLOG(x)   // expands to nothing -> the call is stripped out
#endif
 
// ---------------------------------------------------------------------
// 4. MULTI-LINE MACRO using backslash to escape the newline.
// A macro normally must be one line. A '\' at the end of a line glues
// the next line on. IMPORTANT: no space after the backslash, or you
// escape the space instead of the newline.
// ---------------------------------------------------------------------
#define LOG_TWICE(x) \
    std::cout << x << std::endl; \
    std::cout << x << std::endl

int main() {
    LOG("Hello");          // -> std::cout << "Hello" << std::endl;  prints: Hello
    LOG(42);               // works with any token; prints: 42

    DLOG("only in debug"); // prints in debug build; disappears entirely in release

    LOG_TWICE("echo");     // prints "echo" twice (one statement per line)

    // ---------------------------------------------------------------
    // 5. BUILT-IN PREPROCESSOR MACROS -- mostly handy for debugging.
    // __FILE__ = current file, __LINE__ = current line number.
    // ---------------------------------------------------------------
    std::cout << "This is " << __FILE__ << " line " << __LINE__ << std::endl;

    // ---------------------------------------------------------------
    // 6. #if 0 ... #endif -- a clean way to disable a whole block.
    // Everything between them is removed before compilation, so even
    // broken code inside won't cause errors. Better than /* */ because
    // it nests cleanly and ignores stray comment markers.
    // ---------------------------------------------------------------
#if 0
    this is not even valid C++ but it compiles fine because it is
    stripped out before the compiler ever sees it;
#endif

    WAIT;  // -> std::cin.get();  pauses until you press Enter
    return 0;
}
