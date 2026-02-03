#include <gtest/gtest.h>
#include <graphlib/graph_core.h>
#include <graphlib/graph_io.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

using namespace graphlib;
namespace fs = std::filesystem;

class GraphIOTest : public ::testing::Test {
protected:
    fs::path temp_dir;
    
    void SetUp() override {
        temp_dir = fs::temp_directory_path() / "graphlib_test_io";
        fs::create_directories(temp_dir);
    }
    
    void TearDown() override {
        std::error_code ec;
        fs::remove_all(temp_dir, ec);
    }
    
    std::string read_file_content(const fs::path& path) {
        std::ifstream in(path);
        std::stringstream buffer;
        buffer << in.rdbuf();
        return buffer.str();
    }
};

TEST_F(GraphIOTest, SaveToDot_UndirectedGraph) {
    Graph g(4, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(0, 3);
    
    fs::path output_file = temp_dir / "undirected.dot";
    
    ASSERT_TRUE(save_to_dot(g, output_file.string()));
    ASSERT_TRUE(fs::exists(output_file));
    
    std::string content = read_file_content(output_file);
    
    EXPECT_NE(content.find("graph G {"), std::string::npos);
    EXPECT_NE(content.find("--"), std::string::npos);
    EXPECT_EQ(content.find("->"), std::string::npos);
    EXPECT_EQ(content.find("digraph"), std::string::npos);
    
    EXPECT_NE(content.find("0 [label=\"0\"]"), std::string::npos);
    EXPECT_NE(content.find("1 [label=\"1\"]"), std::string::npos);
    EXPECT_NE(content.find("2 [label=\"2\"]"), std::string::npos);
    EXPECT_NE(content.find("3 [label=\"3\"]"), std::string::npos);
    
    EXPECT_NE(content.find("}"), std::string::npos);
}

TEST_F(GraphIOTest, SaveToDot_DirectedGraph) {
    Graph g(3, true);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 0);
    
    fs::path output_file = temp_dir / "directed.dot";
    
    ASSERT_TRUE(save_to_dot(g, output_file.string()));
    ASSERT_TRUE(fs::exists(output_file));
    
    std::string content = read_file_content(output_file);
    
    EXPECT_NE(content.find("digraph G {"), std::string::npos);
    EXPECT_NE(content.find("->"), std::string::npos);
    EXPECT_EQ(content.find(" -- "), std::string::npos);
    
    EXPECT_NE(content.find("0 -> 1"), std::string::npos);
    EXPECT_NE(content.find("1 -> 2"), std::string::npos);
    EXPECT_NE(content.find("2 -> 0"), std::string::npos);
}

TEST_F(GraphIOTest, SaveToDotLabeled_CustomLabels) {
    Graph g(3, false);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    
    std::vector<std::string> labels = {"Alpha", "Beta", "Gamma"};
    fs::path output_file = temp_dir / "labeled.dot";
    
    ASSERT_TRUE(save_to_dot_labeled(g, output_file.string(), labels));
    ASSERT_TRUE(fs::exists(output_file));
    
    std::string content = read_file_content(output_file);
    
    EXPECT_NE(content.find("graph G {"), std::string::npos);
    EXPECT_NE(content.find("[label=\"Alpha\"]"), std::string::npos);
    EXPECT_NE(content.find("[label=\"Beta\"]"), std::string::npos);
    EXPECT_NE(content.find("[label=\"Gamma\"]"), std::string::npos);
}

TEST_F(GraphIOTest, SaveToDot_WeightedEdges) {
    Graph g(3, true);
    g.add_edge(0, 1, 5);
    g.add_edge(1, 2, 10);
    
    fs::path output_file = temp_dir / "weighted.dot";
    
    ASSERT_TRUE(save_to_dot(g, output_file.string()));
    ASSERT_TRUE(fs::exists(output_file));
    
    std::string content = read_file_content(output_file);
    
    EXPECT_NE(content.find("[label=\"5\"]"), std::string::npos);
    EXPECT_NE(content.find("[label=\"10\"]"), std::string::npos);
}

TEST_F(GraphIOTest, SaveToDot_EmptyGraph) {
    Graph g(0, false);
    
    fs::path output_file = temp_dir / "empty.dot";
    
    ASSERT_TRUE(save_to_dot(g, output_file.string()));
    ASSERT_TRUE(fs::exists(output_file));
    
    std::string content = read_file_content(output_file);
    
    EXPECT_NE(content.find("graph G {"), std::string::npos);
    EXPECT_NE(content.find("}"), std::string::npos);
}

TEST_F(GraphIOTest, SaveToDot_SingleVertex) {
    Graph g(1, false);
    
    fs::path output_file = temp_dir / "single.dot";
    
    ASSERT_TRUE(save_to_dot(g, output_file.string()));
    ASSERT_TRUE(fs::exists(output_file));
    
    std::string content = read_file_content(output_file);
    
    EXPECT_NE(content.find("0 [label=\"0\"]"), std::string::npos);
}

TEST_F(GraphIOTest, SaveToDot_InvalidPath) {
    Graph g(3, false);
    g.add_edge(0, 1);
    
    std::string invalid_path = (temp_dir / "nonexistent_dir" / "subdir" / "file.dot").string();
    
    EXPECT_FALSE(save_to_dot(g, invalid_path));
}

TEST_F(GraphIOTest, SaveToDotLabeled_PartialLabels) {
    Graph g(5, false);
    g.add_edge(0, 1);
    g.add_edge(2, 3);
    
    std::vector<std::string> labels = {"A", "B"};
    fs::path output_file = temp_dir / "partial_labels.dot";
    
    ASSERT_TRUE(save_to_dot_labeled(g, output_file.string(), labels));
    ASSERT_TRUE(fs::exists(output_file));
    
    std::string content = read_file_content(output_file);
    
    EXPECT_NE(content.find("[label=\"A\"]"), std::string::npos);
    EXPECT_NE(content.find("[label=\"B\"]"), std::string::npos);
    EXPECT_NE(content.find("[label=\"2\"]"), std::string::npos);
    EXPECT_NE(content.find("[label=\"3\"]"), std::string::npos);
    EXPECT_NE(content.find("[label=\"4\"]"), std::string::npos);
}
