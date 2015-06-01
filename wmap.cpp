// wmap.cpp

#include "ps.h"
#include "parser.h"
#include "wmap.h"



void CWmap::init()
{
	int x, y, t;
	for (t=0; t<4; t++)	for (y=0; y<WMAPY; y++) for (x=0; x<WMAPX; x++)
		cnt[x][y][t] = 0;
}


bool CWmap::addChip(CChip &chip)
{
	int x = chip.mapX + WMAPOFFSX;
	if (x<0 || WMAPX<=x) return false;
	int y = chip.mapY + WMAPOFFSY;
	if (y<0 || WMAPY<=y) return false;
	int type = chip.mapPos;
	if (type<0 || 4<=type) return false;

	if (cnt[x][y][type]==0)
		bin_min[x][y][type] = bin_max[x][y][type] = chip.bin;
	else
	{
		if (chip.bin==0 || chip.bin==14) bin_max[x][y][type] = chip.bin;
		else if (chip.bin<bin_min[x][y][type])
			bin_min[x][y][type] = chip.bin;
		else if (chip.bin>bin_max[x][y][type])
			bin_max[x][y][type] = chip.bin;
	}
	cnt[x][y][type]++;
	return true;
}


#define BINCOUNT  15

void CWmap::write(CPostscript &ps, CParser &p, CChip &chip, char filename[])
{
	int bin, x, y, t, yield[BINCOUNT];
	for (bin=0; bin<BINCOUNT; bin++) yield[bin] = 0;

	ps.addPage();
	ps.puts("LOCAL begin\n");
	ps.printf("/fileName(%s)def\n", filename);
	ps.printf("/productId(%s)def\n", chip.productId);
	ps.printf("/waferId(%s)def\n", chip.waferId);
	ps.printf("/waferNr(%s)def\n", chip.waferNr);
	ps.printf("/testDate(%s)def\n", p.logTime);

	for (y=0; y<WMAPY; y++) for (x=0; x<WMAPX; x++) for (t=0; t<4; t++)
	{
		if (cnt[x][y][t])
		{
			int bin = bin_max[x][y][t];
			if (bin == 13) bin = 0;
			if (0<=bin && bin <BINCOUNT) yield[bin]++;
		}
	}
	ps.printf("/yield [");
	for (bin=0; bin<BINCOUNT; bin++) ps.printf(" %i", yield[bin]);

	ps.printf("]def\n[\n");

	for (y=0; y<WMAPY; y++) for (x=0; x<WMAPX; x++)
	{
		for (t=0; t<4; t++)	if (cnt[x][y][t])
			ps.printf("[%i %i %i %i]\n",
				bin_max[x][y][t], t, x-WMAPOFFSX, y-WMAPOFFSY);
	}
	ps.printf("]wafermapPage\nend showpage\n");
}

