# ADR-01: Directedness contract

Status: Accepted for v1.x

The legacy boolean constructor remains source-compatible. Algorithms with an undirected
mathematical definition construct a symmetric simple view and ignore self-loops. New public
APIs must state their directedness requirement and reject unsupported representations rather
than silently returning a directed-only interpretation.
