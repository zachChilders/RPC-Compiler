#include "TCPServer.hpp"

TCPServer::TCPServer()
{
	running = true;
	connectSocket = INVALID_SOCKET;
	connectSocket = INVALID_SOCKET;

	//Init winsock and hints
	res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res != 0)
	{
		std::cout << "WSAStartup Failed: " << res << std::endl;
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
		std::cout << "getaddrinfo failed: " << res << std::endl;
		WSACleanup();
		return;
	}

}

TCPServer::~TCPServer()
{
	res = shutdown(connectSocket, SD_SEND);
	closesocket(connectSocket);
	WSACleanup();
}

void TCPServer::open()
{
	connectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (connectSocket == INVALID_SOCKET)
	{
		std::cout << "Error at socket: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return;
	}
}

void TCPServer::bindSocket()
{
	res = bind(connectSocket, result->ai_addr, result->ai_addrlen);
	{
		if (res == SOCKET_ERROR)
		{
			std::cout << "Bind failed with error: " << WSAGetLastError() << std::endl;
			freeaddrinfo(result);
			closesocket(connectSocket);
			WSACleanup();
			return;
		}
	}

	freeaddrinfo(result);
}

void TCPServer::listenOnSocket()
{
	if (listen(connectSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		std::cout << "Listen failed with error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		closesocket(connectSocket);
		WSACleanup();
		return;
	}

	connectSocket = accept(connectSocket, NULL, NULL);
	if (connectSocket == INVALID_SOCKET)
	{
		std::cout << "Accept failed; " << WSAGetLastError() << std::endl;
		closesocket(connectSocket);
		WSACleanup();
		return;
	}
	std::cout << "Connection accepted" << std::endl;
}

void TCPServer::work()
{
	//Work loop - recieves data and echoes it back.
	do {
		res = recv(connectSocket, &recvbuf[0], DEFAULT_BUFLEN, 0);
		if (res > 0){
			std::cout << "Bytes recieved: " << res << "\n";

			//Echo back to the sender
			res = send(connectSocket, &recvbuf[0], res, 0);
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
		}
		else{
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;
			closesocket(connectSocket);
			WSACleanup();
			return;
		}
	} while (res > 0);
}

tcp_error_t TCPServer::start()
{
	std::thread t(&TCPServer::__start__, this);
	t.join();
	return SUCCESS;
}

tcp_error_t TCPServer::stop()
{
	closesocket(connectSocket);
	running = false;
	return SUCCESS;
}

void TCPServer::__start__(void)
{
	open();
	bindSocket();
	while (running)
	{
		listenOnSocket();
		work();
	}

}