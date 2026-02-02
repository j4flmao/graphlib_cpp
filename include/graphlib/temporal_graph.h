#ifndef GRAPHLIB_TEMPORAL_GRAPH_H
#define GRAPHLIB_TEMPORAL_GRAPH_H

#include "export.h"
#include "graph_core.h"
#include <vector>
#include <tuple>
#include <utility>
#include <string>

namespace graphlib {

/**
 * @brief Represents a temporal (time-varying) graph.
 * 
 * Each edge has a timestamp or time interval.
 * Supports queries like:
 * - Snapshot at time t
 * - Edges active in time window [t1, t2]
 * - Temporal paths (paths respecting time ordering)
 */
class GRAPHLIB_API TemporalGraph {
public:
    /**
     * @brief A temporal edge with timestamp.
     */
    struct TemporalEdge {
        int from;
        int to;
        long long weight;
        long long timestamp;    // Point in time
        long long duration;     // Duration (0 for instantaneous)
        
        bool operator<(const TemporalEdge& other) const {
            return timestamp < other.timestamp;
        }
    };

private:
    int n_;
    std::vector<TemporalEdge> edges_;
    bool directed_;
    long long min_time_;
    long long max_time_;
    bool sorted_;

public:
    explicit TemporalGraph(int n, bool directed = true);
    
    /**
     * @brief Adds a temporal edge.
     * 
     * @param from Source vertex.
     * @param to Target vertex.
     * @param timestamp Time of the edge.
     * @param weight Edge weight.
     * @param duration Duration the edge is active (0 for instantaneous).
     */
    void add_edge(int from, int to, long long timestamp, long long weight = 1, long long duration = 0);
    
    /**
     * @brief Adds multiple temporal edges at once.
     */
    void add_edges(const std::vector<TemporalEdge>& edges);
    
    // Accessors
    int vertex_count() const { return n_; }
    int edge_count() const { return static_cast<int>(edges_.size()); }
    bool is_directed() const { return directed_; }
    long long min_timestamp() const { return min_time_; }
    long long max_timestamp() const { return max_time_; }
    const std::vector<TemporalEdge>& edges() const { return edges_; }
    
    /**
     * @brief Sorts edges by timestamp (required for some algorithms).
     */
    void sort_by_time();
    
    /**
     * @brief Gets a snapshot of the graph at time t.
     * 
     * Returns a static graph containing edges active at time t.
     * 
     * @param t The time point.
     * @return Graph Static graph snapshot.
     */
    Graph snapshot_at(long long t) const;
    
    /**
     * @brief Gets a snapshot for time window [t1, t2].
     * 
     * @param t1 Start time (inclusive).
     * @param t2 End time (inclusive).
     * @return Graph Static graph with edges in the window.
     */
    Graph snapshot_window(long long t1, long long t2) const;
    
    /**
     * @brief Gets edges in a time window.
     */
    std::vector<TemporalEdge> edges_in_window(long long t1, long long t2) const;
    
    /**
     * @brief Aggregates into a static weighted graph.
     * 
     * Weight = sum/count of edge occurrences.
     * 
     * @param aggregation "sum", "count", "mean", "max", "min"
     * @return Graph Aggregated static graph.
     */
    Graph aggregate(const std::string& aggregation = "sum") const;
    
    /**
     * @brief Finds temporal shortest path (earliest arrival).
     * 
     * A temporal path must respect time ordering: each edge's timestamp
     * must be >= the previous edge's timestamp.
     * 
     * @param source Source vertex.
     * @param start_time Earliest departure time.
     * @return std::vector<long long> Earliest arrival time at each vertex, or -1 if unreachable.
     */
    std::vector<long long> earliest_arrival(int source, long long start_time = 0) const;
    
    /**
     * @brief Finds latest departure times.
     * 
     * @param target Target vertex.
     * @param arrive_by Must arrive by this time.
     * @return std::vector<long long> Latest departure time from each vertex.
     */
    std::vector<long long> latest_departure(int target, long long arrive_by) const;
    
    /**
     * @brief Finds fastest temporal path (minimum travel time).
     * 
     * @param source Source vertex.
     * @param target Target vertex.
     * @param start_time Earliest departure time.
     * @return Pair of {travel_time, arrival_time}, or {-1, -1} if no path.
     */
    std::pair<long long, long long> fastest_path(int source, int target, long long start_time = 0) const;
    
    /**
     * @brief Finds shortest temporal path (minimum number of edges).
     * 
     * @param source Source vertex.
     * @param target Target vertex.
     * @param start_time Earliest departure time.
     * @return Number of edges in shortest temporal path, or -1 if no path.
     */
    int shortest_temporal_path(int source, int target, long long start_time = 0) const;
    
    /**
     * @brief Computes temporal reachability.
     * 
     * @param source Source vertex.
     * @param start_time Start time.
     * @return std::vector<bool> reachable[v] = true if v is reachable from source.
     */
    std::vector<bool> temporal_reachability(int source, long long start_time = 0) const;
    
    /**
     * @brief Computes temporal betweenness centrality.
     * 
     * @return std::vector<double> Temporal betweenness for each vertex.
     */
    std::vector<double> temporal_betweenness() const;
    
    /**
     * @brief Gets activity timeline for a vertex.
     * 
     * @param v Vertex.
     * @return std::vector<long long> Timestamps of edges incident to v.
     */
    std::vector<long long> vertex_activity(int v) const;
    
    /**
     * @brief Computes burstiness of vertex activity.
     * 
     * Burstiness B = (sigma - mu) / (sigma + mu) where sigma = std dev, mu = mean of inter-event times.
     * B in [-1, 1], B close to 1 = bursty, B close to -1 = regular.
     * 
     * @param v Vertex.
     * @return double Burstiness score.
     */
    double vertex_burstiness(int v) const;
    
    /**
     * @brief Creates time-aggregated graph for each time window.
     * 
     * @param window_size Size of each time window.
     * @return std::vector<Graph> Sequence of static graphs.
     */
    std::vector<Graph> sliding_windows(long long window_size) const;
};

/**
 * @brief Detects temporal strongly connected components.
 * 
 * Two vertices are temporally connected if there exist temporal paths
 * in both directions.
 * 
 * @param g The temporal graph.
 * @param component Output: component[v] = component ID of vertex v.
 * @return int Number of components.
 */
GRAPHLIB_API int temporal_scc(const TemporalGraph& g, std::vector<int>& component);

/**
 * @brief Finds temporal motifs (recurring temporal patterns).
 * 
 * @param g The temporal graph.
 * @param delta Maximum time difference between edges in a motif.
 * @return std::vector<std::vector<TemporalGraph::TemporalEdge>> List of motif instances.
 */
GRAPHLIB_API std::vector<std::vector<TemporalGraph::TemporalEdge>> temporal_motifs(
    const TemporalGraph& g, long long delta);

}

#endif
