/*#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include <iostream>
using namespace std;

#define PORTNUM 8080
#define BACKLOG 5
#define BUFLEN 80

#define FNFSTR "404 Error File Not Found"
#define BRSTR "Bad Request"

class Socket 
{
	int numSocket;
public:
	Socket() 
	{
		try
		{
			if ((numSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
			{
				throw 1;
			}
		}
		catch (int i) 
		{
			cout << "can't create socket\n";
		}
	}
	int getnum() const
	{
		return numSocket;
	}
};

class Address 
{
	struct sockaddr_in SockAddress;
public:
	Address() 
	{
		memset(&SockAddress, 0, sizeof(SockAddress));
	};

	Address(int i) 
	{
		memset(&SockAddress, 0, sizeof(SockAddress));
		SockAddress.sin_family = AF_INET;
		SockAddress.sin_addr.s_addr = INADDR_ANY;
		SockAddress.sin_port = htons(PORTNUM);
	};
	
	const struct sockaddr_in * getaddr() const
	{
		return &SockAddress;
	}	
};

class Server: public Socket, public Address {
public:
	Server(): Socket(), Address(3) {};
	
	void Bind() const
	{
		try
		{
			if (bind(getnum(),(struct sockaddr*) getaddr(), sizeof(*(getaddr())) ) < 0)
			{
				throw 3;
			}
		}
		
		catch (int i)
		{
			cout << "can't bind socket" << endl;
		}
	}
	
	void Listen() const
	{
		try
		{
			if (listen(getnum(), BACKLOG) < 0)
			{
				throw 4;
			}
		}
		
		catch (int i)
		{
			cout << "can't listen socket" <<endl;
		}
	}

};

class Client: public Address {
	socklen_t len;
	int sockfd;
public:
	Client(): Address() 
	{
		len = sizeof(*( getaddr() ));
	}

	void Accept(Server serv)
	{
		try
		{
			if ((sockfd = accept (serv.getnum(), (struct sockaddr*) getaddr(), &len)) < 0)
			{
				throw 3;
			}
		}

		catch (int i)
		{
			cout<<"error accepting connection"<<endl;
		}
	}

	int Request(char* buf, Server serv)
	{
		int len, i, filefd, p;
		try
		{
			if ((p = fork()) == -1)
			{
				throw serv;
			}
			else if (!p)
			{
				close(serv.getnum());
				if ((len = recv(sockfd, buf, BUFLEN, 0)) < 0)
				{
					throw 12;
				}
				if (strncmp(buf, "GET /", 5))
				{
					if (send(sockfd, BRSTR, strlen(BRSTR) + 1, 0) != strlen(BRSTR) + 1) 
					{
						throw 'a';
					}
					shutdown(sockfd, 1);
					close(sockfd);
					return 0;
				}
				for (i = 5; buf[i] && (buf[i] > ' '); i++);
				buf[i] = 0;
				if ((filefd = open(buf+5, O_RDONLY)) < 0)
				{
					if (send(sockfd, FNFSTR, strlen(FNFSTR) + 1, 0) != strlen(FNFSTR) + 1)
					{
						throw 'a';
					}
					shutdown(sockfd, 1);
					close(sockfd);
					return 0;
				}
				while ((len = read(filefd, &buf, BUFLEN)) > 0)
				{
					if (send(sockfd, buf, len, 0) < 0) 
					{
						throw 'a';
					}
				}
				close(filefd);
				shutdown(sockfd, 1);
				close(sockfd);
				return 0; 
			}
			close(sockfd);
		}

		catch (Server serv)
		{
			cout << "can't make a process" << endl;
		}
		catch (char x)
		{
			cout << "error writing socket" << endl;
		}
		catch (int x)
		{
			cout << "error reading socket" << endl;
		}
		return 0;
	}

	int getsock()
	{
		return sockfd;
	}

}; 



int main(int argc, char** argv) 
{
	char buf[BUFLEN];
	Server serv1;
	serv1.Bind();
	serv1.Listen();
	while(1)
	{
		Client newclient;
		newclient.Accept(serv1);
		newclient.Request(buf, serv1);
	}
	return 0;
}*/
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <arpa/inet.h>
#include <sstream>

#define BACKLOG 5
#define BUFLEN 80

class Server{
	int server_socket,client_socket;
	socklen_t client_address_len;
	struct sockaddr_in server_address,client_address;
public:
	Server(int port = 8080)
	{
		server_socket = socket(AF_INET,SOCK_STREAM,0);
		memset(&server_address, '0', sizeof(server_address));
		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(port);
		if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr)<=0) 
		{
			printf("\n inet_pton error\n");
			exit(1);
		}
	}
	
	void sbind()
	{
		if(bind(server_socket,(struct sockaddr*)&server_address,sizeof(server_address))<0){
			std::cerr<<"fatal error: can't bind\n";
			exit(1);
		}
	}
	
	void slisten()
	{
		if(listen(server_socket,BACKLOG)<0)
		{
			std::cerr<<"fatal error: cannot listen\n";
			exit(1);
		}
	}
	
	void saccept()
	{
		client_address_len = sizeof(client_address);
		client_socket = accept(server_socket,(struct sockaddr*) &client_address, &client_address_len);
		if (client_socket < 0)
		{
			std::cout<<"error: losed socked\n";
		}
	}
	void srequest()
	{
		int len;
		char buf[BUFLEN];
		std::stringstream response;
		std::stringstream response_body;
		if((len = recv(client_socket,&buf,BUFLEN,0))<0)
		{
			shutdown(client_socket,1);
			close(client_socket);
			std::cout<<"error: reading socket\n";
			exit(2);
		}
		if(strncmp(buf,"GET /",5)!=0)
		{
			shutdown(client_socket,1);
			close(client_socket);
			std::cout<<"error: BadRequest\n";
			return;
		}
		int i=5;
		while(buf[i] && (buf[i++]>' '));
		buf[i-1]='\0';
		std::cout<<"received="<< buf+5<<'\n';
		int fd;
		if((fd = open(buf+5,O_RDONLY))<0)
		{
			shutdown(client_socket,1);
			close(client_socket);
			response<<"HTTP/1.1 404 Not Found\n";
			len = send(client_socket, response.str().c_str(), response.str().length(), 0);
			std::cout<<"error: 404\n";
			shutdown(client_socket,1);
			close(client_socket);
			return;
		}
		while((len = read(fd,buf,BUFLEN))>0)
			for(int i=0; i<len; i++)
				response_body<<buf[i];
		response << "HTTP/1.1 200 OK\n"
        << "Version: HTTP/1.1\n"
        << "Content-Type: text/html; charset=UTF-8\n"
        << "Content-Length: " << response_body.str().length()
        << "\n\n"
        << response_body.str();
		len = send(client_socket, response.str().c_str(), response.str().length(), 0);
		close(fd);
		shutdown(client_socket,1);
		close(client_socket);
		return;
	}
};

int main(int argc, char ** argv)
{
	Server s(8080);
	s.sbind();
	s.slisten();
	for(;;)
	{
		s.saccept();
		s.srequest();	
	}
	return 0;
}
