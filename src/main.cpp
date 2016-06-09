#include <iostream>
#include <string>

#include "vector_shared.h"
#include "bblock.h"
#include "parse.h"
#include "graph.h"

int main(int argc, char *argv[]) {
    std::string path;
    if(argc < 2) {
        std::cout << "Usage: " << argv[0] << " [filename]" << std::endl
                << "\tfilename - path to x86 binary file" << std::endl;
        return -1;
    }
    path = std::string(argv[1]);

    BlockFile block_file = parse_file(path);

    std::vector<Graph> graphs = make_graphs(block_file.blocks, block_file.calls);

    std::cout << graphs.at(15).print_info() << std::endl;

    return 0;
}