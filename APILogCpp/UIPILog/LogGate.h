
#ifndef _LOGGATE_H_
#pragma once

#include "LogManager.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>
#include <mutex>
#include <ctime>
#include <iomanip>
#include <Windows.h>

using namespace std;

class LogManager;

class LogGate
{
	string idGate;
	std::mutex mutex_log;
	std::mutex mutex_buff;


	string getDateTimeFormated();
	void writeLog(string, string);

public:

	enum LOG_LEVEL :int
	{
		LOG_DISABLE = 0x0000,
		INFO = 0x0001,
		WARNING = 0x0002,
		FERROR = 0x0004,
		DEBUG_ON = 0x0008,
		NATIVE_CLIENT_DEBUG = 0x0018,
		TUNNEL_PROTOCOL_DEBUG = 0x0028,
		ASAI_PROTOCOL_DEBUG = 0x0048,
		REPOSITORY_DEBUG = 0x0088,
		ASAI_CLIENT_DEBUG = 0x0108,
		GENERAL_SERVICE_DEBUG = 0x0208
	};

	LogGate(string id);
	~LogGate();

	string getID();
	bool isDebugLevelActive(unsigned int level);
	
	void writeInfo(string);
	void writeWarn(string);
	void writeError(string);
	void writeDebug(unsigned int, string);
	void writeBytes(unsigned int, string,byte*,int);
};

#endif

