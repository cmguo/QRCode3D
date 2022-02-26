// QRCode3D.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "QRCodeModel.h"

#include <BarcodeFormat.h>
#include <CharacterSet.h>
#include <CharacterSetECI.h>
#include <MultiFormatWriter.h>
#include <TextUtfEncoding.h>
#include <BitMatrix.h>

#include <iostream>
#include <fstream>

using namespace ZXing;

static void PrintUsage(const char* exePath)
{
	std::cout << "Usage: " << exePath << " [-blockSize <blockSize>] [-padding <padding>] [-encoding <encoding>] [-format <format>] [-ecc <level>] <text> <output>\n"
		<< "    -blockSize Block Size of generated model\n"
		<< "    -height	   Height of generated model\n"
		<< "    -padding   Padding around barcode\n"
		<< "    -encoding  Encoding used to encode input text\n"
		<< "    -ecc       Error correction level, [0-8]\n"
		<< "\n"
		<< "Supported formats are:\n";
	for (auto f : BarcodeFormats::all()) {
		if (((int) f & (int)BarcodeFormat::TwoDCodes) != 0)
			std::cout << "    " << ToString(f) << "\n";
	}
	std::cout << "Format can be lowercase letters, with or without '-'.\n";
}

static bool ParseSize(std::string str, int* width, int* height)
{
	std::transform(str.begin(), str.end(), str.begin(), [](char c) { return (char)std::tolower(c); });
	auto xPos = str.find('x');
	if (xPos != std::string::npos) {
		*width = std::stoi(str.substr(0, xPos));
		*height = std::stoi(str.substr(xPos + 1));
		return true;
	}
	return false;
}

static bool ParseOptions(int argc, char* argv[], double & blockSize, size_t& padding, double& height, CharacterSet& encoding,
	int& eccLevel, BarcodeFormat& format, std::map<std::string, int> & configs, std::string& text, std::string& filePath)
{
	int nonOptArgCount = 0;
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-blockSize") == 0) {
			if (++i == argc)
				return false;
			blockSize = std::stod(argv[i]);
		}
		else if (strcmp(argv[i], "-padding") == 0) {
			if (++i == argc)
				return false;
			padding = std::stoi(argv[i]);
		}
		else if (strcmp(argv[i], "-height") == 0) {
			if (++i == argc)
				return false;
			height = std::stod(argv[i]);
		}
		else if (strcmp(argv[i], "-ecc") == 0) {
			if (++i == argc)
				return false;
			eccLevel = std::stoi(argv[i]);
		}
		else if (strcmp(argv[i], "-encoding") == 0) {
			if (++i == argc)
				return false;
			encoding = CharacterSetECI::CharsetFromName(argv[i]);
		}
		else if (strcmp(argv[i], "-format") == 0) {
			if (++i == argc)
				return false;
			format = BarcodeFormatFromString(argv[i]);
			if (format == BarcodeFormat::None) {
				std::cerr << "Unrecognized format: " << argv[i] << std::endl;
				return false;
			}
			if (((int)format & (int)BarcodeFormat::TwoDCodes) == 0) {
				std::cerr << "Not 2D format: " << argv[i] << std::endl;
				return false;
			}
		}
		else if (strncmp(argv[i], "-", 1) == 0) {
			if (++i == argc)
				return false;
			configs.emplace(argv[i - 1] + 1, std::atoi(argv[i]));
		}
		else if (nonOptArgCount == 0) {
			text = argv[i];
			++nonOptArgCount;
		}
		else if (nonOptArgCount == 1) {
			filePath = argv[i];
			++nonOptArgCount;
		}
		else {
			return false;
		}
	}

	return nonOptArgCount > 0;
}

static std::string GetExtension(const std::string& path)
{
	auto fileNameStart = path.find_last_of("/\\");
	auto fileName = fileNameStart == std::string::npos ? path : path.substr(fileNameStart + 1);
	auto extStart = fileName.find_last_of('.');
	auto ext = extStart == std::string::npos ? "" : fileName.substr(extStart + 1);
	std::transform(ext.begin(), ext.end(), ext.begin(), [](char c) { return std::tolower(c); });
	return ext;
}

void PrintCode(std::vector<std::vector<unsigned char>> const& bytes)
{
	printf("\033[107m");
	for (int i = 0; i < bytes.front().size(); ++i)
		printf("  ");
	printf("    \033[40m\n");
	for (int i = 0; i < bytes.size(); ++i) {
		printf("\033[107m  \033[40m");
		for (auto b : bytes[i])
			printf(b ? "  " : "\033[107m  \033[40m");
		printf("\033[107m  \033[40m\n");
	}
	printf("\033[107m");
	for (int i = 0; i < bytes.front().size(); ++i)
		printf("  ");
	printf("    \033[40m\n");
}

int main(int argc, char * argv[])
{
	size_t padding = 1;
	double blockSize = 2.0;
	double height = 4.0;

	int eccLevel = -1;
	std::string text, filePath;
	CharacterSet encoding = CharacterSet::Unknown;
	BarcodeFormat format = BarcodeFormat::QRCode;
	std::map<std::string, int> configs;

	if (!ParseOptions(argc, argv, blockSize, padding, height, encoding, eccLevel, format, configs, text, filePath)) {
		PrintUsage(argv[0]);
		return -1;
	}

	if (filePath.empty()) {
		filePath = "test.stl";
	}

	std::vector<std::vector<unsigned char>> bytes;
	auto writer = MultiFormatWriter(format).setMargin(0).setEncoding(encoding).setEccLevel(eccLevel);
	auto bits = writer.encode(TextUtfEncoding::FromUtf8(text), 0, 0, configs);
	for (int i = 0; i < bits.height(); ++i) {
		auto row = bits.row(i);
		bytes.push_back(std::vector<unsigned char>(row.begin(), row.end()));
	}
	PrintCode(bytes);

	QRCodeModel m("test");
    m.generate(bytes, padding, blockSize, height);

    std::ofstream ofs(filePath);
    m.writeToStl(ofs);
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
