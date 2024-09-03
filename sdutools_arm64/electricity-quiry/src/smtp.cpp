#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <cstring>
#include <string>
#include <map>
#include <cstdio>
#include <sstream>
#include <vector>

using namespace std;
using std::size_t;

bool init_smtp_config(std::map<std::string, std::string> &config){
    string input;
    std::cout << "开始初始化smtp配置文件...\n";

    std::cout << "请输入smtp服务器地址, 如'smtps://smtp.163.com:465':\n";
    std::cin >> input;
    config["smtp_url"] = input;

    std::cout << "请输入发件人邮箱:\n";
    std::cin >> input;
    config["sender_mail"] = input;

    std::cout << "请输入发件人邮箱权限密码:\n";
    std::cin >> input;
    config["smtp_passwd"] = input;

    std::cout << "请输入收件人邮箱, 以逗号分隔多邮箱, 如'a@b,c@d':\n";
    std::cin >> input;
    config["receicers_mail"] = input;

    return 0;
}

vector<string> payload_text;

static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
    static int line_index = 0;
    if (line_index >= payload_text.size()){
        return 0;
    }

    const char *line;
    line = payload_text[line_index].c_str();
    if (line) {
        size_t len = 0;
        len = strlen(line);
        memcpy(ptr, line, len);
        line_index++;
        return len;
    }

    return 0;
}

bool mail(std::map<std::string, std::string> &config, string balance){


    payload_text.push_back("From: xiaofeng476255853@163.com\r\n");
    payload_text.push_back("To: " + config["receicers_mail"] + "\r\n");
    payload_text.push_back("Subject: 电费余额提醒\r\n");
    payload_text.push_back("\r\n");
    payload_text.push_back("宿舍电费低于阈值，请及时充值\r\n");
    payload_text.push_back("当前电费为: " + balance + "元\r\n");



    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        struct curl_slist *recipients = nullptr;

        std::stringstream ss(config["receicers_mail"]);
        std::string token;
        std::vector<std::string> receicers;

        while (std::getline(ss, token, ',')) {
            receicers.push_back(token);
        }
        for(int i=0; i<receicers.size(); i++){
            recipients = curl_slist_append(recipients, receicers[i].c_str());
        }
        // curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.163.com:465");
        // curl_easy_setopt(curl, CURLOPT_USERNAME, "xiaofeng476255853@163.com");
        // curl_easy_setopt(curl, CURLOPT_PASSWORD, "HZDEJJIILBBCUIFY");
        // curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

        // curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "xiaofeng476255853@163.com");
        curl_easy_setopt(curl, CURLOPT_URL, config["smtp_url"].c_str());
        curl_easy_setopt(curl, CURLOPT_USERNAME, config["sender_mail"].c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, config["smtp_passwd"].c_str());
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, config["sender_mail"].c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
        curl_easy_setopt(curl, CURLOPT_READDATA, nullptr);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return true;
}
