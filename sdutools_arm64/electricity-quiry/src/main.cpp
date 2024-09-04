#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>
#include <cstring>
#include <map>
#include <filesystem>
#include <chrono>
#include <thread>
#include <unistd.h>
#include "electricity-quiry.hpp"
#include "smtp.hpp"



bool init_config(std::map<std::string, std::string> &config, std::string config_file){
    init_quiry_config(config);
    init_smtp_config(config);

    std::filesystem::path path(config_file);
    std::filesystem::path dir = path.parent_path();
    std::filesystem::create_directory(dir);
    
    std::ofstream file(config_file);
    if (!file.is_open()) {
        std::cerr << "Unable to open create file: " << std::endl;
        return false;
    }
    for (const auto& pair : config) {
        file << pair.first << "=" << pair.second << '\n';
    }
    file.close();
    return true;

}

bool load_config(const std::string &filename, std::map<std::string, std::string> &config) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open configuration file: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            config[key] = value;
        }
    }

    file.close();
    return true;
}

int main(int argc, char* argv[]) {
    std::string config_file;
    std::map<std::string, std::string> config;
    std::string config_output_file = "/etc/sdutools/electricity-quiry/config.txt";
    int threshold = 10;

    int opt;
    while ((opt = getopt(argc, argv, "c:o:t:")) != -1) {
        switch (opt) {
            case 'c':
                config_file = optarg;
                if (!load_config(config_file, config)) {
                    return 1;
                }
                break;
            case 'o':
                config_output_file = optarg;
                break;
            case 't':
                threshold = std::stoi(optarg);
                break;
            default:
                std::cerr << "Usage: " << argv[0] << " [-c config_path] [-o output_config_path] [-t threshold_number]" << std::endl;
                return 0;
        }
    }

    if (config.empty()){
        std::cout << "配置文件为空，开始生成配置文件\n";
        init_config(config, config_output_file);
        std::cout << "配置完成！\n";
        return 1;
    }

    while(true){
        std::string balance_str = query(config["campus_card"], config["dormitory_building"], config["room_id"]);
        std::cout << "electricity quiry at: " << std::chrono::system_clock::now().time_since_epoch().count() <<", balance is : " << balance_str << std::endl;
        if (std::stoi(balance_str) < threshold){
            mail(config, balance_str);
        }

        auto interval = std::chrono::hours(24);
        std::this_thread::sleep_for(interval);
    }

    
    return 0;
}