
#ifndef _LOGMANAGER_H_

#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <cstring>
#include <map>
#include <mutex>
#include <fstream>

#include "LogGate.h"

#ifdef _WIN32
#define FILE_SEPARATOR	'\\'
#else
#define FILE_SEPARATOR	'/'
#endif


using namespace std;

class LogGate;

class LogManager
{
	LogManager();
	~LogManager();
	
	enum LOG_STATE:int
	{
		NO_INIT = 0,
		APP_START,
		WRITING
	};


	static int bInitLog;

	static string FILE_NAME;
	static string FILE_PATH;
	static unsigned int LOG_MASK;
	static int MAX_FILE_BACKUP_COUNT;
	static long MAX_FILE_SIZE;
	static string DATE_FORMAT;
	static string LOG_HEADER;

	static int idMaxSize;
	static int currentIndex;
	static map<string, LogGate*> gates;
	static std::mutex mtx_log;

	static bool existFile(string);
	static long getFileSize(string);
	static int LogManager::getIndexOfFile(string path);

public:

	static bool initLog(string , string , int , int , string , string);
	static LogGate* createGate(string);
	static bool destroyGate(LogGate*);
	static void setLogLevel(unsigned int);
	static unsigned int getLogLevel();
	static string getDateFormat();
	static void writeToLog(string);
};

#endif