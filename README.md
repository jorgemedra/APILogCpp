# APILogCpp
C ++ static library to generate logs.


```batch
API LOG CPP V1.0 (6 Octubre 2016)
	Author Jorge Oma Medra Torres
	https://github.com/jorgemedra
LOG FILE WITH INDEX :[2]
--------------------------------------
API LOG Test App.
This is a App header.
--------------------------------------
2016-10-07 22:05:24.630	DEBUG	MAIN_LOG	Mensaje de un Buffer Cualqueira.
2016-10-07 22:05:24.630	DEBUG	MAIN_LOG	Buffer: Size = 43
2016-10-07 22:05:24.630	DEBUG	MAIN_LOG	HEX (Bytes  1-10) [10 11 12 13 14 15 16 17 18 19] (..........)
2016-10-07 22:05:24.630	DEBUG	MAIN_LOG	HEX (Bytes 11-20) [40 41 42 43 44 45 46 47 48 49] (@ABCDEFGHI)
2016-10-07 22:05:24.630	DEBUG	MAIN_LOG	HEX (Bytes 21-30) [50 51 52 53 54 55 56 57 58 59] (PQRSTUVWXY)

...

2016-10-07 22:05:32.348	DEBUG	MAIN_LOG	HEX (Bytes 21-30) [50 51 52 53 54 55 56 57 58 59] (PQRSTUVWXY)
2016-10-07 22:05:32.348	DEBUG	MAIN_LOG	HEX (Bytes 31-40) [60 61 62 63 64 65 66 67 68 69] (`abcdefghi)
2016-10-07 22:05:32.348	DEBUG	MAIN_LOG	HEX (Bytes 41-43) [70 71 72                     ] (pqr       )
The log will continue in file [c:\logs\TestLog3.log]...
```
