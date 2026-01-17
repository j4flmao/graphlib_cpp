#ifndef GRAPHLIB_SCC_H
#define GRAPHLIB_SCC_H

#include "export.h"
#include "graph_core.h"
#include <vector>

namespace graphlib {

class GRAPHLIB_API SCC : public Graph {
public:
    explicit SCC(int n);

    int kosaraju(std::vector<int>& component);
    int tarjan(std::vector<int>& component);
};

class GRAPHLIB_API DynamicSCC : public SCC {
public:
    explicit DynamicSCC(int n);

    void add_edge(int from, int to, long long weight = 1);

    int component_count() const;
    const std::vector<int>& components() const;
    int component_id(int v) const;
    bool strongly_connected(int u, int v) const;

    void rebuild() const;

private:
    mutable bool dirty_;
    mutable std::vector<int> component_;
    mutable int comp_count_;

    void ensure_up_to_date() const;
};

class GRAPHLIB_API TwoSAT {
public:
    explicit TwoSAT(int variables);

    int variable_count() const;

    void add_clause(int x, bool x_is_true, int y, bool y_is_true);
    void add_unit_clause(int x, bool x_is_true);
    bool solve(std::vector<bool>& assignment);

private:
    struct Implication {
        int from;
        int to;
    };

    int n_;
    std::vector<Implication> implications_;

    int encode_literal(int var, bool value) const;
    void add_implication(int from, int to);
};

}

#endif
