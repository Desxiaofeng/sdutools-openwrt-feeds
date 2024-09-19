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
    int threshold = 15;

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

    int mail_count = 2;
    while(true){
        std::string balance_str = query(config["campus_card"], config["dormitory_building"], config["room_id"]);
        if (balance_str == "" || (balance_str[0]>'9' || balance_str[0]<'0')){
            std::cout << "获取电费余额失败，5分钟后重试" << std::endl;
            std::cout << std::flush;
            auto interval = std::chrono::minutes(5);
            std::this_thread::sleep_for(interval);
            continue;
        }

        // 获取当前时间点（UTC时间）
        auto now = std::chrono::system_clock::now();
        // 将时间加上8小时，以调整到 UTC+8
        auto now_utc8 = now + std::chrono::hours(8);
        // 转换为 time_t 类型
        std::time_t current_time_utc8 = std::chrono::system_clock::to_time_t(now_utc8);
        // 格式化输出时间
        std::cout << "查询时间: "
                << std::put_time(std::gmtime(&current_time_utc8), "%Y-%m-%d %H:%M:%S") 
                << " UTC+8, 余额为: " << balance_str << std::endl;

        if (std::strtol(balance_str.c_str(), nullptr, 10) > threshold){
            mail_count = 3;
            std::cout << "sleeping 1 hour..." << std::endl;
            std::cout << std::flush;

            auto interval = std::chrono::hours(1);
            std::this_thread::sleep_for(interval);
        }else {
            // wait until morning
            std::tm* utc8_time = std::gmtime(&current_time_utc8);
            int current_hour = utc8_time->tm_hour;
            if (current_hour >= 0 && current_hour < 9) {
                auto wait_duration = std::chrono::hours(9 - current_hour);
                std::cout << "电费已达阈值，但现在时间是 " << current_hour << " 点，等待到9点再尝试通知。" << std::endl;
                std::cout << std::flush;
                std::this_thread::sleep_for(wait_duration);
            }
            // mail
            if (mail_count > 0){
                mail(config, balance_str);
                mail_count--;
                std::cout << "已通知，将每5小时尝试通知一次，剩余" << mail_count << "次" << std::endl;
                std::cout << std::flush;
            }

            auto interval = std::chrono::hours(5);
            std::this_thread::sleep_for(interval);
        }
    }

    
    return 0;
}