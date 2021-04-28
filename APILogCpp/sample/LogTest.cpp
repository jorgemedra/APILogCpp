// LogTest.cpp : Defines the entry point for the console application.
//

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <ostream>
#include <iomanip>
#include <mutex>
#include <ctime>
#include <exception> 
#include <thread>
#include <chrono>

#include <APILogCpp.h>

using namespace std;
using namespace apilog;

int main()
{
	
	stringstream header;

	header << "--------------------------------------" << endl
		   << "API LOG Test App." << endl
		   << "This is a App header." << endl
		   << "--------------------------------------";
		   
	LogManager::getLogManger()->initLog("TestLog", "./logs", 10, 1, "%Y-%m-%d %H:%M:%S.@s", header.str());


	cout << "Generating logs in dir: .logs/\n";

	LogGate log("MAIN_OBJ");
	log.setLogLevel(LOG_LEVEL::INFO | LOG_LEVEL::ERROR | LOG_LEVEL::WARNING | LOG_LEVEL::DEBUG_ON | LOG_LEVEL::DEBUG_LVL_2);

	LogGate logSec("SECN_OBJ");
	
	std::this_thread::sleep_for(std::chrono::duration<suseconds_t>(2));

	char buffer[] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19 ,
					  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 
					  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
					  0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 
					  0x70, 0x71, 0x72};
	int size = 43;
	for (int i = 0; i < 20000; i++)
	{
		stringstream counter;
		counter << "--[" << i << "]-----------";
		log.writeInfo(counter.str());
		log.writeInfo("Este es un mensaje de INFO");
		log.writeWarn("Este es un mensaje de Warning");
		log.writeError("Este es un mensaje de ERROR");
		log.writeDebug(LOG_LEVEL::DEBUG_LVL_1, "Este es un mensaje de DEBUG Lv1");
		log.writeBytes(LOG_LEVEL::DEBUG_LVL_2, "Mensaje de un Buffer Cualqueira.",buffer, size);

		
		logSec.writeInfo("Este es un mensaje de INFO (Si aparece)");
		logSec.writeDebug(LOG_LEVEL::DEBUG_LVL_1, "Este es un mensaje de DEBUG Lv1");
		logSec.writeDebug(LOG_LEVEL::DEBUG_LVL_3, "Este es un mensaje de DEBUG Lv3");
		logSec.writeDebug(LOG_LEVEL::DEBUG_LVL_4, "Este es un mensaje de DEBUG Lv4");


		if(i%1500 == 0)
			std::this_thread::sleep_for(std::chrono::duration<suseconds_t>(1));
	}
	
	cout << "Test ended.\n";
	log.writeInfo("Test ended");
	logSec.writeInfo("log Secundario se termina.");
	return 0;
}
