#include <map>
#include <string>

bool init_smtp_config(std::map<std::string, std::string> &config);
bool mail(std::map<std::string, std::string> &config, std::string balance);