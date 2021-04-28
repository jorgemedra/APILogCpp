#include "LogManager.h"

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
#include <functional>
#include <sys/stat.h>

#ifdef WIN32
#define stat _stat
#endif

#ifndef API_LOG_VERSION
#define API_LOG_VERSION "API LOG CPP V2.0 (27 Abril 2021)\n\tAuthor: Jorge Omar Medra Torres\n\thttps://github.com/jorgemedra"
#endif

using namespace apilog;

LogManager* LogManager::getLogManger()
{
	static LogManager myself;
	return &myself;
}

LogManager::LogManager()
{
	bInitLog = LOG_STATE::NO_INIT;
	currentIndex = 1;
	bRun = true;
}

LogManager::~LogManager()
{	
	if(bRun)
	{	
		bRun = false;
		tQueue->join();
	}
	cout << "\nLogManager has been destroyed\n";
}


bool LogManager::initLog(string name, string path, int filesBackup, int fileSize, string format, string title)
{
	if (bInitLog != LOG_STATE::NO_INIT)
		return false;

	FILE_NAME = name;
	FILE_PATH = path;
	MAX_FILE_BACKUP_COUNT = filesBackup;
	
	if(fileSize <= 0)
		MAX_FILE_SIZE = 10000000; //10 MB by default size.
	else
		MAX_FILE_SIZE = fileSize * 1000000;

	DATE_FORMAT = format;
	LOG_HEADER = title;

	LOG_MASK = 0x0000;
	bInitLog = LOG_STATE::APP_START;

	tQueue = std::make_unique<thread>(bind(&LogManager::_queueProccess, this));

	return true;
}

void LogManager::setLogLevel(unsigned int level)
{
	LOG_MASK = level;
}

string LogManager::getDateFormat()
{
	return DATE_FORMAT;
}

unsigned int LogManager::getLogLevel()
{
	return LOG_MASK;
}

void LogManager::_writeToLog(string message)
{
	//unique_lock<mutex> lckLog(mtx_log);
	static bool started = false;

	fstream f;
	bool isNew = false;
	bool closeIt = false;
	

	stringstream currentLog;
	currentLog << FILE_PATH << FILE_SEPARATOR << FILE_NAME;

	//The process of evalueate the current file only can be made at the first input.
	if (!started)
	{
		currentIndex = findCurrentIndex(currentLog.str(), &closeIt);
		started = true;
		isNew = true;
	}
	else
	{
		//If the file exist, it has to evaluete the size to check if it close the current file.
		stringstream ifile;
		ifile << currentLog .str() << "_" << currentIndex << ".log";
		long fsize = getFileSize(ifile.str());

		if (fsize >= MAX_FILE_SIZE)
			closeIt = true;
	}

	//Open the Current log.
	currentLog << "_" << currentIndex << ".log\0";
	f.open(currentLog.str().c_str(), ios::out | ios::app); 

	//if the current file must be close
	if (closeIt)
	{

		if (++currentIndex > MAX_FILE_BACKUP_COUNT)
			currentIndex = 1;

		f << "The log will continue in file [" << FILE_PATH << FILE_SEPARATOR << FILE_NAME << currentIndex << ".log" << "]..." << endl;
		f.close();

		//Open the new log, if its exist will be reseted.
		stringstream newLog;
		newLog << FILE_PATH << FILE_SEPARATOR << FILE_NAME << "_" << currentIndex << ".log";
		f.open(newLog.str().c_str(), ios::out); //overwrite the file, if it exist.

		isNew = true;
	}


	if (isNew)
	{	
		if (f.is_open())
		{
			f << API_LOG_VERSION << endl;
			f << "LOG FILE WITH INDEX :[" << currentIndex << "]" << endl;
			f << LOG_HEADER << endl;
		}
	}

	if (f.is_open()) {
		f << message;
		f.close();
	}
}

int LogManager::findCurrentIndex(string path, bool*closeCurrent)
{
	int index = 0;
	time_t ytm = 0;
	struct stat attrib;
	bool isNewFile = false;

	*closeCurrent = false;
	
	for (int i = MAX_FILE_BACKUP_COUNT; i > 0; i--)
	{
		time_t tm;
		stringstream ifile;

		ifile << path << "_" << i << ".log";

		if (existFile(ifile.str()))
		{
			isNewFile = false;
			stat(ifile.str().c_str(), &attrib);

			tm = attrib.st_mtime; //mtime = last modified
			if (i == MAX_FILE_BACKUP_COUNT || tm > ytm)
			{
				ytm = tm;
				index = i;
			}
		}//if (existFile(ifile.str()))
		else
		{
			index = i;
			isNewFile = true;
		}
	}

	if (!isNewFile)
	{
		//If the file exist, it has to evaluete the size to check if it close the current file.
		stringstream ifile;
		ifile << path << "_" << index << ".log";
		long fsize = getFileSize(ifile.str());

		if (fsize >= MAX_FILE_SIZE)
			*closeCurrent = true;
	}

	return index;
}

bool LogManager::existFile(string path)
{
	struct stat attributos;
	int rstat;

	rstat = stat(path.c_str(), &attributos);

	if (rstat == 0)
		return true;

	return false;
}

long LogManager::getFileSize(string path)
{
	//long begin, end;
	streamoff begin, end;

	ifstream f(path.c_str(), ios::in | ios::binary);

	begin = f.tellg();
	f.seekg(0, ios::end);
	end = f.tellg();
	f.close();
	
	return (long)(end - begin);
}

int LogManager::getIndexOfFile(string path)
{
	int index = 1;
	bool bNewIndex = false;

	//Check if there log files created.
	for (int i = MAX_FILE_BACKUP_COUNT; i >= 0; i--)
	{
		if (i == 0) //There is no files created.
			return 1;

		stringstream pathAux;
		pathAux << path << "_" << i << ".log";
		
		index = i;
		if (existFile(pathAux.str()))
			break;
	}

	//If The app is starting, leave the las log file create a new file..
	if (bInitLog == LOG_STATE::APP_START)
		bNewIndex = true;
	else
	{
		stringstream pathAux;
		pathAux << path << "_" << index << ".log";
		long size = getFileSize(pathAux.str());

		if(size >= MAX_FILE_SIZE)
			bNewIndex = true;
	}

	//Calcula el siguiente indice.
	if (bNewIndex)
	{
		if (index == MAX_FILE_BACKUP_COUNT)
			index = 1;
		else
			index++;
	}

	return index;
}

void LogManager::_queueProccess()
{
	unique_lock<mutex> lckProc(mtx_log);

	while(bRun)
	{
		if(_empty())
			cvLog.wait(lckProc);

		while(!_empty())
			_writeToLog(_dequeue());
	}
}

void LogManager::writeToLog(string message)
{
	_enqueue(message);
	cvLog.notify_all();
}

bool LogManager::_empty()
{
	unique_lock<mutex> lckQ(mtx_queue);
	return qLog.empty();
}

void LogManager::_enqueue(string message)
{
	unique_lock<mutex> lckQ(mtx_queue);
	qLog.push(message);
}

string LogManager::_dequeue()
{
	unique_lock<mutex> lckQ(mtx_queue);
	string out = qLog.front();
	qLog.pop();
	return out;
}
