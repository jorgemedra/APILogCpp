/***********************************************
UIPILog V1.0
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
#include <Windows.h>

#ifdef _WIN32
#define FILE_SEPARATOR	'\\'
#else
#define FILE_SEPARATOR	'/'
#endif


using namespace std;

class LogManager;
class LogGate;

enum LOG_LEVEL :int
{
	LOG_DISABLE = 0x0000,
	INFO = 0x0001,
	WARNING = 0x0002,
	FERROR = 0x0004,
	DEBUG = 0x0008
};

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
	bool isDebugLevelActive(unsigned int level);

	void writeInfo(string message);
	void writeWarn(string message);
	void writeError(string message);
	void writeDebug(unsigned int level, string message);
	void writeBytes(unsigned int level, string message, byte* buffer, int size);
};

#endif