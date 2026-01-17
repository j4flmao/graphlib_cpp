#include <graphlib/graphlib.h>
#include <iostream>

int main() {
    graphlib::SplayTree tree;

    tree.insert(5);
    tree.insert(3);
    tree.insert(7);
    tree.insert(1);
    tree.insert(9);

    bool found = tree.search(5);
    std::cout << "Found 5: " << (found ? "true" : "false") << std::endl;

    int sum = tree.range_sum(3, 8);
    std::cout << "Range sum [3, 8]: " << sum << std::endl;

    tree.remove(3);
    sum = tree.range_sum(1, 9);
    std::cout << "Range sum [1, 9] after remove: " << sum << std::endl;

    return 0;
}

