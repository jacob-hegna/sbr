#include "ins.h"

Ins::Ins() {
    ins_type = InsType::INS;
}
Ins::Ins(uint64_t loc, uint8_t size, InsType ins_type) {
    init(loc, size, ins_type);
}

void Ins::init(uint64_t loc, uint8_t size, InsType ins_type) {
    this->loc      = loc;
    this->size     = size;
    this->ins_type = ins_type;
}
void Ins::set_size(uint8_t size) {
    this->size = size;
}
void Ins::set_loc(uint64_t loc) {
    this->loc = loc;
}
void Ins::set_type(InsType ins_type) {
    this->ins_type = ins_type;
}
void Ins::set_data(uint64_t data) {
    this->data = data;
}

uint8_t Ins::get_size() {
    return size;
}
uint64_t Ins::get_loc() {
    return loc;
}
InsType Ins::get_ins_type() {
    return ins_type;
}
uint64_t Ins::get_data() {
    return data;
}