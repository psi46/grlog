// wmapinfo.h

#ifndef WMAPINFO_H
#define WMAPINFO_H

#include <TPaveText.h>
#include <TH1.h>


#include "chipdatabase.h"


class CWmapInfo
{
	void (CWmapInfo::*fDraw)();
	TH1F *fHist;
	int totcount;
	int count[24];

	void Reset();
public:
	CWmapInfo();
	~CWmapInfo();
	void Draw();

	void UpdateBin(CWaferDataBase &database);
	void DrawBin();
	void UpdateClass(CWaferDataBase &database);
	void DrawClass();
	void UpdateFailCode(CWaferDataBase &database);
	void DrawFailCode();
	void UpdatePic(CWaferDataBase &database);
	void UpdateIdOn(CWaferDataBase &database);
	void UpdateIaOn(CWaferDataBase &database);
	void UpdateIdInit(CWaferDataBase &database);
	void UpdateIaInit(CWaferDataBase &database);
	void UpdateVanaDAC(CWaferDataBase &database);
	void UpdateVdreg(CWaferDataBase &database);
	void UpdateVaout(CWaferDataBase &database);
	void UpdateCalDel(CWaferDataBase &database);
	void UpdatePm(CWaferDataBase &database);
	void UpdatePstd(CWaferDataBase &database);
	void UpdatePmColDiff(CWaferDataBase &database);
	void UpdatePh1mean(CWaferDataBase &database);
	void UpdatePh1std(CWaferDataBase &database);
	void UpdatePh21mean(CWaferDataBase &database);
	void UpdatePh21std(CWaferDataBase &database);
	void UpdatePhColDiff(CWaferDataBase &database);
};


#endif
