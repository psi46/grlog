// chipdatabase.h

#pragma once

#include <string>
#include "pixelmap.h"
#include "dtime.h"

class CWaferDataBase;
class CPixelMap;
class CScanner;
class CPixelMap;
class CScanner;


class CChip
{
protected:
	CChip *prev, *next;
public:
	typedef enum
	{
// --- table of failcodes -------------------------------------------------
		FAIL5_BIN1   =  0,	// shortcut (bin 1)
		FAIL5_BIN2   =  1,	// low current (< 5mA) (bin 2)
		FAIL5_BIN3   =  2,	// missing token out from ROC (bin 3)
		FAIL5_BIN4   =  3,	// I2C address decoding error (bin 4)
		FAIL5_DEF    =  4,	// > 4000 pixel defect
		FAIL4_COL    =  5,	// dcol defect (not existing for PSI46dig)
		FAIL4_PIXNUM =  6,	// > 40 pixel defect
		FAIL4_MASK   =  7,	// non maskable pixel
		FAIL4_ADDR   =  8,	// pixel with wrong address
		FAIL4_IDON   =  9,	// ID(on) value out of range
		FAIL4_IAON   = 10,	// IA(on) value out of range
		FAIL4_IDINIT = 11,	// ID(init) value out of range
		FAIL4_IAINIT = 12,	// IA(init) value out of range
		FAIL3_1PC    = 13,	// > 4 pixel defect
		FAIL3_TMEAN  = 14,	// Threshold mean value out of range
		FAIL3_TSTD   = 15,	// Threshold RMS out of range
		FAIL3_TDIFF  = 16,	// Threoshold col to col difference to high
		FAIL3_PHOFFS = 17,	// Pulse height offset
		FAIL3_PHGAIN = 18,	// Pulse height gain
		FAIL3_PHDIFF = 19,	// Pulse height gain col to col difference to high
		FAIL3_IDCURRENT = 20, // VD current limits
		FAIL3_IACURRENT = 21, // VA current limits
		FAIL2_1PM    = 22,	// 1..4 pixel defect
		FAIL_NOFAIL  = 23,	// No error
		FAIL_UNDEF   = 24,	// Undefined error
	} TFailCode;


// === measured values (log file entries) ===================================

	void Clear();  // sets all measured values to default value

//	[WAFER]
	CWaferDataBase *wafer;

//	[CHIP] chip on wafer
	bool existCHIP;
	int mapX;   // x position on map
	int mapY;   // y position on map
	int mapPos; // CDAB position (0=C, 1=D, 2=A, 3=B)

//	[CHIP1] single chip (only for single ROC test)
	std::string chipId; // empty if CHIP1 not exists

//	[BEGIN]
	Cdtime startTime; // ROC test start timestamp

//	[END]
	Cdtime endTime; // ROC test end timestamp

//	[PON]
	int nEntry; // 0 = no [PON] section found
	double IdigOn, IanaOn; // mA

//	[INIT]
	bool existINIT;
	double IdigInit, IanaInit;

//	[VDCAP] [VDREG] [VDAC] ([VTOUT] [VAOUT])
	bool  existProbecard;
	double pc_vd_cap;    // VD measured at the capacitor pad
	double pc_vd_reg;    // Vdig reg measured with probecard at the capacitor pad
	double pc_v_dac;     // Vadc reg measured with probecard at the capacitor pad

//  [TOKEN]
	int token;           // token arival time (<0: not existing; 1: ok)

//  [I2C] I2C address decoding test
	int i2c;             // <0 = not existing, 0=error, 1=ok
	int i2c_map[16];

//	[READBACK]
	bool existREADBACK;
	int vdig_u_adc;      // VD unreg (readback)
	int vana_u_adc;      // VA unreg (readback)
	int vana_r_adc;      // Vana reg (readback)
	int vbg_adc;         // V bandgap (readback)
	int iana_adc;        // IA (readback)
	float vdig_u_value;  // VD unreg (DTB)
	float vana_u_value;  // VA unreg (DTB)
	float iana_value;    // IA (DTB)

//	[VANA]
	bool existVANA;
	int dacVana[5];      // vana DAC values
	double Iana[5];      // IA / mA
	int rb_UnregVana[5]; // VA unreg (readback) (-1 = not existing)
	int rb_RegVana[5];   // Vana reg (readback) (-1 = not existing)
	int rb_Iana[5];      // IA (readback) (-1 = not existing)

//	[ITRIM]
	bool existITRIM;
	int Vana24;          // vana DAC value for Iana24
	double Iana24;       // trimmed IA value for Vana24 (~ 24 mA)

//	[CALDELSCAN]
	bool existCALDELSCAN;
	int cds_x;           // x position of test pixel
	int cds_y;           // y position of test pixel
	int cds_del;         // calculated optimal caldel value for test
	std::string cds_scan;  // scan result caldel 0 to 200

//  [PHSCAN]

//	[PIXMAP] [PULSE] [PH1] [PH2] [PUC1] [PUC2] [PUC3]
	CPixelMap pixmap;

//  [CLASS]
	int logChipClass;    // <0 = not existing

//	[POFF]
	int bin;             // <0 = not existing


// === derived values (phase 1) =============================================

	void CalculatePhase1();    // calculates derived values (phase1)
	void CalcPicPos();

//  number of entries in log file for this ROC
	int multiCount;
	int multiNum;

//  pixel shreshold statistics
	int n;              // number of pixels in statistic
	double pm;          // mean
	double pm_col_max; // difference between adjacent cols
	double pstd;        // std dev
	int pmin;           // min
	int pmax;           // max

// pixel pulse height statistics
	int nPh;            // number if pixels in statistic
	double ph1mean;     // mean PH1 values (offset)
	double ph_col_max;  // difference between adjacent cols
	double ph21mean;    // mean PH2-PH1 value (gain)
	double ph1std;      // std dev PH1 values (offset)
	double ph21std;     // std dev PH2-PH1 values (gain)
	int nPhFail;        // # pixel with pulse height outside test criteria

// --- error counts
	int nPixDefect;     // total defect pixels
	int nPixNoSignal;   // dead pixels
	int nPixNoisy;      // noisy pixels
	int nPixUnmaskable; // unmaskable pixels
	int nPixAddrDefect; // pixels with address error
	int nPixNoTrim;     // non trimmable pixels
	int nPixThrOr;      // pixels with threshold overrange


// === derived values (phase 2) =============================================
//     including global wafer information

	void CalculatePhase2();    // calculates derived values (phase2)

//  Final ROC classification (see Calculate )
	int chipClass;

//  Detailed defect code (see Calculate )
	TFailCode failCode;

//  [COMMENT]
	std::string failstring;

//  info for pic file
	int picX;
	int picY;
	int pickGroup;
	int pickClass;

	
// ==========================================================================

//	CChip() { Invalidate(); }
	void Save(FILE *f);
protected:
	void Read(CScanner &Log);
	void ReadCHIP(CScanner &Log);
	void ReadCHIP1(CScanner &Log);
	void ReadBEGIN(CScanner &Log);
	void ReadEND(CScanner &Log);
	void ReadPON(CScanner &Log);
	void ReadINIT(CScanner &Log);
	void ReadProbecard(CScanner &Log);
	void ReadTOKEN(CScanner &Log);
	void ReadI2C(CScanner &Log);
	void ReadREADBACK(CScanner &Log);
	void ReadVANA(CScanner &Log);
	void ReadVANA_Entry(CScanner &Log, int index, int value);
	void ReadITRIM(CScanner &Log);
	void ReadCALDELSCAN(CScanner &Log);
	void ReadPIXMAP(CScanner &Log);
	void ReadPULSE(CScanner &Log);
	void ReadPH1(CScanner &Log);
	void ReadPH2(CScanner &Log);
	void ReadPUC(CScanner &Log);
	void ReadClass(CScanner &Log);
	void ReadPOFF(CScanner &Log);

	void Pic();
private:
	void GetCurrent(CScanner &Log, double *Idig,double *Iana);
	void GetFailString();
	friend class CScanner;
	friend struct CLogFile;
	friend class CWaferDataBase;
};


bool operator>(CChip &a, CChip &b);


inline bool operator==(CChip &a, CChip &b)
{
	return (a.picY == b.picY) && (a.picX == b.picX);
}
