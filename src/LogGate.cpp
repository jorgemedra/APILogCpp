#include "APILogCpp.h"

using namespace apilog;

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

string LogGate::getStatus()
{
	return LogManager::getLogManger()->getStatus();
}

void LogGate::writeCritialMessage(string message)
{
	writeLog("CRITICAL", message, true);
}

void LogGate::writeInfo(string message)
{
	if (isDebugLevelActive(LOG_LEVEL::INFO))
		writeLog("INFO", message);
}

void LogGate::writeWarn(string message)
{
	if (isDebugLevelActive(LOG_LEVEL::WARNING))
		writeLog("WARN", message);
}

void LogGate::writeError(string message)
{
	if (isDebugLevelActive(LOG_LEVEL::ERROR))
		writeLog("ERROR", message);
}

void LogGate::writeDebug(unsigned int level, string message)
{
	if (isDebugLevelActive(level))
		writeLog("DEBUG", message);
}

void LogGate::writeBytes(unsigned int level, string header, char* buffer, int size)
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
			out << "HEX (Bytes " << setw(2) << (i + 1) << "-" << setw(2) << range << ")\t[";
		}
		//32 - 126 Printable characters
		if (buffer[i] >= 32 && buffer[i] <= 126)
			ascii[j] = buffer[i];
		else
			ascii[j] = '.';

		strHex << setw(2) << setfill('0') << hex << (int) (buffer[i] & 0x00ff);
		out << strHex.str();
		//Esta forma de imprir el caracter HExagecimal provoca que el Stream falle al imprimir n�mero de menos de 10, 
		//despues del salto de linea. provocando que el byte 0x01 lo imprima como 0x10. Por eso se asigno un stream exclusivo para
		// obtener el caracter HEX, en formato string.
		//out << setw(2) << setfill('0') << hex << (int)(buffer[i] & 0x00ff) << dec;

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
	//if ((level & lvl) == level)
	if ((level & lvl) != 0)
		return true;
	return false;
}

string LogGate::getDateTimeFormated()
{
	std::stringstream out;
	bool bMicroSeconds = false;
	string format = LogManager::getLogManger()->getDateFormat();
	time_t rawtime;
	struct tm *timeinfo;
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

void LogGate::writeLog(string level, string message, bool force)
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

	if(force)
		LogManager::getLogManger()->forceToWriteToLog(out.str());
	else
		LogManager::getLogManger()->writeToLog(out.str());

	mutex_log.unlock();
}