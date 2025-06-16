#ifndef WEATHERBOT_HPP
#define WEATHERBOT_HPP

#include <string>

class WeatherBot
{
public:
    WeatherBot(const std::string &server, int port,
               const std::string &password,
               const std::string &nick,
               const std::string &user,
               const std::string &channel,
               const std::string &apiHost);
    ~WeatherBot();

    int run();

private:
    int connectToHost(const char *host, int port);
    std::string loadApiKey(const std::string &filename);
    std::string httpGet(const std::string &host, const std::string &path);
    std::string extractField(const std::string &json, const std::string &key);
    std::string parseWeatherSummary(const std::string &json);
    void sendIRCMessage(int sockfd, const std::string &target, const std::string &message);
    std::string handleUserMessage(const std::string &message);

    std::string _server;
    int         _port;
    std::string _password;
    std::string _nick;
    std::string _user;
    std::string _channel;
    std::string _apiHost;

    int         _sockfd;
};

#endif