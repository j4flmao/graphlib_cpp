# ADR-04: Error handling

Status: Accepted for v1.x

Exceptions are used for programmer/precondition errors: invalid sizes, invalid vertex IDs,
unsupported directedness, invalid capacities, and invalid source/sink pairs. Expected absence of
an answer keeps the legacy documented sentinel or empty result for compatibility. New APIs should
prefer an explicit result type or `std::optional` when introducing a new public contract.
