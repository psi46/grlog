// dtime.cpp

#include "stdafx.h"


const char* Cdtime::monthNames[12] =
{
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};


void Cdtime::SetDateTime(int year, int month, int day, int hour, int min, int sec)
{
	Y = year;
	M = month;
	D = day;
	h = hour;
	m = min;
	s = sec;
}


// logDate "Mon Apr 17 13:53:45 2006"

bool Cdtime::ReadLog(const char *logDateTime)
{
	char monstr[4];

	// --- read log time string
	if (sscanf(logDateTime, "%*s %3s %d %d:%d:%d %d",
		monstr, &D, &h, &m, &s, &Y) != 6) return false;

	// --- read month name
	M = 0;
	for (M = 0; M < 12; M++) if (strcmp(monthNames[M], monstr) == 0) break;
	M++;

	// --- check date/time
	if (M > 12) return false;
	if (D<1 || 31<D) return false;
	if (Y<1900 || 2500<Y) return false;
	if (h<0 || 23<h) return false;
	if (m<0 || 59<m) return false;
	if (s<0 || 59<m) return false;

	return true;
}


// xmlDate "2006-04-17 13:53:45"

std::string Cdtime::GetXmlDateTime()
{
	char dt[24];
	sprintf(dt, "%04i-%02i-%02i %02i:%02i:%02i", Y, M, D, h, m, s);
	return dt;
}
