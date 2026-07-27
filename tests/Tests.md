# Test Harness

A test harness for this project isn't one thing — it's three layers, each catching a different class of bug. Let me explain what each layer is, why it exists, and how to build it, in the order you should build them.

## Layer 1: Unit tests — "does each operation do what I hand-computed?"

**Pick a framework rather than hand-rolling.** A test framework gives you three things `main()` with `assert`s doesn't: each test runs independently (one failure doesn't hide the rest), failures report _what_ mismatched ("expected 70, got 0" — which would have caught your `fill` bug instantly with the actual values on screen), and you can run subsets by name. For C++ the realistic choices are **Catch2** (my recommendation for you: single dependency, minimal ceremony, tests read almost like prose), **doctest** (same idea, faster compiles), or **GoogleTest** (the industry standard — worth knowing exists since most firms use it, but heavier setup). All three install through vcpkg in one line, which is your home turf, and integrate with CMake/CTest so `ctest` runs everything.

**Structure: one test file per type, one test per behavior.** A test has three beats — _arrange_ (build the starting state), _act_ (one operation), _assert_ (check every consequence). The discipline that matters: each test verifies one behavior with a name that states it, like `"partial fill reduces quantity and order stays unfilled"`. When that name shows up red in a list of forty green ones, the diagnosis is done before you've opened the debugger.

**Where the test cases come from: your paper.** Remember the hand-worked exercise — five orders processed by hand, book state and trade reports written down after each? Those _are_ the tests, transcribed. For the current `Order` type the list is short: construction stores all fields; partial fill leaves the remainder and `is_filled()` false; exact fill hits zero and `is_filled()` true; two partial fills accumulate. When `Book` exists, the list grows to the scenarios we walked through: an order that walks two levels, the leftover resting as a new best bid, cancel of a resting order, cancel of an already-filled order (assert _reject_ and assert the book is unchanged), cancel twice. Every assertion is a number you computed by hand first — the test encodes your paper, and the engine must agree with the paper.

**Run them in debug with sanitizers.** Compile the test build with `-fsanitize=address,undefined` and without `NDEBUG`, so your `assert(amount <= quantity_)` is live and memory bugs (use-after-free when you get to intrusive lists in v2 — the classic hazard there) crash loudly at the faulty line instead of corrupting silently. This is two lines in CMake and it will save you an entire weekend during v2, guaranteed.

## Layer 2: The invariant checker — "is the book _ever_ in an impossible state?"

Unit tests check specific scenarios you thought of. The invariant checker catches scenarios you _didn't_ think of, and it's the piece that makes this project's testing story interview-worthy.

**An invariant is a property that must hold after every operation, no matter what the operation was.** For your book: best bid < best ask (the spread is never crossed); every level's cached total equals the sum of its orders' quantities; no order has zero quantity while resting; every order in the ID map is reachable through some level, and every resting order in every level has an ID map entry (the two indexes agree exactly — this one would catch a whole family of "removed from one structure but not the other" bugs); bid levels are sorted descending, ask levels ascending.

**Implementation: one function, `check_invariants(book)`, that walks the whole book and asserts each property.** It's deliberately slow — it touches everything — which is fine because it never runs in benchmarks, only in tests. Give `Book` a test-only inspection path (a friend declaration for the test, or simply enough public read-only accessors to walk levels and the map) so the checker can see inside without weakening the production interface.

## Layer 3: The replay harness — "throw a million random operations at it"

This is weekend 2's centerpiece, and it's conceptually simple: a loop that generates a random operation, applies it, and calls `check_invariants` — repeated a million times.

**The generator needs to be _realistically_ random, not uniformly random.** Three details make the difference between a harness that finds bugs and one that doesn't. First, generate prices in a narrow band around the current mid (say ±20 ticks) — uniform prices across the whole `uint32` range never cross the spread, so your matching code never executes and is never tested. Second, weight the operation mix like real markets: mostly adds and cancels, so books build depth and levels empty out (level deletion is a classic bug site). Third — the crucial trick — **when generating cancels, pick IDs from a list of orders you know are resting**, which you can track because your harness sees every add and every trade report. Sprinkle in some known-dead IDs deliberately to exercise the reject path. A generator that cancels random `uint64`s tests nothing but rejection.

**Seed it, and print the seed.** Use a seeded PRNG (`std::mt19937_64{seed}`), print the seed at start, and accept it as a command-line argument. Now every failure is reproducible: "seed 8675309 violates the level-total invariant at operation 412,338" reruns identically every time, and you can bisect. An unreproducible random failure is nearly worthless; a reproducible one is a solved bug in waiting. When a seed does find a bug, hand-shrink it into a small unit test and add it to layer 1 permanently — that's your regression corpus growing organically.

**A second oracle, free of charge: your harness's own bookkeeping.** Since the harness tracks which orders it believes are resting and with what remaining quantity (it watches all the trade reports), it can cross-check the book against its _independent_ model every N operations: same set of IDs, same quantities, same best bid/ask. Two implementations agreeing by accident is much less likely than one implementation being wrong.

## The payoff structure: differential testing in weekend 4

Here's why building this properly is an investment, not overhead. When you write the optimized v2 book, you change _nothing_ in the harness — you run the same seeds through v1 and v2 and assert they produce **identical trade reports and identical final books**. Your slow, well-tested naive implementation becomes the reference oracle for the fast one. Every gnarly pointer bug in the intrusive-list version shows up as "v2 diverged from v1 at operation 77,102, seed 42" — localized, reproducible, and diagnosed against a known-good answer. This technique is called differential testing, it's exactly how real matching engines and compilers are validated, and "I differentially tested my optimized book against my reference implementation across millions of seeded random operations" is a sentence that lands very well in a trading-firm interview.

**Build order for your next session:** wire Catch2 in via vcpkg + CTest, transcribe the paper scenarios as unit tests (watch the partial-fill one catch your current `fill` bug as its first act), then write `check_invariants`, then the seeded generator loop. Roughly a day of work, and everything you build for the rest of the project sits on top of it.

One more habit worth stating because it's your brand already: when the harness catches something, write the postmortem in your notes — symptom, seed, root cause, fix, which invariant caught it. By project's end you'll have a debugging log that reads exactly like your ACIC technical reports, and _that_ document, linked from the README, is as impressive to a hiring manager as the benchmark numbers.

## System Tests Considerations

1. Track max tombstone count as a harness statistic

- A tombstone(_cancelled order in the queue_) deep in a `PriceLevel` lingers until everything ahead of it trades or the level drains

2.  Benchmark scan-cancel vs. tombstone-cancel vs. unlink-cancel, three designs with measured trade-offs,
