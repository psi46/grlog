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

	CChip* GetFirstM() { return first; }
	static CChip* GetPrevM(CChip *chip) { return chip ? chip->prev : NULL; }
	static CChip* GetNextM(CChip *chip) { return chip ? chip->next : NULL; }

	CChip* GetFirst();
	static CChip* GetPrev(CChip *chip);
	static CChip* GetNext(CChip *chip);

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

