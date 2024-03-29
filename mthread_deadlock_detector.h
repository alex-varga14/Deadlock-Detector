#pragma once
#include <string>
#include <vector>

struct Result {
    int edge_index;
    std::vector<std::string> dl_procs;
};

Result detect_deadlock(const std::vector<std::string> & edges);
