Sure. Here's a clean summary you can refer back to.

---

### What the SPSC ring buffer is

A fixed-size circular array that passes data between two threads without locks.

```
Network thread (producer) → pushes MarketUpdate into buffer
Processing thread (consumer) → pops MarketUpdate from buffer
```

No mutex needed because only one thread touches each end.

---

### The data structure

```cpp
template<typename T, std::size_t Capacity>
class SPSCRingBuffer {
    std::array<T, Capacity> buffer_;              // the actual data — NOT atomic
    alignas(64) std::atomic<size_t> write_pos_;   // producer's position — atomic
    alignas(64) std::atomic<size_t> read_pos_;    // consumer's position — atomic
};
```

---

### Why each design decision

**Fixed size / power of 2:**

```
allocated once upfront — no heap allocation during operation
power of 2 → use & (Capacity-1) instead of slow % modulo
```

**`buffer_` not atomic:**

```
producer writes buffer_[write_pos_]
consumer reads  buffer_[read_pos_]
always different indices — never simultaneous access to same slot
no torn reads possible → atomic not needed
```

**`write_pos_` and `read_pos_` ARE atomic:**

```
both threads read write_pos_ simultaneously
both threads read read_pos_ simultaneously
without atomic → torn reads possible (half-written values)
atomic guarantees indivisible read/write
```

**`alignas(64)` on atomics:**

```
cache line = 64 bytes on modern CPUs
write_pos_ and read_pos_ on same cache line → false sharing
one thread writes → invalidates other thread's cache line → cache miss
alignas(64) puts each on its own cache line → no interference
```

---

### Memory ordering — the three levels

```
relaxed  → just atomicity, no ordering
           use when reading YOUR OWN variable nobody else writes
           e.g. producer reading write_pos_ in push()

release  → everything written BEFORE this stays before it
           use when publishing data to another thread
           e.g. producer storing write_pos_ after writing buffer_[w]

acquire  → everything read AFTER this stays after it
           use when receiving data from another thread
           e.g. consumer loading write_pos_ before reading buffer_[r]
```

---

### Why release/acquire needed (not just atomic)

Two separate problems:

```
atomicity alone:
    ✅ prevents torn reads of write_pos_
    ❌ doesn't prevent compiler reordering buffer write after write_pos_ update
    ❌ doesn't prevent CPU store buffer flushing write_pos_ before buffer_[w]
    result: consumer sees signal but buffer_[w] not written yet → garbage

release/acquire adds:
    ✅ compiler barrier — buffer_[w] write cannot move past release store
    ✅ CPU fence — store buffer flushed before release store completes
    ✅ happens-before — consumer guaranteed to see buffer_[w] after acquire load
    result: correct ✅
```

---

### The happens-before handshake

```
producer:
    buffer_[w] = item              ← write data (non-atomic, covered by release)
    write_pos_.store(release)      ← "I'm done, here's everything I wrote"

consumer:
    write_pos_.load(acquire)       ← "I see your signal, give me everything"
    T item = buffer_[r]            ← safe to read, guaranteed visible
```

Release/acquire on `write_pos_` carries the buffer write visibility across threads. `write_pos_` is the signalling mechanism. `buffer_` is the actual data.

---

### Buffer full/empty detection

```
empty: read_pos_ == write_pos_
full:  (write_pos_ + 1) & (Capacity-1) == read_pos_

one slot always wasted to distinguish full from empty
that's why minimum useful capacity is 2
```

---

### Why drop instead of spin inside push()

```
ring buffer = simple fast primitive, no policy decisions
caller decides what to do when full:
    market data → drop (old data worthless anyway)
    other uses  → spin, log, block — caller's choice

if push() spun internally:
    network thread stuck inside push()
    stops calling recvfrom()
    OS drops packets at socket buffer level
    worse outcome than dropping one packet in ring buffer
```

---

### Why ring buffer over alternatives

```
std::queue + mutex:   lock/unlock = 100-300ns, OS involved, thread sleeping
std::list:            heap allocation per node = unpredictable latency
ring buffer (SPSC):   5-10ns, no OS, no heap allocation, pure hardware atomics
```

---

### The three things that break non-atomic shared variables

```
1. compiler caches in register  → loop never sees updated value
2. compiler reorders instructions → signal sent before data written
3. CPU store buffer reorders     → write_pos_ flushed before buffer_[w]

std::atomic with release/acquire prevents all three
```

---

### Relaxed is correct ONLY for reading your own variable

```cpp
// producer — only producer writes write_pos_, relaxed fine:
size_t w = write_pos_.load(memory_order_relaxed);

// consumer — reads write_pos_ written by producer, needs acquire:
write_pos_.load(memory_order_acquire);

// producer — signals consumer, needs release:
write_pos_.store(next_w, memory_order_release);
```

---

### Template parameters

```cpp
template<typename T, std::size_t Capacity>
//       ^^^^^^^^^^  ^^^^^^^^^^^^^^^^^^^
//       type param  non-type param (compile-time value)

SPSCRingBuffer<MarketUpdate, 1024>
//             ^^^^^^^^^^^^  ^^^^
//             T = MarketUpdate  Capacity = 1024 baked in at compile time
```

Must be in `.h` file because compiler needs full template definition at instantiation point.
