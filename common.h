#pragma once
#include <string>
#include <unordered_map>
#include <vector>

/// reads in a line from stdin
/// returns empty string on EOF
/// return string includes trailing '\n' if present
std::string stdin_readline();

/// splits string into tokens (words)
/// separators are sequences of white spaces
std::vector<std::string> split(const std::string & str);

/// joins a vector of strings into a single string, using a separator
std::string
join(const std::vector<std::string> & toks, const std::string & sep = " ");

/// removes leading & trailing white space, and replaces any consecutive
/// whitespaces with a single space ' '
std::string simplify(const std::string & str);

/// check if string is alphanumeric
bool is_alnum(const std::string & str);

class Word2Int {
    int counter = 0;
    std::unordered_map<std::string, int> myset;

public:
    int get(const std::string & w);
};

/// timer class for measuring elapsed time
struct Timer {
    // return elapsed time (in seconds) since last reset/or construction
    // reset_p = true will reset the time
    double elapsed(bool reset_p = false);
    // reset the time to 0
    void reset();
    Timer();
    ~Timer();

private:
    struct Pimpl;
    Pimpl * pimpl_;
};