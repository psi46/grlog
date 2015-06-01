// cmdline.h


#ifndef CMDLINE_H
#define CMDLINE_H


class CCmdLineParameter
{
	char logFileName[150];
	char name[64];
	void SetDefaultParam();
public:
	void Help();
	bool Process(int argc, char* argv[]);
	char* GetLogFileName() { return logFileName; }
	char* GetName()        { return name; }
	char* GetPath(char*s, const char* path);
	char* GetName(char*s, const char* path, const char* append, const char* ext, bool dir);
};


extern CCmdLineParameter gName;


#endif
