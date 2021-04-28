/***********************************************
APILog V2.0
Author: Jorge Omar Medra Torres.
************************************************/

#ifndef _APILOG_CPP_H_
#define _APILOG_CPP_H_

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <sstream>
#include <cstring>
#include <mutex>
#include <fstream>
#include <ctime>
#include <iomanip>


/*
#ifdef _WIN32
//#include <Windows.h>
#define FILE_SEPARATOR	'\\'
#else
#define FILE_SEPARATOR	'/'
#endif
*/

using namespace std;

namespace apilog
{
	class LogManager;
	class LogGate;

	enum LOG_LEVEL : unsigned int
	{
		LOG_DISABLE = 0x0000,
		LOG_ALL 	= 0xFFFF,
		INFO 		= 0x0001,
		WARNING 	= 0x0002,
		ERROR 		= 0x0004,
		DEBUG_ON 	= 0x0018,
		DEBUG_LVL_1 = 0x0010,
		DEBUG_LVL_2 = 0x0020,
		DEBUG_LVL_3 = 0x0040,
		DEBUG_LVL_4 = 0x0080
	};

	string logLevelKey(unsigned int level);

	class LogManager
	{
		LogManager();
		~LogManager();

		enum LOG_STATE :int
		{
			NO_INIT = 0,
			APP_START,
			WRITING
		};


		int bInitLog;

		string FILE_NAME;
		string FILE_PATH;
		unsigned int LOG_MASK;
		int MAX_FILE_BACKUP_COUNT;
		long MAX_FILE_SIZE;
		string DATE_FORMAT;
		string LOG_HEADER;

		int idMaxSize;
		int currentIndex;
		std::mutex mtx_log;

		int findCurrentIndex(string path, bool*closeCurrent);
		bool existFile(string path);
		long getFileSize(string path);
		int getIndexOfFile(string path);

	public:

		static LogManager* getLogManger();

		bool initLog(string name, string path, int filesBackup, int fileSize, string format, string title);
		void setLogLevel(unsigned int level);
		unsigned int getLogLevel();
		string getDateFormat();
		void writeToLog(string message);
	};


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

		void writeInfo(string message);
		void writeWarn(string message);
		void writeError(string message);
		void writeDebug(LOG_LEVEL level, string message);
		void writeBytes(LOG_LEVEL level, string message, char* buffer, int size);
	};
}//apilog
#endif