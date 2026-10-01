# ADR-03: Empty input policy

Status: Accepted for v1.x

Zero-sized containers are valid. Negative sizes are invalid and throw. Algorithms return their
natural identity on empty input where one exists, such as zero flow, zero components, an empty
vector, or zero cost.
