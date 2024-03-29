/******************************************************************************
* SYSBUILD.H - Compile Time Options				Version 1.2.01
*******************************************************************************
* Copyright (c) 1985, Digital Research, Inc. All Rights Reserved. The Software
* Code contained in this listing is proprietary to Digital Research, Inc.,
* Monterey, California and is covered by U.S. and other copyright protection.
* Unauthorized copying, adaptation, distribution, use or display is prohibited
* and may be subject to civil and criminal penalties.  Disclosure to others is
* prohibited.  For the terms and conditions of software code use refer to the
* appropriate Digital Research License Agreement.
*******************************************************************************
* Version   Date   Who  Description
* ======= ======== ===  =======================================================
* v1.2.01 04/15/86 FRH	Add ASMADUMMY
* v1.2.00 04/14/86 FRH	Creation
******************************************************************************/

	/* CPU */

#define MC68K		1		/* 1 = Motorola 68000 		*/
#define IAPX286 	0		/* 1 = Intel 286 Protected Mode	*/
#define IAPX186 	0		/* 1 = Intel 186, 286 Real Mode	*/
#define IAPX86 		0		/* 1 = Intel 8086		*/
#define NECV60		0		/* 1 = NEC V60			*/

	/* C Compiler */

#define	LATTICE		0		/* 1 = Lattice (8086) Big Model	*/
#define METAWARE	0		/* 1 = Metaware Hi-C (286) Big	*/
#define ALCYON		1		/* 1 = Alcyon (68K)		*/

	/* Keyboard ASR Option */

#define KBBREAK		0	/* if 1, ` (backquote) causes breakpt */

	/* Timer/Tick Options */

#define	NEWTICK		1	/* If 1, use new tick system 		*/

	/* Async System Options */

#define EVCHECK		0	/* check for zeroed e_pred field in evremove */
#define ASRCHECK	1	/* check for zero ASRFREE list */
#define NEVTYPES	32	/* up to 32 unique event types supported */
				/* this determines the size of the ecan table*/

	/* Loader Options */

#define CLOADELF	0	/* If 1, use fast .286 loader */

	/* memory management options */

#define GDT_LIMIT 	0x29ff	/* Size of GDT Table */

	/* The defines of the form ASMXXXXX determine whether the	*/
	/*	routine XXXXX has been converted to assembler.		*/
	/*	If defined as 0, the C version is used; otherwise	*/
	/*	use the assembler version 				*/

#define	ASMDOASR	1
#define ASMZOMBIE	1
#define ASMASRINSERT	1
#define ASMNEXTASR	1
#define ASMEVINSERT	1
#define ASMEVREMOVE	1
#define ASMMXREL	0
#define ASMEVDONE	1
#define ASMSIGNAL	1
#define ASMRUNHIM	0
#define ASMASETUP	1
#define	ASMNODISP	0
#define ASMOKDISP 	0
#define ASMPINTON	0	/* 1 = ASM version of pinton()		     */
#define ASMDISP		1	/* 1 = ASM version of disp()		     */
#define ASMMIS		0	/* 1 = ASM Version of MISMAN I/O path	     */
#define ASMPIPE		0	/* 1 = ASM version of PIPEMAN for READ/WRITE */
#define	AFLAGSET	0	/* 1 = ASM version of flagset()		     */
#define AFLAGCLR	0	/* 1 = ASM version of flagclr()		     */
#define AFLAGEVENT	0	/* 1 = ASM version of flagevent()	     */
#define ASMADUMMY	0	/* 1 = ASM version of _adummy()		     */
                                                    
	     */
#define ASMADUMMY	0	/* 1 = ASM version of _adummy()		     */
                                                    