#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::cout << "Benchmark runner: running baseline comparison..." << std::endl;

    std::ifstream f("benchmark/data/sample_input.txt");
    if (!f) {
        std::cout << "No sample input file found; create benchmark/data/sample_input.txt" << std::endl;
        return 0;
    }

    std::string line;
    while (std::getline(f, line)) {
        std::cout << "input: " << line << std::endl;
    }

    // TODO: call baseline and datastructure implementations and measure runtime
    return 0;
}
