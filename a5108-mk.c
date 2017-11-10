
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

typedef unsigned short uint16;
typedef unsigned long uint32;
typedef unsigned char uchar;
typedef unsigned char boolean;

typedef struct _patch
{
	int offsetinObj;
	int length;
	int offsetinROM;
} patch;

typedef struct _patchI
{
	int type; // type = 0, replace the data; type=1, jmp to entry
	uint32 entry;
	uint32 offsetinROM;
} patchI;



#define CODE_AREA_BEGIN		0x001FA6C0
#define CODE_AREA_END		0x001FB230


//004E2734  51 46 01 22
struct _patch patchlist[] = {{0x0000, CODE_AREA_END-CODE_AREA_BEGIN, CODE_AREA_BEGIN}};

struct _patchI patchIlist[] = {
	
	{2, CODE_AREA_BEGIN +  0x1, 0x0019C7D4}, // hook_proto_pkt_header_pull

	{2, CODE_AREA_BEGIN +  0x5, 0x001BFC7E}, // hook_wlc_recv

	{2, CODE_AREA_BEGIN +  0x7, 0x0019B8E0}, // hook_show_string

	{2, CODE_AREA_BEGIN +  0x9, 0x0019C71E}, // hook_dngl_sendpkt
	
	{2, CODE_AREA_BEGIN +  0xB, 0x001AE338}, // hook_wlc_set_chanspec

	{2, CODE_AREA_BEGIN +  0xD, 0x001A8680}, // hook_wlc_down

	{5, 0x1E42A2, 0x1E3F32}, 

	{0, 0, 0},
};

int get_ofs_in_file(int ofs_in_memory)
{
	return (ofs_in_memory-0x198000);
}

unsigned char buf[8*1024*1024];

int main(int argc, char *argv[])
{
	FILE     *obj;
	int      olen=0;
	uint32   d = 0;
	uint32   sum = 0;
	int i,j;
	char fname[80] = "bcmdhd_sta.bin";
	char objname[80] = "main.bin";
	FILE *out;
	FILE *fd;

	system("cmd.exe /c copy bcmdhd_sta.bin.ok bcmdhd_sta.bin");

	if (argc > 2)
	{
	  strcpy(fname, argv[1]);
	}

	obj = fopen(objname, "rb");

	if (obj == NULL)
	{
	  return -1;
	}

	out=fopen(fname, "rb+");

	if (out == NULL)
	{
	  return -1;
	}

	for (i = 0; i < sizeof(patchIlist)/sizeof(patchIlist[0]); i++)
	{
		uint32 ofs;
		uint32 ofsx;
		uint32 dOrg=0;
		if (patchIlist[i].offsetinROM == 0)
		{
			continue;
		}

		ofs = patchIlist[i].offsetinROM;

		ofsx = get_ofs_in_file(ofs);

		fseek(out, ofsx, 0);
		fread(&dOrg, 4, 1, out);
		fseek(out, ofsx, 0);
		if (patchIlist[i].type == 0)
		{
			fwrite(&patchIlist[i].entry, 4, 1, out);
			printf("0x%08x -> %s 0x%08x, [0x%08x], ofsx=0x%x\n", 
				patchIlist[i].entry,
				fname, patchIlist[i].offsetinROM, dOrg,
				ofsx);
		}
		else if (patchIlist[i].type == 1)
		{
			uint32 bl;
			bl = (0xeb000000) + (((patchIlist[i].entry-patchIlist[i].offsetinROM)/4) & 0xffffff) - 2;
			fwrite(&bl, 4, 1, out);
			printf("0x%08x -> %s 0x%08x, [0x%08x], entry=0x%08x, ofsx=0x%08x (32-bit call)\n", 
				bl,
				fname, patchIlist[i].offsetinROM, dOrg, patchIlist[i].entry,
				ofsx);
		}
		else if (patchIlist[i].type == 2 || patchIlist[i].type == 3)
		{
			uint32 bl;
			uint32 joff = ((patchIlist[i].entry-(patchIlist[i].offsetinROM+4)))/2;

			bl =  ((patchIlist[i].type == 2)?(0xf800f000):(0xb800f000))
				 + ((joff & 0x7ff) << 16)
				 + ((joff >> 11) & 0x7ff);

			{
				fwrite(&bl, 4, 1, out);
				printf("0x%08x -> %s 0x%08x, [0x%08x], entry=0x%08x, ofsx=0x%08x (thumb call)\n", 
					bl,
					fname, patchIlist[i].offsetinROM, dOrg, patchIlist[i].entry,
					ofsx);
			}
		}
		else if (patchIlist[i].type == 5 || patchIlist[i].type == 6)
		{
			uint32 bl;
			int j;
			for (j = patchIlist[i].offsetinROM; j < patchIlist[i].entry; j+=2)
			{
			  if (patchIlist[i].type == 5)
			  {
				  unsigned char code_nop[2] = {0xC0, 0x46};
				  fwrite(code_nop, 2, 1, out);
				  bl = 0x46C0;
			  }
			  else
			  {
				  unsigned char fill_bytes[2] = {0xFF, 0xFF};
				  fwrite(fill_bytes, 2, 1, out);
				  bl = 0xFFFF;
			  }
			}
			printf("0x%08x -> %s 0x%08x, [0x%08x], 0x%08x..0x%08x , ofsx=0x%08x\n", 
				bl,
				fname, patchIlist[i].offsetinROM,   dOrg, patchIlist[i].offsetinROM, patchIlist[i].entry,
				ofsx);
		}
#if 0
		else if (patchIlist[i].type == 4)
		{
			if (patchIlist[i].entry == 1)
				patchIlist[i].entry = passwd1;
			else if (patchIlist[i].entry == 2)
				patchIlist[i].entry = passwd2;
			else if (patchIlist[i].entry == 3)
				patchIlist[i].entry = passwd3;

			fwrite((void *)patchIlist[i].entry, 0x24, 1, out);
			printf("%s -> %s 0x%08x, [0x%08x], ofsx=0x%08x\n", 
				patchIlist[i].entry,
				fname, patchIlist[i].offsetinROM, dOrg, 
				ofsx);
		}
#endif
	}

	fseek(obj, 0, 2);
	olen=ftell(obj);
	if (olen > patchlist[0].length)
	{
		fclose(obj);
		fclose(out);

		printf("error! there in no enough space ! %x > %x\n", olen, patchlist[0].length);
		system("cmd.exe /c del bcmdhd_sta.bin");
		exit(1);
	}

	for (i = 0; i < sizeof(patchlist)/sizeof(patchlist[0]); i++)
	{
		uint32 ofs1, ofsx;
		uint32 ofs;

		fseek(obj, patchlist[i].offsetinObj, 0);
		ofs = patchlist[i].offsetinROM;
		ofs1 = get_ofs_in_file(ofs);

		for (j = 0; j < patchlist[i].length; j++)
		{
			unsigned char o;
			
			ofsx = get_ofs_in_file(ofs);

			o=0;
			if (j < olen) {
				fread(&o, 1, 1, obj);
			}

			fseek(out, ofsx, 0);
			fwrite(&o, 1, 1, out);
			ofs++;
		}
/*
		{
			uint32 patchOfs = 0x20017EAC;
			uint32 bl;
			fseek(out, patchOfs-0x2000f764, 0);
			bl = (0xeb000000) + ((0x2004E19C-patchOfs)/4) - 2;
			fwrite(&bl, 4, 1, out);
		}
*/
			printf("%s 0x%08x -> %s 0x%08x\n  overwrite bytes=0x%x patch len(used)=0x%x (0x%x~0x%x)\n", 
				objname, patchlist[i].offsetinObj,
				fname, patchlist[i].offsetinROM,
				patchlist[i].length,
				olen, ofs1, ofsx);
	}
	fclose(obj);
	fclose(out);


	return 0;
}




