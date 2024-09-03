#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <map>

using namespace std;
map<string, string> building_to_id = {
        {"T1", "1574231830"},
        {"T2", "1574231833"},
        {"T3", "1574231835"},
        {"S1", "1503975832"},
        {"S2", "1503975890"},
        {"S5", "1503975967"},
        {"S6", "1503975980"},
        {"S7", "1503975988"},
        {"S8", "1503975995"},
        {"S9", "1503976004"},
        {"S10", "1503976037"},
        {"S11", "1599193777"},
        {"S12", "1599193777"},
        {"S13", "1599193777"},
        {"B1", "1661835249"},
        {"B2", "1661835256"},
        {"B9", "1693031698"},
        {"B10", "1693031710"}
};

bool init_quiry_config(std::map<std::string, std::string> &config){
    string input;
    std::cout << "开始初始化quiry配置文件...\n";

    std::cout << "请输入您的校园卡卡号：\n";
    std::cin >> input;
    config["campus_card"] = input;

    std::cout << "可选宿舍楼:\n";
    for (const auto& pair : building_to_id) {
        std::cout << pair.first << ", ";
    }
    std::cout << '\n';
    std::cout << "请输入您的宿舍楼：\n";
    std::cin >> input;
    config["dormitory_building"] = input;

    std::cout << "请输入您的房间号(例: A123):\n";
    std::cin >> input;
    config["room_id"] = input;

    return true;
}

string create_http_request(const string& host, const string& path, const string& data) {
    string request = "POST " + path + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "User-Agent: C++ Client\r\n";
    request += "Content-Type: application/x-www-form-urlencoded\r\n";
    request += "Content-Length: " + to_string(data.length()) + "\r\n";
    request += "Connection: close\r\n\r\n";
    request += data;
    return request;
}

string query(const string &account, const string &building, const string &room) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        cerr << "Could not create socket" << endl;
        return "";
    }

    sockaddr_in server;
    server.sin_addr.s_addr = inet_addr("10.100.1.24");
    server.sin_family = AF_INET;
    server.sin_port = htons(8988);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        cerr << "Connection failed" << endl;
        close(sock);
        return "";
    }

    // 构建 JSON 数据字符串
    string data = 
        "jsondata={\"query_elec_roominfo\":{"
        "\"aid\":\"0030000000002505\","
        "\"account\":\"" + account + "\","
        "\"room\":{\"roomid\":\"" + room + "\",\"room\":\"" + room + "\"},"
        "\"floor\":{\"floorid\":\"\",\"floor\":\"\"},"
        "\"area\":{\"area\":\"青岛校区\",\"areaname\":\"青岛校区\"},"
        "\"building\":{\"buildingid\":\"" + building_to_id[building] + "\",\"building\":\"" + building + "\"}"
        "}}&funname=synjones.onecard.query.elec.roominfo&json=true";

    string request = create_http_request("10.100.1.24", "/web/Common/Tsm.html", data);

    if (send(sock, request.c_str(), request.length(), 0) < 0) {
        cerr << "Send failed" << endl;
        close(sock);
        return "";
    }

    char response[4096];
    string responseData;
    int received;

    while ((received = recv(sock, response, sizeof(response), 0)) > 0) {
        responseData.append(response, received);
    }

    if (received < 0) {
        cerr << "Receive failed" << endl;
    }

    close(sock);

    // 简单解析返回的 JSON 字符串
    size_t startPos = responseData.find("\"errmsg\":\"");
    if (startPos != string::npos) {
        startPos += 10; // 跳过 "errmsg":" 的长度
        size_t endPos = responseData.find("\"", startPos);
        if (endPos != string::npos) {
            return responseData.substr(startPos, endPos - startPos).substr(24); // 跳过前8个中文字符
        }
    }

    return "";
}
