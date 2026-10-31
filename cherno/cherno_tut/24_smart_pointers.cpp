#include <iostream>
#include <memory>    // std::unique_ptr, std::shared_ptr, std::weak_ptr, make_*

// =====================================================================
// SMART POINTERS
// A smart pointer wraps a raw pointer and AUTOMATES delete. You call
// (or avoid) new, and the memory is freed automatically when the smart
// pointer goes out of scope. Goal: never leak by forgetting delete.
//
//   unique_ptr -- sole owner; cannot be copied; near-zero overhead.
//   shared_ptr -- shared ownership via REFERENCE COUNTING; freed when
//                 the last owner dies.
//   weak_ptr   -- a non-owning observer of a shared_ptr; does NOT bump
//                 the reference count; can be asked if it's still alive.
//
// Preference order: unique_ptr first, shared_ptr only if you truly need
// shared ownership. Smart pointers don't fully replace new/delete, but
// use them whenever you just need a heap object cleaned up for you.
// =====================================================================

class Entity {
public:
    Entity()  { std::cout << "Created Entity!\n"; }
    ~Entity() { std::cout << "Destroyed Entity!\n"; }
    void Print() {}
};

int main() {
    // -----------------------------------------------------------------
    // 1. UNIQUE POINTER -- sole owner, auto-deleted at end of scope.
    // Prefer make_unique over 'new' (exception safety: no dangling
    // pointer / leak if a constructor throws).
    // -----------------------------------------------------------------
    std::cout << "--- unique_ptr ---\n";
    {
        std::unique_ptr<Entity> entity = std::make_unique<Entity>();
        entity->Print();           // use it exactly like a raw pointer

        // std::unique_ptr<Entity> e0 = entity;  // ERROR: copy is DELETED.
        // Copying would give two owners of one block -> double free when
        // both die. unique_ptr's copy ctor/assignment are =delete'd.
    }                              // entity dies here -> Entity destroyed

    // -----------------------------------------------------------------
    // 2. SHARED POINTER -- reference counted; copyable.
    // make_shared is preferred: it allocates the object AND its control
    // block (where the ref count lives) in ONE allocation. Using
    // shared_ptr(new Entity) would take two allocations.
    // -----------------------------------------------------------------
    std::cout << "--- shared_ptr (ref counting) ---\n";
    {
        std::shared_ptr<Entity> e0;                 // empty: owns nothing yet (count 0)
        {
            std::shared_ptr<Entity> sharedEntity =
                std::make_shared<Entity>();         // Entity created, count = 1
            e0 = sharedEntity;                      // copy -> shared ownership, count = 2
        }                                           // sharedEntity dies, count = 1
        std::cout << "(inner scope ended; Entity still alive via e0)\n";
    }                                               // e0 dies, count = 0 -> Entity destroyed

    // -----------------------------------------------------------------
    // 3. WEAK POINTER -- observes a shared_ptr WITHOUT owning it.
    // Assigning a shared_ptr to a weak_ptr does NOT raise the ref count,
    // so it won't keep the object alive. Use it when you want a
    // reference you can check but don't want to control the lifetime.
    // -----------------------------------------------------------------
    std::cout << "--- weak_ptr (non-owning) ---\n";
    std::weak_ptr<Entity> weakEntity;
    {
        std::shared_ptr<Entity> sharedEntity = std::make_shared<Entity>();  // count = 1
        weakEntity = sharedEntity;          // weak_ptr does NOT bump count -> still 1
        std::cout << "expired inside scope? " << std::boolalpha
                  << weakEntity.expired() << "\n";   // false: still alive
    }                                       // sharedEntity dies, count = 0 -> destroyed
    std::cout << "expired after scope?  " << weakEntity.expired() << "\n";  // true: gone

    // To safely use a weak_ptr, lock() it into a temporary shared_ptr
    // (null if the object is already gone):
    if (auto locked = weakEntity.lock())
        std::cout << "still usable\n";
    else
        std::cout << "cannot lock: object already destroyed\n";

    return 0;
}
