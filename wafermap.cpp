// wafermap.cpp


#include "stdafx.h"


bool CWaferPos::Exist()
{
	if (pos<0 || 3<pos) return false;
	if (y<0 || 7<y) return false;
	if (x<0 || 10<x) return false;
//	if ((y==0 || y==7) && (x<3 || 6<x)) return false;
//	if ((y==1 || y==5 || y==6) && (x<1 || 8<x)) return false;
	return true;
}


void  TChipBox::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
	if (event == kButton1Down)
	{
		printf("open chip %X%X%c\n",	fChipbox->fPos.x, fChipbox->fPos.y,
			"CDAB"[fChipbox->fPos.pos]);

		CChipView *chipview = 
			new CChipView(fChipbox->fChipData, gClient->GetRoot(),250,200);

	}

	TBox::ExecuteEvent(event,px,py);
}


CChipBox::CChipBox(CChip *chip, CWaferPos &pos, float value, int colNum)
{
	fValue = value;
	fChipData = chip;
	fPos = pos;
	float x = 0.05 + float(fPos.x)*0.075;
	float y = 0.84 - float(fPos.y)*0.112;
	switch (fPos.pos)
	{
		case 1: x += 0.036; break;
		case 2: y += 0.054; break;
		case 3: x += 0.036, y += 0.054;
	}
	fChip = new TChipBox(this, x,y,x+0.033,y+0.051);
//	char s[20];
//	sprintf(s,"%i%i%c", fPos.x, fPos.y, "CDAB"[fPos.pos]);
//	fChip->SetToolTipText(s, 500);
	fChip->SetFillColor(colNum);
	fChip->IsBeingResized();
}


CChipBox::~CChipBox()
{
	delete fChip;
}


void CChipBox::SetColor(int colNum)
{
	fChip->SetFillColor(colNum);
	fChip->IsBeingResized();
}


void CChipBox::Draw()
{
	char s[16];
	fChip->Draw();
	if (fChipData->multiCount)
	{
		double x = fChip->GetX1() + 0.012;
		double y = fChip->GetY1() + 0.020;
		sprintf(s, "%i", fChipData->multiCount);
		TText t;
		t.SetTextSize(0.02);
		t.DrawText(x, y, s);
	}
}


// --- wafer map ---------------------------------------------------------

CWaferMap::CWaferMap()
{
	fBorder = new TEllipse(0.43, 0.5, 0.43, 0.49);
	fBorder->SetFillColor(18); // 12..19, 10
	fFirst = NULL;

	TH2F *h2 = new TH2F("h2","Dummy Histogram", 40, -4, 4, 40, -20, 20);
	h2->SetMaximum(100);
	h2->SetEntries(1);
	h2->SetStats(kFALSE);

	gStyle->SetPalette(55);  // rainbow
//	gStyle->SetPalette(53);  // dark body radiator
	scale = new CColorScale(0.94, 0.05, 0.98, 0.95);
	scale->SetRange(10.0, 30.0);
}


CWaferMap::~CWaferMap()
{
	Clear();
	delete scale;
	delete fBorder;
}


void CWaferMap::Clear()
{
	CChipBox *p = fFirst;
	while (p)
	{
		fFirst = p->fNext;
		delete p;
		p = fFirst;
	}

//	scale.SetMode(CScale::SCALE_INVISIBLE);
}



bool CWaferMap::Add(CChip *chip, CWaferPos &chippos, float value)
{
	CChipBox *p = new CChipBox(chip, chippos, value, scale->GetColor(value));
	p->fNext = fFirst;
	fFirst = p;
	return true;
}



bool CWaferMap::Draw()
{
	gPad->SetEditable(kTRUE);
	gPad->Range(0.0, 0.0, 1.0, 1.0);

	fBorder->Draw();

	CChipBox *p = fFirst;
	while (p)
	{
		p->Draw();
		p = p->fNext;
	}

	scale->Draw();

	gPad->SetEditable(kFALSE);
	return true;
}


void CWaferMap::UpdateBin(CWaferDataBase &database)
{
	Clear();
	scale->SetBin();
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		if (pos.Exist()) Add(p, pos, p->bin);
		p = database.GetNext(p);
	}
}

void CWaferMap::UpdateClass(CWaferDataBase &database)
{
	Clear();
	scale->SetClass();
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		if (pos.Exist()) Add(p, pos, p->chipClass-1);
		p = database.GetNext(p);
	}
}

void CWaferMap::UpdateFailCode(CWaferDataBase &database)
{
	Clear();
	scale->SetFail();
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		if (pos.Exist()) Add(p, pos, p->failCode);
		p = database.GetNext(p);
	}
}

/*
void CWaferMap::UpdatePic(CWaferDataBase &database)
{
	Clear();
	scale->SetGroup();
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		int value = p->pickGroup - 1;
		if (p->pickClass == 1)
		{
			if (pos.Exist()) Add(p,pos,COLOR_PIC_GOOD[value%12]);
		}
		else if (p->pickClass == 2)
		{
			if (pos.Exist()) Add(p,pos,COLOR_PIC_CL2);
		}
		else
		{
			if (pos.Exist()) Add(p,pos,COLOR_PIC_BAD[value%10]);
		}
		p = database.GetNext(p);
	}
}
*/


void CWaferMap::UpdateIdOn(CWaferDataBase &database)
{
	Clear();
	scale->SetRange(0.0, 50.0);
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		Float_t value = p->IdigOn;
		if (pos.Exist() && p->nEntry) Add(p,pos,value);
		p = database.GetNext(p);
	}
}

void CWaferMap::UpdateIaOn(CWaferDataBase &database)
{
	Clear();
	scale->SetRange(0.0, 20.0);
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		Float_t value = p->IanaOn;
		if (pos.Exist() && p->nEntry) Add(p,pos,value);
		p = database.GetNext(p);
	}
}

void CWaferMap::UpdateIdInit(CWaferDataBase &database)
{
	Clear();
	scale->SetRange(0.0, 60.0);
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		Float_t value = p->IdigInit;
		if (pos.Exist() && p->existINIT) Add(p,pos,value);
		p = database.GetNext(p);
	}
}

void CWaferMap::UpdateIaInit(CWaferDataBase &database)
{
	Clear();
	scale->SetRange(0.0, 50.0);
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		Float_t value = p->IanaInit;
		if (pos.Exist() && p->existINIT) Add(p,pos,value);
		p = database.GetNext(p);
	}
}

void CWaferMap::UpdateVanaDAC(CWaferDataBase &database)
{
	Clear();
	scale->SetRange(40.0, 120.0);
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		int value = p->Vana24;
		if (pos.Exist() && p->existITRIM) Add(p,pos,value);
		p = database.GetNext(p);
	}
}


void CWaferMap::UpdateVdreg(CWaferDataBase &database)
{
	Clear();
	scale->SetRange(1.8, 2.2);
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		if (p->existProbecard)
		{
			Float_t value = p->pc_vd_reg;
			if (pos.Exist()) Add(p,pos,value);
		}
		p = database.GetNext(p);
	}
}


void CWaferMap::UpdateCalDel(CWaferDataBase &database)
{
	Clear();
	scale->SetRange(0.0, 200.0);
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		if (p->existProbecard)
		{
			Float_t value = p->cds_del;
			if (pos.Exist()) Add(p,pos,value);
		}
		p = database.GetNext(p);
	}
}


void CWaferMap::UpdatePm(CWaferDataBase &database)
{
	Clear();
	scale->SetRange(60.0, 160.0);
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		Float_t value = p->pm;
		if (pos.Exist() && p->n) Add(p,pos,value);
		p = database.GetNext(p);
	}
}


void CWaferMap::UpdatePstd(CWaferDataBase &database)
{
	Clear();
	scale->SetRange(0.0, 10.0);
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		Float_t value = p->pstd;
		if (pos.Exist() && p->n) Add(p,pos,value);
		p = database.GetNext(p);
	}
}


void CWaferMap::UpdatePmColDiff(CWaferDataBase &database)
{
	Clear();
	scale->SetRange(0.0, 10.0);
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		Float_t value = p->pm_col_max;
		if (pos.Exist() && p->n) Add(p,pos,value);
		p = database.GetNext(p);
	}
}


void CWaferMap::UpdatePh1mean(CWaferDataBase &database)
{
	Clear();
	scale->SetRange(0.0, 180.0);
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		Float_t value = p->ph1mean;
		if (pos.Exist() && p->nPh) Add(p,pos,value);
		p = database.GetNext(p);
	}
}


void CWaferMap::UpdatePh1std(CWaferDataBase &database)
{
	Clear();
	scale->SetRange(0.0, 20.0);
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		Float_t value = p->ph1std;
		if (pos.Exist() && p->nPh) Add(p,pos,value);
		p = database.GetNext(p);
	}
}


void CWaferMap::UpdatePh21mean(CWaferDataBase &database)
{
	Clear();
	scale->SetRange(0.0, 80.0);
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		Float_t value = p->ph21mean;
		if (pos.Exist() && p->nPh) Add(p,pos,value);
		p = database.GetNext(p);
	}
}


void CWaferMap::UpdatePh21std(CWaferDataBase &database)
{
	Clear();
	scale->SetRange(0.0, 10.0);
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		Float_t value = p->ph21std;
		if (pos.Exist() && p->nPh) Add(p,pos,value);
		p = database.GetNext(p);
	}
}


void CWaferMap::UpdatePhColDiff(CWaferDataBase &database)
{
	Clear();
	scale->SetRange(0.0, 15.0);
	CChip *p = database.GetFirst();
	while (p)
	{
		CWaferPos pos(p->mapX,p->mapY,p->mapPos);
		Float_t value = p->ph_col_max;
		if (pos.Exist() && p->nPh) Add(p,pos,value);
		p = database.GetNext(p);
	}
}
