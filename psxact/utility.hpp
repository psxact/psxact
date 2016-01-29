#ifndef __UTILITY_HPP__
#define __UTILITY_HPP__

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
}

#endif
