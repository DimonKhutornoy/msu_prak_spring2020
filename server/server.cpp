#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <arpa/inet.h>
#include <ctime>

#define BACKLOG 5
#define BUFLEN 1024

char* itoa(int i)
{
    char* str = new char[11];
    sprintf(str, "%d", i);
    return str;
}
int fcount(int fd)
{
    int size = 0, len;
    char c[1024];
    while ((len = read(fd, &c, 1024)) > 0)
        size += len;
    lseek(fd, 0, 0);
    return size;
}
int port = 8080;


class pid_arr
{
    int* a;
    int len;

public:
    pid_arr()
    {
        len = 0;
        a = new int[0];
    }
    void add(int pid)
    {
        int* new_a = new int[++len];
        for (int i = 0; i < len - 1; i++)
        {
            new_a[i] = a[i];
        }
        new_a[len - 1] = pid;
        delete[] a;
        a = new_a;
    }
    void del(int pid)
    {
        int* new_a = new int[--len];
        int i = 0;
        while (a[i] != pid)
        {
            new_a[i] = a[i];
            i++;
        }
        for (int j = i + 1; j < len + 1; j++)
            new_a[j - 1] = a[i];
        delete[] a;
        a = new_a;
    }
    int get_len() const
    {
        return len;
    }
    int* get_a()
    {
        return a;
    }
    int& operator[](int index)
    {
        return a[index];
    }
    ~pid_arr()
    {
        delete[] a;
    }
};

pid_arr pids;

class Server
{
    int serverSocket;
    struct sockaddr_in ServerAddress;

public:
    Server(int port = 8080)
    {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        memset(&ServerAddress, '0', sizeof(ServerAddress));
        ServerAddress.sin_family = AF_INET;
        ServerAddress.sin_port = htons(port);
        if (inet_pton(AF_INET, "127.0.0.1", &ServerAddress.sin_addr) <= 0)
        { 
            printf("\n inet_pton error occured\n");
            exit(1);
        }
    }
    int get_sock() const
    {
        return serverSocket;
    }
    void sbind()
    {
        if (bind(serverSocket, (struct sockaddr*)&ServerAddress, sizeof(ServerAddress)) < 0)
        {
            std::cerr << "fatal error: cannot bind\n";
            exit(1);
        }
    }
    void slisten()
    {
        if (listen(serverSocket, BACKLOG) < 0)
        {
            std::cerr << "fatal error: cannot listen\n";
            exit(1);
        }
    }
};

class Client
{
    int clSocket;
    socklen_t ClAddrLen;
    struct sockaddr_in ClientAddress;
    int char_search(char* str, char c) const
    {
        int j = 0;
        while ((str[j] != '\0') && (str[j] != c))
            j++;
        if (str[j] == '0')
            return 0;
        return j;
    }

public:
    int get_sock() const
    {
        return clSocket;
    }
    void cl_accept(int serverSocket)
    {
        ClAddrLen = sizeof(ClientAddress);
        clSocket = accept(serverSocket, (struct sockaddr*)&ClientAddress, &ClAddrLen);
        if (clSocket < 0)
        {
            std::cout << "error: losed socked\n";
        }
    }
    int cl_request()
    {
        int len;
        char buf[BUFLEN];
        int cont_type;
        if ((len = recv(clSocket, &buf, BUFLEN, 0)) < 0)
        {
            shutdown(clSocket, 1);
            close(clSocket);
            std::cout << "error: reading socked\n";
            exit(2);
        }
        if (strncmp(buf, "GET /", 5))
        {
            // BadRequest
            strcpy(buf, "HTTP/1.0 501 NotImplemented modelserver\nServer: Model HTTP "
                        "Server/0.1\nConnection: keep-alive\nAllow: GET\nDate: ");
            time_t tm = time(NULL);
            strcat(buf, asctime(localtime(&tm)));
            strcat(buf, "\n\n");
            len = strlen(buf);
            send(clSocket, &buf, len, 0);
            shutdown(clSocket, 1);
            close(clSocket);
            return 501;
        }
        int i = 5;
        int fd;
        while (buf[i] && (buf[i++] > ' '))
            ;
        buf[i - 1] = 0;
        i = 5;
        while (buf[i] && (buf[i++] != '.'))
            ;
        cont_type = 2;
        if (buf[i])
        {
            if (!strncmp(buf + i, "html", 4))
                cont_type = 0;
            else if (!strncmp(buf + i, "jpeg", 4))
                cont_type = 1;
        }
        else if (strlen(buf) > 5)
        { // cgi
            cont_type = 0;
            int pid;
            if ((pid = fork()) > 0)
            {
                pids.add(pid);
                return 1;
            }
            else
            {
                char* argv[2];
                argv[0] = buf + 5;
                argv[1] = NULL;
                char tmp_file[11];
                strcpy(tmp_file, "p");
                char* fname = itoa(getpid());
                strcat(tmp_file, fname);
                delete[] fname;
                creat(tmp_file, 0666);
                int tmp = open(tmp_file, O_TRUNC | O_WRONLY);
                dup2(tmp, 1);
                close(tmp);
                int offset = char_search(buf + 5, '?');
                (buf + 5)[offset] = '\0';
                char** envp = new char*[7];
                envp[0] = new char[(int)strlen("SERVER_ADDR=127.0.0.1") + 1];
                strcpy(envp[0], "SERVER_ADDR=127.0.0.1");
                envp[1] = new char[(int)strlen("CONTENT_TYPE=text/plain") + 1];
                strcpy(envp[1], "CONTENT_TYPE=text/plain");
                envp[2] = new char[(int)strlen("SERVER_PROTOCOL=HTTP/1.0") + 1];
                strcpy(envp[2], "SERVER_PROTOCOL=HTTP/1.0");
                envp[3] = new char[(int)strlen("SCRIPT_NAME=") + 1 + strlen(buf + 5)];
                strcpy(envp[3], "SCRIPT_NAME=");
                strcat(envp[3], buf + 5);
                envp[4] = new char[(int)strlen("SERVER_PORT=") + 5];
                char* str_port = itoa(port);
                strcpy(envp[4], "SERVER_PORT=");
                strcat(envp[4], str_port);
                delete[] str_port;
                envp[5]
                    = new char[(int)strlen("QUERY_STRING=") + 1 + (int)strlen(buf + 6 + offset)];
                strcpy(envp[5], "QUERY_STRING=");
                strcat(envp[5], buf + 6 + offset);
                envp[6] = NULL;
                execve(argv[0], argv, envp);
                std::cerr << "cgi error\n";
                delete[] envp;
                exit(1);
                // обработка ошибок запуска
            }
        }
        if (strlen(buf) == 5)
        {
            cont_type = 0;
            fd = open("main.html", O_RDONLY);
        }
        else if ((fd = open(buf + 5, O_RDONLY)) < 0)
        {
            fd = open("err404.html", O_RDONLY);
            strcpy(buf, "HTTP/1.0 404 PageNotFound modelserver\nServer: Model HTTP "
                        "Server/0.1\nAllow: GET\nConnection: keep-alive\nContent-type: "
                        "text/html\nDate: ");
            time_t tm = time(NULL);
            strcat(buf, asctime(localtime(&tm)));
            strcat(buf, "\n\n");
            len = strlen(buf);
            send(clSocket, &buf, len, 0);
            while ((len = read(fd, buf, BUFLEN)) > 0)
                send(clSocket, &buf, len, 0);
            close(fd);
            shutdown(clSocket, 1);
            close(clSocket);
            return 404;
        }
        strcpy(buf, "HTTP/1.0 200 OK modelserver\nServer: Model HTTP Server/0.1\nAllow: "
                    "GET\nConnection: keep-alive\nDate: ");
        time_t tm = time(NULL);
        strcat(buf, asctime(localtime(&tm)));
        strcat(buf, "Content-length: ");
        char* cl = itoa(fcount(fd));
        strcat(buf, cl);
        delete[] cl;
        switch (cont_type)
        {
            case 0:
                strcat(buf, "\nContent-type: text/html\n\n");
                break;
            case 1:
                strcat(buf, "\nContent-type: image/jpeg\n\n");
                break;
            case 2:
                strcat(buf, "\nContent-type: text/plain\n\n");
                break;
        }
        len = strlen(buf);
        send(clSocket, &buf, len, 0);
        while ((len = read(fd, buf, BUFLEN)) > 0)
            send(clSocket, &buf, len, 0);
        close(fd);
        shutdown(clSocket, 1);
        close(clSocket);
        return 200;
    }
};

int main(int argc, char** argv)
{
    if (argc > 1)
        port = atoi(argv[1]);
    Server s(port);
    s.sbind();
    s.slisten();
    Client client;
    fd_set readset;
    timeval timeout;
    timeout.tv_sec = 600;
    timeout.tv_usec = 0;

    for (;;)
    {
        FD_ZERO(&readset);
        FD_SET(s.get_sock(), &readset);
        int activity, status;
        if (pids.get_len() == 0)
        {
            activity = select(s.get_sock() + 1, &readset, NULL, NULL, NULL);
        }
        else
        {
            timeout.tv_sec = 1;
            timeout.tv_usec = 0;
            activity = select(s.get_sock() + 1, &readset, NULL, NULL, &timeout);
        }
        if (activity < 0)
        {
            std::cerr << "Error with select\n";
            exit(4);
        }
        else if (activity > 0)
        {
            if (FD_ISSET(s.get_sock(), &readset))
            {
                fcntl(client.get_sock(), F_SETFL, O_NONBLOCK);
                client.cl_accept(s.get_sock());
                if (client.cl_request() == 1)
                    continue; // cgi
            }
        }
        for (int i = 0; i < pids.get_len(); i++)
        {
            if (waitpid(pids[i], &status, WNOHANG))
            {
                if (!(status))
                {
                    printf("OK\n");
                    char fname[11];
                    strcpy(fname, "p");
                    char* spid = itoa(pids[i]);
                    strcat(fname, spid);
                    delete[] spid;
                    char buf[512];
                    strcpy(buf, "HTTP/1.0 200 OK modelserver\nServer: Model HTTP "
                                "Server/0.1\nAllow: GET\nConnection: keep-alive\nDate: ");
                    time_t tm = time(NULL);
                    strcat(buf, asctime(localtime(&tm)));
                    strcat(buf, "Content-length: ");
                    int fd = open(fname, O_RDONLY);
                    char* cl = itoa(fcount(fd));
                    strcat(buf, cl);
                    delete[] cl;
                    strcat(buf, "\nContent-type: text/plain\n\n");
                    int len = strlen(buf);
                    send(client.get_sock(), &buf, len, 0);
                    while ((len = read(fd, buf, BUFLEN)) > 0)
                        send(client.get_sock(), &buf, len, 0);
                    close(fd);
                    remove(fname);
                }
                else
                {
                    std::cout << "notOK\n";
                    char buf[512];
                    strcpy(buf, "HTTP/1.0 500 notOK modelserver\nServer: Model HTTP "
                                "Server/0.1\nAllow: GET\nConnection: keep-alive\nContent-type: "
                                "text/html\nDate: ");
                    time_t tm = time(NULL);
                    strcat(buf, asctime(localtime(&tm)));
                    strcat(buf, "\n\n");
                    int len = strlen(buf);
                    send(client.get_sock(), &buf, len, 0);
                    int fd = open("cgierr.html", O_RDONLY);
                    while ((len = read(fd, buf, BUFLEN)) > 0)
                        send(client.get_sock(), &buf, len, 0);
                    close(fd);
                    char fname[11];
                    strcpy(fname, "p");
                    char* spid = itoa(pids[i]);
                    strcat(fname, spid);
                    delete[] spid;
                    remove(fname);
                }
                pids.del(pids[i]);
                shutdown(client.get_sock(), 1);
                close(client.get_sock());
            }
        }
    }
    return 0;
}
