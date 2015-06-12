// waferdatabase.cpp

#include "stdafx.h"


// === chip list operations =================================================

// --- Iterates through ROCs and skips multiple tests of the same ROC -------

CChip* CWaferDataBase::GetFirst()
{
	CChip *p = first;
	while (p)
	{
		if (p->multiNum == 0) return p;
		p = p->next;
	}
	return 0;
}


CChip* CWaferDataBase::GetPrev(CChip *chip)
{
	CChip *p = chip ? chip->prev : 0;
	while (p)
	{
		if (p->multiNum == 0) return p;
		p = p->prev;
	}
	return 0;
}


CChip* CWaferDataBase::GetNext(CChip *chip)
{
	CChip *p = chip ? chip->next : 0;
	while (p)
	{
		if (p->multiNum == 0) return p;
		p = p->next;
	}
	return 0;
}


// ---- Iterates through good ROCs (skips multiple tests) -------------------

CChip* CWaferDataBase::GetFirstGood()
{
	CChip *p = first;
	while (p)
	{
		if (p->chipClass == 1 && p->multiNum == 0) return p;
		p = p->next;
	}
	return 0;
}


CChip* CWaferDataBase::GetPrevGood(CChip *chip)
{
	CChip *p = chip ? chip->prev : 0;
	while (p)
	{
		if (p->chipClass == 1 && p->multiNum == 0) return p;
		p = p->prev;
	}
	return 0;
}


CChip* CWaferDataBase::GetNextGood(CChip *chip)
{
	CChip *p = chip ? chip->next : 0;
	while (p)
	{
		if (p->chipClass == 1 && p->multiNum == 0) return p;
		p = p->next;
	}
	return 0;
}


// --- Iterates through bad ROCs (skips multiple tests) ---------------------

CChip* CWaferDataBase::GetFirstBad()
{
	CChip *p = first;
	while (p)
	{
		if (p->chipClass != 1 && p->multiNum == 0) return p;
		p = p->next;
	}
	return 0;
}


CChip* CWaferDataBase::GetPrevBad(CChip *chip)
{
	CChip *p = chip ? chip->prev : 0;
	while (p)
	{
		if (p->chipClass != 1 && p->multiNum == 0) return p;
		p = p->prev;
	}
	return 0;
}


CChip* CWaferDataBase::GetNextBad(CChip *chip)
{
	CChip *p = chip ? chip->next : 0;
	while (p)
	{
		if (p->chipClass != 1 && p->multiNum == 0) return p;
		p = p->next;
	}
	return 0;
}


// --- Iterates through multiple tests --------------------------------------

CChip* CWaferDataBase::GetPrevTest(CChip *chip)
{
	CChip *p = chip ? chip->prev : 0;
	if (p) if (*p == *chip) return p;
	return 0;
}


CChip* CWaferDataBase::GetNextTest(CChip *chip)
{
	CChip *p = chip ? chip->next : 0;
	if (p) if (*p == *chip) return p;
	return 0;
}



bool CWaferDataBase::Add(CChip *chip)
{
	chip->prev = last;
	chip->next = 0;
	if (last) last->next = chip;
	last = chip;
	if (!first) first = chip;
	chipCount++;
	return true;
}


void CWaferDataBase::DeleteAllChips()
{
	CChip *p = first;
	while (p)
	{
		CChip *q = p;
		p = p->next;
		delete q;
	}
	first = last = 0;
	chipCount = 0;
}


void CWaferDataBase::Swap(CChip *entry)
{
	if (entry->next == 0) return;

	CChip *p = entry->next;
	if (entry->prev) entry->prev->next = p; else first = p;
	if (p->next) p->next->prev = entry; else last = entry;
	entry->next = p->next;
	p->prev = entry->prev;
	entry->prev = p;
	p->next = entry;
}


void CWaferDataBase::SortPicOrder()
{	// bubble sort
	if (first == last) return;
	bool swapped;
	do
	{
		swapped = false;
		CChip *p = first;
		while (p && p->next)
		{
			if (*p > *(p->next)) { Swap(p); swapped = true; }
			p = p->next;
		}
	} while (swapped);
}



// === helper functions =====================================================

// chips must be sorted before (SortPicOrder)
void CWaferDataBase::CalcMulti()
{
	CChip *p, *s;
	p = first;
	while (p)
	{
		int cnt = 0;
		s = p->next;
		while (s && (*s == *p))
		{
			cnt++;
			s = s->next;
		}
		for (int i = cnt; i >= 0; i--)
		{
			p->multiCount = cnt;
			p->multiNum   = i;
			p = p->next;
		}
	}
}


void CWaferDataBase::SetPicGroups()
{
	if (first == NULL) return;

	// count groups
	int nGroups = 0;
	int nClass;
	for (nClass=1; nClass<=5; nClass++)
	{
		int gchip = 0;
		CChip *p = first;
		while (p)
		{
			if (p->pickClass == nClass)
			{
				p->pickGroup = nGroups+1;
				gchip++;
				if (gchip>=16) { nGroups++;	gchip = 0; }
			}
			p = p->next;
		}
		if (gchip > 0) nGroups++;
	}
}


void CWaferDataBase::Clear()
{
	DeleteAllChips();
	logTime.SetDateTime(1900,1,1, 0,0,0);
	logVersion = "";
	center     = "PSI"; //as default. changed if section [CENTER] is present in the log.
	productId  = "";
	waferId    = "";
	waferNr    = "";
	startTime.SetDateTime(1900,1,1, 0,0,0);
}



// === data processing ======================================================

void CWaferDataBase::Calculate()
{
	CChip *p;

	// Calculate chip phase 1
	p = GetFirstM();
	while (p)
	{
		p->CalculatePhase1();
		p = GetNextM(p);
	}

	CalculatePhase1Unsorted();
	SortPicOrder();
	CalcMulti();

	// Calculate wafer phase 1 (with chip phase 1 data available)
	CalculatePhase1();

	// Calculate chip phase 2 (with wafer phase 1 data available)
	p = GetFirstM();
	while (p)
	{
		p->CalculatePhase2();
		p->GetFailString();
		p = GetNextM(p);
	}

	// Calculate wafer phase 2 (including chip phase 2 data)
	CalculatePhase2();
	SetPicGroups();
}


void CWaferDataBase::CalculatePhase1Unsorted()
{
// chips are in test order
	CChip *p = GetFirstM();
	if (p) startTime = p->startTime;
}


void CWaferDataBase::CalculatePhase1()
{
	// chips are sorted

}


void CWaferDataBase::CalculatePhase2()
{
// chips are sorted

}



// === generate output files ================================================

bool CWaferDataBase::GeneratePickFile(const std::string &filename)
{
	if (first == NULL) return false;
	FILE *f = fopen(filename.c_str(), "wt");
	if (f == NULL) return false;

	// count groups
	int nGroups = 0;
	int nClass;
	for (nClass=1; nClass<=5; nClass++)
	{
		int gchip = 0;
		CChip *p = GetFirst();
		while (p)
		{
			if (p->pickClass == nClass)
			{
				gchip++;
				if (gchip>=16) { nGroups++;	gchip = 0; }
			}
			p = GetNext(p);
		}
		if (gchip > 0) nGroups++;
	}

	time_t t;
	struct tm *dt;
	time(&t);
	dt = localtime(&t);

	fprintf(f, "Wafer: %s\n", waferId.c_str());
	fprintf(f,"Datum: %i.%i.%i\n", int(dt->tm_mday), int(dt->tm_mon+1), int(dt->tm_year+1900));
	fprintf(f,"Gruppen: %i\n", nGroups);
	fprintf(f,"Kommentar: none\n\n");

	fputs("	Chip#1	Chip#2	Chip#3	Chip#4	Chip#5	Chip#6	Chip#7	Chip#8"
		"	Chip#9	Chip#10	Chip#11	Chip#12	Chip#13	Chip#14	Chip#15	Chip#16 Klasse\n"
		"Gruppe:  X/Y     X/Y     X/Y     X/Y     X/Y     X/Y     X/Y     X/Y"
		    "     X/Y     X/Y     X/Y     X/Y     X/Y     X/Y     X/Y     X/Y\n",f);


	int group = 1;
	for (nClass=1; nClass<=5; nClass++)
	{
		int gchip = 0;
		CChip *p = GetFirst();
		while (p)
		{
			if (p->pickClass == nClass)
			{
				if (gchip == 0) fprintf(f,"  %3i", group);

				fprintf(f,"   %2i/%-2i", p->picX, p->picY);
				gchip++;

				if (gchip>=16)
				{
					group++;
					gchip = 0;
					fprintf(f,"  %4i\n", nClass);
				}
			}
			p = GetNext(p);
		}
		if (gchip > 0)
		{
			while (gchip<16) { fputs("        ",f);  gchip++; }
			fprintf(f,"  %4i\n", nClass);
			group++;
		}
	}

	fclose(f);
	return true;
}


bool CWaferDataBase::GenerateJSONfile(const std::string &filename)
{
	if (first == NULL) return false;
	FILE *f = fopen(filename.c_str(), "wt");
	if (f == NULL) return false;

	CChip *p = GetFirst();
	std::stringstream sstr;
	while (p)
	{
		char rocId[16];
		CChip &chip = *p;

		sprintf(rocId, "%s-%i%i%c", waferId.c_str(), chip.mapY, chip.mapX, "CDAB"[chip.mapPos]);
		//to handle reticle with different ROC types:
		std::string pid = productId;
		std::string strroc = rocId;
        if (!pid.compare("v2-v2B")) {
			if(!strroc.compare(strroc.size()-1,1,"B")) productId = "v2";
			else productId = "v2B";
		}
		//-----

		//to set -1 to variables not present in the log file:
		if(!chip.existREADBACK) { 
			chip.vdig_u_adc = chip.vdig_u_value = chip.vana_u_adc = chip.vana_u_value = -1;
			chip.vana_r_adc = chip.vbg_adc = chip.iana_adc = chip.iana_value = -1;
		}

		sstr << "{\"ROC_ID\": \""     << rocId << "\", ";
		sstr << "\"ROC_TYPE\": \""    << productId << "\", ";
		sstr << "\"TEST_DATE\": \""   << chip.startTime.GetXmlDateTime() << "\", ";
		sstr << "\"TEST_CENTER\": \"" << center << "\", ";
		sstr << "\"IDIGI\": "         << chip.IdigInit << ", ";
		sstr << "\"IANA\": "          << chip.Iana24 << ", ";
		sstr << "\"V24\": "           << chip.Vana24 << ", ";
		sstr << "\"VDCAP\": "         << chip.pc_vd_cap << ", ";
		sstr << "\"VDREG\": "         << chip.pc_vd_reg << ", ";
		sstr << "\"VDAC\": "          << chip.pc_v_dac << ", ";
		sstr << "\"DEFECTPIXELS\": "  << chip.nPixDefect << ", ";
		sstr << "\"TRIMPIXELS\": "    << chip.nPixNoTrim << ", ";
		sstr << "\"MASKPIXELS\": "    << chip.nPixUnmaskable << ", ";
		sstr << "\"THRESHOLDPIXELS\": " << chip.nPixThrOr << ", ";
		sstr << "\"NSIGPIXELS\": "    << chip.nPixNoSignal << ", ";
		sstr << "\"NOISEPIXELS\": "   << chip.nPixNoisy << ", ";
		sstr << "\"ADDRPIX\": "       << chip.nPixAddrDefect << ", ";
		sstr << "\"VDIGU_VOLTS\": "   << chip.vdig_u_value << ", ";
		sstr << "\"VDIGU_ADC\": "     << chip.vdig_u_adc << ", ";
		sstr << "\"VANAU_VOLTS\": "   << chip.vana_u_value << ", ";
		sstr << "\"VANAU_ADC\": "     << chip.vana_u_adc << ", ";
		sstr << "\"VANAR_ADC\": "     << chip.vana_r_adc << ", ";
		sstr << "\"VBG_ADC\": "       << chip.vbg_adc << ", ";
		sstr << "\"IANA_ADC\": "      << chip.iana_adc << ", ";
    	sstr << "\"IANA_MILLIAMPS\": " << chip.iana_value << ", ";
	    sstr << "\"VANASCAN\": "      << "[[" << chip.dacVana[0] << ", " << chip.Iana[0] << ", " << chip.rb_UnregVana[0] << ", " << chip.rb_RegVana[0] << ", " << chip.rb_Iana[0] << "], ";
								sstr  << "["  << chip.dacVana[1] << ", " << chip.Iana[1] << ", " << chip.rb_UnregVana[1] << ", " << chip.rb_RegVana[1] << ", " << chip.rb_Iana[1] << "], ";
 							    sstr  << "["  << chip.dacVana[2] << ", " << chip.Iana[2] << ", " << chip.rb_UnregVana[2] << ", " << chip.rb_RegVana[2] << ", " << chip.rb_Iana[2] << "], ";
		                        sstr  << "["  << chip.dacVana[3] << ", " << chip.Iana[3] << ", " << chip.rb_UnregVana[3] << ", " << chip.rb_RegVana[3] << ", " << chip.rb_Iana[3] << "], ";
		                        sstr  << "["  << chip.dacVana[4] << ", " << chip.Iana[4] << ", " << chip.rb_UnregVana[4] << ", " << chip.rb_RegVana[4] << ", " << chip.rb_Iana[4] << "]] , ";
		sstr << "\"RESULT\": "       << chip.chipClass << ", ";
		sstr << "\"COMMENT\": \""    << chip.failstring << "\"";
		sstr << "}\n";

		p = GetNext(p);
	}
    std::string output = sstr.str();
	fputs(output.c_str(), f);
	fprintf(f, "\n");

	fclose(f);
	return true;
}


bool CWaferDataBase::WriteXML_File(const std::string &pathname, CChip &chip)
{
	std::string filename;
	std::stringstream ss(filename);
	ss << pathname << waferId << '_' << chip.mapY << chip.mapX << "CDAB"[chip.mapPos];

	FILE *f = fopen(filename.c_str(),"wt");
	if (f==NULL) return false;

	fputs(
		"<?xml version='1.0' encoding='UTF-8'?>\n"
		"<!DOCTYPE root []>\n"
		" <ROOT>\n"
		" <HEADER>\n"
		"  <TYPE>\n"
		"   <EXTENSION_TABLE_NAME>BRL_ROC_ON_WAFER_TEST</EXTENSION_TABLE_NAME>\n"
		"   <NAME>BRL ROC On-Wafer Test</NAME>\n"
		"  </TYPE>\n"
		"  <RUN>\n", f);
	fprintf(f,
		"   <RUN_NAME>PSI46V2 %s</RUN_NAME>\n"
		"   <RUN_BEGIN_TIMESTAMP>%s</RUN_BEGIN_TIMESTAMP>\n",
		waferId.c_str(), chip.startTime.GetXmlDateTime().c_str());
	fputs(
		"   <COMMENT_DESCRIPTION>ROC On-Wafer Test</COMMENT_DESCRIPTION>\n"
		"   <INITIATED_BY_USER>Beat Meier</INITIATED_BY_USER>\n"
		"  </RUN>\n"
		" </HEADER>\n"
		" <DATA_SET>\n", f);
	fprintf(f,
		"  <COMMENT_DESCRIPTION>Test results from PSI for %s</COMMENT_DESCRIPTION>\n"
		"  <DATA_FILE_NAME>%s.txt</DATA_FILE_NAME>\n"
		"  <PART_ASSEMBLY>\n"
		"   <PARENT_PART>\n"
		"    <NAME_LABEL>%s</NAME_LABEL>\n"
		"  </PARENT_PART>\n"
		"   <CHILD_UNIQUELY_IDENTIFIED_BY>\n"
		"     <ATTRIBUTE>\n"
		"      <NAME>BRL ROC Number</NAME>\n"
		"      <VALUE>%i%i%c</VALUE>\n"
		"     </ATTRIBUTE>\n"
		"    </CHILD_UNIQUELY_IDENTIFIED_BY>\n"
		"  </PART_ASSEMBLY>\n", waferId.c_str(), waferId.c_str(), waferId.c_str(),
			chip.mapY, chip.mapX, "CDAB"[chip.mapPos]);
	fputs(
		"  <DATA>\n",f);

	// limit values for data base
	if (chip.chipClass < 0) chip.chipClass = 0;
	if (chip.chipClass > 9) chip.chipClass = 9;

	if (chip.IdigInit > 999.0) chip.IdigInit = 999.0;
	if (chip.IanaInit > 999.0) chip.IanaInit = 999.0;

	if (chip.Vana24 > 255) chip.Vana24 = 255;

	if      (chip.nPixDefect<0)        chip.nPixDefect = 0;
	else if (chip.nPixDefect>4160)     chip.nPixDefect = 4160;

	if      (chip.nPixNoSignal<0)      chip.nPixNoSignal = 0;
	else if (chip.nPixNoSignal>4160)   chip.nPixNoSignal = 4160;

	if      (chip.nPixUnmaskable<0)    chip.nPixUnmaskable = 0;
	else if (chip.nPixUnmaskable>4160) chip.nPixUnmaskable = 4160;

	if      (chip.nPixNoisy<0)         chip.nPixNoisy = 0;
	else if (chip.nPixNoisy>4160)      chip.nPixNoisy = 4160;

	if      (chip.nPixNoTrim<0)        chip.nPixNoTrim = 0;
	else if (chip.nPixNoTrim>4160)     chip.nPixNoTrim = 4160;

	if      (chip.nPixAddrDefect<0)    chip.nPixAddrDefect = 0;
	else if (chip.nPixAddrDefect>4160) chip.nPixAddrDefect = 4160;

	if (chip.bin<0) chip.bin = 0; else if (chip.bin>99) chip.bin=99;

	// begin of data set
	fprintf(f,"   <CHIPCLASS>%u</CHIPCLASS>\n",             chip.chipClass);
	fprintf(f,"   <CURRENT_DIG>%0.1f</CURRENT_DIG>\n",      chip.IdigInit>=0 ? chip.IdigInit : 0.0);
	fprintf(f,"   <CURRENT_ANA>%0.1f</CURRENT_ANA>\n",      chip.IanaInit>=0 ? chip.IanaInit : 0.0);
	fprintf(f,"   <VOLTAGE_ANA>%u</VOLTAGE_ANA>\n",         chip.Vana24>=0 ? chip.Vana24 : 0);
	fprintf(f,"   <PIXEL_DEFECT>%u</PIXEL_DEFECT>\n",       chip.nPixDefect);
	fprintf(f,"   <NO_SIGNAL>%i</NO_SIGNAL>\n",             chip.nPixNoSignal);
	fprintf(f,"   <MASK_DEFECT>%i</MASK_DEFECT>\n",         chip.nPixUnmaskable);
	fprintf(f,"   <NOISY>%i</NOISY>\n",                     chip.nPixNoisy);
	fprintf(f,"   <TRIM_BIT_DEFECT>%i</TRIM_BIT_DEFECT>\n", chip.nPixNoTrim);
	fprintf(f,"   <ADDRESS_DEFECT>%u</ADDRESS_DEFECT>\n",   chip.nPixAddrDefect);
	fprintf(f,"   <FLAG>%u</FLAG>\n",                       chip.bin);
	// end of data set

	fputs(
		"  </DATA>\n"
		" </DATA_SET>\n"
		"</ROOT>\n", f);

	fclose(f);
	return true;
}


bool CWaferDataBase::GenerateXML(const std::string &pathname)
{
	CChip *p = GetFirst();
	while (p)
	{
		if (!WriteXML_File(pathname, *p)) return false;
		p = GetNext(p);
	}
	return true;
}

void CWaferDataBase::GenerateFailStrings()
{
	CChip *p = GetFirst();
	while (p)
	{
			p->GetFailString();
			printf("%s\n",p->failstring.c_str());
		p = GetNext(p);
	}
	return;
}

bool CWaferDataBase::GenerateErrorReport(const std::string &filename)
{
	FILE *f = fopen(filename.c_str(), "wt");
	if (f==NULL) return false;

	CChip *p = GetFirst();
	while (p)
	{
		if (p->chipClass > 1)
		{
			fprintf(f,"Chip %i%i%c Class %i: ",
				p->mapY, p->mapX, "CDAB"[p->mapPos], p->chipClass);
			fprintf(f,p->failstring.c_str());
			fputs("\n", f);
		}
		p = GetNext(p);
	}

	fclose(f);
	return true;
}

bool CWaferDataBase::GenerateClassList(const std::string &filename) //fail code added for final stats
{
	FILE *f = fopen(filename.c_str(), "wt");
	if (f==NULL) return false;
	CChip *p = GetFirst();
	while (p)
	{
  	   fprintf(f,"%i%i%c %i %i",
				p->mapY, p->mapX, "CDAB"[p->mapPos], p->chipClass, (int)p->failCode);
			fputs("\n", f);
		p = GetNext(p);
	}
	fclose(f);
	return true;
}

bool CWaferDataBase::GenerateDataTable(const std::string &filename)
{
	FILE *f = fopen(filename.c_str(), "wt");
	if (f==NULL) return false;

	fprintf(f, "WAFER     POS  PX PY BIN C GR  IDIG0 IANA0 IDIGI IANAI VDREG VDAC  IANA V24  BLLVL ADSTP  DC DD WB TS DB DP  DPIX ADDR TRIM MASK NSIG NOIS THRO T2F T2P  PCNT PMEAN  PSTD PMCOL PMI PMA   NPH PHFAIL PHOMEAN PHOSTD PHGMEAN PHGSTD  FAIL  FAILSTRING\n");
	//          XY4L6GT   03C  11 11 11  4 12  123.5 123.4 137.5 130.0  2.23 2.00  24.0 123   -7.5  68.2  26 26 26 26 26 26  4000 4000 4000 4000 4000 4000 4000   9   9  4000  49.3 12.00 10.56  45  64  4000   4000   100.0   20.0   100.0  101.0    10  xxxxxxxxxxxxxxxx
	CChip *p = GetFirst();
	while (p)
	{
		fprintf(f, "%-9s %i%i%c  %2i %2i %2i  %i %2i  %5.1f %5.1f",
			waferId.c_str(), p->mapY, p->mapX, "CDAB"[p->mapPos], p->picX, p->picY, p->bin,
			p->pickClass, p->pickGroup, p->IdigOn, p->IanaOn);
		if (p->IdigInit>=0.0)  fprintf(f," %5.1f", p->IdigInit);    else fputs("      ",f);
		if (p->IanaInit>=0.0)  fprintf(f," %5.1f", p->IanaInit);    else fputs("      ",f);

		if (p->existProbecard)
		{
			fprintf(f," %5.2f %4.2f", p->pc_vd_reg, p->pc_v_dac);
		} else fputs("           ",f);

		if (p->Vana24 >= 0)  fprintf(f,"  %4.1f %3i", p->Iana24, p->Vana24);
		else fputs("          ",f);
		fputs("      ",f); // Aout
		fputs("                   ",f); // DCol

		if (p->failCode > CChip::FAIL5_BIN4)
		{
			fprintf(f,"  %4i", p->nPixDefect);
			fprintf(f," %4i",  p->nPixAddrDefect);
			fprintf(f," %4i",  p->nPixNoTrim);
			fprintf(f," %4i",  p->nPixUnmaskable);
			fprintf(f," %4i",  p->nPixNoSignal);
			fprintf(f," %4i",  p->nPixNoisy);
			fprintf(f," %4i",  p->nPixThrOr);

			fprintf(f,"  %4i", p->n);
			if (p->n > 0)
				fprintf (f," %5.1f %5.2f %5.2f %3i %3i",
					p->pm, p->pstd, p->pm_col_max, p->pmin, p->pmax);
			else fputs("                          ",f);

		} else fputs("                                                                            ",f);

		if (p->chipClass < 5 && p->nPh)
			fprintf(f,"  %4i   %4i %7.1f %6.1f %7.1f %6.1f",
				p->nPh, p->nPhFail, p->ph1mean, p->ph1std,
				p->ph21mean, p->ph21std);
			else fputs("                                           ",f);

		fprintf(f, "   %3i  ", p->failCode);
		fprintf(f,p->failstring.c_str());
		fputs("\n", f);
		p = GetNext(p);
	}
	fclose(f);
	return true;
}



/*
   wafer #chips 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
*/
bool CWaferDataBase::GenerateStatistics(const std::string &filename)
{
	FILE *f = fopen(filename.c_str(), "wt");
	if (f==NULL) return false;

	CChip *p = GetFirst();
	if (!p) { fclose(f); return false; }

	int i;
	int n = 0;
	int fail[24] = { 0 };
	int cl[5] = { 0 };

	fprintf(f,"wafer:  %s\n", waferId);

	while (p)
	{
		fail[p->failCode]++;
		cl[p->chipClass-1]++;
		n++;
		p = GetNext(p);
	}

	fprintf(f,"#Chips: %4i\n#fail: ", n);
	for (i=0; i<24; i++) fprintf(f," %4i", fail[i]);
	fputs("\n%fail: ",f);
	for (i=0; i<24; i++) fprintf(f," %4.1f", fail[i]*100.0/n);
	fprintf(f,"\n#Class:  ");
	for (i=0; i<5; i++) fprintf(f," %4i", cl[i]);
	fputs("\n%Class:  ",f);
	for (i=0; i<5; i++) fprintf(f," %4.1f", cl[i]*100.0/n);
	fputs("\n",f);
	fclose(f);
	return true;
}


#define MAXBINCOUNT 25

#define WMAPX  12
#define WMAPY  12
#define WMAPOFFSX  0
#define WMAPOFFSY  0


bool CWaferDataBase::GenerateWaferMap(const std::string &filename, unsigned int mode)
{
	int bincount;
	switch (mode)
	{
		case 0: // mode 0; bin
			bincount = 13; break;
		case 1: // mode 1: fail
			bincount = 24; break;
		case 2: // mode 2: class
			bincount = 5; break;
		default: return false;
	}

	CPostscript ps;

	int bin, yield[MAXBINCOUNT];
	for (bin=0; bin<bincount; bin++) yield[bin] = 0;

	if (!ps.open(filename.c_str())) return false;
	ps.putTempl("");
	ps.putTempl("prolog_begin.tmpl");
	ps.putTempl("wmap.tmpl");
	ps.putTempl("prolog_end.tmpl");

	CChip *p = GetFirst();
	if (p)
	{
		ps.addPage();
		ps.puts("LOCAL begin\n");
		switch (mode)
		{
			case 1:  ps.printf("/plotType(fail code)def\n"); break;
			case 2:  ps.printf("/plotType(class)def\n"); break;
			default: ps.printf("/plotType(bin)def\n"); break;
		}
		ps.printf("/center(%s)def\n",    center.c_str());
		ps.printf("/productId(%s)def\n", productId.c_str());
		ps.printf("/waferId(%s)def\n",   waferId.c_str());
		ps.printf("/testDate(%s)def\n",  startTime.GetXmlDateTime().c_str());
		switch (mode)
		{
			case 1:  ps.printf("/legend {legend_fail} def\n");  break;
			case 2:  ps.printf("/legend {legend_class} def\n"); break;
			default: ps.printf("/legend {legend_bin} def\n");   break;
		}
	}

	double totChips = 0.;
	double CL1Chips = 0.;

	while (p)
	{
		switch (mode)
		{
			case 1:  bin = p->failCode;  break;
			case 2:  bin = p->chipClass-1; break;
			default: bin = p->bin;
		}
		if (0<=bin && bin<bincount) yield[bin]++;
		p = GetNext(p);
	}
	ps.printf("/yield [");
	for (bin=0; bin<bincount; bin++) ps.printf(" %i", yield[bin]);
	ps.printf("]def\n[\n");

	p = GetFirst();
	while (p)
	{
		totChips++;
		switch (mode)
		{
			case 1:  bin = CColorScale::GetFailColorId(p->failCode);  break;
			case 2:  bin = CColorScale::GetClassColorId(p->chipClass-1); break;
			default: bin = CColorScale::GetBinColorId(p->bin);
		}
		if((p->chipClass)== 1) CL1Chips++;
		ps.printf("[%i %i %i %i]\n",
		bin, p->mapPos, p->mapX-WMAPOFFSX, p->mapY-WMAPOFFSY);
		p = GetNext(p);
	}
	double Yield = (CL1Chips/totChips)*100;
	char finalyield [20];
    sprintf(finalyield,"%.1f %%",Yield);	
	ps.printf("/Result(%s)def\n",   finalyield);
	
	//some shell output:
	if(mode==2){
	  std::cout << "Test date: " << startTime.GetXmlDateTime().c_str() << std::endl ;
	  std::cout << "# Chips: "   << totChips << std::endl ;
  	  std::cout << "# Chips CL1: " << CL1Chips << std::endl ;
	  std::cout << "Yield(CL1): " << finalyield << std::endl ;
	}

	ps.printf("]wafermapPage\nend showpage\n");
	ps.close();
	return true;
}

//new - just a first attempt to get yields from all the wafers by reading classlist.dat - to be integrated in the genaral structure
bool CWaferDataBase::GenerateYieldsFile(const std::string &filename, const std::string &batchname){

    std::string wlistfile = gName.GetPath_YieldsFile() + "waferlist_" + batchname + ".dat";
	std::cout << wlistfile << std::endl;

	int Fail[24] = {};
	int Cl[5] = {};
	int n=1;	
	std::vector<std::string> waferlist;
    ifstream infile;
    infile.open (wlistfile);
	if (infile == NULL) 
	{
		std::cout<< "ERROR: can not open " << wlistfile << std::endl;
		return false;
	}
    while(!infile.eof()) 
	{
		std::string wafern;
        std::getline(infile,wafern);
		waferlist.push_back(wafern);
    }
    infile.close();

	for(int j=0; j< waferlist.size(); j++)
	{
	  std::string clistfile = gName.GetPath_YieldsFile().c_str() + waferlist[j] + "_classlist.txt";
	  ifstream infile;
	  infile.open(clistfile);
	  if (infile == NULL) 
	  {
	    std::cout << "can not open file " << clistfile;
	    return false;
	  }
      int fail, cl, roc;
	  cl = fail = 0;
	  char g;
      while(!infile.eof())
	  {
	    std::string line;
	    getline(infile,line);
	    sscanf(line.c_str(),"%i%c %i %i",&roc,&g,&cl,&fail);	  
	    for(int i=0; i<5; i++) {
 		  if(cl == i+1) Cl[i]++;
	    }  
	    for(int i=0; i<24; i++) {
		  if(fail == i) Fail[i]++;
	    }  
	    n++;
      }n--;
      infile.close();
    }

    ofstream foutput;
    foutput.open (filename);  
	foutput << "Batch: " << batchname << "  totWafers: " << waferlist.size() << "  totROCs: " << n-1 << std::endl << std::endl;
	foutput << "Fail\t#ROCs\tyield(%)" << std::endl;
	foutput.precision(2);
	foutput << std::fixed;
	for(int i=0; i<24; i++)
	{
		double yield = ((double)Fail[i])/(double)(n-1)*100;
		foutput << i << "\t" << Fail[i] << "\t" << yield  << std::endl;
	}  
	foutput << std::endl << "Class\t#ROCs\tyield(%)" << std::endl;
	for(int i=0; i<5; i++)
	{
		double yield = ((double)Cl[i])/(double)(n-1)*100;
		foutput << i+1 << "\t" << Cl[i] << "\t" << yield << std::endl;
	}  
    foutput.close();
    return true;
}
