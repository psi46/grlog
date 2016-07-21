// logfile.cpp

#include "stdafx.h"


void CLogFile::Open(const char logFileName[])
{
	if (!Log.open(logFileName)) ERROR_ABORT(ERROR_OPEN)
}


void CLogFile::Read(CWaferDataBase &w)
{
	CChip *chip;
	char s1[40], s2[40], s3[10];

	wafer = &w;
	wafer->Clear();

	ReadLogHeader();
	do
	{
		// read [CENTER] section if exist - default is PSI
		if (Log.isSection("CENTER"))
		{
			if (sscanf(Log.getNextLine(),"%39s", s1) == 1)
			{
				wafer->center = s1;
				Log.getNextSection();
			}
			else ERROR_ABORT(ERROR_CENTER)
		}

		// read [WAFER] section if exist
		if (Log.isSection("WAFER"))
		{
			if (sscanf(Log.getNextLine(),"%39s%39s%9s",	s1, s2, s3) == 3)
			{
				wafer->productId = s1;
				wafer->waferId   = s2;
				wafer->waferNr   = s3;
				Log.getNextSection();
			}
			else ERROR_ABORT(ERROR_WAFERID)
			TranslateProductId();
		}

		// read [CHIP] if exist
		chip = ReadChip();
		if (chip) wafer->Add(chip);
	}
	while (chip);
	ReadCLOSE();

}


void CLogFile::TranslateProductId()
{
	//ROC type rename to align with DB convention:
	printf("%s\n", wafer->productId.c_str());
	if      (!wafer->productId.compare("PROC600V2")) { printf("PROC600V2\n"); }
	else if (!wafer->productId.compare(wafer->productId.size()-2,2,"V2")) { wafer->productId = "v2-v2B"; printf("1\n"); }
	else if (!wafer->productId.compare(wafer->productId.size()-4,4,"V2.1")) { wafer->productId = "v21"; printf("2\n"); }
	else if (!wafer->productId.compare(wafer->productId.size()-10,10,"V2.1respin")) { wafer->productId = "v21respin"; printf("3\n"); }
	else if (!wafer->productId.compare(wafer->productId.size()-6,6,"V2.1.2")) { wafer->productId = "v21respin"; printf("4\n"); }
	else printf("ERROR: %s product Id not known - please insert it in chipdatabase.cpp\n", wafer->productId.c_str());
	//no more needed, bug fixed -
	//if (id.compare("PSI46digV2")) { sprintf(lettersOrder,"CDAB"); }
	//else sprintf(lettersOrder,"CDAB");
}


void CLogFile::ReadLogHeader()
{
	// read [OPEN] section
	if (!Log.isSection("OPEN")) ERROR_ABORT(ERROR_NO_LOGFILE)

	if (!wafer->logTime.ReadLog(Log.getNextLine()))
		ERROR_ABORT(ERROR_NO_LOGFILE);

	// read [VERSION] section
	if (!Log.getNextSection("VERSION")) ERROR_ABORT(ERROR_NO_LOGFILE)
	Log.getNextLine();
	if (strlen(Log.getLine())<3) ERROR_ABORT(ERROR_NO_LOGFILE)
	wafer->logVersion = Log.getNextLine();
	Log.getNextSection();

	if (Log.isSection("CPU")) Log.getNextSection();

	if (Log.isSection("DTB")) Log.getNextSection();
}


CChip* CLogFile::ReadChip()
{
	CChip *chip;

	if (Log.isSection("CHIP") || Log.isSection("CHIP1"))
	{
		chip = new CChip;
		chip->Clear();
		chip->wafer = wafer;
		chip->Read(Log);
		return chip;
	}
	return 0;
}


void CLogFile::ReadCLOSE()
{
	if (!Log.isSection("CLOSE")) ERROR_ABORT(ERROR_CLOSE)
}
