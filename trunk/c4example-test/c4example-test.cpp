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
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

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
	/*
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
	*/

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
	// no good method.
	_CrtDumpMemoryLeaks();

	return 0;
}

