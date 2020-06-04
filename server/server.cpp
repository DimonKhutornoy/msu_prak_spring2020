#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctime>
#include <vector>
#include <string.h>
#include <signal.h>
#include <sys/select.h>
#include <sstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

using namespace std;

#define PORTNUM 8080
#define BACKLOG 10
#define BUFLEN 10000

class Server
{
private:
    int Serverfd;
    int port;
    char Request[BUFLEN];
    int Reqfd;

public:
    Server(string LogFile);
    void Run();
    ~Server();
};

char* get_params(char* str)
{
    char* last = new char[BUFLEN];
    int i = 5, j = 0;
    while (str[i] != 0)
    {
        if (str[i] != '/')
            last[j++] = str[i];
        else
            j = 0;
        i++;
    }
    last[j] = 0;
    return last;
}


char* get_file_name(char* str)
{
    char* first = new char[BUFLEN];
    strcpy(first, str);
    int i = 0;
    while (first[i++] != '?')
        ;
    first[i - 1] = 0;
    return first;
}

bool is_cgi(char* str)
{
    char* last = get_params(str);
    for (size_t i = 0; i < strlen(last); i++)
    {
        if (last[i] == '?')
        {
            delete[] last;
            return true;
        }
    }
    delete[] last;
    return false;
}

void file_res(int Clientfd, string error_file)
{
    int len = 0;
    char buf[BUFLEN];
    int Filefd = open(error_file.c_str(), O_RDONLY);
    while ((len = read(Filefd, &buf, BUFLEN)))
    {
        if (send(Clientfd, buf, len, 0) < 0)
        {
            close(Filefd);
            perror(error_file.c_str());
            shutdown(Clientfd, SHUT_RDWR);
            close(Clientfd);
            exit(1);
        }
    }
    close(Filefd);
}

char* itoa(int i)
{
    char* str = new char[11];
    sprintf(str, "%d", i);
    return str;
}

int char_search(char* str, char c)
{
    int j = 0;
    while ((str[j] != '\0') && (str[j] != c))
        j++;
    if (str[j] == '0')
        return 0;
    return j;
}

char* Response(int code, string msg, bool flag)
{
    string str = "HTTP/1.1 ";
    str += to_string(code);
    str += " ";
    str += msg;
    str += "\r\nStatus: ";
    str += to_string(code);
    str += " ";
    str += msg;
    if (flag)
    {
        str += "\r\nContent-type: text/html; charset=utf-8\r\n\r\n<html><body><p>";
        str += msg;
        str += "</p></body></html>";
    }
    else
    {
        str += "\r\nContent-type: text/html; charset=utf-8\r\n\r\n";
    }
    int n = str.length();
    char* res = (char*)malloc((n + 1) * sizeof(char));
    strcpy(res, str.c_str());
    return res;
}

Server::Server(string ReqFile)
{
    Reqfd = open(ReqFile.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0777);
    time_t st_time = time(NULL);
    char* str = asctime(localtime(&st_time));
    write(Reqfd, str, strlen(str));
    port = PORTNUM;
    struct sockaddr_in ServerAddress;
    if ((Serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "Can't create socket!\n";
        exit(1);
    }
    memset(&ServerAddress, 0, sizeof(ServerAddress));
    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(port);
    ServerAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(Serverfd, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) < 0)
    {
        cout << "Can't bind socket\n";
        close(Serverfd);
        exit(1);
    }
    if (listen(Serverfd, BACKLOG) < 0)
    {
        cout << "Can't listen socket\n";
        close(Serverfd);
        exit(1);
    }
}

void Server::Run()
{
    cout << "Server is ready!" << endl;
    while (true)
    {
        bool home = true;
        struct sockaddr_in ClientAddress;
        size_t ClAddrLen = sizeof(ClientAddress);
        int Clientfd = accept(Serverfd, (struct sockaddr*)&ClientAddress, (socklen_t*)&ClAddrLen);
        if (Clientfd < 0)
        {
            cout << "Client Error\n";
            close(Serverfd);
            close(Clientfd);
            exit(1);
        }
        int req = recv(Clientfd, Request, BUFLEN, 0);
        if (req < 0)
        {
            cout << "Server Error\n";
            close(Serverfd);
            close(Clientfd);
            exit(1);
        }
        write(Reqfd, Request, strlen(Request));
        if (!strncmp(Request, "GET", 3))
        {
            int i = 5;
            char c = Request[i];
            while (c != ' ')
            {
                i++;
                c = Request[i];
            }
            char path[i - 3];
            if (i != 5)
            {
                home = false;
                copy(&Request[5], &Request[i], &path[0]);
                path[i - 4] = '\0';
            }
            else
            {
                path[0] = '/';
                path[1] = '\0';
            }
            cout << "File: " << path << endl;
            if (is_cgi(path))
            {
                chdir("./bin");
                int status;
                string name = to_string(getpid()) + ".txt";
                int Filefd = open(name.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                dup2(Filefd, 1);
                if (!fork())
                {
                    chdir("./bin");
                    string processName = get_file_name(path);
                    int tmpFd = open(processName.c_str(), O_RDONLY, 0644);
                    ifstream is(processName.c_str());
                    string s;
                    is >> s;
                    while (s[0] == '#' || s[0] == '!' || s[0] == ' ')
                        s.erase(s.begin());

                    close(tmpFd);
                    char* argv[] = { (char*)processName.c_str(), NULL };

                    int offset = char_search(Request + 5, '?');
                    char** envp = new char*[7];
                    envp[0] = new char[(int)strlen("SERVER_ADDR=127.0.0.1") + 1];
                    strcpy(envp[0], "SERVER_ADDR=127.0.0.1");
                    envp[1] = new char[(int)strlen("CONTENT_TYPE=text/plain") + 1];
                    strcpy(envp[1], "CONTENT_TYPE=text/plain");
                    envp[2] = new char[(int)strlen("SERVER_PROTOCOL=HTTP/1.0") + 1];
                    strcpy(envp[2], "SERVER_PROTOCOL=HTTP/1.0");
                    envp[3] = new char[(int)strlen("SCRIPT_NAME=") + 1 + strlen(Request + 5)];
                    strcpy(envp[3], "SCRIPT_NAME=");
                    strcat(envp[3], Request + 5);
                    envp[4] = new char[(int)strlen("SERVER_PORT=") + 5];
                    char* str_port = itoa(port);
                    strcpy(envp[4], "SERVER_PORT=");
                    strcat(envp[4], str_port);
                    delete[] str_port;
                    envp[5] = new char[(int)strlen("QUERY_STRING=") + 1
                        + (int)strlen(Request + 6 + offset)];
                    strcpy(envp[5], "QUERY_STRING=");
                    strcat(envp[5], Request + 6 + offset);
                    envp[6] = NULL;

                    execve(s.c_str(), argv, envp);
                    perror("execve() ");
                    exit(1);
                }
                wait(&status);
                close(Filefd);
                if (WIFEXITED(status))
                {
                    if (WEXITSTATUS(status) == 0)
                    {
                        Filefd = open(name.c_str(), O_RDONLY, 0644);
                        if (Filefd > 0)
                        {
                            char buf[BUFLEN];
                            int len = 0;
                            char* res = Response(200, "OK", false);
                            send(Clientfd, res, strlen(res), 0);
                            free(res);
                            while ((len = read(Filefd, &buf, BUFLEN)))
                            {
                                send(Clientfd, buf, len, 0);
                            }
                            close(Filefd);
                        }
                        else
                        {
                            string error = "Ошибка открытия файла " + name;
                            perror(error.c_str());
                            char* res = Response(500, "", true);
                            send(Clientfd, res, strlen(res), 0);
                            free(res);
                            file_res(Clientfd, "error/cgierr.html");
                        }
                    }
                    else
                    {
                        string error = "CGI процесс завершился со статусом "
                            + to_string(WEXITSTATUS(status));
                        perror(error.c_str());
                        char* res = Response(500, "", true);
                        send(Clientfd, res, strlen(res), 0);
                        free(res);
                        file_res(Clientfd, "error/cgierr.html");
                    }
                }
                else if (WIFSIGNALED(status))
                {
                    string error = "CGI процесс прислал сигнал " + to_string(WIFSIGNALED(status));
                    perror(error.c_str());
                    char* res = Response(500, "", true);
                    send(Clientfd, res, strlen(res), 0);
                    free(res);
                    file_res(Clientfd, "error/cgierr.html");
                }
                remove(name.c_str());
                chdir("./..");
            }
            else
            {
                int Filefd;
                if (!home)
                {
                    Filefd = open(path, O_RDONLY);
                }
                if (Filefd < 0)
                {
                    char* res = Response(404, "", true);
                    send(Clientfd, res, strlen(res), 0);
                    free(res);
                    file_res(Clientfd, "err404.html");
                }
                else
                {
                    char buf[BUFLEN];
                    int len = 0;
                    char* res = Response(200, "OK", false);
                    send(Clientfd, res, strlen(res), 0);
                    free(res);
                    if (!home)
                        file_res(Clientfd, path);
                    else
                        file_res(Clientfd, "main.html");
                }
            }
        }
        else
        {
            char* res = Response(501, "Not Implemented", true);
            send(Clientfd, res, strlen(res), 0);
            free(res);
        }
        shutdown(Clientfd, SHUT_RDWR);
        close(Clientfd);
    }
}


Server::~Server()
{
    close(Serverfd);
    close(Reqfd);
}

int main()
{
    Server server = Server("File_with_Requests.txt");
    server.Run();
    return 0;
}
