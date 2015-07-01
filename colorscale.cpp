// colorscale.cpp

#include "stdafx.h"


CColorScale::CColorScale(Double_t x1, Double_t y1, Double_t x2 ,Double_t y2)
	: TPave(x1, y1, x2, y2)
{
	xmin = x1;
	xmax = x2;
	ymin = y1;
	ymax = y2;

	InstallColorTable();
	fAxis = new TGaxis(xmin, ymin, xmin, ymax, 0, 1, 510,"S-R");
	fAxis->SetLabelSize(0.03);
	visible = false;
	mode = CS_BIN;
}


CColorScale::~CColorScale()
{
	delete fAxis;
}


const Int_t CColorScale::ndivz = 80;



void CColorScale::InstallColor(int colNum, float red, float green, float blue)
{
	// get the color and if it does not exist create it
	if (!gROOT->GetColor(colNum))
	{
		TColor *color = new TColor(colNum, red, green, blue,"");
	}
	else
	{
		TColor *color = gROOT->GetColor(colNum);
		color->SetRGB(red, green, blue);
	}
}


#define COLOR_BIN_BASE    256


void CColorScale::InstallColorTable()
{
	for (int i=0; i<SIZE_OF_COLORTABLE; i++)
		InstallColor(COLOR_BIN_BASE + i, COLOR_TABLE[i].r, COLOR_TABLE[i].g, COLOR_TABLE[i].b);
}


// === color tables =========================================================

const Int_t CColorScale::SIZE_OF_COLORTABLE = 32;
const CColor CColorScale::COLOR_TABLE[SIZE_OF_COLORTABLE] =
{
	{ 0.00, 0.00, 0.00 },  //  0
	{ 0.50, 0.50, 0.50 },  //  1
	{ 0.78, 0.78, 0.78 },  //  2
	{ 1.00, 1.00, 1.00 },  //  3

	{ 0.59, 0.00, 0.00 },  //  4
	{ 1.00, 0.00, 0.00 },  //  5
	{ 1.00, 0.35, 0.39 },  //  6
	{ 1.00, 0.55, 0.51 },  //  7
	{ 1.00, 0.78, 0.75 },  //  8
	{ 1.00, 0.47, 0.28 },  //  9

	{ 0.71, 0.39, 0.00 },  // 10
	{ 0.78, 0.55, 0.00 },  // 11
	{ 1.00, 0.71, 0.00 },  // 12
	{ 1.00, 0.82, 0.39 },  // 13

	{ 0.40, 0.36, 0.125 },  // 14
	{ 0.61, 0.61, 0.125 },  // 15
	{ 1.00, 1.00, 0.00 },  // 16
	{ 0.90, 1.00, 0.71 },  // 17

	{ 0.00, 0.00, 1.00 },  // 18
	{ 0.00, 0.39, 1.00 },  // 19
	{ 0.00, 0.71, 1.00 },  // 20
	{ 0.00, 1.00, 1.00 },  // 21
	{ 0.55, 1.00, 1.00 },  // 22

	{ 0.44, 0.00, 0.63 },  // 23
	{ 1.00, 0.00, 1.00 },  // 24
	{ 0.71, 0.43, 1.00 },  // 25
	{ 1.00, 0.70, 1.00 },  // 26

	{ 0.00, 0.39, 0.00 },  // 27
	{ 0.00, 0.78, 0.00 },  // 28
	{ 0.47, 1.00, 0.16 },  // 29
	{ 0.71, 1.00, 0.51 },  // 30
	{ 0.16, 1.00, 0.63 }   // 31
};


const Int_t CColorScale::nColorBin = 13;
const Int_t CColorScale::colorBin[nColorBin] =
{
	28, // bin  0
	 5, // bin  1
	15, // bin  2
	 9, // bin  3
	17, // bin  4
	13, // bin  5
	24, // bin  6
	26, // bin  7
	14, // bin  8
	18, // bin  9
	19, // bin 10
	20, // bin 11
	30  // bin 12
};


const Int_t CColorScale::nColorFail = 24;
const Int_t CColorScale::colorFail[nColorFail] =
{
	 5,  // fail  0
	 4,  // fail  1
	 7,  // fail  2
	 8,  // fail  3
	 9,  // fail  4
	 3,  // fail  5 ?//not used
	16,  // fail  6
	10,  // fail  7
	13,  // fail  8
	14,  // fail  9
	15,  // fail 10
	17,  // fail 11
	 3,  // fail 12
	26,  // fail 13
	23,  // fail 14
	24,  // fail 15
	25,	 // fail 16
	31,  // fail 17
	20,  // fail 18
	22,  // fail 19
	 1,  // fail 20
	 2,  // fail 21
	18,  // fail 22
	28   // fail 23
};


const Int_t CColorScale::nColorClass = 5;
const Int_t CColorScale::colorClass[nColorClass] =
{
	 28,  // CLASS 1
	 18,  // CLASS 2
	 23,  // CLASS 3
	 16,  // CLASS 4
	  5   // CLASS 5
};


const Int_t CColorScale::nColorGroup = 12;
const Int_t CColorScale::colorGroup[nColorGroup] =
{
	 7,
	18,
	 8,
	17,
	 9,
	16,
	10,
	15,
	11,
	14,
	12,
	13
};



void CColorScale::SetRange(Double_t min, Double_t max)
{
	mode = CS_RANGE;
	visible = true;
	wmin = min;
	wmax = max;
	fAxis->SetWmin(wmin);
	fAxis->SetWmax(wmax);
}


Int_t CColorScale::GetBinColor(Int_t w)   { return COLOR_BIN_BASE + colorBin[w];   };

Int_t CColorScale::GetFailColor(Int_t w)  { return COLOR_BIN_BASE + colorFail[w];  };

Int_t CColorScale::GetClassColor(Int_t w) { return COLOR_BIN_BASE + colorClass[w]; };

Int_t CColorScale::GetGroupColor(Int_t w) { return COLOR_BIN_BASE + colorGroup[w]; };


Int_t CColorScale::GetColor(Double_t w)
{
	Int_t nColors;
	Int_t c, theColor;

	switch (mode)
	{
	case CS_BIN:
		c = Int_t(w + 0.4);
		if (c < 0) c = 0; else if (c >= nColorBin) c = nColorBin-1;
		return colorBin[c] + COLOR_BIN_BASE;
	case CS_FAIL:
		c = Int_t(w + 0.4);
		if (c < 0) c = 0; else if (c >= nColorFail) c = nColorFail-1;
		return colorFail[c] + COLOR_BIN_BASE;
	case CS_CLASS:
		c = Int_t(w + 0.4);
		if (c < 0) c = 0; else if (c >= nColorClass) c = nColorClass-1;
		return colorClass[c] + COLOR_BIN_BASE;
	case CS_GROUP:
		c = Int_t(w + 0.4);
		if (c < 0) c = 0; else if (c >= nColorGroup) c = nColorGroup-1;
		return colorGroup[c] + COLOR_BIN_BASE;
	case CS_RANGE:
		nColors = gStyle->GetNumberOfColors();
		if (w < wmin) w = wmin;	else if (w > wmax) w = wmax;
		c = Int_t(0.01 + (w - wmin) * ndivz/(wmax-wmin));
		theColor = Int_t((c + 0.99) * Double_t(nColors) / Double_t(ndivz));
		if (theColor >= nColors) theColor = nColors - 1;
		return gStyle->GetColorPalette(theColor);
	}

	return 0;
}


void CColorScale::Paint(Option_t *option)
{
	if (!visible) return;

	ConvertNDCtoPad();
	SetFillStyle(1001);

	Double_t y1, y2;

	Double_t wspan = wmax - wmin;
	Double_t wscale = (ymax-ymin)/wspan;
	Double_t w1, w2;

	Double_t scale = wspan/ndivz;

	for (Int_t i = 0; i < ndivz; i++)
	{
		w1 = wmin + i*scale;
		w2 = w1 + scale;

		y1 = ymin + (w1 - wmin) * wscale;
		y2 = ymin + (w2 - wmin) * wscale;

		SetFillColor(GetColor(w1));
		TAttFill::Modify();
		gPad->PaintBox(xmin, y1, xmax, y2);
	}
	fAxis->Paint();
}
