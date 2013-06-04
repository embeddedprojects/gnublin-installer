#ifndef CMDPARSER_H_
#define CMDPARSER_H_

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class CommandParser {
  private:
    vector<string> cmdline;

  public:
    CommandParser(int argc, char* argv[]);
    string getParameter(string keyword, string keyword_s);
    bool hasFlag(string flag, string flag_s);
};


#endif

