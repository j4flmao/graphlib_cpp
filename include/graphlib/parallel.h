#ifndef GRAPHLIB_PARALLEL_H
#define GRAPHLIB_PARALLEL_H

#include "export.h"
#include "graph_core.h"
#include "sparse_graph.h"
#include <vector>
#include <functional>

#include <thread>
#include <vector>
#include <algorithm>
#include <mutex>

namespace graphlib {
namespace parallel {

/**
 * @brief Execution policy for parallel algorithms.
 */
enum class ExecutionPolicy {
    Sequential,    // Single-threaded execution
    Parallel       // Multi-threaded execution (uses std::thread or OpenMP if available)
};

/**
 * @brief Configuration for parallel execution.
 */
struct ParallelConfig {
    int num_threads = 0;        // 0 = auto (use hardware concurrency)
    int grain_size = 1024;      // Minimum work per thread
    bool enable_stealing = true; // Work stealing between threads
};

/**
 * @brief Sets global parallel configuration.
 */
GRAPHLIB_API void set_parallel_config(ParallelConfig config);

/**
 * @brief Gets current number of threads being used.
 */
GRAPHLIB_API int get_num_threads();

namespace detail {
    // Helper to get global config (internal usage)
    GRAPHLIB_API ParallelConfig& get_global_config();

    template<typename Func>
    void parallel_for(int start, int end, Func func, ExecutionPolicy policy) {
        ParallelConfig& config = get_global_config();
        
        if (policy == ExecutionPolicy::Sequential || end - start <= config.grain_size) {
            for (int i = start; i < end; ++i) {
                func(i);
            }
            return;
        }
        
        int num_threads = get_num_threads();
        int chunk_size = (end - start + num_threads - 1) / num_threads;
        
        std::vector<std::thread> threads;
        for (int t = 0; t < num_threads; ++t) {
            int chunk_start = start + t * chunk_size;
            int chunk_end = std::min(chunk_start + chunk_size, end);
            
            if (chunk_start >= end) break;
            
            threads.emplace_back([chunk_start, chunk_end, &func]() {
                for (int i = chunk_start; i < chunk_end; ++i) {
                    func(i);
                }
            });
        }
        
        for (auto& t : threads) {
            t.join();
        }
    }
}


// ============== Parallel Graph Algorithms ==============

/**
 * @brief Parallel BFS from a source vertex.
 * 
 * Uses frontier-based parallelism.
 * 
 * @param g The graph (CSR format recommended for performance).
 * @param source Source vertex.
 * @param policy Execution policy.
 * @return std::vector<int> Distance from source to each vertex (-1 if unreachable).
 */
GRAPHLIB_API std::vector<int> parallel_bfs(const Graph& g, int source, 
                                            ExecutionPolicy policy = ExecutionPolicy::Parallel);

GRAPHLIB_API std::vector<int> parallel_bfs(const CSRGraph& g, int source,
                                            ExecutionPolicy policy = ExecutionPolicy::Parallel);

/**
 * @brief Parallel connected components using Shiloach-Vishkin algorithm.
 * 
 * @param g The graph.
 * @param component Output: component[v] = component ID.
 * @param policy Execution policy.
 * @return int Number of connected components.
 */
GRAPHLIB_API int parallel_connected_components(const Graph& g, std::vector<int>& component,
                                                ExecutionPolicy policy = ExecutionPolicy::Parallel);

GRAPHLIB_API int parallel_connected_components(const CSRGraph& g, std::vector<int>& component,
                                                ExecutionPolicy policy = ExecutionPolicy::Parallel);

/**
 * @brief Parallel PageRank computation.
 * 
 * @param g The graph.
 * @param d Damping factor.
 * @param iterations Number of iterations.
 * @param tolerance Convergence tolerance.
 * @param policy Execution policy.
 * @return std::vector<double> PageRank scores.
 */
GRAPHLIB_API std::vector<double> parallel_pagerank(const Graph& g, double d = 0.85,
                                                    int iterations = 100, double tolerance = 1e-6,
                                                    ExecutionPolicy policy = ExecutionPolicy::Parallel);

/**
 * @brief Parallel triangle counting.
 * 
 * @param g The graph.
 * @param policy Execution policy.
 * @return long long Number of triangles.
 */
GRAPHLIB_API long long parallel_triangle_count(const Graph& g,
                                                ExecutionPolicy policy = ExecutionPolicy::Parallel);

GRAPHLIB_API long long parallel_triangle_count(const CSRGraph& g,
                                                ExecutionPolicy policy = ExecutionPolicy::Parallel);

/**
 * @brief Parallel single-source shortest paths using Delta-Stepping.
 * 
 * Delta-Stepping is a parallel-friendly SSSP algorithm.
 * Works well for graphs with small positive integer weights.
 * 
 * @param g The graph.
 * @param source Source vertex.
 * @param delta Bucket width (auto-tuned if 0).
 * @param policy Execution policy.
 * @return std::vector<long long> Distances from source.
 */
GRAPHLIB_API std::vector<long long> parallel_sssp_delta_stepping(const Graph& g, int source,
                                                                  long long delta = 0,
                                                                  ExecutionPolicy policy = ExecutionPolicy::Parallel);

/**
 * @brief Parallel betweenness centrality using Brandes algorithm.
 * 
 * @param g The graph.
 * @param policy Execution policy.
 * @return std::vector<double> Betweenness centrality for each vertex.
 */
GRAPHLIB_API std::vector<double> parallel_betweenness_centrality(const Graph& g,
                                                                  ExecutionPolicy policy = ExecutionPolicy::Parallel);

/**
 * @brief Parallel label propagation community detection.
 * 
 * @param g The graph.
 * @param max_iterations Maximum iterations.
 * @param policy Execution policy.
 * @return std::vector<int> Community assignment.
 */
GRAPHLIB_API std::vector<int> parallel_label_propagation(const Graph& g, int max_iterations = 100,
                                                          ExecutionPolicy policy = ExecutionPolicy::Parallel);

/**
 * @brief Parallel k-core decomposition.
 * 
 * @param g The graph.
 * @param policy Execution policy.
 * @return std::vector<int> Core number for each vertex.
 */
GRAPHLIB_API std::vector<int> parallel_kcore(const Graph& g,
                                              ExecutionPolicy policy = ExecutionPolicy::Parallel);

/**
 * @brief Parallel graph contraction.
 * 
 * Contracts vertices based on a mapping.
 * 
 * @param g The graph.
 * @param mapping mapping[v] = new vertex ID.
 * @param new_n Number of vertices in contracted graph.
 * @param policy Execution policy.
 * @return Graph Contracted graph.
 */
GRAPHLIB_API Graph parallel_contract(const Graph& g, const std::vector<int>& mapping, int new_n,
                                      ExecutionPolicy policy = ExecutionPolicy::Parallel);

// ============== Parallel Utilities ==============

/**
 * @brief Parallel prefix sum (scan).
 */
GRAPHLIB_API void parallel_prefix_sum(std::vector<long long>& arr,
                                       ExecutionPolicy policy = ExecutionPolicy::Parallel);

/**
 * @brief Parallel for-each over vertices.
 */
GRAPHLIB_API void parallel_for_vertices(const Graph& g, std::function<void(int)> func,
                                         ExecutionPolicy policy = ExecutionPolicy::Parallel);

/**
 * @brief Parallel for-each over edges.
 */
GRAPHLIB_API void parallel_for_edges(const Graph& g, std::function<void(int, int, long long)> func,
                                      ExecutionPolicy policy = ExecutionPolicy::Parallel);

/**
 * @brief Parallel reduction over vertices.
 */
template<typename T>
T parallel_reduce_vertices(const Graph& g, std::function<T(int)> map_func,
                                         std::function<T(T, T)> reduce_func, T identity,
                                         ExecutionPolicy policy = ExecutionPolicy::Parallel) {
    int n = g.vertex_count();
    
    if (policy == ExecutionPolicy::Sequential) {
        T result = identity;
        for (int i = 0; i < n; ++i) {
            result = reduce_func(result, map_func(i));
        }
        return result;
    }
    
    int num_threads = get_num_threads();
    std::vector<T> results(num_threads, identity);
    
    // We can't use parallel_for directly easily because we need thread ID or return value.
    // So we invoke detail::parallel_for with a wrapper or custom logic.
    // For simplicity, we use the same chunking logic as parallel_for here manually or adapt.
    // Actually detail::parallel_for doesn't give thread index.
    // Let's reimplement small chunk logic here for reduction.
    
    int chunk_size = (n + num_threads - 1) / num_threads;
    std::vector<std::thread> threads;
    
    for (int t = 0; t < num_threads; ++t) {
        int start = t * chunk_size;
        int end = std::min(start + chunk_size, n);
        
        if (start >= n) break;
        
        threads.emplace_back([start, end, t, &results, &map_func, &reduce_func, identity]() {
            T local_res = identity;
            for (int i = start; i < end; ++i) {
                local_res = reduce_func(local_res, map_func(i));
            }
            results[t] = local_res;
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    T final_result = identity;
    for (const auto& val : results) {
        final_result = reduce_func(final_result, val);
    }
    
    return final_result;
}

/**
 * @brief Finds a Maximal Independent Set (MIS) in parallel.
 * Luby's Algorithm.
 */
GRAPHLIB_API std::vector<int> parallel_maximal_independent_set(const Graph& g, 
                                                                ExecutionPolicy policy = ExecutionPolicy::Parallel);

/**
 * @brief Coloring using Jones-Plassmann algorithm.
 * 
 * @param g Graph
 * @param policy Execution Policy
 * @return vector of colors
 */
GRAPHLIB_API std::vector<int> parallel_coloring(const Graph& g, 
                                                 ExecutionPolicy policy = ExecutionPolicy::Parallel);


} // namespace parallel
} // namespace graphlib

#endif
