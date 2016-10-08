// LogTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <ostream>
#include <iomanip>
#include <mutex>
#include <ctime>
#include <Windows.h>
#include <exception> 

#include <APILogCpp.h>

using namespace std;


int main()
{
	
	stringstream header;

	header << "--------------------------------------" << endl
		   << "API LOG Test App." << endl
		   << "This is a App header." << endl
		   << "--------------------------------------";


	LogGate log("MAIN_LOG");
	LogManager::getLogManger()->initLog("TestLog", "c:\\logs", 10, 1, "%Y-%m-%d %H:%M:%S.@s", header.str());
	
	log.setLogLevel(LOG_LEVEL::DEBUG | LOG_LEVEL::INFO);
	

	byte buffer[] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19 ,
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
		log.writeDebug(LOG_LEVEL::DEBUG, "Este es un mensaje de DEBUG");
		log.writeBytes(LOG_LEVEL::DEBUG, "Mensaje de un Buffer Cualqueira.",buffer, size);
	}
	
	return 0;
}
