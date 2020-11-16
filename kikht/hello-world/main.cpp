#include <iostream>

int main(int argc, char** argv) {
    std::cout << "Hello, ";
    if (argc > 1) {
        for (size_t i = 1; i < argc-1; i++) {
            std::cout << argv[1] << " ";
        }
        std::cout << argv[argc-1];
    } else {
        std::cout << "world";//esketit
    }
    std::cout << "!" << std::endl;
    return 0;//test1
}
