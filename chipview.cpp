// chipview.cpp


#include "stdafx.h"


ClassImp(CChipView)

void CChipView::AddButton(TGCompositeFrame *frame,
							   char *name, char *caller)
{
	TGTextButton *btn = new TGTextButton(frame,name);
	btn->Connect("Clicked()","CChipView",this, caller);
	frame->AddFrame(btn,
		new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,3,3,2,2));
}


CChipView::CChipView(CChip *chip, const TGWindow *p, UInt_t w, UInt_t h)
	: TGMainFrame(p,w,h)
{
	fChip = chip;

	pixMap = new TH2I("pixel","pixel map", 52, 0, 52, 80, 0, 80);
	pixMap->SetStats(kFALSE);
	pixMap->SetOption("COLZ");
	Double_t Red[6]   = { 0.30, 0.05, 0.00, 0.09, 0.80, 0.99 };
	Double_t Green[6] = { 0.01, 0.05, 0.70, 0.90, 0.80, 0.00 };
	Double_t Blue[6]  = { 0.30, 0.80, 0.30, 0.00, 0.05, 0.10 };
	Double_t Stops[6] = { 0.00, 0.2,  0.35, 0.5,  0.7,  1.0 };
	TColor::CreateGradientColorTable(6, Stops, Red, Green, Blue, 40);

	fHist = NULL; // new TH1F("hist1","level", 100, 0, 100);

	fMain = new TGMainFrame(p,w,h, kHorizontalFrame);

	// -- embedded canvas
	fEcanvas = new TRootEmbeddedCanvas("ECanvas",fMain,600,600);
	fMain->AddFrame(fEcanvas,
		new TGLayoutHints(kLHintsExpandX|kLHintsExpandY, 5,5,5,5));

	TCanvas *fCanvas = fEcanvas->GetCanvas();
	CreateSubPads(fCanvas);
	fCanvas->cd(2);
	gPad->SetLogy();

	// --- create button frame -------------------------------------------
	TGVerticalFrame *buttonFrame = new TGVerticalFrame(fMain,50,200);

	// --- wafer map selection
	TGButtonGroup *selWmap = new TGButtonGroup(buttonFrame,
		"chip view", kVerticalFrame);
	TGRadioButton *db, *rb;
	db = new TGRadioButton(selWmap, new TGHotString("defects"), CHVIEW_PIXDEF);
	db->Connect("Clicked()", "CChipView", this, "DoSetViewMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("Vthr"),   CHVIEW_VTHR);
	rb->Connect("Clicked()", "CChipView", this, "DoSetViewMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("trim 3"), CHVIEW_TRIM3);
	rb->Connect("Clicked()", "CChipView", this, "DoSetViewMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("trim 2"), CHVIEW_TRIM2);
	rb->Connect("Clicked()", "CChipView", this, "DoSetViewMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("trim 1"), CHVIEW_TRIM1);
	rb->Connect("Clicked()", "CChipView", this, "DoSetViewMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("trim 0"), CHVIEW_TRIM0);
	rb->Connect("Clicked()", "CChipView", this, "DoSetViewMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("pulse"),  CHVIEW_PULSE);
	rb->Connect("Clicked()", "CChipView", this, "DoSetViewMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("ph 1"),   CHVIEW_PH1);
	rb->Connect("Clicked()", "CChipView", this, "DoSetViewMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("ph 2"),   CHVIEW_PH2);
	rb->Connect("Clicked()", "CChipView", this, "DoSetViewMode()");
	rb = new TGRadioButton(selWmap, new TGHotString("ph 2-1"), CHVIEW_PH2_PH1);
	rb->Connect("Clicked()", "CChipView", this, "DoSetViewMode()");

	db->SetState(kButtonDown);
	buttonFrame->AddFrame(selWmap,
		new TGLayoutHints(kLHintsCenterX|kLHintsExpandX,3,3,2,2));

	AddButton(buttonFrame, "&prev chip",   "DoGoPrevChip()");

	AddButton(buttonFrame, "&next chip",   "DoGoNextChip()");

	AddButton(buttonFrame, "prev test",   "DoGoPrevChipTest()");

	AddButton(buttonFrame, "next test",   "DoGoNextChipTest()");

	AddButton(buttonFrame, "prev defect", "DoGoPrevDefectChip()");

	AddButton(buttonFrame, "next defect", "DoGoNextDefectChip()");

	AddButton(buttonFrame, "prev good",   "DoGoPrevGoodChip()");

	AddButton(buttonFrame, "next good",   "DoGoNextGoodChip()");

	// --- button "print"
	AddButton(buttonFrame, "p&rint",   "PrintChipView()");

	fMain->AddFrame(buttonFrame,new TGLayoutHints(kLHintsLeft,5,10,3,3));
	// -------------------------------------------------------------------

	SetViewMode(CHVIEW_PIXDEF);
	SetTitle();

	fMain->MapSubwindows();
	fMain->Resize(fMain->GetDefaultSize());
	fMain->MapWindow();
}


CChipView::~CChipView()
{
	fMain->Cleanup();
	delete pixMap;
	delete fHist;
	delete fMain;
}


void CChipView::CreateSubPads(TPad *upad)
{
	if (!upad->IsEditable()) return;

	TPad *padsav = (TPad*)gPad;
	upad->cd();

	TPad *pad;
	char *name  = new char [strlen(upad->GetName())+6];

	Int_t color = upad->GetFillColor();

	sprintf(name,"%s_0",upad->GetName());
	pad = new TPad(name,name,0.40,0.35,0.99,0.99,color);
	pad->SetNumber(1);
	pad->Draw();

	sprintf(name,"%s_1",upad->GetName());
	pad = new TPad(name,name,0.40,0.01,0.99,0.34,color);
	pad->SetNumber(2);
	pad->Draw();

	sprintf(name,"%s_2",upad->GetName());
	pad = new TPad(name,name,0.01,0.01,0.39,0.99,color);
	pad->SetNumber(3);
	pad->Draw();

	delete [] name;

	upad->Modified();
	if (padsav) padsav->cd();
}


void CChipView::SetTitle()
{
	char s[32];
	sprintf(s, "Chip: %i%i%c", fChip->mapY, fChip->mapX,
		"CDAB"[fChip->mapPos]);
	fMain->SetWindowName(s);
}


void CChipView::DoSetViewMode()
{
	TGButton *btn = (TGButton *) gTQSender;
	Int_t id = btn->WidgetId();
	if (id == fViewMode) return;

	SetViewMode(id);
}


void CChipView::SetViewMode(int mode)
{
	if (mode>=0) fViewMode = mode;

	switch (fViewMode)
	{
		case CHVIEW_PIXDEF:
			UpdatePixDef();
			break;

		case CHVIEW_VTHR:
			UpdateLevel();
			break;

		case CHVIEW_TRIM0:
			UpdateTrim(0);
			break;

		case CHVIEW_TRIM1:
			UpdateTrim(1);
			break;

		case CHVIEW_TRIM2:
			UpdateTrim(2);
			break;

		case CHVIEW_TRIM3:
			UpdateTrim(3);
			break;

		case CHVIEW_PULSE:
			UpdatePulse();
			break;

		case CHVIEW_PH1:
			UpdatePh1();
			break;

		case CHVIEW_PH2:
			UpdatePh2();
			break;

		case CHVIEW_PH2_PH1:
			UpdatePh21();
			break;
	}
	UpdateView();
}


void CChipView::DoGoPrevChip()
{
	CChip *chip = CWaferDataBase::GetPrev(fChip);
	if (!chip) return;
	fChip = chip;
	SetTitle();
	SetViewMode();
}


void CChipView::DoGoNextChip()
{
	CChip *chip = CWaferDataBase::GetNext(fChip);
	if (!chip) return;
	fChip = chip;
	SetTitle();
	SetViewMode();
}


void CChipView::DoGoPrevChipTest()
{
	CChip *chip = CWaferDataBase::GetPrevTest(fChip);
	if (!chip) return;
	fChip = chip;
	SetTitle();
	SetViewMode();
}


void CChipView::DoGoNextChipTest()
{
	CChip *chip = CWaferDataBase::GetNextTest(fChip);
	if (!chip) return;
	fChip = chip;
	SetTitle();
	SetViewMode();
}


void CChipView::DoGoPrevDefectChip()
{
	CChip *chip = CWaferDataBase::GetPrevBad(fChip);
	if (!chip) return;
	fChip = chip;
	SetTitle();
	SetViewMode();
}


void CChipView::DoGoNextDefectChip()
{
	CChip *chip = CWaferDataBase::GetNextBad(fChip);
	if (!chip) return;
	fChip = chip;
	SetTitle();
	SetViewMode();
}


void CChipView::DoGoPrevGoodChip()
{
	CChip *chip = CWaferDataBase::GetPrevGood(fChip);
	if (!chip) return;
	fChip = chip;
	SetTitle();
	SetViewMode();
}


void CChipView::DoGoNextGoodChip()
{
	CChip *chip = CWaferDataBase::GetNextGood(fChip);
	if (!chip) return;
	fChip = chip;
	SetTitle();
	SetViewMode();
}


void CChipView::PrintChipView()
{
	std::string filename;
	_mkdir(gName.GetPath_ChipView().c_str());
	
	char cc[10];
	sprintf(cc,"_%i%i%c", fChip->mapY, fChip->mapX, "CDAB"[fChip->mapPos]);
	std::string str = cc;
	switch (fViewMode)
	{
		case CHVIEW_PIXDEF: str = str + "_def";   break;
		case CHVIEW_VTHR:   str = str + "_vthr";  break;
		case CHVIEW_TRIM0:  str = str + "_trim0"; break;
		case CHVIEW_TRIM1:  str = str + "_trim1"; break;
		case CHVIEW_TRIM2:  str = str + "_trim2"; break;
		case CHVIEW_TRIM3:  str = str + "_trim3"; break;
		case CHVIEW_PULSE:  str = str + "_pulse";  break;
		case CHVIEW_PH1:  str = str + "_ph1";  break;
		case CHVIEW_PH2:  str = str + "_ph2";  break;
		case CHVIEW_PH2_PH1:  str = str + "_ph2ph1";  break;
		default: return;
	}

	filename = gName.GetName_ChipView(str);
	TCanvas *fCanvas = fEcanvas->GetCanvas();
	fCanvas->Print(filename.c_str());
}



void CChipView::UpdateView()
{

	TCanvas *fCanvas = fEcanvas->GetCanvas();

	fCanvas->cd(1);
	pixMap->Draw();

	if (fHist)
	{
		fCanvas->cd(2);
		fHist->Draw();
	}

	fCanvas->cd(3);
	DrawInfo();

	fCanvas->cd();
	fCanvas->Update();
}



void CChipView::DrawInfoText(int x, int y, const char name[], const char value[])
{
/*
	TText *t = new TText(0.05, 0.95-0.03*y, name);
	t->SetTextSize(0.08);
	t->SetNDC(true);
	t->Draw();
	t = new TText(0.05*x, 0.95-0.03*y, value);
	t->SetTextSize(0.08);
	t->SetNDC(true);
	t->Draw();
*/
	TText t;
	t.SetTextSize(0.05);
	t.DrawTextNDC(0.05, 0.9-0.03*y, name);
	t.DrawTextNDC(0.05*x, 0.9-0.03*y, value);
}


void CChipView::DrawInfoFormat(int x, int y, const char name[],
		const char *format, ...)
{
	char s[64];

	va_list marker;
	va_start(marker, format);
	vsprintf(s,format, marker);
	va_end(marker);

	DrawInfoText(x, y, name, s);
}


void CChipView::DrawInfoDouble(int x, int y, const char name[], double value, int prec)
{
	char s[32];
	sprintf(s, "%0.*f", prec, value);
	DrawInfoText(x, y, name, s);
}


void CChipView::DrawInfoInt(int x, int y, const char name[], int value)
{
	char s[32];
	sprintf(s, "%i", value);
	DrawInfoText(x, y, name, s);
}


void CChipView::DrawInfo()
{
	int y = 1;
	char s[128];
	const int posx = 7;

	gPad->Clear();
	TText t;

	gPad->Range(0.0, 0.0, 1.0, 1.0);
	t.SetTextSize(0.06);

	// Wafer id
	sprintf(s,"Wafer: %s", fChip->wafer->waferId.c_str());
	t.DrawText(0.05,0.96,s);

	// Chip id
	if (fChip->multiCount)
		sprintf(s,"Chip:   %i%i%c (%i/%i) (%i of %i)", fChip->mapY, fChip->mapX,
			"CDAB"[fChip->mapPos], fChip->picX, fChip->picY,
			fChip->multiCount - fChip->multiNum + 1, fChip->multiCount+1);
	else
		sprintf(s,"Chip:   %i%i%c (%i/%i)", fChip->mapY, fChip->mapX,
			"CDAB"[fChip->mapPos], fChip->picX, fChip->picY);

	t.DrawText(0.05,0.93,s);

	DrawInfoInt(posx, y++, "Log entry:", fChip->nEntry);

	sprintf(s, "%i  %s", int(fChip->failCode), fChip->failstring.c_str());
	DrawInfoText(posx, y++,   "fail:", s);
	DrawInfoInt(posx, y++, "class:", fChip->chipClass);
	DrawInfoInt(posx, y++, "bin:",   fChip->bin);
	y++;

	if (fChip->nEntry)
	{
		DrawInfoDouble(posx, y++, "IdigOn:", fChip->IdigOn, 1);
		DrawInfoDouble(posx, y++, "IanaOn:", fChip->IanaOn, 1);
	}
	else
	{
		DrawInfoText(posx, y++, "IdigOn:", "-");
		DrawInfoText(posx, y++, "IanaOn:", "-");
	}

	if (fChip->existINIT)
	{
		DrawInfoDouble(posx, y++, "IdigInit:", fChip->IdigInit, 1);
		DrawInfoDouble(posx, y++, "IanaInit:", fChip->IanaInit, 1);
	}
	else
	{
		DrawInfoText(posx, y++, "IdigInit:", "-");
		DrawInfoText(posx, y++, "IanaInit:", "-");
	}

	if (fChip->token >= 0) DrawInfoInt(posx, y++, "Token:", fChip->token);
	else DrawInfoText(posx, y++, "Token:", "-");

	if (fChip->i2c >= 0) DrawInfoText(posx, y++, "I2C:", fChip->i2c? "ok": "error");
	else DrawInfoText(posx, y++, "I2C:", "-");

	if (fChip->existProbecard)
	{
		DrawInfoDouble(posx, y++, "V_dac:",  fChip->pc_v_dac, 2);
		DrawInfoDouble(posx, y++, "Vd_cap:", fChip->pc_vd_cap, 2);
		DrawInfoDouble(posx, y++, "Vd_reg:", fChip->pc_vd_reg, 2);
	}
	else
	{
		DrawInfoText(posx, y++, "V_dac:",  "-");
		DrawInfoText(posx, y++, "Vd_cap:", "-");
		DrawInfoText(posx, y++, "Vd_reg:", "-");
	}

	y++;
	
	DrawInfoText(4, y++, "start:", fChip->startTime.GetXmlDateTime().c_str());
	DrawInfoText(4, y++, "end:",   fChip->endTime.GetXmlDateTime().c_str());
}


void CChipView::UpdatePixDef()
{
	pixMap->Reset();

	if (fHist) delete fHist;
	fHist = new TH1F("hist1","defects", 7, 0, 7);

	if (!fChip->pixmap.levelExist) return;
	int col, row;
	for (col=0; col<52; col++) for (row=0; row<80; row++)
	{
		int value = 0;
		if (!fChip->pixmap.IsDefect(col,row)) continue;
		if      (fChip->pixmap.GetUnmaskedCount(col,row)==0) value=6;
		else if (fChip->pixmap.GetMaskedCount(col,row)>1)    value=5;
		else if (fChip->pixmap.GetMaskedCount(col,row)==0)   value=4;
		else if (fChip->pixmap.GetDefectColCode(col,row))    value=3;
		else if (fChip->pixmap.GetDefectRowCode(col,row))    value=2;
		else if (fChip->pixmap.GetDefectTrimBit(col,row))    value=1;
		else continue;

		pixMap->SetBinContent(col+1,row+1,value);
		fHist->Fill(value);
	}
}

void CChipView::UpdateLevel()
{
	pixMap->Reset();

	if (fHist) delete fHist;
	fHist = new TH1F("hist1","vthr", 100, 70, 170);

	if (!fChip->pixmap.levelExist) return;
	int col, row;
	for (col=0; col<52; col++) for (row=0; row<80; row++)
	{
		int level = fChip->pixmap.GetRefLevel(col,row);
		pixMap->SetBinContent(col+1,row+1,level);
		fHist->Fill(level);
	}
}

void CChipView::UpdateTrim(int bit)
{
	pixMap->Reset();

	char s[16];
	sprintf(s,"trim %i", bit);
	if (fHist) delete fHist;
	fHist = new TH1F("hist1",s, 50, 0, 150);

	if (!fChip->pixmap.levelExist) return;
	int col, row;
	for (col=0; col<52; col++) for (row=0; row<80; row++)
	{
		int value = fChip->pixmap.GetRefLevel(col,row)
			- fChip->pixmap.GetLevel(col,row,bit);
		pixMap->SetBinContent(col+1,row+1,value);
		fHist->Fill(value);
	}
}

void CChipView::UpdatePulse()
{
	pixMap->Reset();

	if (fHist) delete fHist;
	fHist = new TH1F("hist1","pulse", 100, 0, 100);

	if (!fChip->pixmap.pulseHeightExist) return;
	int col, row;
	for (col=0; col<52; col++) for (row=0; row<80; row++)
		if (!fChip->pixmap.IsDefect(col,row))
		{
			int level = fChip->pixmap.GetPulseHeight(col,row);
			pixMap->SetBinContent(col+1,row+1,level);
			fHist->Fill(level);
		}
}

void CChipView::UpdatePh1()
{
	pixMap->Reset();

	if (fHist) delete fHist;
	fHist = new TH1F("hist1","ph1", 150, 0, 150);

	if (!fChip->pixmap.pulseHeight1Exist) return;
	int col, row;
	for (col=0; col<52; col++) for (row=0; row<80; row++)
		if (!fChip->pixmap.IsDefect(col,row))
		{
			int level = fChip->pixmap.GetPulseHeight1(col,row);
			if (level<10000)
			{
				pixMap->SetBinContent(col+1,row+1,level);
				fHist->Fill(level);
			}
		}
}

void CChipView::UpdatePh2()
{
	pixMap->Reset();

	if (fHist) delete fHist;
	fHist = new TH1F("hist1","ph2", 150, 0, 150);

	if (!fChip->pixmap.pulseHeight2Exist) return;
	int col, row;
	for (col=0; col<52; col++) for (row=0; row<80; row++)
		if (!fChip->pixmap.IsDefect(col,row))
		{
			int level = fChip->pixmap.GetPulseHeight2(col,row);
			if (level<10000)
			{
				pixMap->SetBinContent(col+1,row+1,level);
				fHist->Fill(level);
			}
		}
}

void CChipView::UpdatePh21()
{
	pixMap->Reset();

	if (fHist) delete fHist;
	fHist = new TH1F("hist1","ph2-ph1", 80, 0, 80);

	if (!fChip->pixmap.pulseHeight1Exist) return;
	if (!fChip->pixmap.pulseHeight2Exist) return;
	int col, row;
	for (col=0; col<52; col++) for (row=0; row<80; row++)
		if (!fChip->pixmap.IsDefect(col,row))
		{
			int level1 = fChip->pixmap.GetPulseHeight1(col,row);
			int level2 = fChip->pixmap.GetPulseHeight2(col,row);
			if (level1<10000 && level2<10000)
			{
				pixMap->SetBinContent(col+1,row+1,level2-level1);
				fHist->Fill(level2-level1);
			}
		}
}
