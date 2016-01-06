#ifndef ANYPANEL_H
#define ANYPANEL_H

#include <time.h>
#include <vector>
#include <string>
#include <pthread.h>

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
    void pollUdp();
    void checkThreads();
    void checkScripts();

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

    struct Thread
    {
        Thread(const std::string &command, const std::string &cell) : command(command), cell(cell), finished(false), ignoreOutput(false) { }
        pthread_t thread;
        std::string command;
        std::string cell;
        std::string output;
        bool finished;
        bool ignoreOutput;
    };
    std::vector<Thread*> threads_;
    void createThread(const std::string &command, const std::string &cell);
    static void *threadFunc(void *data);

    struct Script
    {
        std::string cell;
        std::string command;
        int frequency;
        time_t lastTime;
    };
    std::vector<Script> scripts_;
};

#endif // ANYPANEL_H
