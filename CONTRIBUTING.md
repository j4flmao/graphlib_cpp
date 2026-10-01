# Contributing to GraphLib

## Commits

Commits use Conventional Commits:

```text
<type>(optional-scope): short imperative description
```

Examples: `fix(flow): reject equal source and sink`, `feat(mst): add arborescence oracle`,
`test(planarity): cover K3,3`.

Allowed types are `build`, `chore`, `ci`, `docs`, `feat`, `fix`, `perf`, `refactor`, `revert`,
and `test`. The commit subject must be non-empty and no longer than 100 characters.

## Local hooks

After installing Node.js development dependencies, run:

```bash
npm install
npm run prepare
```

Husky then runs `lint-staged` before commits and Commitlint for every commit message. The hooks
are checks only; they do not create commits or push branches.

## C++ verification

```bash
cmake --build build-werror --parallel 2
git diff --check
```
