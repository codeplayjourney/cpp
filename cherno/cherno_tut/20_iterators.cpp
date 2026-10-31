#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>

// =====================================================================
// ITERATORS
// An iterator is a way to traverse ("iterate over") a collection of
// elements. For a vector or array you COULD just use an index with [],
// but that only works for contiguous, index-able data. For a set, map,
// tree, etc. there is no simple incrementing index -- so iterators are
// how you walk through them.
//
// A range-based for loop is just shorthand for an explicit iterator
// loop: it works because the container provides begin() and end(),
// which return iterators.
// =====================================================================

int main() {
    std::vector<int> values = {1, 2, 3, 4, 5};

    // -----------------------------------------------------------------
    // 1. CLASSIC INDEX LOOP
    // Works for vectors/arrays because they support operator[] with a
    // numeric index. Prints 1 2 3 4 5.
    // -----------------------------------------------------------------
    std::cout << "--- index loop ---" << std::endl;
    for (int i = 0; i < values.size(); i++)
        std::cout << values[i] << std::endl;

    // -----------------------------------------------------------------
    // 2. RANGE-BASED FOR LOOP (C++11)
    // Cleaner when you don't need the index 'i'. 'value' is a copy of
    // the current element each iteration.
    // -----------------------------------------------------------------
    std::cout << "--- range-based for ---" << std::endl;
    for (int value : values)
        std::cout << value << std::endl;

    // -----------------------------------------------------------------
    // 3. EXPLICIT ITERATOR LOOP -- what #2 expands to under the hood.
    //   begin() -> iterator at the first element
    //   end()   -> iterator ONE PAST the last element (not a valid
    //              element, just a "stop here" marker)
    //   ++it    -> advance to the next element
    //   *it     -> dereference (like a pointer) to get the element
    // You rarely write this for a vector, but it's the mechanism that
    // makes the range-based loop work.
    // -----------------------------------------------------------------
    std::cout << "--- explicit iterator ---" << std::endl;
    for (std::vector<int>::iterator it = values.begin(); it != values.end(); it++)
        std::cout << *it << std::endl;

    // =================================================================
    // MAPS -- where iterators become NECESSARY
    // An unordered_map (hash map) has no order and no numeric index, so
    // an index loop is impossible. You must iterate.
    // =================================================================
    std::unordered_map<std::string, int> map;
    map["Cherno"] = 5;
    map["C++"]    = 2;

    // -----------------------------------------------------------------
    // 4. EXPLICIT ITERATOR OVER A MAP
    // The iterator type is long, so a 'using' alias helps. Each element
    // is a std::pair: use the arrow operator to reach
    //   it->first  = the key
    //   it->second = the value
    // const_iterator = read-only access (can't mutate the elements).
    // -----------------------------------------------------------------
    using ScoreMap = std::unordered_map<std::string, int>;
    std::cout << "--- map via explicit iterator ---" << std::endl;
    for (ScoreMap::const_iterator it = map.begin(); it != map.end(); it++) {
        const std::string& key = it->first;
        int value = it->second;
        std::cout << key << " = " << value << std::endl;
    }

    // -----------------------------------------------------------------
    // 5. RANGE-BASED FOR OVER A MAP
    // 'kv' is each std::pair; reach into it with .first / .second.
    // -----------------------------------------------------------------
    std::cout << "--- map via range-based for ---" << std::endl;
    for (const auto& kv : map)
        std::cout << kv.first << " = " << kv.second << std::endl;

    // -----------------------------------------------------------------
    // 6. STRUCTURED BINDINGS (C++17) -- the cleanest form.
    // Unpack the pair's key and value directly into named variables.
    // Requires compiling with -std=c++17 or later.
    // -----------------------------------------------------------------
    std::cout << "--- map via structured bindings (C++17) ---" << std::endl;
    for (const auto& [key, value] : map)
        std::cout << key << " = " << value << std::endl;

    return 0;
}
