# ADR-02: Self-loops and parallel edges

Status: Accepted for v1.x

The base `Graph` container preserves self-loops and parallel edges. Algorithms must document
their interpretation: simple-graph algorithms deduplicate edges and ignore self-loops, while
multigraph algorithms preserve multiplicity. No algorithm may silently rely on a unique reverse
edge unless that precondition is checked.
