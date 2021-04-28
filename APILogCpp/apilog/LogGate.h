
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

//#ifdef _WIN32
//#include <Windows.h>
//#endif

using namespace std;

namespace apilog
{
	class LogManager;
	enum LOG_LEVEL : unsigned int
	{
		LOG_DISABLE = 0x0000,
		LOG_ALL 	= 0xFFFF,
		INFO 		= 0x0001,
		WARNING 	= 0x0002,
		ERROR 		= 0x0004,
		DEBUG_ON 	= 0x0018,
		DEBUG_LVL_1	= 0x0010,
		DEBUG_LVL_2 = 0x0020,
		DEBUG_LVL_3 = 0x0040,
		DEBUG_LVL_4 = 0x0080
	};

	string logLevelKey(unsigned int level);

	class LogGate
	{
		string idGate;
		std::mutex mutex_log;
		std::mutex mutex_buff;

		string getDateTimeFormated();
		void writeLog(string, string);
		
	public:

		

		LogGate(string id);
		~LogGate();

		string getID();
		void setLogLevel(unsigned int level);
		bool isLogLevelActive(unsigned int level);

		void writeInfo(string);
		void writeWarn(string);
		void writeError(string);
		void writeDebug(LOG_LEVEL , string);
		void writeBytes(LOG_LEVEL , string, char*,int);
	};
} //apilog
#endif

