// colorscale.h


#ifndef COLORSCALE_H
#define COLORSCALE_H


#ifndef ROOT_TPave
#include "TPave.h"
#endif
#ifndef ROOT_TGaxis
#include "TGaxis.h"
#endif


struct CColor {	float r, g, b; };


class CColorScale : public TPave
{
public:
	enum CSMode { CS_BIN, CS_FAIL, CS_CLASS, CS_GROUP, CS_RANGE };
	CColorScale(Double_t x1, Double_t y1, Double_t x2 ,Double_t y2);
	~CColorScale();

	void SetBin()   { mode = CS_BIN;   visible = false; }
	void SetFail()  { mode = CS_FAIL;  visible = false; }
	void SetClass() { mode = CS_CLASS; visible = false; }
	void SetGroup() { mode = CS_GROUP; visible = false; }
	void SetRange(Double_t min, Double_t max);

	static Int_t GetBinColorId(Int_t w)   { return colorBin[w];   };
	static Int_t GetFailColorId(Int_t w)  { return colorFail[w];  };
	static Int_t GetClassColorId(Int_t w) { return colorClass[w]; };
	static Int_t GetGroupColorId(Int_t w) { return colorGroup[w]; };

	static Int_t GetBinColor(Int_t w);
	static Int_t GetFailColor(Int_t w);
	static Int_t GetClassColor(Int_t w);
	static Int_t GetGroupColor(Int_t w);
	Int_t GetColor(Double_t w);

	virtual void  Paint(Option_t *option="");
protected:
	bool visible;
	CSMode mode;
	static const Int_t ndivz;  // number of contour levels
	Double_t xmin, xmax, ymin, ymax;
	Double_t wmin;
	Double_t wmax;
	TGaxis *fAxis;

	static void InstallColor(int colNum, float red, float green, float blue);
	static void InstallColorTable();

	static const Int_t SIZE_OF_COLORTABLE;
	static const CColor COLOR_TABLE[28];

	static const Int_t nColorBin;
	static const Int_t colorBin[13];

	static const Int_t nColorFail;
	static const Int_t colorFail[24];

	static const Int_t nColorClass;
	static const Int_t colorClass[5];

	static const Int_t nColorGroup;
	static const Int_t colorGroup[12];
};

#endif
