#ifndef ANYPANEL_H
#define ANYPANEL_H

#include <time.h>
#include <vector>
#include <string>

#include <json.hpp>
using json = nlohmann::json;

#include <asio.hpp>
using asio::ip::udp;

struct ARect
{
    int x;
    int y;
    int width;
    int height;
};

class AnyPanel
{
public:
    AnyPanel(const std::string &appDataLocation);
    ~AnyPanel();

    bool loadPreferences();
    std::string generateHtml() const;
    double opacity() const;
    ARect geometry() const;
    std::vector<std::string> poll();

private:
    void start_receive();
    void commandToJavascript(const std::string &command);
    int port() const;

private:
    json preferences2_;
    time_t lastModified_;
    asio::io_service io_service_;
    udp::socket *socket_;
    udp::endpoint sender_endpoint_;
    enum { max_length = 1024 };
    char data_[max_length];
    std::vector<std::string> queue_;
    std::string preferencesPath_;
};

#endif // ANYPANEL_H