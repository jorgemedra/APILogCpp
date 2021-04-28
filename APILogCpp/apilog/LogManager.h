
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
#include <queue>
#include <thread>
#include <future>

#include "LogManager.h"

#ifdef _WIN32
#define FILE_SEPARATOR	'\\'
#else
#define FILE_SEPARATOR	'/'
#endif


using namespace std;

namespace apilog
{
	class LogGate;

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

		std::unique_ptr<thread> tQueue;
		std::mutex mtx_queue;
		std::condition_variable cvLog;
		std::queue<string> qLog;
		std::atomic<bool> bRun;
		bool _empty();
		void _queueProccess();
		void _enqueue(string message);
		string _dequeue();
		void _writeToLog(string message);

	public:

		static LogManager* getLogManger();

		bool initLog(string name, string path, int filesBackup, int fileSize, string format, string title);
		void setLogLevel(unsigned int level);
		unsigned int getLogLevel();
		string getDateFormat();
		void writeToLog(string message);
	};
}
#endif