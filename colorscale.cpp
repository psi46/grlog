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

const Int_t CColorScale::SIZE_OF_COLORTABLE = 28;
const CColor CColorScale::COLOR_TABLE[SIZE_OF_COLORTABLE] =
{
	{ 0.0, 0.0, 0.0 },  //  0 black
	{ 0.5, 0.5, 0.5 },  //  1 dark gray
	{ 1.0, .76, .76 },  //  2
	{ 1.0, 1.0, 1.0 },  //  3 white

	{ 0.7, 0.0, 0.0 },  //  4 dark red
	{ 1.0, 0.0, 0.0 },  //  5 red
	{ .06, 1.0, .81 },  //  6 light red  //0.4

	{ 0.0, 0.3, 0.0 },  //  7 dark green
	{ 0.0, .75, 0.0 },  //  8 green
	{ 0.0, 1.0, 0.0 },  //  9 green
	{ 0.0, .79, .64 },  // 10 yellow green
	{ 1.0, .84, 0.0 },  // 11 gold

	{ 0.0, 0.5, 0.5 },  // 12 dark blue green

	{ 0.0, 0.0, 1.0 },  // 13 dark blue
	{ .25, 0.5, 1.0 },  // 14 blue
	{ 1.0, .54, .54 },  // 15
	{ 0.5, .75, 1.0 },  // 16 blue
	{ 0.0, 1.0, 1.0 },  // 17 light blue
	{ 0.5, 1.0, 1.0 },  // 18 light blue

	{ 0.5, .25, 0.0 },  // 19 dark brown
	{ .75, 0.5, 0.0 },  // 20 brown
	{ 1.0, 0.5, 0.25 },  // 21
	{ 1.0, 0.5, 0.0 },  // 22 light yellow
	{ 1.0, 1.0, 0.0 },  // 23 yellow
	{ 1.0, 1.0, .85 },  // 24 light yellow

	{ 0.5, 0.0, 0.5 },  // 25 dark red blue
	{ 1.0, 0.0, 1.0 },  // 26 red blue
	{ 1.0, 0.5, 1.0 }   // 27 light red blue
};


const Int_t CColorScale::nColorBin = 13;
const Int_t CColorScale::colorBin[nColorBin] =
{
	 8, // bin  0
	 5, // bin  1
	23, // bin  2
	21, // bin  3
	24, // bin  4
	25, // bin  5
	26, // bin  6
	27, // bin  7
	12, // bin  8
	13, // bin  9
	14, // bin 10
	16, // bin 11
	18  // bin 12
};


const Int_t CColorScale::nColorFail = 24;
const Int_t CColorScale::colorFail[nColorFail] =
{
	 5,  // fail  0
	 4,  // fail  1
	 0,  // fail  2
	 1,  // fail  3
	24,  // fail  4
	 3,  // fail  5 ?
	23,  // fail  6
	22,  // fail  7
	17,  // fail  8
	26,  // fail  9
	27,  // fail 10
	19,  // fail 11
	20,  // fail 12
	25,  // fail 13
	15,  // fail 14
	 2,  // fail 15
	11,	 // fail 16
	 3,  // fail 17 ?
	10,  // fail 18
	 6,  // fail 19
	16,  // fail 20
	14,  // fail 21
	13,  // fail 22
	 8   // fail 23
};


const Int_t CColorScale::nColorClass = 5;
const Int_t CColorScale::colorClass[nColorClass] =
{
	  8,  // CLASS 1
	 13,  // CLASS 2
	 25,  // CLASS 3
	 23,  // CLASS 4
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
