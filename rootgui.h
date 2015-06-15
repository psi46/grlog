// rootgui.h

#pragma once

#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>

#include "wafermap.h"
#include "wmapinfo.h"
#include "waferdatabase.h"
#include "chipdatabase.h"
#include "cmdline.h"


enum
{
	WMAP_BIN=1,
	WMAP_CLASS,
	WMAP_FAILCODE,
	WMAP_PICGROUP,
	WMAP_IDIGON,
	WMAP_IANAON,
	WMAP_IDIGINI,
	WMAP_IANAINI,
	WMAP_VANA,
	WMAP_PC_VDREG,
	WMAP_CALDEL,
	WMAP_PM,
	WMAP_PH1MEAN,
	WMAP_PH21MEAN
};


class GrLogMainFrame : public TGMainFrame
{
private:
	TGMainFrame  *fMain;
	TRootEmbeddedCanvas *fEcanvas;
	int fWmapMode;
	CWaferDataBase &fDatabase;

	void AddButton(TGCompositeFrame *frame, char *name, char *caller);
	CWaferMap fWmap;
	CWmapInfo fWmapInfo;

	void UpdateWmap();
public:
	GrLogMainFrame(CWaferDataBase &waferData,
		const TGWindow *p, UInt_t w, UInt_t h);
	virtual ~GrLogMainFrame();
	void CreateSubPads(TPad *upad);
	void Update() { SetMapMode(); }
	CWaferDataBase* GetDB() { return &fDatabase; }
	void DoSetMapMode();
	void SetMapMode(int mode = -1);
	bool Add(CChip *chip) { return fDatabase.Add(chip); }
	void DoOpenChip();
	void DoFailList();
	void DoClassList();
	void DoReport();
	void DoGeneratePickFile();
	void DoGenerateJSONfile();
	void DoGenerateXMLfiles();
	void DoPDReport();
	void DoPSIReport();
	void DoFullReport();
	void DoStatistics();
	void DoGenerateWaferMap();
	void PrintWaferMap();
	void DoGenerateYieldsFile();
	ClassDef(GrLogMainFrame,1)
};

