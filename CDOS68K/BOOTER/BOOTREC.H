/*	@(#)bootrec.h	1.1		*/

/*
 *	This is the funky boot record structure definition
 *	for Concurrent DOS.  It is defined as BYTE arrays because
 *	it is not WORD aligned, and the bytes within WORDs and
 *	LONGs are swapped.
 */

typedef struct
{				/* NOTE: STRUCTURE NOT WORD ALLIGNED!!	*/
	BYTE	jmp[3];		/* first three bytes are jump (branch)	*/
	BYTE	name[8];	/* eight byte OEM name and version	*/
	BYTE	sctrsz[2];	/* bytes per sector			*/
	BYTE	clstrsz;	/* sectors per cluster (power of 2)	*/
	BYTE	rsvsc[2];	/* reserved sectors			*/
	BYTE	numfats;	/* number of FATS			*/
	BYTE	rtdir[2];	/* number of root dir entries		*/
	BYTE	dksz[2];	/* number of sectors in logical disk	*/
	BYTE	meddsc;		/* media descriptor ID			*/
	BYTE	fatsz[2];	/* number of sectors in a FAT		*/
	BYTE	spt[2];		/* sectors per track			*/
	BYTE	hds[2];		/* number of heads			*/
	BYTE	hidsc[2];	/* hidden sectors			*/
	BYTE	xdksz[4];	/* extended disk size if dksz 0 	*/
	BYTE	fdats[2];	/* first sector num after root dir	*/
	BYTE	cdld[4];	/* code load base			*/
	BYTE	cdln[4];	/* code length				*/
	BYTE	dtld[4];	/* data load base			*/
	BYTE	dtln[4];	/* data length				*/
} BTREC;
*/
	BYTE	cdln[4];	/* code length				*/
	BYTE	dtld[4];	/* data load base			*/
	BYTE	dtln[