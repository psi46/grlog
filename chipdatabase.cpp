// chipdatabase.cpp
//-new M.Dall'Osso 05-03-15

#include "stdafx.h"



void CChip::Clear()
{
	int i;

	wafer = 0;

	// [CHIP]
	existCHIP = false;
	mapX = mapY = mapPos = 0;

	// [CHIP1]
	chipId = "";

	// [BEGIN]
	startTime.SetDateTime(1900,1,1, 0,0,0);

	// [END]
	endTime.SetDateTime(1900,1,1, 0,0,0);

	// [PON]
	nEntry     = 0;
	IdigOn = IanaOn = 0.0;

	// [INIT]
	existINIT  = false;
	IdigInit = IanaInit = 0.0;

	//	[VDCAP] [VDREG] [VDAC]
	existProbecard = false;
	pc_vd_cap = 0.0;
	pc_vd_reg = 0.0;
	pc_v_dac  = 0.0; 

	// [TOKEN]
	token = -1;

	// [I2C]
	i2c   = -1;

	// [READBACK]
	existREADBACK = false;
	vdig_u_adc = vana_u_adc = vana_r_adc = vbg_adc = iana_adc = 0;
	vdig_u_value = vana_u_value = iana_value = 0.0;

	// [VANA]
	existVANA = false;
	for (i=0; i<5; i++)
	{
		dacVana[i] = 0.0;
		Iana[i] = 0.0;
		rb_UnregVana[i] = -1;
		rb_RegVana[i] = -1;
		rb_Iana[i] = -1;
	}

	// [ITRIM]
	existITRIM = false;
	Vana24 = 0;
	Iana24 = 0.0;

	// [CALDELSCAN]
	existCALDELSCAN = false;
	cds_x    = 0;
	cds_y    = 0;
	cds_del  = 0;
	cds_scan = "";

	// [PIXMAP] [PULSE] [PH1] [PH2] [PUC1] [PUC2] [PUC3]
	pixmap.Init();

	// [CLASS]
	logChipClass = -1;

	// [POFF]
	bin = -1;


// --- some derived values

	multiCount = 0;
	multiNum   = 0;

	chipClass = 5;
	pickClass = 5;
	pickGroup = 99;
	failCode  = FAIL_UNDEF;
	failstring = "";

	n       = 0;
	nPh     = 0;
	nPhFail = 0;
}


void CChip::GetCurrent(CScanner &Log, double *Idig, double *Iana)
{
	if (Log.getLine()[0] == 0) return;
	char s[8];
	double i;
	if (sscanf(Log.getLine(),"%5s %lf", s, &i)==2)
	{
		if (strcmp("Iana=",s)==0) *Iana = i;
		else *Idig = i;
	}
	Log.getNextLine();
}


void CChip::Save(FILE *f)
{
	std::string datetime;

	fprintf(f,"nEntry:    %i\n", nEntry);
	fprintf(f,"productId: %s\n", wafer->productId.c_str());
	fprintf(f,"waferId:   %s\n", wafer->waferId.c_str());
	fprintf(f,"waferNr:   %s\n", wafer->waferNr.c_str());
	fprintf(f,"mapX:      %i\n", mapX);
	fprintf(f,"mapY:      %i\n", mapY);
	fprintf(f,"mapPos:    %i\n", mapPos);
	fprintf(f,"chipId:    %s\n", chipId.c_str());
	fprintf(f,"startTime: %s",   startTime.GetXmlDateTime().c_str());
	fprintf(f,"endTime:   %s",   endTime.GetXmlDateTime().c_str());
	fprintf(f,"IdigOn:    %0.1f mA\n", IdigOn);
	fprintf(f,"IanaOn:    %0.1f mA\n", IanaOn);
	fprintf(f,"IdigInit:  %0.1f mA\n", IdigInit);
	fprintf(f,"IanaInit:  %0.1f mA\n", IanaInit);
	fprintf(f,"probecard.isValid: %s\n", existProbecard ? "true" : "false");
	if (existProbecard)
	{
		fprintf(f,"probecard.v_dac:  %0.3f V\n", pc_v_dac);
		fprintf(f,"probecard.vd_cap: %0.3f V\n", pc_vd_cap);
		fprintf(f,"probecard.vd_reg: %0.3f V\n", pc_vd_reg);
	}
	fprintf(f,"Iana:      %0.1f  %0.1f  %0.1f  %0.1f  %0.1f\n",
		Iana[0],Iana[1],Iana[2],Iana[3],Iana[4]);
	fprintf(f,"Vana24:  %i\n",       Vana24);
	fprintf(f,"Iana24:  %0.1f mA\n", Iana24);

	fprintf(f,"token:     %i\n", token);
	fprintf(f,"i2c:       %i\n", i2c);
	fprintf(f,"bin:       %i\n", bin);
//	pixmap.Save(f);
//	dcol.Save(f);
}


void CChip::Read(CScanner &Log)
{
	// read [CHIP] [CHIP1] section
	if (Log.isSection("CHIP")) ReadCHIP(Log); // chip on wafer
	else if (Log.isSection("CHIP1")) ReadCHIP1(Log); // to read single chip test
	else ERROR_ABORT(ERROR_OK)

	// read [BEGIN] section
	if (Log.isSection("BEGIN")) ReadBEGIN(Log);	else ERROR_ABORT(ERROR_BEGIN)

	// read [PON] section
	if (Log.isSection("PON")) ReadPON(Log);	else ERROR_ABORT(ERROR_PON)

	// read [INIT] section if exist
	if (Log.isSection("INIT")) ReadINIT(Log);

	// read [VDCAP] if exist
	if (Log.isSection("VDCAP")) ReadProbecard(Log);

	// read [TOKEN] section if exist
	if (Log.isSection("TOKEN")) ReadTOKEN(Log);

	// read [I2C] section if exist
	if (Log.isSection("I2C")) ReadI2C(Log);

	// read [READBACK] section if exist
	if (Log.isSection("READBACK")) ReadREADBACK(Log);

	// read [VANA] section - improved to read both new and old version (old:2 entries, new:6 entries)
	if (Log.isSection("VANA")) ReadVANA(Log);

	// read [ITRIM] section if exist
	if (Log.isSection("ITRIM")) ReadITRIM(Log);

	// read [CALDELSCAN] section
	if (Log.isSection("CALDELSCAN")) ReadCALDELSCAN(Log);

	// results not used.
	if (Log.isSection("PHSCAN")) Log.getNextSection();
	if (Log.isSection("PHSCAN")) Log.getNextSection();

	// read [PIXMAP] section if exist
	if (Log.isSection("PIXMAP")) ReadPIXMAP(Log);

	if (Log.isSection("PULSE")) ReadPULSE(Log);

	if (Log.isSection("PH1")) ReadPH1(Log);

	if (Log.isSection("PH2")) ReadPH2(Log);

	// read [PUCn] sections if exist
	if (Log.isSection("PUC1")) ReadPUC(Log);

	// read [CLASS] section
	if (Log.isSection("CLASS")) ReadClass(Log);

	// read [POFF] section
	if (Log.isSection("POFF")) ReadPOFF(Log); else ERROR_ABORT(ERROR_POFF)

	// read [END] section
	if (Log.isSection("END")) ReadEND(Log);	else ERROR_ABORT(ERROR_END)
}


void CChip::ReadCHIP(CScanner &Log)
{
	char ch;
	if (sscanf(Log.getNextLine(),"%i%i %c",&mapX, &mapY, &ch)!=3)
		ERROR_ABORT(ERROR_CHIP);

	switch (ch)
	{	//to re-arrange chip order.
		case 'C': mapPos = 0; break;
		case 'D': mapPos = 1; break;
		case 'A': mapPos = 2; break;
		case 'B': mapPos = 3; break;
		default: ERROR_ABORT(ERROR_CHIP);
	}

	char s[16];
	sprintf(s, "%i%i%c", mapY, mapX, ch);
	chipId = s;
	existCHIP = true;
	Log.getNextSection();
}

void CChip::ReadCHIP1(CScanner &Log)
{
	if (sscanf(Log.getNextLine(),"%40s",chipId) != 1)
		ERROR_ABORT(ERROR_CHIP);
	mapX = mapY = mapPos = 0;
}

void CChip::ReadBEGIN(CScanner &Log)
{
	Log.getNextLine();
	if (strlen(Log.getLine()) < 12) ERROR_ABORT(ERROR_BEGIN)
	if (!startTime.ReadLog(Log.getLine())) ERROR_ABORT(ERROR_BEGIN)
	Log.getNextSection();
}

void CChip::ReadEND(CScanner &Log)
{
	Log.getNextLine();
	if (strlen(Log.getLine())<12) ERROR_ABORT(ERROR_END)
	if (!endTime.ReadLog(Log.getLine())) ERROR_ABORT(ERROR_END)
	Log.getNextSection();
}

void CChip::ReadPON(CScanner &Log)
{
	if (sscanf(Log.getNextLine(),"%i", &nEntry)!=1)	ERROR_ABORT(ERROR_PON)
	Log.getNextLine();
	GetCurrent(Log,&IdigOn,&IanaOn);
	GetCurrent(Log,&IdigOn,&IanaOn);
	Log.getNextSection();
}

void CChip::ReadINIT(CScanner &Log)
{
	Log.getNextLine();
	Log.getNextLine();
	GetCurrent(Log,&IdigInit,&IanaInit);
	GetCurrent(Log,&IdigInit,&IanaInit);
	existINIT = true;
	Log.getNextSection();
}

void CChip::ReadProbecard(CScanner &Log)
{
	if (sscanf(Log.getNextLine(), "%lf", &pc_vd_cap)!=1)
		ERROR_ABORT(ERROR_PROBECARD)

	if (!Log.getNextSection("VDREG","TOKEN"))
		ERROR_ABORT(ERROR_PROBECARD);
	if (sscanf(Log.getNextLine(), "%lf", &pc_vd_reg)!=1)
		ERROR_ABORT(ERROR_PROBECARD);

	if (!Log.getNextSection("VDAC","TOKEN"))
		ERROR_ABORT(ERROR_PROBECARD);
	if (sscanf(Log.getNextLine(), "%lf", &pc_v_dac)!=1)
		ERROR_ABORT(ERROR_PROBECARD);

	if (!Log.getNextSection("VTOUT","TOKEN"))
		ERROR_ABORT(ERROR_PROBECARD);

	if (!Log.getNextSection("VAOUT","TOKEN"))
		ERROR_ABORT(ERROR_PROBECARD);

	existProbecard = true;
	Log.getNextSection();
}

void CChip::ReadTOKEN(CScanner &Log)
{
	if (sscanf(Log.getNextLine(),"%i", &token) != 1)
		ERROR_ABORT(ERROR_TOKEN)
	Log.getNextSection();
}

void CChip::ReadI2C(CScanner &Log)
{
	i2c = 1;
	Log.getNextLine();
	Log.getNextLine();
	char *s;
	int mask = 1;
	for (int i=0; i<16; i++)
	{
		s = Log.getNextLine();
		if (strlen(s) < 35) ERROR_ABORT(ERROR_I2C)
		int map = 0;
		for (int k = 15; k >= 0; k--)
		{
			map <<= 1;
			char ch = s[k*2+4];
			if (ch == '1') map++;
			else if (ch != '.') ERROR_ABORT(ERROR_I2C)
		}
		i2c_map[i] = map;
		if (map != mask) i2c = 0;
		mask <<= 1;
	}
	Log.getNextSection();
}

void CChip::ReadREADBACK(CScanner &Log)
{
	float prev;
	Log.getNextLine();
	Log.getNextLine();

	// --- empty log section
	if (strlen(Log.getLine()) < 6) { Log.getNextSection(); return; }

	if (sscanf(Log.getLine(),"Vdig_u %i %f %f", &vdig_u_adc, &prev, &vdig_u_value) != 3)
		ERROR_ABORT(ERROR_READBACK);
	if (sscanf(Log.getNextLine(),"Vana_u %i %f %f", &vana_u_adc, &prev,&vana_u_value) != 3)
		ERROR_ABORT(ERROR_READBACK);
	if (sscanf(Log.getNextLine(),"Vana_r %i", &vana_r_adc) != 1)
		ERROR_ABORT(ERROR_READBACK);
	if (sscanf(Log.getNextLine(),"Vbg %i", &vbg_adc) != 1)
		ERROR_ABORT(ERROR_READBACK);
	if (sscanf(Log.getNextLine(),"Iana %i %f %f", &iana_adc, &prev, &iana_value) != 3)
		ERROR_ABORT(ERROR_READBACK);
	existREADBACK = true;
	Log.getNextSection();
}

void CChip::ReadVANA(CScanner &Log)
{
	Log.getNextLine();
	ReadVANA_Entry(Log, 0,  20);
	ReadVANA_Entry(Log, 1,  60);
	ReadVANA_Entry(Log, 2, 100);
	ReadVANA_Entry(Log, 3, 140);
	ReadVANA_Entry(Log, 4, 180);
	existVANA = true;
	Log.getNextSection();
}

void CChip::ReadVANA_Entry(CScanner &Log, int index, int value)
{
	char *line = Log.getNextLine();
	if (strlen(line) < 15)
	{
		if (sscanf(line, "%i %lf", &dacVana[index], &Iana[index]) != 2)
			ERROR_ABORT(ERROR_VANA);
	}
	else
	{
		if(sscanf(line, "%i %lf mA %i %i %i",
				&dacVana[index], &Iana[index], &rb_UnregVana[index], &rb_RegVana[index], &rb_Iana[index]) != 5)
			ERROR_ABORT(ERROR_VANA);
	}

	if (dacVana[index] != value) ERROR_ABORT(ERROR_VANA);
}


void CChip::ReadITRIM(CScanner &Log)
{
	if (sscanf(Log.getNextLine(),"%i %lf", &Vana24, &Iana24) != 2)
		ERROR_ABORT(ERROR_ITRIM)
	existITRIM = true;
	Log.getNextSection();
}

void CChip::ReadCALDELSCAN(CScanner &Log)
{
	if (sscanf(Log.getNextLine(),"%i %i %i", &cds_x, &cds_y, &cds_del) != 3)
		ERROR_ABORT(ERROR_CALDELSCAN)
	cds_scan = Log.getNextLine();
	existCALDELSCAN = true;
	Log.getNextSection();
}

void CChip::ReadPIXMAP(CScanner &Log)
{
	Log.getNextLine();
	if (!pixmap.ReadPixMap(Log)) ERROR_ABORT(ERROR_PIXEL)
	Log.getNextSection();
}

void CChip::ReadPULSE(CScanner &Log)
{
	Log.getNextLine();
	if (!pixmap.ReadPulseHeight(Log)) ERROR_ABORT(ERROR_PH)
	Log.getNextSection();
}

void CChip::ReadPH1(CScanner &Log)
{
	Log.getNextLine();
	if (!pixmap.ReadPulseHeight1(Log)) ERROR_ABORT(ERROR_PH1)
	Log.getNextSection();
}

void CChip::ReadPH2(CScanner &Log)
{
	Log.getNextLine();
	if (!pixmap.ReadPulseHeight2(Log)) ERROR_ABORT(ERROR_PH2)
	Log.getNextSection();
}

void CChip::ReadPUC(CScanner &Log)
{
	// read [PUC1] section
	Log.getNextLine();
	if (!pixmap.ReadRefLevel(Log)) ERROR_ABORT(ERROR_PUC)
	Log.getNextSection();

	// read [PUC2] section
	if (!Log.isSection("PUC2"))	ERROR_ABORT(ERROR_PUC)
	Log.getNextLine();
	if (!pixmap.ReadLevel(Log,3)) ERROR_ABORT(ERROR_PUC)
	Log.getNextSection();

	// read [PUC3] section
	if (!Log.isSection("PUC3")) ERROR_ABORT(ERROR_PUC)
	Log.getNextLine();
	if (!pixmap.ReadLevel(Log,2)) ERROR_ABORT(ERROR_PIXEL)
	Log.getNextSection();

	pixmap.levelExist = true;
}

void CChip::ReadClass(CScanner &Log)
{
	if (sscanf(Log.getNextLine(),"%i", &logChipClass)!=1) ERROR_ABORT(ERROR_CLASS)
	Log.getNextSection();
}

void CChip::ReadPOFF(CScanner &Log)
{
	if (sscanf(Log.getNextLine(),"%i", &bin) != 1) ERROR_ABORT(ERROR_POFF)
	Log.getNextSection();
}


// ordering relation pixY, pixX, nEntry
bool operator>(CChip &a, CChip &b)
{
	if (a.picY > b.picY) return true;
	if (a.picY==b.picY)
	{
		if (a.picX>b.picX) return true;
		if ((a.picX==b.picX) && (a.nEntry > b.nEntry)) return true;
	}
	return false;
}


void CChip::CalcPicPos()
{

	if (existCHIP)
	{
		int pos = mapPos ^ 2;
		picX = 2*mapX + pos%2 + 1;
		picY = 2*mapY + pos/2 + 1;
		
	}
	else { picX = picY = 0; }
}


void CChip::CalculatePhase1()
{
	// set pic coordinates
	CalcPicPos();

	// -- mask trim bit 2/3 error for pixel (0/0) and (0/1)  - DEBUG - Is the bug still present in psi46test?
	pixmap.SetDefectTrimBit(0, 0, 2, false);
	pixmap.SetDefectTrimBit(0, 0, 3, false);
	pixmap.SetDefectTrimBit(0, 1, 2, false);
	pixmap.SetDefectTrimBit(0, 1, 3, false);

	// --- calculate threshold (mean, std, max col to col difference)
	n    = 0;
	pm   = 0.0;
	pm_col_max = 0.0;
	pstd = 0.0;
	pmin = 100;
	pmax = 0;

	if (bin==13) bin = 0;
	nPixDefect     = 0;
	nPixNoSignal   = 0;
	nPixNoisy      = 0;
	nPixUnmaskable = 0;
	nPixAddrDefect = 0;
	nPixNoTrim     = 0;
	nPixThrOr      = 0;

	int col, row, sum = 0, sum2 = 0;
	for (col=0; col<52; col++) for (row=0; row<80; row++)
	{
		if (pixmap.GetMaskedCount(col,row) > 0) nPixUnmaskable++;
		if (pixmap.GetUnmaskedCount(col,row) == 0) nPixNoSignal++;
		else if (pixmap.GetUnmaskedCount(col,row) > 1) nPixNoisy++;
		if (pixmap.GetDefectAddrCode(col,row)) nPixAddrDefect++;
		if (pixmap.GetDefectTrimBit(col,row)) { if (col && row) nPixNoTrim++; }
		if (pixmap.IsDefect(col,row)) { nPixDefect++; continue; }

		int y = pixmap.GetRefLevel(col,row);

		if (y < 200)
		{
			n++;
			sum  += y;
			sum2 += y*y;
			if (y<pmin) pmin = y;
			if (y>pmax) pmax = y;
		}
	}


	if (n>0)
	{
		pm    = double(sum)/n;
		pstd  = sqrt(double(sum2)/n - pm*pm);

		int n_col;
		double pm_col[52];
		for (col=0; col<52; col++)
		{
			n_col = 0;
			pm_col[col] = 0.0;
			int pcolsum = 0;
			for (row=0; row<80; row++)
				if (!pixmap.IsDefect(col,row))
				{
					n_col++;
					pcolsum += pixmap.GetRefLevel(col,row);
				}
			if (n_col>0) pm_col[col] = (double)pcolsum/n_col; else break;
		}
		if (n_col>0)
			for (col=1; col<52; col++)
			{
				double pm_col_diff = fabs(pm_col[col] - pm_col[col-1]);
				if ((col==1) || (col==51)) pm_col_diff /= 3.0;
				if (pm_col_diff > pm_col_max) pm_col_max = pm_col_diff;
			}
	} // if (n>0)


	// --- calculate pulse height (mean, std, max col to col difference)
#define PH1TOL   90
#define PH21TOL  60
	nPh      = 0;
	ph1mean  = 0.0;
	ph21mean = 0.0;
	ph1std   = 0.0;
	ph21std  = 0.0;
	ph_col_max = 0.0;
	nPhFail = 0;
	int sum1=0, sum1_2=0, sum21 = 0, sum21_2=0;
	if (pixmap.pulseHeight1Exist && pixmap.pulseHeight2Exist)
	{
		for (col=0; col<52; col++) for (row=0; row<80; row++)
		{
			int ph1 = pixmap.GetPulseHeight1(col,row);
			int ph2 = pixmap.GetPulseHeight2(col,row);
			if (ph1<10000 && ph2<10000) // value exists
			{
				nPh++;
				sum1    += ph1;
				sum1_2  += ph1*ph1;
				int phgain = ph2-ph1;
				sum21   += phgain;
				sum21_2 += phgain*phgain;
			}
		}
		if (nPh>0)
		{
			ph1mean  = double(sum1)/nPh;
			ph21mean = double(sum21)/nPh;
			ph1std  = sqrt(double(sum1_2)/nPh  - ph1mean*ph1mean);
			ph21std = sqrt(double(sum21_2)/nPh - ph21mean*ph21mean);
		}

		for (col=0; col<52; col++) for (row=0; row<80; row++)
		{
			int ph1 = pixmap.GetPulseHeight1(col,row);
			int ph2 = pixmap.GetPulseHeight2(col,row);
			if (ph1<10000 && ph2<10000) // value exists
			{
				int phgain = ph2-ph1;
				if (ph1<(ph1mean-PH1TOL) || ph1>(ph1mean+PH1TOL) ||
				    phgain<(ph21mean-PH21TOL) || phgain>(ph21mean+PH21TOL))
					nPhFail++;
			}
		}
	}

	if (nPixThrOr > nPixDefect) nPixDefect = nPixThrOr;
	if (nPhFail   > nPixDefect) nPixDefect = nPhFail;

	if (nPh>0)
	{
		int n_col;
		double ph_col[52];
		for (col=0; col<52; col++)
		{
			n_col = 0;
			ph_col[col] = 0.0;
			int pcolsum = 0;
			for (row=0; row<80; row++) if (!pixmap.IsDefect(col,row))
			{
				int ph1 = pixmap.GetPulseHeight1(col,row);
				int ph2 = pixmap.GetPulseHeight2(col,row);
				if (ph1<10000 && ph2<10000) // value exists
				{
					n_col++;
					pcolsum += ph2-ph1;
				}
			}
			if (n_col>0) ph_col[col] = (double)pcolsum/n_col; else break;
		}
		if (n_col>0)
			for (col=1; col<52; col++)
			{
				double ph_col_diff = fabs(ph_col[col] - ph_col[col-1]);
				if ((col==1) || (col==51)) ph_col_diff /= 3.0;
				if (ph_col_diff > ph_col_max) ph_col_max = ph_col_diff;
			}
	}
}


void CChip::CalculatePhase2()
{
	// === final chip classification ========================================

#define CHIPFAIL(code, failstr) { failCode = (code); return; }

	failCode = FAIL_NOFAIL;

	// --- class 5 -------------------------------------------------------
	chipClass = pickClass = 5;

	if (bin == 1) {	CHIPFAIL(FAIL5_BIN1) }
	if (bin == 2) {	CHIPFAIL(FAIL5_BIN2) }
	if (bin == 3) {	CHIPFAIL(FAIL5_BIN3) }
	if (bin == 4) {	CHIPFAIL(FAIL5_BIN4) }
	if ((bin == 5) && (nPixDefect>=4000)) {	CHIPFAIL(FAIL5_DEF) }

	// --- class 4 -------------------------------------------------------
	chipClass = pickClass = 4;

	if (nPixDefect     > 40) { CHIPFAIL(FAIL4_PIXNUM) }
	if (nPixThrOr      > 40) { CHIPFAIL(FAIL4_PIXNUM) }
	if (nPixUnmaskable > 0)  { CHIPFAIL(FAIL4_MASK) }
	if (nPixAddrDefect > 0)  { CHIPFAIL(FAIL4_ADDR) }

	if (65.0 < IdigOn) { CHIPFAIL(FAIL4_IDON) }
	if (65.0 < IanaOn) { CHIPFAIL(FAIL4_IAON) }
	if (IdigInit < 15.0 || 40.0 < IdigInit) { CHIPFAIL(FAIL4_IDINIT) }
	std::string pid = wafer->productId;
    if (!pid.compare("v2-v2B")) {
		if (IanaInit < 10.0 || 35.0 < IanaInit) { CHIPFAIL(FAIL4_IAINIT) }
	}
	else {
		if (IanaInit < 20.0 || 35.0 < IanaInit) { CHIPFAIL(FAIL4_IAINIT) }
	}

	// --- class 3 -------------------------------------------------------
	chipClass = pickClass = 3;

	if (nPixDefect>4) { CHIPFAIL(FAIL3_1PC) }

//  not used criteria (results are not in the log files):
//	int pdiff = pmax-pmin;

	if (pm    < 85.0 || 125.0 < pm)    CHIPFAIL(FAIL3_TMEAN)
	if (pstd  <  0.5 ||  8.0  < pstd)  CHIPFAIL(FAIL3_TSTD)
	if (pm_col_max > 5.0)              CHIPFAIL(FAIL3_TDIFF)


	if (ph1mean  <  20.0 || 140.0 < ph1mean) CHIPFAIL(FAIL3_PHOFFS)
	if (ph1std   >  12.0)                    CHIPFAIL(FAIL3_PHOFFS)
	if (ph21mean <  30.0 || 60.0 < ph21mean) CHIPFAIL(FAIL3_PHGAIN)
	if (ph21std  >   4.0)                    CHIPFAIL(FAIL3_PHGAIN)
	if (ph_col_max > 6.0)                    CHIPFAIL(FAIL3_PHDIFF)

	if (fabs(IdigInit - wafer->IdigInitMean) > 4.0) CHIPFAIL(FAIL3_IDCURRENT)
	if (fabs(IanaInit - wafer->IanaInitMean) > 5.0) CHIPFAIL(FAIL3_IACURRENT)


	// --- class 2 -------------------------------------------------------
	chipClass = 2; pickClass = 2;

	if (nPixDefect>0) { CHIPFAIL(FAIL2_1PM) }

	// --- class 1 -------------------------------------------------------
	chipClass = pickClass = 1;

}


void CChip::GetFailString()
{
	char s[128];

	switch (failCode)
	{
	case FAIL_NOFAIL: return;

	// --- class 5 -------------------------------------------------------
	case FAIL5_BIN1: 
		failstring = "shortcut"; break;

	case FAIL5_BIN2: 
		failstring = "IdigOn < 5 mA"; break;

	case FAIL5_BIN3: 
		failstring = "missing token"; break;

	case FAIL5_BIN4: 
		failstring = "I2C error"; break;

	case FAIL5_DEF:
		sprintf(s, "%i pixel defect", nPixDefect);
		failstring = s; break;

	// --- class 4 -------------------------------------------------------
	case FAIL4_COL:
		failstring = "0 dcol defect"; break; //not used

	case FAIL4_PIXNUM:
		sprintf(s, "%i pixel defect", nPixDefect);
		failstring = s; break;

	case FAIL4_MASK:
		sprintf(s, "%i pixel not maskable", nPixUnmaskable); break;

	case FAIL4_ADDR:
		sprintf(s, "%i pixel address defect", nPixAddrDefect);
		failstring = s; break;

	case FAIL4_IDON:
		sprintf(s, "IdigOn = %0.1f mA (<65 mA)", IdigOn);
		failstring = s; break;

	case FAIL4_IAON:
		sprintf(s, "IanaOn = %0.1f mA (<65 mA)", IanaOn);
		failstring = s; break;

	case FAIL4_IDINIT:
		sprintf(s, "IdigInit = %0.1f mA (15...40 mA)", IdigInit);
		failstring = s; break;

	case FAIL4_IAINIT:
        if (!wafer->productId.compare("v2-v2B")) {

			sprintf(s, "IanaInit = %0.1f mA (10...35 mA)", IanaInit);
			failstring = s;
		}
		else {
			sprintf(s, "IanaInit = %0.1f mA (20...35 mA)", IanaInit);
			failstring = s;
		}
		break;

	// --- class 3 -------------------------------------------------------
	case FAIL3_1PC:
		sprintf(s,"%i pixel defect (<=1%%)", nPixDefect);
		failstring = s; break;

	case FAIL3_TMEAN:
		sprintf(s,"Thrshold(mean) = %0.1f (30...80)", pm);
		failstring = s; break;

	case FAIL3_TSTD:
		sprintf(s,"Threshold(rms) = %0.2f (0.5...4.0)", pstd);
		failstring = s; break;

	case FAIL3_TDIFF:
		sprintf(s,"Threshold(max-min) = %i (5...30)", pmax-pmin);
		failstring = s; break;

	case FAIL3_PHOFFS:
		sprintf(s,"Pulse height offset = %0.1f(+/-%0.1f)", ph1mean, ph1std);
		failstring = s; break;

	case FAIL3_PHGAIN:
		sprintf(s,"Pulse height gain = %0.1f(+/-%0.1f)", ph21mean, ph21std);
		failstring = s; break;

	case FAIL3_PHDIFF:
		sprintf(s,"Pulse height gain diff = %0.1f", ph_col_max);
		failstring = s; break;

	case FAIL3_IDCURRENT:
		sprintf(s,"IdigInit = %0.1f mA (ID(wafer mean)+/-4 mA)", IdigInit);
		failstring = s; break;

	case FAIL3_IACURRENT:
		sprintf(s,"IanaInit = %0.1f mA (IA(wafer mean)+/-5 mA)", IanaInit);
		failstring = s; break;

	// --- class 2 -------------------------------------------------------
	case FAIL2_1PM:
		sprintf(s, "%i pixel defect", nPixDefect);
		failstring = s; break;
		
	default:
		failstring = "undefined fail code";
	}
}

