// wmap.h

#ifndef WMAP_H
#define WMAP_H


#define TYPE_A  0
#define TYPE_B  1
#define TYPE_C  2
#define TYPE_D  3

#define WMAPX  12
#define WMAPY  12
#define WMAPOFFSX  1
#define WMAPOFFSY  1

class CWmap
{
	int cnt[WMAPX][WMAPY][4];
	int bin_min[WMAPX][WMAPY][4];
	int bin_max[WMAPX][WMAPY][4];
public:
	void init();
	bool addChip(CChip &chip);
	void write(CPostscript &ps, CParser &p, CChip &chip, char filename[]);
};


#endif
