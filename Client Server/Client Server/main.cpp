#include "TCPServer.hpp"
#include "TCPClient.h"
#include "preprocessor.h"
#include "aes.h"
#include <fstream>
#include <stdlib.h>
#include <zlib.h>
#include "database.h"


int main()
{
	//Server Test 
	/*TCPServer s;+
	s.start();
	*/
	//Client Test
	//TCPClient c("172.22.1.82");
	//c.start();
	//c.stop();

	//s.stop();

	/*Preprocessor p;
	std::tuple<std::queue<std::string>, std::queue<unsigned long>> tup = p.listFiles("./");
	p.compressDir(tup, (MODE)0);*/
	//p.decompressDir("./Debug/!zipped/bytes.!zp");
	database d;
	bool err = d.addUser("tylor", "password");
	err = d.removeUser("tylor");

	err = d.addCompiler("gcc");
	err = d.removeCompiler("gcc");

	err = d.addCompiler("gcc");
	err = d.addFlag("gcc", "-f", true);
	err = d.removeFlag("gcc", "-f");

	err = d.addUser("tylor", "password");
	err = d.addSetting("compile", "gcc -compilethis bitch.c", "tylor");
	err = d.removeSetting("compile", "tylor");
	std::string s = d.login("tylor", "password");
}