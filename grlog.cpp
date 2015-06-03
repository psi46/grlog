// grlog.cpp


#include "stdafx.h"


#define GRLOGDIR     "plots"
#define DATABASEDIR  "database"


// global data structure
CWaferDataBase waferData;


int ReadLogFile(const std::string &filename)
{
	CLogFile p;

	// === Read log file =======================================================

	printf("Open File: %s\n", gName.GetName_Log().c_str());
	try
	{
		p.Open(filename.c_str());
		p.Read(waferData);
	}
	catch (CLogError &e)
	{
		printf("ERROR reading log file: %s\n", e.ErrorMsg());
		return 1;
	}

	// === Process log file ====================================================

	try
	{
		waferData.Calculate();
	}
	catch (...)
	{
		printf("Error processing database!\n");
		return 2;
	}
	return 0;
}



int main(int argc, char* argv[])
{
	printf("grlog (28.5.2015)\n");

	// === Retrieve log file name ==============================================

	if (!gName.Process(argc, argv)) return 1;

//	_mkdir(gName.GetPath().c_str());

	// === read logfile -> waferData ===========================================
	int res = ReadLogFile(gName.GetName_Log());
	if (res) return res;

	// === Start GUI ===========================================================

	TRint *theApp = new TRint("grlog", 0, NULL, /* &argc, argv, */ NULL, 0);
	GrLogMainFrame *gui
		= new GrLogMainFrame(waferData, gClient->GetRoot(),250,200);
	gui->Update();
	theApp->Run();
	delete gui;

	return 0;
}
