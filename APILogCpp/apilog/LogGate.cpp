//#include "APILogCpp.h"
#include "LogGate.h"

using namespace apilog;

string apilog::logLevelKey(unsigned int level)
{
		
		if(level == LOG_LEVEL::INFO)
			return "INFO";
		else if(level == LOG_LEVEL::WARNING)
			return "WARN";
		else if(level == LOG_LEVEL::ERROR)
			return "ERROR";
		else if(level == (LOG_LEVEL::DEBUG_ON | LOG_LEVEL::DEBUG_LVL_1))
			return "DBG_1";
		else if(level == (LOG_LEVEL::DEBUG_ON | LOG_LEVEL::DEBUG_LVL_2))
			return "DBG_2";
		else if(level == (LOG_LEVEL::DEBUG_ON | LOG_LEVEL::DEBUG_LVL_3))
			return "DBG_3";
		else if(level == (LOG_LEVEL::DEBUG_ON | LOG_LEVEL::DEBUG_LVL_4))
			return "DBG_4";

	return ".....";
}

LogGate::LogGate(string id)
{
	idGate = id;
}

LogGate::~LogGate()
{
}

string LogGate::getID()
{
	return idGate;
}


void LogGate::writeInfo(string message)
{
	if (isLogLevelActive(LOG_LEVEL::INFO))
		writeLog(logLevelKey(LOG_LEVEL::INFO), message);
}

void LogGate::writeWarn(string message)
{
	if (isLogLevelActive(LOG_LEVEL::WARNING))
		writeLog(logLevelKey(LOG_LEVEL::WARNING), message);
}

void LogGate::writeError(string message)
{
	if (isLogLevelActive(LOG_LEVEL::ERROR))
		writeLog(logLevelKey(LOG_LEVEL::ERROR), message);
}

void LogGate::writeDebug(LOG_LEVEL level, string message)
{
	if (isLogLevelActive( LOG_LEVEL::DEBUG_ON | level))
	{
		string type = logLevelKey( LOG_LEVEL::DEBUG_ON | level);
		writeLog(type, message);
	}
}
			  
void LogGate::writeBytes(LOG_LEVEL level, string header, char* buffer, int size)
{
	unique_lock<mutex> wbLck(mutex_buff);

	stringstream out;
	char ascii[11];
	int i, j;

	if (!isLogLevelActive( LOG_LEVEL::DEBUG_ON | level))
		return;
	

	out << header << endl;
	out << "Buffer: Size = " << size  << endl;
	//out << "--- DATA START --- " << endl;

	j = 0;
	memset(ascii, '\0', 11);
	for (i = 0; i < size; i++)
	{
		stringstream strHex;
		if (j == 0)
		{
			int range = 0;

			if ((i + 10) < size)
				range = i + 10;
			else
				range = size;
			out << "HEX (Bytes " << setw(2) << (i + 1) << "-" << setw(2) << range << ") [";
		}
		//32 - 126 Printable characters
		if (buffer[i] >= 32 && buffer[i] <= 126)
			ascii[j] = buffer[i];
		else
			ascii[j] = '.';

		strHex << setw(2) << setfill('0') << hex << (int) (buffer[i] & 0x00ff);
		out << strHex.str();

		j++;

		if (j >= 10 || (i + 1) >= size)
		{
			int missing = 0;
			missing = 10 - j;

			if (missing > 0)
				out << setfill(' ') << setw(missing * 3) << ' ';

			out << "] (" << setfill(' ') << left << (char*)ascii;

			if (missing > 0)
				out << setfill(' ') << setw(missing) << ' ';

			out << ")" << endl;

			j = 0;
			memset(ascii, '\0', 11);
		}
		else
			out << " ";
	}

	//out << "--- DATA END --- " << endl;
	string type = logLevelKey( LOG_LEVEL::DEBUG_ON | level);
	writeLog(type, out.str());
}

void LogGate::setLogLevel(unsigned int level)
{
	LogManager::getLogManger()->setLogLevel(level);
}

bool LogGate::isLogLevelActive(unsigned int level)
{
	unsigned int lvl = LogManager::getLogManger()->getLogLevel();

	if ((level & lvl) == level)
		return true;
	return false;
}

string LogGate::getDateTimeFormated()
{
	std::stringstream out;
	bool bMicroSeconds = false;
	string format = LogManager::getLogManger()->getDateFormat();
    time_t rawtime;
	struct tm * timeinfo;
    char buffer[128];
    

	if (format.find("@s") >= 0)
	{
		bMicroSeconds = true;
		format.erase(format.find("@s"), 2);
	}

    time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, format.c_str(), timeinfo);    
    std::string dt2(buffer);
	out << dt2;


	if (bMicroSeconds)
	{
		//Get the system clock fot NOW
    	chrono::system_clock::time_point today = chrono::system_clock::now(); 

		// Get the time_t from today
		rawtime = chrono::system_clock::to_time_t(today); 
		// Convert to time_point fromtd_tm, wich include till seconds.
		chrono::system_clock::time_point td_tm = chrono::system_clock::from_time_t(rawtime);

		// Get the diferece tiks betwen them.
		chrono::system_clock::time_point units(today - td_tm);

		//Getting Milliseconds
		auto mseconds = chrono::duration_cast<chrono::milliseconds>(units.time_since_epoch()).count(); 

		out << std::setw(3) << std::setfill('0') << mseconds;
	}

    return out.str();
}


void LogGate::writeLog(string level, string message)
{
	unique_lock<mutex> wlLck(mutex_log);

	char line[2048];
	stringstream buff;
	stringstream out;
	string dt;
	
	dt = getDateTimeFormated();
	
	buff << message;

	do
	{
		buff.getline(line, 2048);
		if (strcmp(line, "") != 0)
			out << dt << "\t"
				<< setw(5) << setfill(' ') << level << "\t"				
				<< idGate << "\t" 
				<< line << endl;
	} while (strcmp(line, "") != 0);

	LogManager::getLogManger()->writeToLog(out.str());
}
