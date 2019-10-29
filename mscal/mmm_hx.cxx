#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

static std::map<std::string, double> __context;

extern "C" {

#include "./mmm_h.h"
#define MAX_IDENTIFY_LENGTH 50

double* get_identify_ptr(char* id)
{
    size_t strl = ::strlen(id);
    if(strl > MAX_IDENTIFY_LENGTH)
        throw *new std::out_of_range("Identify length is too long.");
    if(__context.find(id) == __context.end()){
        __context[id] = 0;
    }
    return &__context[id];
}

void show_context()
{
    std::cout << "Context:  ------------------------------------" << std::endl;
    for(auto bi = __context.begin(); bi != __context.end(); ++bi)
        std::cout << bi->first << ": " << bi->second << std::endl;
    std::cout << std::endl;
    return;
}

}
