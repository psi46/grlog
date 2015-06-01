// dtime.cpp

#pragma once


// logDate "Apr 17 13:53:45 2006"
// xmlDate "04-17-2006 13:53:45"

class Cdtime
{
	static const char* monthNames[12];
	int Y, M, D, h, m, s;
public:
	void SetDateTime(int year, int month, int day, int hour, int min, int sec);
	bool ReadLog(const char *logDateTime);
	std::string GetXmlDateTime();
};
