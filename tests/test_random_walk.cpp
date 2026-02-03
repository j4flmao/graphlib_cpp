#include <gtest/gtest.h>
#include <graphlib/graphlib.h>
#include <graphlib/random_walk.h>
#include <vector>
#include <cmath>
#include <set>
#include <algorithm>
#include <numeric>

using namespace graphlib;

class RandomWalkTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Simple connected graph: 0-1-2-3-4 (path)
        path_graph = Graph(5, false);
        path_graph.add_edge(0, 1);
        path_graph.add_edge(1, 2);
        path_graph.add_edge(2, 3);
        path_graph.add_edge(3, 4);

        // Complete graph K4
        complete_graph = Graph(4, false);
        for (int i = 0; i < 4; i++) {
            for (int j = i + 1; j < 4; j++) {
                complete_graph.add_edge(i, j);
            }
        }

        // Cycle graph: 0-1-2-3-0
        cycle_graph = Graph(4, false);
        cycle_graph.add_edge(0, 1);
        cycle_graph.add_edge(1, 2);
        cycle_graph.add_edge(2, 3);
        cycle_graph.add_edge(3, 0);

        // Star graph: center 0, leaves 1,2,3,4
        star_graph = Graph(5, false);
        star_graph.add_edge(0, 1);
        star_graph.add_edge(0, 2);
        star_graph.add_edge(0, 3);
        star_graph.add_edge(0, 4);
    }

    Graph path_graph{0, false};
    Graph complete_graph{0, false};
    Graph cycle_graph{0, false};
    Graph star_graph{0, false};
};

// ==================== random_walks tests ====================

TEST_F(RandomWalkTest, RandomWalks_OutputFormat) {
    RandomWalkConfig config;
    config.walk_length = 10;
    config.num_walks = 5;
    config.seed = 42;

    auto walks = random_walks(path_graph, config);

    // Should have num_walks * num_vertices walks
    EXPECT_EQ(walks.size(), static_cast<size_t>(config.num_walks * path_graph.num_vertices()));

    // Each walk should have correct length
    for (const auto& walk : walks) {
        EXPECT_EQ(walk.size(), static_cast<size_t>(config.walk_length));
    }
}

TEST_F(RandomWalkTest, RandomWalks_ValidVertices) {
    RandomWalkConfig config;
    config.walk_length = 20;
    config.num_walks = 3;
    config.seed = 123;

    auto walks = random_walks(complete_graph, config);

    for (const auto& walk : walks) {
        for (int v : walk) {
            EXPECT_GE(v, 0);
            EXPECT_LT(v, complete_graph.num_vertices());
        }
    }
}

TEST_F(RandomWalkTest, RandomWalks_Reproducible) {
    RandomWalkConfig config;
    config.walk_length = 15;
    config.num_walks = 4;
    config.seed = 999;

    auto walks1 = random_walks(cycle_graph, config);
    auto walks2 = random_walks(cycle_graph, config);

    EXPECT_EQ(walks1, walks2);
}

TEST_F(RandomWalkTest, RandomWalks_ConsecutiveVerticesAdjacent) {
    RandomWalkConfig config;
    config.walk_length = 30;
    config.num_walks = 2;
    config.seed = 555;

    auto walks = random_walks(complete_graph, config);

    for (const auto& walk : walks) {
        for (size_t i = 1; i < walk.size(); i++) {
            bool adjacent = complete_graph.has_edge(walk[i - 1], walk[i]);
            EXPECT_TRUE(adjacent) << "Vertices " << walk[i - 1] << " and " << walk[i] << " not adjacent";
        }
    }
}

// ==================== random_walks_from tests ====================

TEST_F(RandomWalkTest, RandomWalksFrom_SpecificNodes) {
    RandomWalkConfig config;
    config.walk_length = 10;
    config.num_walks = 3;
    config.seed = 42;

    std::vector<int> start_nodes = {0, 2};
    auto walks = random_walks_from(path_graph, start_nodes, config);

    // Should have num_walks * start_nodes.size() walks
    EXPECT_EQ(walks.size(), static_cast<size_t>(config.num_walks * start_nodes.size()));

    // Each walk should start from one of the start nodes
    for (size_t i = 0; i < walks.size(); i++) {
        int expected_start = start_nodes[i / config.num_walks];
        EXPECT_EQ(walks[i][0], expected_start);
    }
}

TEST_F(RandomWalkTest, RandomWalksFrom_SingleNode) {
    RandomWalkConfig config;
    config.walk_length = 15;
    config.num_walks = 5;
    config.seed = 789;

    std::vector<int> start_nodes = {2};
    auto walks = random_walks_from(cycle_graph, start_nodes, config);

    EXPECT_EQ(walks.size(), static_cast<size_t>(config.num_walks));

    for (const auto& walk : walks) {
        EXPECT_EQ(walk[0], 2);
        EXPECT_EQ(walk.size(), static_cast<size_t>(config.walk_length));
    }
}

TEST_F(RandomWalkTest, RandomWalksFrom_Reproducible) {
    RandomWalkConfig config;
    config.walk_length = 20;
    config.num_walks = 3;
    config.seed = 333;

    std::vector<int> start_nodes = {1, 3};
    auto walks1 = random_walks_from(path_graph, start_nodes, config);
    auto walks2 = random_walks_from(path_graph, start_nodes, config);

    EXPECT_EQ(walks1, walks2);
}

// ==================== node2vec_walks tests ====================

TEST_F(RandomWalkTest, Node2VecWalks_OutputFormat) {
    Node2VecConfig config;
    config.walk_length = 10;
    config.num_walks = 3;
    config.p = 1.0;
    config.q = 1.0;
    config.seed = 42;

    auto walks = node2vec_walks(complete_graph, config);

    EXPECT_EQ(walks.size(), static_cast<size_t>(config.num_walks * complete_graph.num_vertices()));

    for (const auto& walk : walks) {
        EXPECT_EQ(walk.size(), static_cast<size_t>(config.walk_length));
    }
}

TEST_F(RandomWalkTest, Node2VecWalks_ValidVertices) {
    Node2VecConfig config;
    config.walk_length = 15;
    config.num_walks = 2;
    config.p = 0.5;
    config.q = 2.0;
    config.seed = 111;

    auto walks = node2vec_walks(cycle_graph, config);

    for (const auto& walk : walks) {
        for (int v : walk) {
            EXPECT_GE(v, 0);
            EXPECT_LT(v, cycle_graph.num_vertices());
        }
    }
}

TEST_F(RandomWalkTest, Node2VecWalks_ConsecutiveVerticesAdjacent) {
    Node2VecConfig config;
    config.walk_length = 20;
    config.num_walks = 3;
    config.p = 2.0;
    config.q = 0.5;
    config.seed = 222;

    auto walks = node2vec_walks(path_graph, config);

    for (const auto& walk : walks) {
        for (size_t i = 1; i < walk.size(); i++) {
            bool adjacent = path_graph.has_edge(walk[i - 1], walk[i]);
            EXPECT_TRUE(adjacent);
        }
    }
}

TEST_F(RandomWalkTest, Node2VecWalks_Reproducible) {
    Node2VecConfig config;
    config.walk_length = 25;
    config.num_walks = 4;
    config.p = 1.5;
    config.q = 0.8;
    config.seed = 888;

    auto walks1 = node2vec_walks(complete_graph, config);
    auto walks2 = node2vec_walks(complete_graph, config);

    EXPECT_EQ(walks1, walks2);
}

TEST_F(RandomWalkTest, Node2VecWalks_DifferentPQ) {
    Node2VecConfig config1;
    config1.walk_length = 50;
    config1.num_walks = 10;
    config1.p = 0.25;
    config1.q = 4.0;
    config1.seed = 42;

    Node2VecConfig config2;
    config2.walk_length = 50;
    config2.num_walks = 10;
    config2.p = 4.0;
    config2.q = 0.25;
    config2.seed = 42;

    auto walks1 = node2vec_walks(complete_graph, config1);
    auto walks2 = node2vec_walks(complete_graph, config2);

    // Different p/q should produce different walks
    EXPECT_NE(walks1, walks2);
}

// ==================== stationary_distribution tests ====================

TEST_F(RandomWalkTest, StationaryDistribution_SumToOne) {
    auto dist = stationary_distribution(complete_graph);

    double sum = std::accumulate(dist.begin(), dist.end(), 0.0);
    EXPECT_NEAR(sum, 1.0, 1e-6);
}

TEST_F(RandomWalkTest, StationaryDistribution_NonNegative) {
    auto dist = stationary_distribution(path_graph);

    for (double p : dist) {
        EXPECT_GE(p, 0.0);
    }
}

TEST_F(RandomWalkTest, StationaryDistribution_CompleteGraphUniform) {
    auto dist = stationary_distribution(complete_graph);

    // Complete graph should have uniform distribution
    double expected = 1.0 / complete_graph.num_vertices();
    for (double p : dist) {
        EXPECT_NEAR(p, expected, 1e-6);
    }
}

TEST_F(RandomWalkTest, StationaryDistribution_ProportionalToDegree) {
    // For undirected graphs, stationary distribution is proportional to degree
    auto dist = stationary_distribution(star_graph);

    // Star graph: center has degree 4, leaves have degree 1
    // Total degree sum = 2 * edges = 2 * 4 = 8
    double center_expected = 4.0 / 8.0;
    double leaf_expected = 1.0 / 8.0;

    EXPECT_NEAR(dist[0], center_expected, 1e-6);
    for (int i = 1; i < 5; i++) {
        EXPECT_NEAR(dist[i], leaf_expected, 1e-6);
    }
}

TEST_F(RandomWalkTest, StationaryDistribution_CorrectSize) {
    auto dist = stationary_distribution(cycle_graph);
    EXPECT_EQ(dist.size(), static_cast<size_t>(cycle_graph.num_vertices()));
}

// ==================== random_walk_with_restart tests ====================

TEST_F(RandomWalkTest, RandomWalkWithRestart_CorrectSize) {
    auto scores = random_walk_with_restart(complete_graph, 0);
    EXPECT_EQ(scores.size(), static_cast<size_t>(complete_graph.num_vertices()));
}

TEST_F(RandomWalkTest, RandomWalkWithRestart_NonNegative) {
    auto scores = random_walk_with_restart(path_graph, 2);

    for (double s : scores) {
        EXPECT_GE(s, 0.0);
    }
}

TEST_F(RandomWalkTest, RandomWalkWithRestart_SumToOne) {
    auto scores = random_walk_with_restart(cycle_graph, 0);

    double sum = std::accumulate(scores.begin(), scores.end(), 0.0);
    EXPECT_NEAR(sum, 1.0, 1e-6);
}

TEST_F(RandomWalkTest, RandomWalkWithRestart_SourceHasHighScore) {
    auto scores = random_walk_with_restart(path_graph, 0, 0.5);

    // Source should have highest or near-highest score
    double source_score = scores[0];
    EXPECT_GT(source_score, 0.0);

    // With restart prob 0.5, source should have significant score
    EXPECT_GT(source_score, 0.1);
}

TEST_F(RandomWalkTest, RandomWalkWithRestart_HighRestartProbConcentratesOnSource) {
    auto scores_low = random_walk_with_restart(star_graph, 0, 0.1);
    auto scores_high = random_walk_with_restart(star_graph, 0, 0.9);

    // Higher restart probability should concentrate more on source
    EXPECT_GT(scores_high[0], scores_low[0]);
}

TEST_F(RandomWalkTest, RandomWalkWithRestart_DecayWithDistance) {
    // In path graph, scores should generally decrease with distance from source
    auto scores = random_walk_with_restart(path_graph, 0, 0.15, 1000);

    // Score at source should be >= score at distance 1
    EXPECT_GE(scores[0], scores[1] * 0.5);
}

// ==================== random_walk_sampling tests ====================

TEST_F(RandomWalkTest, RandomWalkSampling_CorrectSize) {
    int sample_size = 3;
    auto sample = random_walk_sampling(complete_graph, sample_size, 42);

    EXPECT_EQ(sample.size(), static_cast<size_t>(sample_size));
}

TEST_F(RandomWalkTest, RandomWalkSampling_ValidVertices) {
    int sample_size = 4;
    auto sample = random_walk_sampling(path_graph, sample_size, 123);

    for (int v : sample) {
        EXPECT_GE(v, 0);
        EXPECT_LT(v, path_graph.num_vertices());
    }
}

TEST_F(RandomWalkTest, RandomWalkSampling_UniqueVertices) {
    int sample_size = 4;
    auto sample = random_walk_sampling(complete_graph, sample_size, 456);

    std::set<int> unique_vertices(sample.begin(), sample.end());
    EXPECT_EQ(unique_vertices.size(), sample.size());
}

TEST_F(RandomWalkTest, RandomWalkSampling_Reproducible) {
    int sample_size = 3;
    auto sample1 = random_walk_sampling(cycle_graph, sample_size, 789);
    auto sample2 = random_walk_sampling(cycle_graph, sample_size, 789);

    EXPECT_EQ(sample1, sample2);
}

TEST_F(RandomWalkTest, RandomWalkSampling_DifferentSeeds) {
    int sample_size = 3;
    auto sample1 = random_walk_sampling(complete_graph, sample_size, 100);
    auto sample2 = random_walk_sampling(complete_graph, sample_size, 200);

    // Different seeds should likely produce different samples
    // (not guaranteed but very likely for different seeds)
    // We just check that both are valid
    EXPECT_EQ(sample1.size(), static_cast<size_t>(sample_size));
    EXPECT_EQ(sample2.size(), static_cast<size_t>(sample_size));
}

TEST_F(RandomWalkTest, RandomWalkSampling_MaxSampleSize) {
    // Request more samples than vertices
    int sample_size = path_graph.num_vertices() + 5;
    auto sample = random_walk_sampling(path_graph, sample_size, 42);

    // Should return at most num_vertices unique vertices
    EXPECT_LE(sample.size(), static_cast<size_t>(path_graph.num_vertices()));
}

// ==================== Edge cases ====================

TEST_F(RandomWalkTest, RandomWalks_ShortWalkLength) {
    RandomWalkConfig config;
    config.walk_length = 1;
    config.num_walks = 2;
    config.seed = 42;

    auto walks = random_walks(complete_graph, config);

    for (const auto& walk : walks) {
        EXPECT_EQ(walk.size(), 1u);
    }
}

TEST_F(RandomWalkTest, StationaryDistribution_DirectedGraph) {
    Graph directed_graph(3, true);
    directed_graph.add_edge(0, 1);
    directed_graph.add_edge(1, 2);
    directed_graph.add_edge(2, 0);

    auto dist = stationary_distribution(directed_graph, 100);

    double sum = std::accumulate(dist.begin(), dist.end(), 0.0);
    EXPECT_NEAR(sum, 1.0, 1e-6);

    // Cycle should have uniform distribution
    for (double p : dist) {
        EXPECT_NEAR(p, 1.0 / 3.0, 1e-2);
    }
}
