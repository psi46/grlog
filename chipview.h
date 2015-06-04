// chipview.h

#ifndef CHIPVIEW_H
#define CHIPVIEW_H


#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH2.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>

#include "chipdatabase.h"
#include "cmdline.h"


enum
{
	CHVIEW_PIXDEF=1,
	CHVIEW_VTHR,
	CHVIEW_TRIM0,
	CHVIEW_TRIM1,
	CHVIEW_TRIM2,
	CHVIEW_TRIM3,
	CHVIEW_PULSE,
	CHVIEW_PH1,
	CHVIEW_PH2,
	CHVIEW_PH2_PH1
};


class CChipView : public TGMainFrame
{
	TH2I *pixMap;
	TH1F *fHist;

	TGMainFrame  *fMain;
	TRootEmbeddedCanvas *fEcanvas;
	int fViewMode;
	CChip *fChip;


	void AddButton(TGCompositeFrame *frame, char *name, char *caller);

	void UpdateView();
	void DrawInfoText(int x, int y, const char name[], const char value[]);
	void DrawInfoFormat(int x, int y, const char name[], const char *format, ...);
	void DrawInfoDouble(int x, int y, const char name[], double value, int prec);
	void DrawInfoInt(int x, int y, const char name[], int value);
	void DrawInfo();
public:
	CChipView(CChip *chip, const TGWindow *p, UInt_t w, UInt_t h);
	virtual ~CChipView();
	void CreateSubPads(TPad *upad);
	void SetTitle();
	void Update() { SetViewMode(); }
	void DoSetViewMode();
	void SetViewMode(int mode = -1);
	void DoGoPrevChip();
	void DoGoNextChip();
	void DoGoPrevChipTest();
	void DoGoNextChipTest();
	void DoGoPrevDefectChip();
	void DoGoNextDefectChip();
	void DoGoPrevGoodChip();
	void DoGoNextGoodChip();
	void PrintChipView();

	void UpdatePixDef();
	void UpdateLevel();
	void UpdateTrim(int bit);
	void UpdatePulse();
	void UpdatePh1();
	void UpdatePh2();
	void UpdatePh21();

	ClassDef(CChipView,1)
};


#endif
