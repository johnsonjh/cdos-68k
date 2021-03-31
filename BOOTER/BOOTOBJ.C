/*	@(#)bootobj.c	1.4		*/
/*
 *	These routines are used to get and check the object file
 *	header of the program we want to boot into memory.
 *
 *	ASSUMPTIONS:
 *
 *	- The object file has been relocated to an absolute address.
 *
 *	- This address does not conflict with the booter's program
 *	  and data space.
 *
 *	- The object file is COFF or Old Format (12/27/84 - Old Format Only Now)
 *
 *	STILL TO BE DONE:
 *
 *	- Handle non-contiguous segments.
 *	
 *	- COFF format files.
 *	
 *	- An initial start address in the header.  Now it is zero.
 */

#include "portab.h"
#include "cdosboot.h"

/*
 *	THIS INFORMATION DEFINES THE OLD FORMAT OBJECT FILES.
 *
 *	Old Format object files are produced by the CP/M-68K
 *	compilers.
 */

/*
 *	Two of_magic numbers exist for Old Format object files
 *	one for contiguous segments, one for non-contiguous.
 *
 *	We don't muck with non-contiguous segments.
 */

#define MAGIC1	0x601a		/* contiguous segments */
#define MAGIC2	0x601b		/* non-contiguous segments */

/*
 *	Define the header structure where we get the object file
 *	layout for Old Format object files.
 */

typedef struct
{
	WORD	of_magic;
	LONG	of_tsize;	/* all sizes in BYTES! */
	LONG	of_dsize;
	LONG	of_bsize;
	LONG	of_symsize;
	LONG	of_reserved;	/* not used */
	BYTE	*of_tstart;
	WORD	of_relocinfo;	/* if 0 - relocatable; */
} OF_OBJHD;


GetObjectFileInfo(p_objinfo,bp)
REG OBJINFO	*p_objinfo;
BS	*bp;
{
	REG OF_OBJHD	*of_objhd;

	/*
	 *	We must be at least able to read the header!
	 */

	if( (bp->numbytes = ReadPcdos(bp->buf)) < sizeof(OF_OBJHD) )
	{
		PutMsg("Can't read object file header.");
		return ERROR;
	}

	of_objhd = (OF_OBJHD *)bp->buf;		/* template over buffer */
	bp->current = sizeof(OF_OBJHD);

	if( of_objhd->of_magic != MAGIC1 )
	{
		PutMsg("Bad object file of_magic number.");
		return ERROR;
	}

	if( of_objhd->of_relocinfo == 0 )
	{
		PutMsg("Object file is relocatable, must be absolute.");
		return ERROR;
	}

	p_objinfo->textsize = of_objhd->of_tsize;
	p_objinfo->datasize = of_objhd->of_dsize;
	p_objinfo->bsssize = of_objhd->of_bsize;
	p_objinfo->loadaddress = of_objhd->of_tstart;

	/*
	 *	We assume system starts execution at 0.
	 */

	p_objinfo->startaddress = (BYTE *)0L;
	return OK;
}
f_tstart;

	/*
	 *	We assume system starts execution at 0.
	 */

	p_objinfo->startadd