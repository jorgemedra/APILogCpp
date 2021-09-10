/***********************************************
UIPILog V1.0
Author: Jorge Omar Medra Torres.
************************************************/

#ifndef _UIPILOG_H_
#define _UIPILOG_H_

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
#include <atomic> //Since V1.2
#include <queue> //Since V1.2
#include <thread> //Since V1.2

#ifdef _WIN32
#define FILE_SEPARATOR	'\\'
#else
#define FILE_SEPARATOR	'/'
#endif


using namespace std;

namespace apilog
{
	class LogManager;
	class LogGate;

	enum LOG_LEVEL :int
	{
		LOG_DISABLE = 0x0000,
		INFO = 0x0001,
		WARNING = 0x0002,
		ERROR = 0x0004,
		SERVICE = 0x0008,
		DEBUG_LVL_1 = 0x0010,
		DEBUG_LVL_2 = 0x0020,
		DEBUG_LVL_3 = 0x0040,
		DEBUG_LVL_4 = 0x0080,		
		LOG_ALL = 0x03FFF
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
		atomic<bool> _kRunning; //Since V1.2
		atomic<bool> _qEmtpy; //Since V1.2
		queue<string> _qLog; //Since V1.2
		std::thread *pThread; //Since V1.2
		std::mutex mtx_queue;

		int findCurrentIndex(string path, bool*closeCurrent);
		bool existFile(string path);
		long getFileSize(string path);
		int getIndexOfFile(string path);
		
		void _writeToLog(string message); //Sinve V1.2, from public to private

	public:

		static LogManager* getLogManger();

		void waitForLogs(int dummy); //Since V1.2

		bool initLog(string name, string path, int filesBackup, int fileSize, string format, string title);
		void stop();
		void setLogLevel(unsigned int level);
		unsigned int getLogLevel();
		string getDateFormat();
		void writeToLog(string message);
		void forceToWriteToLog(string message);
		string getStatus();
	};


	class LogGate
	{
		string idGate;
		std::mutex mutex_log;
		std::mutex mutex_buff;


		string getDateTimeFormated();
		void writeLog(string, string, bool force=false);

	public:

		LogGate(string id);
		~LogGate();

		string getID();
		void setLogLevel(unsigned int level);
		bool isDebugLevelActive(unsigned int level);

		void writeCritialMessage(string message);
		void writeInfo(string message);
		void writeWarn(string message);
		void writeError(string message);
		void writeDebug(unsigned int level, string message);
		void writeBytes(unsigned int level, string message, char* buffer, int size);

		string getStatus();
	};
}//namepace

#endif