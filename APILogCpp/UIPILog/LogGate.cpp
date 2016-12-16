#include "APILogCpp.h"

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
	writeLog("INFO", message);
}

void LogGate::writeWarn(string message)
{
	writeLog("WARN", message);
}

void LogGate::writeError(string message)
{
	writeLog("ERROR", message);
}

void LogGate::writeDebug(unsigned int level, string message)
{
	if (isDebugLevelActive(level))
		writeLog("DEBUG", message);
}

void LogGate::writeBytes(unsigned int level, string header, byte* buffer, int size)
{
	stringstream out;
	char ascii[11];
	int i, j;

	if (!isDebugLevelActive(level))
		return;

	mutex_buff.lock();

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
	mutex_buff.unlock();

	writeLog("DEBUG", out.str());
}

void LogGate::setLogLevel(unsigned int level)
{
	LogManager::getLogManger()->setLogLevel(level);
}

bool LogGate::isDebugLevelActive(unsigned int level)
{
	unsigned int lvl = LogManager::getLogManger()->getLogLevel();

	int l = level & lvl;
	if ((level & lvl) == level)
		return true;
	return false;
}

string LogGate::getDateTimeFormated()
{
	string format = LogManager::getLogManger()->getDateFormat();
	stringstream out;
	SYSTEMTIME st;
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[128];
	bool bMicroSeconds = false;
	
	if (format.find("@s") >= 0)
	{
		bMicroSeconds = true;
		format.erase(format.find("@s"), 2);
	}

	GetSystemTime(&st);

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, 80, format.c_str(), timeinfo);

	out << buffer;
	if (bMicroSeconds)
		out << std::setw(3) << std::setfill('0') << st.wMilliseconds;

	return out.str();
}


void LogGate::writeLog(string level, string message)
{
	mutex_log.lock();

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

	mutex_log.unlock();
}
