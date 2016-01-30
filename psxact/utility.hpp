#ifndef __utility_hpp__
#define __utility_hpp__

#include <fstream>
#include <string>
#include <vector>

namespace utility {
    using namespace std;

    int read_all_bytes(string filename, char*& result) {
        ifstream ifs(filename, ios::binary | ios::ate);
        int length = int(ifs.tellg());

        result = new char[length];
        ifs.seekg(0, ios::beg);
        ifs.read(result, length);

        return length;
    }

    template<unsigned min, unsigned max>
    inline bool between(unsigned value) {
        return (value & ~(min ^ max)) == min;
    }
}

#endif
