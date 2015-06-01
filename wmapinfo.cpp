// wmapinfo.cpp


#include "stdafx.h"



CWmapInfo::CWmapInfo()
{
	fDraw = NULL;
	fHist = NULL;
}


CWmapInfo::~CWmapInfo()
{
	if (fHist) delete fHist;
}


void CWmapInfo::Reset()
{
	if (fHist) { delete fHist;  fHist = NULL; }
}


void CWmapInfo::Draw()
{
//	gPad->Draw();
	gPad->Clear();
//	gPad->AbsCoordinates(true);
//	gPad->GetListOfPrimitives()->Remove(fHist);
	if (fDraw != NULL) (this->*fDraw)();
	else if (fHist) fHist->Draw();
}


// -----------------------------------------------------------------------

void CWmapInfo::UpdateBin(CWaferDataBase &database)
{
	Reset();

	totcount = 0;
	for (int i=0; i<13; i++) count[i] = 0;

	CChip *p = database.GetFirst();
	while(p)
	{
		int bin = p->bin;
		if (0<=bin && bin <=12) { totcount++; count[bin]++; }
		p = database.GetNext(p);
	}
	fDraw = &CWmapInfo::DrawBin;
}

void CWmapInfo::DrawBin()
{
	TText t1;
	TBox r;
	char s[128];

	gPad->Range(0.0, 0.0, 1.0, 1.0);
	sprintf(s, "Bin (Total Chips: %2i)", totcount);
	t1.SetTextSize(0.08);
	t1.DrawText(0.05, 0.9, s);

	t1.SetTextSize(0.06);
	r.SetFillStyle(1001);

	if (totcount>0)
		for (int i=0; i<=12; i++)
		{
			Double_t x = 0.05 +(i/7)*0.55;
			Double_t y = 0.75-(i%7)*0.1;
			r.SetFillColor(CColorScale::GetBinColor(i));
			r.DrawBox(x, y-0.01, x+0.035, y+0.055);
			sprintf(s, "%2i: %3i (%0.1f%%)", i, count[i],
				double(count[i])/totcount*100);
			t1.DrawText(x+0.05, y, s);
		}
}


void CWmapInfo::UpdateClass(CWaferDataBase &database)
{
	Reset();

	totcount = 0;
	for (int i=1; i<=5; i++) count[i] = 0;

	CChip *p = database.GetFirst();
	while(p)
	{
		int cclass = p->chipClass;
		if (1<=cclass && cclass <=5) { totcount++; count[cclass]++; }
		p = database.GetNext(p);
	}
	fDraw = &CWmapInfo::DrawClass;
}

void CWmapInfo::DrawClass()
{
	TText t1;
	TBox r;
	char s[128];

	gPad->Range(0.0, 0.0, 1.0, 1.0);
	sprintf(s, "Class (Total Chips: %2i)", totcount);
	t1.SetTextSize(0.08);
	t1.DrawText(0.05, 0.9, s);

	t1.SetTextSize(0.06);
	r.SetFillStyle(1001);

	if (totcount>0)
		for (int i=1; i<=5; i++)
		{
			Double_t x = 0.05 +(i/7)*0.55;
			Double_t y = 0.85-(i%7)*0.1;
			r.SetFillColor(CColorScale::GetClassColor(i-1));
			r.DrawBox(x, y-0.01, x+0.035, y+0.055);
			sprintf(s, "Class %2i: %3i (%0.1f%%)", i, count[i],
				double(count[i])/totcount*100);
			t1.DrawText(x+0.05, y, s);
		}
}


void CWmapInfo::UpdateFailCode(CWaferDataBase &database)
{
	Reset();

	totcount = 0;
	for (int i=0; i<=24; i++) count[i] = 0;

	CChip *p = database.GetFirst();
	while(p)
	{
		int fc = p->failCode;
		if (0 <= fc && fc < 24) { totcount++; count[fc]++; }
		p = database.GetNext(p);
	}
	fDraw = &CWmapInfo::DrawFailCode;
}

void CWmapInfo::DrawFailCode()
{
	TText t1;
	TBox r;
	char s[128];

	gPad->Range(0.0, 0.0, 1.0, 1.0);
	sprintf(s, "Fail (Total Chips: %2i)", totcount);
	t1.SetTextSize(0.08);
	t1.DrawText(0.05, 0.9, s);

	t1.SetTextSize(0.06);
	r.SetFillStyle(1001);

	if (totcount>0)
		for (int i=0; i<=24; i++)
		{
			Double_t x = 0.05 +(i/8)*0.35;
			Double_t y = 0.75-(i%8)*0.1;
			r.SetFillColor(CColorScale::GetFailColor(i));
			r.DrawBox(x, y-0.01, x+0.035, y+0.055);
			sprintf(s, "%2i: %3i (%0.1f%%)", i, count[i],
				double(count[i])/totcount*100);
			t1.DrawText(x+0.05, y, s);
		}
}


void CWmapInfo::UpdatePic(CWaferDataBase &database)
{
	Reset();
//	gPad->Range(0.0, 0.0, 1.0, 1.0);
	gPad->PaintBox(0.1, 0.1, 0.5, 0.5);
	fDraw = NULL;
}


void CWmapInfo::UpdateIdOn(CWaferDataBase &database)
{
	Reset();
	fHist = new TH1F("hist","Id(on) [mA]",160, 0,80);

	CChip *p = database.GetFirst();
	while(p)
	{
		fHist->Fill(p->IdigOn);
		p = database.GetNext(p);
	}
	fDraw = NULL;
}

void CWmapInfo::UpdateIaOn(CWaferDataBase &database)
{
	Reset();
	fHist = new TH1F("hist","Ia(on) [mA]",100, 0,100);

	CChip *p = database.GetFirst();
	while(p)
	{
		fHist->Fill(p->IanaOn);
		p = database.GetNext(p);
	}
	fDraw = NULL;
}

void CWmapInfo::UpdateIdInit(CWaferDataBase &database)
{
	Reset();
	fHist = new TH1F("hist","Id(init) [mA]",100, 0,100);

	CChip *p = database.GetFirst();
	while(p)
	{
		if (p->IdigInit >= 0) fHist->Fill(p->IdigInit);
		p = database.GetNext(p);
	}
	fDraw = NULL;
}

void CWmapInfo::UpdateIaInit(CWaferDataBase &database)
{
	Reset();
	fHist = new TH1F("hist","Ia(init) [mA]",100, 0,100);

	CChip *p = database.GetFirst();
	while(p)
	{
		if (p->IanaInit >= 0) fHist->Fill(p->IanaInit);
		p = database.GetNext(p);
	}
	fDraw = NULL;
}

void CWmapInfo::UpdateVanaDAC(CWaferDataBase &database)
{
	Reset();
	fHist = new TH1F("hist","Va(DAC)@24mA", 90, 0,180);

	CChip *p = database.GetFirst();
	while(p)
	{
		if (p->Vana24 >= 0) fHist->Fill(p->Vana24);
		p = database.GetNext(p);
	}
	fDraw = NULL;
}

void CWmapInfo::UpdateVdreg(CWaferDataBase &database)
{
	Reset();
	fHist = new TH1F("hist","Vd(reg) [mA]",100, 1,3);

	CChip *p = database.GetFirst();
	while(p)
	{
		if (p->existProbecard)
		{
			fHist->Fill(p->pc_vd_reg);
		}
		p = database.GetNext(p);
	}
	fDraw = NULL;
}

void CWmapInfo::UpdateCalDel(CWaferDataBase &database)
{
	Reset();
	fHist = new TH1F("hist","CalDel",100, 0, 200);

	CChip *p = database.GetFirst();
	while(p)
	{
		fHist->Fill(p->cds_del);
		p = database.GetNext(p);
	}
	fDraw = NULL;
}

void CWmapInfo::UpdatePm(CWaferDataBase &database)
{
	Reset();
	fHist = new TH1F("hist","Threshold mean", 50, 60, 160);

	CChip *p = database.GetFirst();
	while(p)
	{
		if (p->n) fHist->Fill(p->pm);
		p = database.GetNext(p);
	}
	fDraw = NULL;
}

void CWmapInfo::UpdatePh1mean(CWaferDataBase &database)
{
	Reset();
	fHist = new TH1F("hist","Ph1mean (Offset)",180, 0, 180);

	CChip *p = database.GetFirst();
	while(p)
	{
		fHist->Fill(p->ph1mean);
		p = database.GetNext(p);
	}
	fDraw = NULL;
}

void CWmapInfo::UpdatePh21mean(CWaferDataBase &database)
{
	Reset();
	fHist = new TH1F("hist","Ph21mean (Gain)",80, 0, 80);

	CChip *p = database.GetFirst();
	while(p)
	{
		if (p->nPh) fHist->Fill(p->ph21mean);
		p = database.GetNext(p);
	}
	fDraw = NULL;
}
