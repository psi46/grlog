// cmdline.cpp

#include "stdafx.h"


CCmdLineParameter gName;


void CCmdLineParameter::SetDefaultParam()
{
	logFileName[0] = 0;
	name[0] = 0;
}


void CCmdLineParameter::Help()
{
	puts
	(
		"grlog [-h] logfile\n"
		"  -h    print this help text\n"
	);
}


bool CCmdLineParameter::Process(int argc, char* argv[])
{
	SetDefaultParam();
	if (argc < 2) { puts("too few arguments!\n"); Help(); return false; }

	int i = 1;
	while (i<argc)
	{
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'h': Help(); break;
			default: puts("illegal argument!\n"); Help(); return false;
			}
		}
		else
		{
			strncpy(logFileName, argv[i], 63);
			strncpy(name, argv[i], 63);
			int pos = strlen(name) - 1;
			while (pos > 0)
			{
				if (name[pos] == '.') { name[pos] = 0; break; }
				pos--;
			}
		}
		i++;
	}
	if (name[0] == 0) { puts("no file name!\n"); Help(); return false; }
	return true;
}



// path structure
// wafer_<name> [\ <path>]

char* CCmdLineParameter::GetPath(char* s, const char* path)
{
	strcpy(s, "wafer_");
	strcat(s, GetName());
	if (path) {  strcat(s, "\\"); strcat(s, path); }
	return s;
}


// name structure
// wafer_<name> \ [<name>] [_<append>] [.<ext>]

char* CCmdLineParameter::GetName(char* s,
	const char* path, const char* append, const char* ext, bool dir)
{
	if(dir){ GetPath(s,path);
			 strcat(s, "\\"); } 
	else { strcpy(s, "");
			if (path) {  strcat(s, path); strcat(s, "\\");}
	}
	strcat(s, GetName()); 
	if (append) { strcat(s, "_"); strcat(s, append); }
	if (ext)    { strcat(s, "."); strcat(s, ext); }
	return s;
}