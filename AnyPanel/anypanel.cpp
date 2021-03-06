#include "anypanel.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstdio>

#include <tinyformat.h>
#include <striptease.h>

#include "resources/ansi_up.js.inc"
#include "resources/index.html.inc"
#include "resources/main.css.inc"
#include "resources/preferences-default.json.inc"

AnyPanel::AnyPanel(const std::string &appDataLocation, int defaultX, int defaultY, int defaultWidth, int defaultHeight)
{
    preferencesPath_ = appDataLocation + "/preferences.json";
    defaultX_ = defaultX;
    defaultY_ = defaultY;
    defaultWidth_ = defaultWidth;
    defaultHeight_ = defaultHeight;
}

AnyPanel::~AnyPanel()
{
}

static std::string stripteaseUndress(const char *fileName)
{
    char *content = NULL;
    size_t size = striptease_undress(fileName, &content);

    if (size == 0)
        return std::string();

    std::string result(content, content + size);
    striptease_dress(&content);
    return result;
}

static time_t lastModified(const char *fileName)
{
    struct stat attr;
    if (stat(fileName, &attr) == 0)
        return attr.st_mtime;
    return 0;
}

std::string AnyPanel::generateHtml() const
{
    json panel = preferences2_["panels"][0];

    std::string css = (const char*)main_css;

    css = tfm::format(css.c_str(),
            panel["color"].get<std::string>(),
            panel["background-color"].get<std::string>(),
            panel["color"].get<std::string>(),
            panel["background-color"].get<std::string>(),
            panel["color-2"].get<std::string>(),
            panel["background-color-2"].get<std::string>(),
            preferences2_["font-family"].get<std::string>(),
            preferences2_["font-size"].get<std::string>(),
            panel["border"].get<std::string>());

    std::string js = (const char*)ansi_up_js;
    std::string html = (const char*)index_html;

    int rows = panel["rows"];
    int columns = panel["columns"];

    bool r = panel["color-alternating-rows"];
    bool c = panel["color-alternating-columns"];

    std::string body;
    body += "<table>\n";
    for (int j = 0; j < rows; ++j)
    {
        body += "<tr>\n";
        for (int i = 0; i < columns; ++i)
        {
            std::string klass;
            if (r & c)
            {
                klass = (i & 1) ^ (j & 1) ? "color2" : "color1";
            }
            else if (!r & c)
            {
                klass = (i & 1) ? "color2" : "color1";
            }
            else if (r & !c)
            {
                klass = (j & 1) ? "color2" : "color1";
            }
            else
            {
                klass = "color1";
            }
            body += tfm::format("<td id='%d,%d' class='%s'>&nbsp;</td>\n", j + 1, i + 1, klass);
        }
        body += "</tr>\n";
    }
    body += "</table>\n";

    std::string result = tfm::format(html.c_str(), css, js, body);

#if 0
    QFile file("/tmp/index.html");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << QString::fromStdString(result);
#endif

    return result;
}

static int parseFrequency(const std::string &frequency)
{
    if (frequency.size() <= 1)
        return 0;

    if (frequency.back() != 's' && frequency.back() != 'm' && frequency.back() != 'h')
        return 0;

    switch (frequency.back())
    {
    case 's':
        return std::atoi(frequency.c_str());
    case 'm':
        return 60 * std::atoi(frequency.c_str());
    case 'h':
        return 3600 * std::atoi(frequency.c_str());
    }

    return 0;
}

static bool validatePreferences(const json &preferences)
{
    if (!preferences.is_object())
        return false;

    if (!preferences.count("opacity") || !preferences["opacity"].is_number())
        return false;
    if (!preferences.count("font-family") || !preferences["font-family"].is_string())
        return false;
    if (!preferences.count("font-size") || !preferences["font-size"].is_string())
        return false;
    if (!preferences.count("geometry") || !preferences["geometry"].is_object())
        return false;

    json geometry = preferences["geometry"];

    if (!geometry.count("x") || !geometry["x"].is_number())
        return false;
    if (!geometry.count("y") || !geometry["y"].is_number())
        return false;
    if (!geometry.count("width") || !geometry["width"].is_number())
        return false;
    if (!geometry.count("height") || !geometry["height"].is_number())
        return false;

    if (!preferences.count("panels") || !preferences["panels"].is_array())
        return false;

    json panels = preferences["panels"];

    if (panels.empty())
        return false;

    for (std::size_t i = 0; i < panels.size(); ++i)
    {
        json panel = panels[i];

        if (!panel.is_object())
            return false;

        if (!panel.count("name") || !panel["name"].is_string())
            return false;

        if (!panel.count("rows") || !panel["rows"].is_number())
            return false;

        if (!panel.count("columns") || !panel["columns"].is_number())
            return false;

        if (!panel.count("color") || !panel["color"].is_string())
            return false;

        if (!panel.count("background-color") || !panel["background-color"].is_string())
            return false;

        if (!panel.count("color-2") || !panel["color-2"].is_string())
            return false;

        if (!panel.count("background-color-2") || !panel["background-color-2"].is_string())
            return false;

        if (!panel.count("color-alternating-rows") || !panel["color-alternating-rows"].is_boolean())
            return false;

        if (!panel.count("color-alternating-columns") || !panel["color-alternating-columns"].is_boolean())
            return false;

        if (!panel.count("border") || !panel["border"].is_string())
            return false;
    }

    if (!preferences.count("scripts") || !preferences["scripts"].is_array())
        return false;

    json scripts = preferences["scripts"];

    for (std::size_t i = 0; i < scripts.size(); ++i)
    {
        json script = scripts[i];

        if (!script.count("cell") || !script["cell"].is_string())
            return false;

        if (!script.count("frequency") || !script["frequency"].is_string())
            return false;

        if (parseFrequency(script["frequency"]) <= 0)
            return false;

        if (!script.count("command") || !script["command"].is_string())
            return false;
    }

    return true;
}

bool AnyPanel::loadPreferences()
{
    if (lastModified(preferencesPath_.c_str()) == 0)
    {
        std::FILE *f = std::fopen(preferencesPath_.c_str(), "wb");
        fprintf(f, "%s", tfm::format((const char*)preferences_default_json, defaultX_, defaultY_, defaultWidth_, defaultHeight_).c_str());
        fclose(f);
    }

    time_t t = lastModified(preferencesPath_.c_str());
    if (t == lastModified_)
        return false;
    lastModified_ = t;

    std::string buffer = stripteaseUndress(preferencesPath_.c_str());

    try
    {
        preferences2_ = json::parse(buffer);
    }
    catch (std::exception &)
    {
        return false;
    }

    if (validatePreferences(preferences2_) == false)
        return false;

    for (std::size_t i = 0; i < threads_.size(); ++i)
        threads_[i]->ignoreOutput = true;

    scripts_.clear();

    json scripts = preferences2_["scripts"];

    for (std::size_t i = 0; i < scripts.size(); ++i)
    {
        json script = scripts[i];

        std::string cell = script["cell"];
        int frequency = parseFrequency(script["frequency"]);
        std::string command = script["command"];

        scripts_.push_back({cell, command, frequency, 0});
    }

    return true;
}


static bool isControlCodeExcept0x1b(char c)
{
    if (c == 0x1b)
        return false;

    if (0x00 <= c && c <= 0x1f)
        return true;

    return false;
}

void AnyPanel::commandToJavascript(const std::string &command)
{
    size_t index = command.find(' ');

    if (index != std::string::npos)
    {
        std::string id = command.substr(0, index);
        std::string content = command.substr(index + 1);

        if (content.empty())
            content = " ";

        queue_.push_back(tfm::format("updateCell('%s', '%s');", id, content));
    }
}

double AnyPanel::opacity() const
{
    return preferences2_["opacity"];
}

ARect AnyPanel::geometry() const
{
    json geometry = preferences2_["geometry"];
    int x = geometry["x"];
    int y = geometry["y"];
    int width = geometry["width"];
    int height = geometry["height"];
    return ARect{x, y, width, height};
}

void AnyPanel::checkThreads()
{
    if (threads_.empty())
        return;

    for (size_t i = threads_.size(); i --> 0 ;)
    {
        if (threads_[i]->finished)
        {
            if (threads_[i]->ignoreOutput == false)
                commandToJavascript(threads_[i]->cell + " " + threads_[i]->output);
            delete threads_[i];
            threads_.erase(threads_.begin() + i);
        }
    }
}

void AnyPanel::checkScripts()
{
    time_t curr = time(NULL);

    for (std::size_t i = 0; i < scripts_.size(); ++i)
    {
        if (curr >= scripts_[i].lastTime + scripts_[i].frequency)
        {
            createThread(scripts_[i].command, scripts_[i].cell);
            scripts_[i].lastTime = curr;
        }
    }
}

std::vector<std::string> AnyPanel::poll()
{
    checkThreads();
    checkScripts();

    std::vector<std::string> queue;
    std::swap(queue_, queue);

    return queue;
}

void *AnyPanel::threadFunc(void *data)
{
    Thread *thread = static_cast<Thread*>(data);

    FILE *f = popen(thread->command.c_str(), "r");

    if (f)
    {
        char c; // TODO: use bigger buffer
        while (fread(&c, 1, 1, f) > 0)
            if (!isControlCodeExcept0x1b(c))
                thread->output.push_back(c);
        pclose(f);
    }

    thread->finished = true;

    return NULL;
}

void AnyPanel::createThread(const std::string &command, const std::string &cell)
{
    Thread *thread = new Thread(command, cell);
    pthread_create(&thread->thread, NULL, threadFunc, (void*)thread);
    threads_.push_back(thread);
}


