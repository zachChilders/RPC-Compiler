#include "TCPServer.hpp"

TCPServer::TCPServer()
{
	this->recvbuf = std::unique_ptr<byte>(new byte[DEFAULT_BUFLEN]);
	running = true;
	clientSocket = INVALID_SOCKET;

	//Init winsock and hints
	res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res != 0)
	{
		std::cerr << "WSAStartup Failed: " << res << std::endl;
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;


	//Resolve server address and port
	res = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (res != 0)
	{
		std::cerr << "getaddrinfo failed: " << res << std::endl;
		WSACleanup();
		return;
	}

}

TCPServer::~TCPServer()
{
	res = shutdown(clientSocket, SD_SEND);
	closesocket(clientSocket);
	WSACleanup();
}

void TCPServer::open()
{
	clientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cerr << "Error at socket: " + WSAGetLastError() + '\n';
		freeaddrinfo(result);
		WSACleanup();
		return;
	}
}

void TCPServer::bindSocket()
{
	res = bind(clientSocket, result->ai_addr, result->ai_addrlen);
	{
		if (res == SOCKET_ERROR)
		{
			std::cerr << "Bind failed with error: " + WSAGetLastError() + '\n';
			freeaddrinfo(result);
			closesocket(clientSocket);
			WSACleanup();
			return;
		}
	}

	freeaddrinfo(result);
}

void TCPServer::listenOnSocket()
{
	if (listen(clientSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cerr << "Listen failed with error: " + WSAGetLastError() + '\n';
		freeaddrinfo(result);
		closesocket(clientSocket);
		WSACleanup();
		return;
	}

	SOCKADDR_IN client_info = { 0 };
	int addrsize = sizeof(client_info);

	connectSocket = accept(clientSocket, (struct sockaddr*)&client_info, &addrsize);
	if (connectSocket == INVALID_SOCKET)
	{
		std::cerr << "Accept failed; " + WSAGetLastError() + '\n';
		closesocket(clientSocket);
		WSACleanup();
		return;
	}
	closesocket(clientSocket);
	addrsize = sizeof(client_info);
	getpeername(connectSocket, (struct sockaddr*)&client_info, &addrsize);

	ipAddress = inet_ntoa(client_info.sin_addr);

	//std::cout << "Connection accepted" << std::endl;
}
/*
void TCPServer::work()
{
	//Work loop - recieves data and echoes it back.
	//do {
	res = recv(connectSocket, (char*)recvbuf.get(), DEFAULT_BUFLEN, 0);
		if (res > 0){
			std::cout << "Bytes recieved: " << res << "\n";

			for (int i = 0; i < res; i++)
			{
				std::cout << recvbuf.get()[i];
			}
			std::cout << std::endl;
			//Echo back to the sender
			res = send(connectSocket, recvbuf, res, 0);
			if (res == SOCKET_ERROR){
				std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
				closesocket(connectSocket);
				WSACleanup();
				return;
			}
			std::cout << "Bytes sent: " << res << std::endl;

		}
		else if (res == 0){
			std::cout << "Connection closing...." << std::endl;
			closesocket(connectSocket);
			
		}
		else{
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;
			closesocket(connectSocket);
			return;
		}
	//} while (res > 0);
}*/

tcp_error_t TCPServer::start()
{
	open();
	bindSocket();
	listenOnSocket();
	//work();
	/*while (running)
	{
		listenOnSocket();
		//work();
	}*/
	return SUCCESS;
}

tcp_error_t TCPServer::stop()
{
	closesocket(clientSocket);
	running = false;
	return SUCCESS;
}