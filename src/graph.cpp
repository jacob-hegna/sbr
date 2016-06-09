#include "graph.h"

std::vector<Graph> make_graphs(vector_shared<BBlock> super_set,
                               std::vector<uint64_t> calls) {
    std::vector<Graph> graphs;
    std::vector<uint64_t> finished_calls;

    int a = 0;
    while(finished_calls.size() != calls.size()) {
        for(uint64_t call : calls) {
            if(std::find(finished_calls.begin(), finished_calls.end(), call)
                != finished_calls.end()) continue;
            if(Graph::good_call(super_set, call, finished_calls)) {
                Graph graph(super_set, call);
                if(graph.get_root() != nullptr)
                    graphs.push_back(graph);

                // add the call from the regular vector to the finished vector
                finished_calls.push_back(call);
            }
        }
        if(a++ > 10000) break; // because we are not testing full files, we will
                              // never finish every call
    }

    return graphs;
}

/*
 * determines whether a call contains any other calls nested within it
 * it ignore nested calls that are in the finished_calls vector
 */
bool Graph::good_call(vector_shared<BBlock> super_set, uint64_t addr,
                      const std::vector<uint64_t> finished_calls) {
    static std::vector<uint64_t> parsed_blocks;
    if(std::find(parsed_blocks.begin(), parsed_blocks.end(), addr) != parsed_blocks.end()) {
        return true;
    }
    parsed_blocks.push_back(addr);

    std::shared_ptr<BBlock> block = nullptr;
    for(std::shared_ptr<BBlock> i : super_set) {
        if(i->get_loc() == addr) {
            block = i;
            break;
        }
    }

    // we return true, because even though we cannot find a block matching the
    // addr, returning false creates an infinite loop - but returning true
    // creates the desired output
    if(block == nullptr) return true;

    // record how many calls are made inside the relevant call
    static int depth = 0;

    // if the call block has another call inside of it, it's "bad"
    for(std::shared_ptr<Ins> ins : block->get_ins()) {
        if(ins->get_ins_type() == InsType::CALL) {
            ++depth;
            bool finished = false;
            // ...unless that call and all sub-calls have already been accounted
            // for
            for(uint64_t finished_call : finished_calls) {
                if(addr == finished_call) {
                    finished = true;
                    break;
                }
            }
            if(!finished) return false;
        }
    }

    vector_shared<Ins> ins = block->get_ins();
    if(ins.size() >= 2) {
        if((ins.end()[-2])->get_ins_type() == InsType::RET) {

            if(depth == 0) {
                parsed_blocks.clear();
                return true;
            }
            --depth;
        }
    }

    bool good_jmp  = good_call(super_set, block->get_jmp(), finished_calls);
    bool good_fall = good_call(super_set, block->get_fall(), finished_calls);

    return good_jmp && good_fall;
}

Graph::Graph() {
    root = nullptr;
}

Graph::Graph(vector_shared<BBlock> super_set, uint64_t addr) {
    root = nullptr;
    init(super_set, addr);
}

/*
 * root initialization function
 * calls the recursive init()
 * public
 */
void Graph::init(vector_shared<BBlock> super_set, uint64_t leaf) {
    this->super_set = super_set;

    for(std::shared_ptr<BBlock> block : super_set) {
        if(block->get_loc() == leaf) root = block;
    }

    init(root);
}

/*
 * recursive initialization function
 * finds the stems of the tree
 * private
 */
void Graph::init(std::shared_ptr<BBlock> leaf) {
    if(leaf == nullptr) return;

    for(std::shared_ptr<BBlock> block : super_set) {
        if(block->get_loc() == leaf->get_fall()) {
            if(search(block->get_loc()) == nullptr) {
                leaf->fall = block;
                init(block);
            }
        }

        if(block->get_loc() == leaf->get_jmp()) {
            if(search(block->get_loc()) == nullptr) {
                leaf->jmp = block;
                init(block);
            }
        }
    }
}

void Graph::insert(std::shared_ptr<BBlock> parent, std::shared_ptr<BBlock> child,
                   bool is_jmp) {
    if(is_jmp) {
        parent->jmp = child;
    } else {
        parent->fall = child;
    }
}

std::shared_ptr<BBlock> Graph::search(uint64_t addr) {
    return search(addr, root);
}

std::shared_ptr<BBlock> Graph::search(uint64_t addr, std::shared_ptr<BBlock> leaf) {
    static vector_shared<BBlock> searched_nodes; // prevents infinite recursion
    if(std::find(searched_nodes.begin(), searched_nodes.end(), leaf)
         != searched_nodes.end()) {
        return nullptr;
    } else {
        searched_nodes.push_back(leaf);
    }
    if(leaf != nullptr) {
        if(leaf->get_loc() == addr) {
            searched_nodes.clear();
            return leaf;
        } else {
            std::shared_ptr<BBlock> ret = nullptr;
            if((ret = search(addr, leaf->fall)) != nullptr) {
                searched_nodes.clear();
                return ret;
            } else if((ret = search(addr, leaf->jmp)) != nullptr) {
                searched_nodes.clear();
                return ret;
            }
        }
    }
    searched_nodes.clear();
    return nullptr;
}

std::string Graph::print_info(std::shared_ptr<BBlock> leaf) {
    std::stringstream ss;

    if(leaf == nullptr) leaf = root;
    ss << leaf->print_info() << std::endl;

    if(leaf->jmp != nullptr)  ss << print_info(leaf->jmp);
    if(leaf->fall != nullptr) ss << print_info(leaf->fall);

    return ss.str();
}

std::shared_ptr<BBlock> Graph::get_root() {
    return root;
}
