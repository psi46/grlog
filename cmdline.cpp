// cmdline.cpp

#include "stdafx.h"


CCmdLineParameter gName;


void CCmdLineParameter::Help()
{
	puts
	(
		"grlog [-h] logfile\n"
		"  -h    print this help text\n"
	);
}


// s -> logPath, logName, logType
void CCmdLineParameter::SplitName(const std::string &s)
{
	std::string s2;
	size_t p = s.find_last_of("/\\");
	if (p != std::string::npos)
	{
		logPath = s.substr(0, p+1);
		s2 = s.substr(p+1);
	}
	else
	{
		logPath = "";
		s2 = s;
	}

	p = s2.find_last_of(".");
	if ((p != std::string::npos) && (p != 0))
	{
		logName = s2.substr(0, p);
		logType = s2.substr(p);
	}
	else
	{
		logName = s2;
		logType = "";
	}
}


bool CCmdLineParameter::Process(int argc, char* argv[])
{
	if (argc < 2) { puts("too few arguments!\n"); Help(); return false; }

	dataStructure = 0;
	logFileName = "";
	for (int i = 1; i<argc; i++)
	{
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'h': Help(); break;
			case 'p': dataStructure = 1;
			default: puts("illegal argument!\n"); Help(); return false;
			}
		}
		else
		{
			logFileName = argv[i];
		}
	}

	if (logFileName.length() == 0) { Help(); return false; }
	SplitName(logFileName);
	if (logName.length() == 0) { Help(); return false; }

	GenerateNames();

	return true;
}


void CCmdLineParameter::GenerateNames()
{
	switch (dataStructure)
	{
		case 1: GenNamesPSI(); break; // PSI report directory/naming structure structure
		default: GenNames();  // rename and create method for default report directory structure
	}
}


void CCmdLineParameter::GenNames()
{
	// --- Data structure (PSI version)

	std::string path = logPath + "report\\";
	_mkdir(path.c_str());
	std::string path1 = logPath;
	path1.erase (path1.end()-1,path1.end()); //to get folder name only

	path_Report     = path;
	path_ClassList  = path;
	path_FailList   = path;
	path_Statistics = path;
	path_Pick       = path;
	path_JSON       = path + "database\\";
	path_XML        = path;
	path_WaferMap   = path + "maps\\";
	path_YieldsFile = path;

	name_Batch      = path1;
	name_Report     = path_Report + logName + "_report.txt";
	name_ClassList  = path_ClassList + logName + "_classlist.txt";
	name_FailList   = path_FailList + logName + "_FailList.txt";
	name_Statistics = path_Statistics + logName + "_stat.txt";
	name_Pick       = path_Pick + logName + "_pick.txt";
	name_JSON       = path_JSON + logName + "_db.json";
	name_WaferMap   = path_WaferMap + logName + "_wmap_";
	name_YieldsFile = path_YieldsFile + "yields_";
}


void CCmdLineParameter::GenNamesPSI()
{
	// --- Data structure (PSI version)

	std::string path = logPath + "report\\";
	_mkdir(path.c_str());
	std::string path1 = logPath;
	path1.erase (path1.end()-1,path1.end()); //to get folder name only

	path_Report     = path;
	path_ClassList  = path;
	path_FailList   = path;
	path_Statistics = path;
	path_Pick       = path;
	path_JSON       = path + "database\\";
	path_XML        = path;
	path_WaferMap   = path + "maps\\";

	name_Batch      = path1;
	name_Report     = path_Report + logName + "_report.txt";
	name_ClassList  = path_ClassList + logName + "_classlist.txt";
	name_FailList   = path_FailList + logName + "_FailList.txt";
	name_Statistics = path_Statistics + logName + "_stat.txt";
	name_Pick       = path_Pick + logName + "_pick.txt";
	name_JSON       = path_JSON + logName + "_db.json";
	name_WaferMap   = path_WaferMap + logName + "_wmap_";
	name_YieldsFile = path_YieldsFile + "yields_";
}

