#ifndef STR_UTIL_H
#define STR_UTIL_H

#include <string>




namespace string
{
    struct tokenizer {
        char delim = ' ';
        size_t curr = 0;
        std::string s;

        tokenizer(std::string s);
        tokenizer(std::string s, char delim);

        std::string next_token();
    };
} // namespace string

#endif