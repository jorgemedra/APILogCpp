# APILogCpp


This API provides a tool to generate logs, it's functionalies are:

+ Cyclic files of log, which are configurable, to keep trackinf the information.
+ Capability to wirte binary information an get the Hex and ASC represetnation.
+ A queue to delegate the writing to logs and return the thread of execution asap.

## Requirements

+ **Visual Studio Code**
+ **C++17.**
+ **gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0**
+ *PlugIn* **C/C++ for Visual Studio Code**
+ **GNU gdb (Ubuntu 9.2-0ubuntu1~20.04) 9.2**

# Build and Install

Build APILig Lib and Sample:

```shell
$ make
```

Build only the APILog Lib:

```shell
$ make apilog
```

Build only the sample:

```shell
$ make sample
```

Install the APILog:

```shell
$ sudo make install
```

Run the sample: 
```shell
$ make run
```
```shell
$ ./bin/sample
```

Uninstall the APILog:

```shell
$ sudo make uninstall
```

Clean all the compiled objects and binary files,log files from test as well:

```shell
$ make clean
```



# How to use

1. Include the header file `APILogCpp.h` which is allocated into the directory `include` and the namespace `apilog`:

```cpp
#include <APILogCpp.h>
using namespace apilog;
```

2. Initi the log with its parameters:

```cpp
bool initLog(string name, string path, int filesBackup, int fileSize, string format, string title);
```

+ **name**: Name of the logfile.
+ **path**: The path where the log files will be created.
+ **filesBackup**: The amount of files that will be create the backup cycle.
+ **fileSize**: The size, in MemgaBytes, that each file can grow.
+ **format**: The string format for the Date. It will follow the format of `strftime`, but if you want to add milliseconds, you have to add at the end of the format the postfix '.@s'.
+ **title**: The title that each log file will have at the moment of its creation.

2. Create As many `LogGate` which will have a specific name to be added after the type of message:

```cpp
LogGate log("MAIN_LOG");

``` cmd
2016-10-07 22:05:24.630	DEBUG	MAIN_LOG	Mensaje de un Buffer Cualqueira.
```

3. Set the debug level of the LogGate, in this way you can filter the log message by type:

```cpp
log.setLogLevel(LOG_LEVEL::INFO | LOG_LEVEL::ERROR | LOG_LEVEL::WARNING | LOG_LEVEL::DEBUG_ON | LOG_LEVEL::DEBUG_LVL_2);
```

The different levels available are:

```cpp
enum LOG_LEVEL : unsigned int
{
	LOG_DISABLE = 0x0000,
	LOG_ALL 	= 0xFFFF,
	INFO 		= 0x0001,
	WARNING 	= 0x0002,
	ERROR 		= 0x0004,
	DEBUG_ON 	= 0x0018,
	DEBUG_LVL_1 = 0x0010,
	DEBUG_LVL_2 = 0x0020,
	DEBUG_LVL_3 = 0x0040,
	DEBUG_LVL_4 = 0x0080
};
```

4. Use the differentes methos to write a message log:

```cpp
log.writeInfo("Este es un mensaje de INFO");
log.writeWarn("Este es un mensaje de Warning");
log.writeError("Este es un mensaje de ERROR");
log.writeDebug(LOG_LEVEL::DEBUG, "Este es un mensaje de DEBUG");
log.writeBytes(LOG_LEVEL::DEBUG, "Mensaje de un Buffer Cualqueira.",buffer, size);
```


# Sample

```cpp
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

	
	LOG_LEVEL::DEBUG_LVL_2);

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
```

## Output sample

```s
UIPI LOG V1.3 (Jan 12th 2018)
LOG FILE WITH INDEX :[1]
--------------------------------------
API LOG Test App.
This is a App header.

--------------------------------------

2021-09-10 13:24:04.625	 INFO	MAIN_OBJ	--[10216]-----------
2021-09-10 13:24:04.625	 INFO	MAIN_OBJ	Este es un mensaje de INFO
2021-09-10 13:24:04.625	 WARN	MAIN_OBJ	Este es un mensaje de Warning
2021-09-10 13:24:04.625	ERROR	MAIN_OBJ	Este es un mensaje de ERROR
2021-09-10 13:24:04.626	DEBUG	MAIN_OBJ	Mensaje de un Buffer Cualqueira.

...

2021-09-10 13:24:06.840	 INFO	SECN_OBJ	Este es un mensaje de INFO (Si aparece)
2021-09-10 13:24:06.840	 INFO	MAIN_OBJ	--[12256]-----------
2021-09-10 13:24:06.840	 INFO	MAIN_OBJ	Este es un mensaje de INFO
2021-09-10 13:24:06.840	 WARN	MAIN_OBJ	Este es un mensaje de Warning
2021-09-10 13:24:06.840	ERROR	MAIN_OBJ	Este es un mensaje de ERROR
2021-09-10 13:24:06.840	DEBUG	MAIN_OBJ	Mensaje de un Buffer Cualqueira.
2021-09-10 13:24:06.840	DEBUG	MAIN_OBJ	Buffer: Size = 43
2021-09-10 13:24:06.840	DEBUG	MAIN_OBJ	HEX (Bytes  1-10)	[10 11 12 13 14 15 16 17 18 19] (..........)
2021-09-10 13:24:06.840	DEBUG	MAIN_OBJ	HEX (Bytes 11-20)	[40 41 42 43 44 45 46 47 48 49] (@ABCDEFGHI)
2021-09-10 13:24:06.840	DEBUG	MAIN_OBJ	HEX (Bytes 21-30)	[50 51 52 53 54 55 56 57 58 59] (PQRSTUVWXY)
2021-09-10 13:24:06.840	DEBUG	MAIN_OBJ	HEX (Bytes 31-40)	[60 61 62 63 64 65 66 67 68 69] (`abcdefghi)
2021-09-10 13:24:06.840	DEBUG	MAIN_OBJ	HEX (Bytes 41-43)	[70 71 72                     ] (pqr       )
The log will continue in file [/home/jorge/vsworkspace/APILogCpp/logs/TestLog.3.log]...
```
