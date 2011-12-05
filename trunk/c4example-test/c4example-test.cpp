/************************************************************************/
/*                                                                      */
/* c4-lib example                                                       */
/* Memory Testing                                                       */
/*                                                                      */
/* Version: 0.1                                                         */
/* Author:  wei_w (weiwl07@gmail.com)                                   */
/* Distributed under GPLv3                                              */
/*                                                                      */
/* Project URL: http://code.google.com/p/c4-lib                         */
/*                                                                      */
/* Copyright 2011 wei_w                                                 */
/*                                                                      */
/************************************************************************/
// c4example-test.cpp : Defines the entry point for the console application.
//

#pragma  once

#if defined( WIN32 ) && defined( TUNE )
#include <crtdbg.h>
_CrtMemState startMemState;
_CrtMemState endMemState;
#endif

#include <string>
#include <fstream>
#include <iostream>
#include "stdafx.h"
#include "../c4/c4context.h"
#ifdef _DEBUG
#pragma comment(lib, "../Debug/c4.lib")
#else
#pragma comment(lib, "../Release/c4.lib")
#endif

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{

#if defined( WIN32 ) && defined( TUNE )
	_CrtMemCheckpoint( &startMemState );
#endif	

	{
		wstring charmap = L"charmap.xml";
		CC4Context context(charmap, L"../charmaps/");
		if (!context.init())
		{
			cout<<"init failed. check charmap."<<endl;
			wcout<<context.getLastErrorMessage()<<endl;
			return 0;
		}
		cout<<"load charmap.xml success."<<endl;
		cout<<"amount of charmaps in charmap.xml:"<<context.getEncodeAmount()<<endl;
		list<wstring> &names = context.getEncodesNameList();
		list<wstring>::iterator name_iter;
		for (name_iter = names.begin(); name_iter != names.end(); ++name_iter)
		{
			wcout<<*name_iter<<endl;
		}
		cout<<endl;
		context.finalize();
	}
	
	{
		CC4Context *newContext = new CC4Context(L"charmap-chinese.xml", L"../charmaps/");
		if (!newContext->init())
		{
			cout<<"init failed. check charmap."<<endl;
			wcout<<newContext->getLastErrorMessage()<<endl;
			return 0;
		}
		cout<<"load charmap-chinese success."<<endl;
		cout<<"amount of charmaps in charmap-chinese:"<<newContext->getEncodeAmount()<<endl;
		list<wstring> &chn_names = newContext->getEncodesNameList();
		list<wstring>::iterator chnname_iter;
		for (chnname_iter = chn_names.begin(); chnname_iter != chn_names.end(); ++chnname_iter)
		{
			wcout<<*chnname_iter<<endl;
		}
		cout<<endl;
		newContext->finalize();
		delete newContext;
	}

	// CC4EncodeUTF16 and CC4EncodeUTF8 static instances will be deleted automatically.
	/*
	{
		CC4EncodeUTF16* instance_utf16 = CC4EncodeUTF16::getInstance();
		CC4EncodeUTF8*  instance_utf8  = CC4EncodeUTF8::getInstance();
		delete instance_utf16;
		delete instance_utf8;
	}
	_CrtDumpMemoryLeaks();
	*/

#if defined( WIN32 ) && defined( TUNE )
	_CrtMemCheckpoint( &endMemState );
	//_CrtMemDumpStatistics( &endMemState );

	_CrtMemState diffMemState;
	_CrtMemDifference( &diffMemState, &startMemState, &endMemState );
	_CrtMemDumpStatistics( &diffMemState );
#endif

	return 0;
}

