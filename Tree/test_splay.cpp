#include <iostream>
#include <vector>
#include <string>
#include "SplayTree.hpp"

int main() {
    std::vector<int> init = {10, 20, 5, 15, 25, 3, 7, 12, 18, 30, 7, 7, 20};
    SPT<int> tree(init);

    std::cout << "Initial build done.\n";

    std::cout << "search(15): " << std::boolalpha << tree.search(15) << '\n';
    std::cout << "search(100): " << std::boolalpha << tree.search(100) << '\n';

    std::cout<<"current root: "<<tree.get_root()<<'\n';
    tree.insert(17);
    std::cout<<"current root: "<<tree.get_root()<<'\n';
    tree.insert(17);
    tree.insert(1);
    std::cout << "Inserted 17, 17, 1.\n";

    std::cout << "search(17): " << std::boolalpha << tree.search(17) << '\n';
    std::cout << "search(1): " << std::boolalpha << tree.search(1) << '\n';
    std::cout<<"current root: "<<tree.get_root()<<'\n';
    std::cout << "search(999): " << std::boolalpha << tree.search(999) << '\n';
    std::cout<<"current root: "<<tree.get_root()<<'\n';

    tree.remove(7);
    tree.remove(7);
    tree.remove(7);  // duplicate removals
    tree.remove(20);
    tree.remove(1000); // non-existing
    std::cout << "Removed 7 three times, 20 once, 1000 once.\n";

    std::cout << "search(7): " << std::boolalpha << tree.search(7) << '\n';
    std::cout << "search(20): " << std::boolalpha << tree.search(20) << '\n';
    std::cout << "search(17): " << std::boolalpha << tree.search(17) << '\n';

    std::cout << "All test operations finished.\n";
    return 0;
}
