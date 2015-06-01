// logfile.h

#pragma once

#include "error.h"
#include "waferdatabase.h"
#include "chipdatabase.h"


struct CLogFile
{
	CScanner Log;
	CWaferDataBase *wafer;

	void ReadLogHeader();
	CChip* ReadChip();
	void ReadCLOSE();
	void TranslateProductId();
public:
	void Open(const char logFilename[]);
	void Close() { Log.close(); }
	void Read(CWaferDataBase &w);
};

