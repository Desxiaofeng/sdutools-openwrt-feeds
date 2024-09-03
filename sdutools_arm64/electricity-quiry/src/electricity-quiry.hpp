#include <string>
#include <map>

bool init_quiry_config(std::map<std::string, std::string> &config);
std::string query(const std::string &account, const std::string &building, const std::string &room);