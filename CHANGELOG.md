# Changelog

All notable changes to GraphLib are documented here.

## [Unreleased]

- Stabilize CMake target configuration and test discovery.
- Register every `tests/test_*.cpp` file automatically.
- Add bounded GoogleTest/CTest timeouts and cross-platform CI.
- Make warning-as-error checking opt-in for consumers and available in CI.
- Add contest algorithms for widest paths, shortest-path counting, bounded-hop paths, and
  lexicographically smallest/counting topological orders.
- Add functional-graph decomposition and graphical-sequence realization.
- Make `Graph` copyable with exception-safe deep-copy semantics and support `Graph(0)`.
- Add deterministic test seed and validator helpers.
- Add accepted API convention ADRs for directedness, empty inputs, loops/parallel edges, and
  error handling.
- Add Husky and Commitlint Conventional Commit hooks for local development.

## [1.0.4]

- Existing release baseline.
