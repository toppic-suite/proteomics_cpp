#ifndef PROT_STRING_UTIL_HPP_
#define PROT_STRING_UTIL_HPP_

#include <sstream>
#include <string>
#include <vector>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

namespace prot {

inline std::string& trim(std::string& s) {
  s.erase(s.begin(), 
          std::find_if(s.begin(), s.end(), 
                       std::not1(std::ptr_fun<int, int>(std::isspace))));
  s.erase(std::find_if(s.rbegin(), s.rend(),
                       std::not1(std::ptr_fun<int, int>(std::isspace))).base(), 
          s.end());
  return s;
}

inline std::vector<std::string>& split(const std::string &s, char delim, 
                                       std::vector<std::string> &elems) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}


inline std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, elems);
  return elems;
}

inline std::string basename(std::string& s) {
  unsigned int dot_pos = s.find(".");
  if (dot_pos < s.length()) {
    return s.substr(0, dot_pos);
  }
  return s;
}

}
#endif
