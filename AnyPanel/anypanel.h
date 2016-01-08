#ifndef ANYPANEL_H
#define ANYPANEL_H

#include <time.h>
#include <vector>
#include <string>
#include <pthread.h>

#include <json.hpp>
using json = nlohmann::json;

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
    AnyPanel(const std::string &appDataLocation, int defaultX, int defaultY, int defaultWidth, int defaultHeight);
    ~AnyPanel();

    bool loadPreferences();
    std::string generateHtml() const;
    double opacity() const;
    ARect geometry() const;
    std::vector<std::string> poll();

private:
    void commandToJavascript(const std::string &command);
    void checkThreads();
    void checkScripts();

private:
    json preferences2_;
    time_t lastModified_;
    std::vector<std::string> queue_;
    std::string preferencesPath_;
    int defaultX_, defaultY_, defaultWidth_, defaultHeight_;

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
