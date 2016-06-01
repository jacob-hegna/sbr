#include "bblock.h"


BBlock::BBlock() {
    fall   = nullptr;
    jmp    = nullptr;
    parent = nullptr;
}
BBlock::BBlock(uint64_t block_tag, uint64_t block_addr) : BBlock() {
    init(block_tag, block_addr);
}

void BBlock::init(uint64_t block_tag, uint64_t block_addr) {
    this->block_tag  = block_tag;
    this->block_addr = block_addr;
}

std::string BBlock::print_info() {
    std::stringstream ss;
    ss << "tag:\t" << std::hex << this->get_tag()  << std::endl
    << "jmp:\t"    << std::hex << this->get_jmp()  << std::endl
    << "fall:\t"   << std::hex << this->get_fall() << std::endl;

    return ss.str();
}

uint64_t BBlock::next() {
    return this->combined_h();
}

std::shared_ptr<Jmp> BBlock::get_last() {
    return std::static_pointer_cast<Jmp>(ins.back());
}

uint64_t BBlock::get_fall() {
    if(get_last() != nullptr) {
        return this->get_last()->get_loc()
            + this->get_last()->get_size();
    } else {
        return 0xFFFFFFFFFFFFFFFF;
    }
}

uint64_t BBlock::get_jmp() {
    if(get_last() != nullptr) {
        return this->get_last()->get_to();
    } else {
        return 0xFFFFFFFFFFFFFFFF;
    }
}

void BBlock::push_back(std::shared_ptr<Ins> i) {
    ins.push_back(i);
}

void BBlock::set_ins(std::vector<std::shared_ptr<Ins>> ins) {
    this->ins = ins;
}

uint64_t BBlock::get_loc() {
    return block_addr;
}

uint64_t BBlock::get_tag() {
    return block_tag;
}

std::vector<std::shared_ptr<Ins>> BBlock::get_ins() {
    return ins;
}