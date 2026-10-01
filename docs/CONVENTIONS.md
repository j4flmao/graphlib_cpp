# GraphLib conventions

## Empty and invalid inputs

- `Graph(0)` and other zero-sized containers are valid values.
- Negative sizes throw `std::invalid_argument`.
- A vertex outside `[0, vertex_count())` throws `std::out_of_range` when the API has an
  explicit vertex argument.
- A flow source equal to its sink throws `std::invalid_argument`.
- An algorithm with no mathematical answer returns its documented sentinel (`-1`, `INF`, or
  an empty result) until a newer `std::optional` API is introduced.

## Directedness

The legacy `Graph(int, bool)` API remains supported. Algorithms that are defined on an
undirected simple graph build a deduplicated undirected view and ignore self-loops. New APIs
must document whether they accept directed input.

## Self-loops and parallel edges

The `Graph` container preserves self-loops and parallel edges. Individual algorithms either
ignore loops or deduplicate them; algorithms that require multiplicity must say so explicitly.

## Reproducibility

Randomized tests use `GRAPHLIB_TEST_SEED`; failures must print or preserve the seed needed to
reproduce the case.
