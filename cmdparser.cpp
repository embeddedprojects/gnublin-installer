#include "cmdparser.h"

CommandParser::CommandParser(int argc, char* argv[]) {
  for(int i = 0; i < argc; i++) {
    cmdline.push_back(string(argv[i]));
  }
}


string CommandParser::getParameter(string keyword, string keyword_s) {
  for(unsigned int i = 0; i < cmdline.size(); i++) {
    if(cmdline[i].compare("--" + keyword) == 0 || cmdline[i].compare("-" + keyword_s) == 0) {
      if(i + 1 < cmdline.size()) {
        return cmdline[i+1];
      } else {
        return "";
      }
    }
  }
  return "";
}

bool CommandParser::hasFlag(string flag, string flag_s) {
  for(unsigned int i = 0; i < cmdline.size(); i++) {
    if(cmdline[i].compare("--" + flag) == 0 || cmdline[i].compare("-" + flag_s) == 0) {
      return true;
    }
  }
  return false;
}
