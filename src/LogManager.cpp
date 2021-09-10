#include "APILogCpp.h"
#include <filesystem>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <sys/stat.h>

#ifndef UIPI_LOG_VERSION
#define UIPI_LOG_VERSION "UIPI LOG V1.3 (Jan 12th 2018)"
#endif

using namespace std;
using namespace apilog;


LogManager* LogManager::getLogManger()
{
	static LogManager myself;
	return &myself;
}

LogManager::LogManager():
	bInitLog(LOG_STATE::NO_INIT),
	currentIndex(1),
	_kRunning(false), //Since V1.2
	_qEmtpy(true), //Since V1.2
	pThread(NULL) //Since V1.2
{
	
}

LogManager::~LogManager()
{
	//mtx_queue.lock();
	cout << endl << "~LogManager" << endl;
	//mtx_queue.unlock();
}

string LogManager::getStatus()
{
	stringstream out;

	out << "\tLog status:";
	
	if (LOG_MASK == LOG_DISABLE)
		out << endl << "\t\tThere is no logs ativated.";
	else
	{
		if( (LOG_MASK & INFO) != 0)
			out << endl << "\t\tINFO Log Level is ACTIVATED.";
		if ((LOG_MASK & WARNING) != 0)
			out << endl << "\t\tWARNING Log Level is ACTIVATED.";
		if ((LOG_MASK & ERROR) != 0)
			out << endl << "\t\tERRORS Log Level is ACTIVATED.";
		if ((LOG_MASK & SERVICE) != 0)
			out << endl << "\t\tGENERAL SERVICE RUNTIME Log Level is ACTIVATED.";
		if ((LOG_MASK & DEBUG_LVL_1) != 0)
			out << endl << "\t\tDEBUG Log Level 1 is ACTIVATED.";
		if ((LOG_MASK & DEBUG_LVL_2) != 0)
			out << endl << "\t\tDEBUG Log Level 2 is ACTIVATED.";
		if ((LOG_MASK & DEBUG_LVL_3) != 0)
			out << endl << "\t\tDEBUG Log Level 3 is ACTIVATED.";
		if ((LOG_MASK & DEBUG_LVL_4) != 0)
			out << endl << "\t\tDEBUG Log Level 4 is ACTIVATED.";
	}
	
	return out.str();
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

	//Since V1.2, Start the Log Thread for write sockets.
	auto fAsync = std::bind(&LogManager::waitForLogs, this, placeholders::_1);
	_kRunning = true;
	pThread = new thread(fAsync, 0); 

	return true;
}

//Since V1.2
void LogManager::stop()
{
	_kRunning = false;
	if (pThread != NULL)
	{
		if (pThread->joinable())
			pThread->join();
		delete pThread;
	}
}

//Since V1.2
void LogManager::waitForLogs(int dummy)
{	
	mutex mtx_w;
	condition_variable cv_w;
	unique_lock<mutex> lkc_w(mtx_w);

	while (_kRunning || !_qEmtpy) //While the service is running or there are message to write.
	{	
		bool bWrite = false;
		string msgToLog;

		mtx_queue.lock();

		_qEmtpy = _qLog.empty();
		if (!_qEmtpy)
		{
			msgToLog = _qLog.front();
			_qLog.pop();
		}

		mtx_queue.unlock();

		if (_qEmtpy)
			cv_w.wait_for(lkc_w, chrono::milliseconds(10)); //Wait for10 ms to check the queue.
		else
			_writeToLog(msgToLog);
	}
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

//Since V1.2, se encola para delegar la escritura a otro thread.
void LogManager::writeToLog(string message)
{
	if (!_kRunning) return; //Onece stop process has been started, it's not allowed more logs.
	
	//Since V1.3, si los logs fueron desahabilitados, ya no se encola el mensaje.
	if (LOG_DISABLE == getLogLevel()) return; 

	mtx_queue.lock(); 
	_qLog.push(message);
	_qEmtpy = false;
	mtx_queue.unlock();
}

void LogManager::forceToWriteToLog(string message)
{
	if (!_kRunning) return; //Onece stop process has been started, it's not allowed more logs.

	mtx_queue.lock();
	_qLog.push(message);
	_qEmtpy = false;
	mtx_queue.unlock();
}

void LogManager::_writeToLog(string message)
{

	//mtx_queue.lock(); //Since V1.2
	static bool started = false;

	fstream f;
	long size = 0;
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
		ifile << currentLog.str() << "." << currentIndex << ".log";
		long fsize = getFileSize(ifile.str());

		if (fsize >= MAX_FILE_SIZE)
			closeIt = true;
	}

	//Open the Current log.
	currentLog << "." << currentIndex << ".log\0";
	f.open(currentLog.str().c_str(), ios::out | ios::app); 

	//if the current file must be close
	if (closeIt)
	{

		if (++currentIndex > MAX_FILE_BACKUP_COUNT)
			currentIndex = 1;

		f << "The log will continue in file [" << FILE_PATH << FILE_SEPARATOR << FILE_NAME << "." << currentIndex << ".log" << "]..." << endl;
		f.close();

		//Open the new log, if its exist will be reseted.
		stringstream newLog;
		newLog << FILE_PATH << FILE_SEPARATOR << FILE_NAME << "." << currentIndex << ".log";
		f.open(newLog.str().c_str(), ios::out); //overwrite the file, if it exist.

		isNew = true;
	}


	if (isNew)
	{	
		if (f.is_open())
		{
			f << UIPI_LOG_VERSION << endl;
			f << "LOG FILE WITH INDEX :[" << currentIndex << "]" << endl;
			f << LOG_HEADER << endl;
		}
	}

	if (f.is_open()) {
		f << message;
		f.close();
	}

	//mtx_queue.unlock();
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

		ifile << path << "." << i << ".log";

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
		ifile << path << "." << index << ".log";
		long fsize = getFileSize(ifile.str());

		if (fsize >= MAX_FILE_SIZE)
			*closeCurrent = true;
	}

	return index;
}

bool LogManager::existFile(string path)
{
	/*
	struct _stat attributos;
	int rstat;

	rstat = _stat(path.c_str(), &attributos);

	if (rstat == 0)
		return true;
	*/
	return std::filesystem::exists(path);
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
		pathAux << path << "." << i << ".log";
		
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
		pathAux << path << "." << index << ".log";
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





