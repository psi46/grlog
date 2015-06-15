// cmdline.h

#pragma once

#include <string>


class CCmdLineParameter
{
	int dataStructure;  // 1 = PSI

	std::string logPath;
	std::string logName;
	std::string logType;

	std::string logFileName;
	std::string name;
	void SplitName(const std::string &s);

	void GenerateNames();
	void GenNames();
	void GenNamesPSI();

	std::string name_Batch;
	std::string path_Report;
	std::string name_Report;
	std::string path_ClassList;
	std::string name_ClassList;
	std::string path_FailList;
	std::string name_FailList;
	std::string path_Statistics;
	std::string name_Statistics;
	std::string path_Pick;
	std::string name_Pick;
	std::string path_JSON;
	std::string name_JSON;
	std::string path_XML;
	std::string path_WaferMap;
	std::string name_WaferMap;
	std::string path_YieldsFile;
	std::string name_YieldsFile;
public:
	void Help();
	bool Process(int argc, char* argv[]);

	std::string GetName()     { return logName; }
	std::string GetName_Log() { return logFileName; }
	std::string GetName_Batch() { return name_Batch; }

	std::string GetPath_Report() { return path_Report; }
	std::string GetPath_ClassList() { return path_ClassList; }
	std::string GetPath_FailList() { return path_FailList; }
	std::string GetPath_Statistics() { return path_Statistics; }
	std::string GetPath_Pick() { return path_Pick; }
	std::string GetPath_JSON() { return path_JSON; }
	std::string GetPath_XML() { return path_XML; }
	std::string GetPath_WaferMap() { return path_WaferMap; }
	std::string GetPath_YieldsFile() { return path_YieldsFile; }

	std::string GetName_Report() { return name_Report; }
	std::string GetName_ClassList() { return name_ClassList; }
	std::string GetName_FailList() { return name_FailList; }
	std::string GetName_Statistics() { return name_Statistics; }
	std::string GetName_Pick() { return name_Pick; }
	std::string GetName_JSON() { return name_JSON; }
	std::string GetName_WaferMap(const std::string &suffix) { return name_WaferMap + suffix + ".ps"; }
	std::string GetName_YieldsFile(const std::string &suffix) { return name_YieldsFile + suffix + ".txt"; }
};


extern CCmdLineParameter gName;

