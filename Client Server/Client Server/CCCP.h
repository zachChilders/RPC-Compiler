#pragma once
#include <string>
#include <vector>
#include <memory>
#include "TCPSocket.hpp"
#include <aes.h>

class CCCP
{
protected:
	bool started;

	std::unique_ptr<TCPSocket> connection;

	CryptoPP::SecByteBlock aesKey;
	byte aesIV[CryptoPP::AES::DEFAULT_KEYLENGTH];

	void send(std::string message);
	std::string receive();

	CCCP();

public:
	enum State{ client, server };

	static CCCP* Create(State type);

	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void command(std::string command);

	virtual void compile() = 0;
};