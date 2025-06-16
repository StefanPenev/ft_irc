#include "WeatherBot.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

/**
 * Constructor for WeatherBot.
 * Initializes connection parameters and IRC state.
 */
WeatherBot::WeatherBot(const std::string &server, int port,
                       const std::string &password,
                       const std::string &nick,
                       const std::string &user,
                       const std::string &channel,
                       const std::string &apiHost)
    : _server(server),
      _port(port),
      _password(password),
      _nick(nick),
      _user(user),
      _channel(channel),
      _apiHost(apiHost),
      _sockfd(-1)
{
}

/**
 * Destructor for WeatherBot.
 * Closes socket if still open.
 */
WeatherBot::~WeatherBot()
{
    if (_sockfd >= 0)
        close(_sockfd);
}

/**
 * Connects to a given host and port via TCP.
 * Returns the socket file descriptor on success, or -1 on failure.
 */
int WeatherBot::connectToHost(const char* host, int port)
{
    struct addrinfo hints, *res = NULL;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    std::ostringstream portStr;
    portStr << port;

    if (getaddrinfo(host, portStr.str().c_str(), &hints, &res) != 0)
    {
        std::cerr << "ERROR: getaddrinfo() failed: " << host << std::endl;
        return -1;
    }

    int sockfd = -1;
    for (struct addrinfo* p = res; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd < 0)
            continue;
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(sockfd);
            sockfd = -1;
            continue;
        }
        break;
    }

    freeaddrinfo(res);

    if (sockfd < 0)
        std::cerr << "ERROR: Could not connect to " << host << ":" << port << std::endl;
    return sockfd;
}

/**
 * Loads the API key from a given file.
 * Returns the API key string or empty string if not found.
 */
std::string WeatherBot::loadApiKey(const std::string &filename)
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

/**
 * Performs an HTTP GET request to the specified host and path.
 * Returns the full HTTP response as a string or empty string on failure.
 * This function is synchronous and blocks until the response is received.
 */
std::string WeatherBot::httpGet(const std::string &host, const std::string &path)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        return "";

    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    struct hostent *server = gethostbyname(host.c_str());
    if (!server)
    {
        close(sockfd);
        return "";
    }

    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(80);
    std::memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    struct pollfd fds[1];
    fds[0].fd = sockfd;
    fds[0].events = POLLOUT; 
    if (poll(fds, 1, 2000) <= 0 || !(fds[0].revents & POLLOUT))
    {
        close(sockfd);
        return "";
    }

    std::ostringstream request;
    request << "GET " << path << " HTTP/1.1\r\n"
            << "Host: " << host << "\r\n"
            << "Connection: close\r\n\r\n";
    std::string reqStr = request.str();
    send(sockfd, reqStr.c_str(), reqStr.size(), 0);

    fds[0].events = POLLIN; 
    std::string response;
    char buffer[BUFFER_SIZE];

    while (true)
    {
        int p = poll(fds, 1, 2000); 
        if (p < 0)
            break;
        if (p == 0)
            continue;

        if (fds[0].revents & POLLIN)
        {
            int n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
            if (n <= 0)
                break; 
            buffer[n] = '\0';
            response += buffer;
        }
        else
        {
            break;
        }
    }
    close(sockfd);
    return response;
}

/**
 * Extracts a field value from JSON-like string by key.
 * Returns the value string or empty if key not found.
 * This is a simple parser, not a full JSON parser.
 */
std::string WeatherBot::extractField(const std::string &json, const std::string &key)
{
    std::string::size_type pos = json.find(key);
    if (pos == std::string::npos)
        return "";
    pos += key.length();
    while (pos < json.length() &&
           (json[pos] == ' ' || json[pos] == ':' || json[pos] == '"'))
    {
        ++pos;
    }

    std::string value;
    while (pos < json.length() &&
           (std::isalnum(json[pos]) || json[pos] == '.' ||
            json[pos] == '-' || json[pos] == ' '))
    {
        if (json[pos] == '"')
            break;
        value += json[pos++];
    }
    return value;
}

/**
 * Parses weather summary information from raw JSON response.
 * Returns a formatted string with weather details.
 */
std::string WeatherBot::parseWeatherSummary(const std::string &json)
{
    std::string description = extractField(json, "description");
    std::string temp       = extractField(json, "\"temp\":");
    std::string feels      = extractField(json, "\"feels_like\":");
    std::string humidity   = extractField(json, "\"humidity\":");
    std::string wind       = extractField(json, "\"speed\":");

    std::ostringstream result;
    result << "Weather: " << description;
    if (!temp.empty())
        result << ", Temp: " << temp << "°C";
    if (!feels.empty())
        result << ", Feels Like: " << feels << "°C";
    if (!humidity.empty())
        result << ", Humidity: " << humidity << "%";
    if (!wind.empty())
        result << ", Wind: " << wind << " m/s";

    return result.str();
}

/**
 * Sends a PRIVMSG IRC message to the specified target.
 */
void WeatherBot::sendIRCMessage(int sockfd,
                                const std::string &target,
                                const std::string &message)
{
    std::string fullMsg = "PRIVMSG " + target + " :" + message + "\r\n";
    send(sockfd, fullMsg.c_str(), fullMsg.length(), 0);
}

/**
 * Helper function to handle user-supplied commands.
 * Returns an empty string if no direct response is needed,
 * otherwise returns the message to send.
 */
std::string WeatherBot::handleUserMessage(const std::string &message)
{
    std::string lowerMessage = message;
    for (size_t i = 0; i < lowerMessage.size(); i++)
        lowerMessage[i] = static_cast<char>(std::tolower(lowerMessage[i]));

    if (lowerMessage.find("hello") != std::string::npos ||
        lowerMessage.find("hi") != std::string::npos)
    {
        return "Hi! I'm a weatherbot!";
    }
    else if (lowerMessage.find("help") != std::string::npos)
    {
        return "Available commands: hello, help, about, !weather <city>, roll";
    }
    else if (lowerMessage.find("about") != std::string::npos)
    {
        return "I'm a bot created for the ft_irc project at 42 by Stefan Penev and Ana Ilchenko.";
    }
    else if (lowerMessage.find("roll") != std::string::npos)
    {
        int rollVal = (std::rand() % 6) + 1;
        std::ostringstream oss;
        oss << "You rolled: " << rollVal;
        return oss.str();
    }
    else if (lowerMessage.find("!weather ") == 0)
    {
        std::string city = message.substr(9);
        std::string apiKey = loadApiKey("/home/stefan/Documents/42/ft_irc/bonus/config.txt");
        if (apiKey.empty())
            return "API key not found.";

        std::string path = "/data/2.5/weather?q=" + city +
                           "&appid=" + apiKey +
                           "&units=metric";
        std::string resp = httpGet(_apiHost, path);
        if (resp.empty())
            return "Failed to fetch weather data.";

        std::string summary = parseWeatherSummary(resp);
        return "Weather in " + city + ": " + summary;
    }
    return "Sorry, I don't understand.";
}

/**
 * Runs the main bot loop:
 * - Connects to IRC server
 * - Performs IRC handshake and joins channel
 * - Listens for messages, replies to PINGs
 * - Parses user messages and sends responses
 * Returns 0 on clean exit or 1 on connection failure.
 */
int WeatherBot::run()
{
    _sockfd = connectToHost(_server.c_str(), _port);
    if (_sockfd < 0)
        return 1;

    fcntl(_sockfd, F_SETFL, O_NONBLOCK);

    std::ostringstream ss;
    ss << "PASS " << _password << "\r\n"
       << "NICK " << _nick << "\r\n"
       << "USER " << _user << " 0 * :Weather Bot\r\n"
       << "JOIN " << _channel << "\r\n";
    std::string init = ss.str();
    send(_sockfd, init.c_str(), init.length(), 0);

    char buffer[BUFFER_SIZE];
    std::string data;

    struct pollfd fds[1];
    fds[0].fd = _sockfd;
    fds[0].events = POLLIN;

    while (true)
    {
        int pollRes = poll(fds, 1, 1000);
        if (pollRes < 0)
            break;
        else if (pollRes == 0)
            continue;
            
        if (fds[0].revents & POLLIN)
        {
            int n = recv(_sockfd, buffer, BUFFER_SIZE - 1, 0);
            if (n <= 0)
                break;
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
                    send(_sockfd, pong.c_str(), pong.length(), 0);
                    continue;
                }

                std::string::size_type msgPos = line.find("PRIVMSG");
                if (msgPos != std::string::npos)
                {
                    std::string::size_type textPos = line.find(" :", msgPos);
                    if (textPos != std::string::npos)
                    {
                        std::string message = line.substr(textPos + 2);
                        std::string response = handleUserMessage(message);
                        if (!response.empty())
                        {
                            sendIRCMessage(_sockfd, _channel, response);
                        }
                    }
                }
            }
        }
    }
    close(_sockfd);
    _sockfd = -1;
    data.clear();
    data.shrink_to_fit();
    return 0;
}