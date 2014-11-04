#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <zlib.h>
#include <queue>
#include <tuple>
#include <Windows.h>


typedef enum MODE{RAW=0, COMPRESSED=2, ENCRYPTED=4};
class Preprocessor
{
	public:
		Preprocessor(std::string path);
		~Preprocessor(){};
		Byte* getBytes(MODE m);
	private:
		Byte* bytes;
		void compress(std::fstream *file);
		void compressFile(std::string path);

		void decompressFile(std::string path);
		Byte* encrypt(Byte* data);
		Byte* decrypt(Byte* data);

		std::tuple<std::queue<std::string>, std::queue<unsigned long>> listFiles(std::string path);
};