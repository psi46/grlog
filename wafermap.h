// wafermap.h

#ifndef WAFERMAP_H
#define WAFERMAP_H

//#include "TH2.h"
#include <TH2.h>
#include <TArc.h>
#include <TBox.h>
#include <TPaletteAxis.h>

#include "chipdatabase.h"
#include "colorscale.h"


struct CWaferPos
{
	int x, y, pos;
	CWaferPos() {};
	CWaferPos(int wx, int wy, int wpos) { x=wx; y=wy; pos=wpos; }
	int GetPicX() { return 2*x + pos%2 + 1; }
	int GetPicY() { return 2*y - pos/2 + 2; }
	bool Exist();
};


class CChipBox;
class CWaferMap;


class TChipBox : public TBox
{
	CChipBox *fChipbox;
public:
	TChipBox(CChipBox *chipbox, Double_t x1, Double_t y1,
		Double_t x2, Double_t y2)
	: TBox(x1,y1,x2,y2)
	{ fChipbox = chipbox; }

	virtual void  ExecuteEvent(Int_t event, Int_t px, Int_t py);
};


class CChipBox
{
protected:
	CChipBox *fNext;
	float fValue;
	CChip *fChipData;
	TChipBox *fChip;
	CWaferPos fPos;
	int fColor;
public:
	CChipBox(CChip *chip, CWaferPos &pos, float value, int colNum);
	~CChipBox();
	float GetValue() {  return fValue; }
	void SetColor(int colNum);
	void Draw();
	friend class TChipBox;
	friend class CWaferMap;
};


class CWaferMap
{
	TEllipse *fBorder;
	CChipBox *fFirst;

	TH2C *thDummy;
	CColorScale *scale;

	bool fScaleFixed;
	bool fScaleValid;

	bool Add(CChip *chip, CWaferPos &chippos, float value);
public:
	CWaferMap();
	~CWaferMap();
	void Clear();
	bool Draw();

	void UpdateBin(CWaferDataBase &database);
	void UpdateClass(CWaferDataBase &database);
	void UpdateFailCode(CWaferDataBase &database);
//	void UpdatePic(CWaferDataBase &database);
	void UpdateIdOn(CWaferDataBase &database);
	void UpdateIaOn(CWaferDataBase &database);
	void UpdateIdInit(CWaferDataBase &database);
	void UpdateIaInit(CWaferDataBase &database);
	void UpdateVanaDAC(CWaferDataBase &database);
	void UpdateBlack(CWaferDataBase &database);
	void UpdateVdreg(CWaferDataBase &database);
	void UpdateVaout(CWaferDataBase &database);
	void UpdateCalDel(CWaferDataBase &database);
	void UpdatePm(CWaferDataBase &database);
	void UpdatePh1mean(CWaferDataBase &database);
	void UpdatePh21mean(CWaferDataBase &database);
};


#endif
