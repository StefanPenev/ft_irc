#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>
#include <sstream>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>

#define IRC_SERVER "127.0.0.1"
#define IRC_PORT 8080
#define IRC_PASSWORD "pass"
#define IRC_NICK "weatherbot"
#define IRC_USER "weatherbot"
#define IRC_CHANNEL "#weather"
#define WEATHER_API_HOST "api.openweathermap.org"
#define BUFFER_SIZE 4096

std::string loadApiKey(const std::string& filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
        return "";

    std::string line;
    while (std::getline(file, line))
    {
        if (line.find("API_KEY=") == 0)
            return line.substr(8);
    }
    return "";
}

int connectToHost(const char* host, int port)
{
    struct hostent* server = gethostbyname(host);
    if (!server)
    {
        std::cerr << "ERROR: No such host: " << host << std::endl;
        return -1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

std::string httpGet(const std::string& host, const std::string& path)
{
    int sockfd = connectToHost(host.c_str(), 80);
    if (sockfd < 0)
        return "";

    std::ostringstream request;
    request << "GET " << path << " HTTP/1.1\r\n";
    request << "Host: " << host << "\r\n";
    request << "Connection: close\r\n\r\n";

    std::string reqStr = request.str();
    send(sockfd, reqStr.c_str(), reqStr.length(), 0);

    char buffer[BUFFER_SIZE];
    std::string response;
    int n;
    while ((n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0)) > 0)
    {
        buffer[n] = '\0';
        response += buffer;
    }
    close(sockfd);
    return response;
}

std::string extractField(const std::string& json, const std::string& key)
{
    std::string::size_type pos = json.find(key);
    if (pos == std::string::npos) return "";
    pos += key.length();
    while (pos < json.length() && (json[pos] == ' ' || json[pos] == ':' || json[pos] == '"')) ++pos;
    std::string value;
    while (pos < json.length() && (isalnum(json[pos]) || json[pos] == '.' || json[pos] == '-' || json[pos] == ' '))
    {
        if (json[pos] == '"') break;
        value += json[pos++];
    }
    return value;
}

std::string parseWeatherSummary(const std::string& json)
{
    std::string description = extractField(json, "description");
    std::string temp = extractField(json, "\"temp\":");
    std::string feels = extractField(json, "\"feels_like\":");
    std::string humidity = extractField(json, "\"humidity\":");
    std::string wind = extractField(json, "\"speed\":");

    std::ostringstream result;
    result << "Weather: " << description;
    if (!temp.empty()) result << ", Temp: " << temp << "°C";
    if (!feels.empty()) result << ", Feels Like: " << feels << "°C";
    if (!humidity.empty()) result << ", Humidity: " << humidity << "%";
    if (!wind.empty()) result << ", Wind: " << wind << " m/s";

    return result.str();
}

void sendIRCMessage(int sockfd, const std::string& target, const std::string& message)
{
    std::string fullMsg = "PRIVMSG " + target + " :" + message + "\r\n";
    send(sockfd, fullMsg.c_str(), fullMsg.length(), 0);
}

int main()
{
    int sockfd = connectToHost(IRC_SERVER, IRC_PORT);
    if (sockfd < 0)
        return 1;

    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    std::ostringstream ss;
    ss << "PASS " << IRC_PASSWORD << "\r\n";
    ss << "NICK " << IRC_NICK << "\r\n";
    ss << "USER " << IRC_USER << " 0 * :Weather Bot\r\n";
    ss << "JOIN " << IRC_CHANNEL << "\r\n";
    std::string init = ss.str();
    send(sockfd, init.c_str(), init.length(), 0);

    char buffer[BUFFER_SIZE];
    std::string data;

    struct pollfd fds[1];
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;

    while (true)
    {
        int pollRes = poll(fds, 1, 1000);
        if (pollRes > 0 && (fds[0].revents & POLLIN))
        {
            int n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
            if (n <= 0) break;
            buffer[n] = '\0';
            data += buffer;

            std::string::size_type end;
            while ((end = data.find("\r\n")) != std::string::npos)
            {
                std::string line = data.substr(0, end);
                data.erase(0, end + 2);

                if (line.find("PING") == 0)
                {
                    std::string pong = "PONG" + line.substr(4) + "\r\n";
                    send(sockfd, pong.c_str(), pong.length(), 0);
                    continue;
                }

                std::string::size_type msgPos = line.find("PRIVMSG");
                if (msgPos != std::string::npos)
                {
                    std::string::size_type textPos = line.find(" :", msgPos);
                    if (textPos != std::string::npos)
                    {
                        std::string message = line.substr(textPos + 2);
                        if (message.find("!weather ") == 0)
                        {
                            std::string city = message.substr(9);
                            std::string apiKey = loadApiKey("./bonus/config.txt");
                            std::string path = "/data/2.5/weather?q=" + city + "&appid=" + apiKey + "&units=metric";
                            std::string response = httpGet(WEATHER_API_HOST, path);
                            std::string summary = parseWeatherSummary(response);
                            sendIRCMessage(sockfd, IRC_CHANNEL, "Weather in " + city + ": " + summary);
                        }
                    }
                }
            }
        }
    }
    close(sockfd);
    return 0;
}