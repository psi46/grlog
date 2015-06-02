// waferdatabase.h

#pragma once

#include <string>
#include "chipdatabase.h"



class CWaferDataBase
{
	CChip *first;
	CChip *last;
	unsigned int chipCount;
	void Swap(CChip *entry);
	bool WriteXML_File(char path[], CChip &chip);

	void SortPicOrder();
	void SetPicGroups();
	void GenerateFailStrings();
	void CalcMulti();
	
	void CalculatePhase1Unsorted();
	void CalculatePhase1();
	void CalculatePhase2();

public:
	CWaferDataBase() : first(0), last(0), chipCount(0) {}
	~CWaferDataBase() { DeleteAllChips(); }

	// Iterates through all ROCs
	CChip* GetFirstM() { return first; }
	static CChip* GetPrevM(CChip *chip) { return chip ? chip->prev : 0; }
	static CChip* GetNextM(CChip *chip) { return chip ? chip->next : 0; }

	// Iterates through ROCs and skips multiple tests of the same ROC
	CChip* GetFirst();
	static CChip* GetPrev(CChip *chip);
	static CChip* GetNext(CChip *chip);

	// Iterates through good ROCs (skips multiple tests)
	CChip* GetFirstGood();
	static CChip* GetPrevGood(CChip *chip);
	static CChip* GetNextGood(CChip *chip);

	// Iterates through bad ROCs (skips multiple tests)
	CChip* GetFirstBad();
	static CChip* GetPrevBad(CChip *chip);
	static CChip* GetNextBad(CChip *chip);

	// Iterates through multiple tests
	static CChip* GetPrevTest(CChip *chip);
	static CChip* GetNextTest(CChip *chip);


	bool Add(CChip *chip);
	void DeleteAllChips();

	unsigned int GetChipCount() { return chipCount; }


	bool GeneratePickFile(char filename[]);
	bool GenerateJSONfile(char filename[]);
	bool GenerateXML(char path[]);
	bool GenerateErrorReport(char filename[]);
	bool GenerateClassList(char filename[]);
	bool GenerateDataTable(char filename[]);
	bool GenerateStatistics(const char filename[]);
	bool GenerateWaferMap(char filename[], unsigned int mode);


	void Clear();
	void Calculate();

// === log file entries =====================================================

	Cdtime logTime;
	std::string logVersion;
	std::string center;
	std::string productId;
	std::string waferId;
	std::string waferNr;

// === calculated values (phase 1) ==========================================
	Cdtime startTime;

// === calculated values (phase 2) ==========================================


};

