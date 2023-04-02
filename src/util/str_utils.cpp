#include <util/str_utils.h>

string::tokenizer::tokenizer(std::string s) {
    this->s = s;
}

string::tokenizer::tokenizer(std::string s, char delim) {
    this->s = s;
    this->delim = delim;
}

std::string string::tokenizer::next_token() {
    size_t next = s.find(delim, curr);
    std::string output = s.substr(curr, next);
    curr = next + 1;
    return output;
}