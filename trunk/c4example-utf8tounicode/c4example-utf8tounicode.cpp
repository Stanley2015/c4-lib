// c4test.cpp : Defines the entry point for the console application.
//
#pragma once

#include "stdafx.h"
#include <string>
#include <fstream>
#include <iostream>
#include "../c4/c4encode.h"
#ifdef _DEBUG
#pragma comment(lib, "../Debug/c4.lib")
#else
#pragma comment(lib, "../Release/c4.lib")
#endif

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	string filename = "../testfiles/utf-8/wukongzhuang-utf-8.txt";
	ifstream infile(filename.c_str(), ios::in|ios::binary);
	if (!infile)
	{
		cerr<<"unable to open input file!\n";
		return -1;
	}
	ofstream outfile("../testfiles/out/wukongzhuang-unicode.txt", ios::binary);
	if (!outfile)
	{
		cerr<<"unable to open output file\n";
		infile.close();
		return -1;
	}

	infile.seekg(0, ios::end);
	int length = infile.tellg();
	char * stringBuffer = new char[length+1];
	memset((void*)stringBuffer, 0, length+1);
	infile.seekg(0, 0);
	infile.read(stringBuffer, length);

	outfile.write(CC4Encode::LITTLEENDIAN_BOM, 2);
	// or
	// outfile.write((char*)&(CC4Encode::LITTLEENDIAN_MARK),2);

	if (CC4EncodeUTF8::_match(stringBuffer, length))
	{
		cout<<"utf-8 matched!"<<endl;

		// output
		/*
		wstring resultBuffer = CC4EncodeUTF8::convert2unicode(stringBuffer, length);
		cout<<"length: "<<resultBuffer.length()<<endl;
		const wchar_t *p = resultBuffer.c_str();
		unsigned int resultLength = resultBuffer.length();
		outfile.write((const char*)p, resultLength*sizeof(wchar_t));
		*/
		// another method to convet
		unsigned int resultLenth = CC4EncodeUTF8::calcUnicodeStringLength(stringBuffer, length);
		wchar_t *resultBuffer = new wchar_t[resultLenth];
		CC4EncodeUTF8::convert2unicode(stringBuffer, length, resultBuffer, resultLenth,false);
		cout<<"length: "<<resultLenth<<endl;
		outfile.write((char*)resultBuffer, resultLenth*sizeof(wchar_t));
		delete []resultBuffer;
	}
	
	infile.close();
	outfile.close();
	delete []stringBuffer;

	CC4EncodeUTF8 *utf8Encode = CC4EncodeUTF8::getInstance();
	if (utf8Encode != NULL)
	{
		cout<<"address:"<<(int)utf8Encode<<endl;
		utf8Encode = CC4EncodeUTF8::getInstance();
		cout<<"address:"<<(int)utf8Encode<<endl;
		// Do not need to delete the instance!
		cout<<"get UTF-8 Encode instance success!"<<endl;
		utf8Encode = NULL;
	}

	return 0;
}

