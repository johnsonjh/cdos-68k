/******************************************************************************
* SUP.H - Supervisor Definitions				Version 1.2.00
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
* v1.2.00 04/24/86 FRH	Change DEF_END from 1 to 0xff.  Fixes ^A stdout file
*			name problem.
* v1.0.00 06/17/85 KSO	Concurrent DOS 286 1.0 Release Version
******************************************************************************/

/*****************************************************************************\
 *									     *
 * FUNCT	Function Table Entry					     *
 *									     *
 *	The Supervision is table driven by function to optimize	flow into    *
 *	internal modules.  Each function has an entry describing interface   *
 *	activities that are necessary.					     *
 *									     *
 *	+-------+-------+-------+-------+-------+-------+---------------+    *
 *	| flags |  mod  | func  | size  | mmask | omask |     fmask	|    *
 *	+-------+-------+-------+-------+-------+-------+---------------+    *
 *	flags	describes special actions to be taken before the module is   *
 *		called							     *
 *	mod	The Module that accepts this function when the FT_1MOD flag  *
 *		is on.  When FT_1MOD is off, this is a bit map of modules    *
 *		that accepts this func.					     *
 *	func	The Internal Function # in the module to be called	     *
 *	size	The parameter block size, in bytes for this function	     *
 *	mmask	Mode field mask to detect invalid modes	for this function    *
 *	omask	Option field mask to detect invalid options for this funciton*
 *	fmask	Flags field mask to detect invalid flags for this function   *
 *									     *
\*****************************************************************************/

#define FUNCT	struct _fstruct

FUNCT
{
	UBYTE	ft_flags;	/* Function characteristics	*/
	UBYTE	ft_mod;		/* Module map or module #	*/
	UBYTE	ft_func;	/* Internal function #		*/
	UBYTE	ft_size;	/* Parameter block size		*/
	UBYTE	ft_mmask;	/* Legal mode mask		*/
	UBYTE	ft_omask;	/* Legal options mask		*/
	UWORD	ft_fmask;	/* Legal flags mask		*/
};

	/* ftab_flags 				*/

#define	FTF_FNUM	0x01	/* File Number Analysis	needed	*/
#define FTF_XLAT	0x02	/* Prefix Translation on name	*/
#define FTF_BUF		0x04	/* Buffer Range Check needed	*/
#define FTF_1MOD	0x08	/* Single Module Function.  If	*/
				/*   on, ftab_mod is module #	*/
				/*   If off, mod is a map of	*/
				/*   legal modules. Module is	*/
				/*   found through fnum or name.*/
#define FTF_PBLK 	0x10	/* Parameter Block Used		*/
#define FTF_ASYNC	0x20	/* Async Allowed		*/
#define FTF_SPEC 	0x40	/* Special Action by SUP needed	*/

	/* ftab_mod bits when used as a map	*/

#define FTM_KERN	0x01	/* Kernal			*/
#define FTM_PIPE	0x02	/* Pipe Manager			*/
#define FTM_DISK	0x04	/* Disk File System Manager	*/
#define FTM_CON		0x08	/* Console System Manager	*/
#define FTM_LOAD	0x10	/* Loader (Command)		*/
#define FTM_GSX		0x20	/* Graphics Manager		*/
#define FTM_NET		0x40	/* Network Manager		*/
#define FTM_DVR		0x80	/* Driver Manager		*/

	/* ftab_access 				*/

#define	FTP_SET		0x01	/* Set access			*/
#define	FTP_WRITE	0x02	/* Write access			*/
#define	FTP_READ	0x04	/* Read access			*/

/*****************************************************************************\
 *									     *
 * FNUMT	Open File Number Table Entry				     *
 *	The file number table associates an I/O request with the proper	     *
 *	resource manager to call for the particular request		     *
 *									     *
\*****************************************************************************/

	/* fnumt_access 			*/

#define	FA_SET		0x01	/* Set access			*/
#define	FA_EXEC		0x02	/* Execute access		*/
#define	FA_WRITE	0x04	/* Write access			*/
#define	FA_READ		0x08	/* Read access			*/

#define FNUMT	struct _fnumt
FNUMT			/* Fnum table entry			*/
{
	WORD	fnt_unique;	/* Unique number, matched with  */
				/*   unique field of fnum on	*/
				/*   file number validation	*/
	BYTE	fnt_table;	/* Table type			*/
	BYTE	fnt_offset;	/* Offset into fnum ext. struct	*/
				/*    of open file info.	*/
	BYTE	*fnt_ctrl;	/* Module supplied CTRL block	*/
	BYTE	fnt_mod;	/* Module that owns this open	*/
				/*     file entry		*/
	BYTE	fnt_fill;	/* Unused			*/
};

#define	FNUMTX	struct	_fnumtx
FNUMTX			/* Process info. of owner of open file	*/
{
	WORD	fe_rnid;	/* Node id of process		*/
	WORD	fe_rfid;	/* Family id of process		*/
	LONG	fe_rpid;	/* Process id of process	*/
	UWORD	fe_acc;		/* Access bits of open file	*/
};

	/* Structure to overlay file number as returned from	*/
	/* F_OPEN and supplied by user for I/O functions	*/

#define FNUM	struct fnumstruct
FNUM
{
	UWORD	fn_unique;	/* unique # to match in file	*/
				/*   number table.		*/
	UWORD	fn_index;	/* index into file number table	*/
};

/*****************************************************************************\
 *									     *
 * PBLK - Function Parameter Block					     *
 *									     *
 *	All Operating System Calls have two parameters, the Function Number  *
 *	and either a LONG value or the address of a Parameter Block.  The    *
 *	PBLK is designed to optimize flow through the SUPervisor to the      *
 *	resourc managers						     *
 *									     *
\*****************************************************************************/

#define	PBLK	struct pblkstruct

PBLK			/* Generic parameter block definition	*/
{
	BYTE	pa_mode;	/* See Mode Flags below		*/
	BYTE	pa_option;	/* Byte Parameter		*/
	UWORD	pa_flags;	/* See Flags below		*/
	BYTE	*pa_swi;	/* Software Interrupt Routine	*/
	LONG	pa_id;		/* usually File Number or Name	*/
	BYTE	*pa_buf;	/* Buffer Checking done on buf	*/
	LONG	pa_bsiz;	/*	and bsiz if necessary	*/
	LONG	pa_p6;		/* General param. place holder	*/
	LONG	pa_p7;		/* General param. place holder	*/
	FNUMT	*pa_fnaddr;	/* Fnum entry address		*/
};

	/*  pa_mode definition  */

#define PAM_ASYNC	0x01	/* Asynchronous function	*/

#define PAM_UADDR	0x02	/* Addresses originate from User*/
				/*	space			*/

#define	PAM_COMMAND	0x04	/* Function called by command	*/

#define	PAM_FE		0x08	/* Front End originated call	*/

	/*  pa_flags definition  */

#define PAF_LIT		0x4000	/* No DEFINE substitution to be	*/
				/* 	done on name in pa_id	*/

	/* pa_options definitions*/

#define	PAO_DIR		0x0001	/* Creating a directory		*/
#define	PAO_VCON	0x0002	/* Creating virtual console	*/

/*****************************************************************************\
 *									     *
 * DEFINE structure							     *
 *									     *
\*****************************************************************************/

#define	MAX_PREFIX	128	/* Maximum prefix length name	*/
#define	MAX_PATH	128	/* Maximum path name length	*/
#define	NAME_LEN	9	/* Max. logical name length	*/

#define	PAF_SYS		0x01	/* System/process table search	*/
#define	PAF_MODE	0x02	/* Get/Set mode bit		*/

#define	DBLKLEN		512	/* Original define block length		*/
#define	MAXLOOP		99	/* Max. loops through define tables	*/
				/*    when translating names		*/
#define	DEF_ADD		256	/* Size of additional space given to	*/
				/*    define block when growing		*/
#define	DEF_END		0xff	/* Define string terminator byte	*/

#define	DBD	struct	_dbd	/* DEFINE block descriptor		*/
DBD
{
	BYTE	*db_dblk;	/* Define memory buffer			*/
	UWORD	db_free;	/* Offset of next available space	*/
	UWORD	db_unique;	/* Next unique id			*/
	UWORD	db_len;		/* Length of define block		*/
	UWORD	db_ucnt;	/* Use count of block			*/
};

	/* Misc. macros		*/

#define	DOLCASE(x)	( ((x >= 'A') && (x <= 'Z')) ? x + 0x20 : x )
t;	/* Use count of block			*/
};

	/* Misc. macros		*/

#define	DOLCASE(x)	
(x >= 'A') && (x <= 'Z')) ? x + 0x20 : x )
t;	/* Use count of block			*/
};

	/* Misc. macros		*/

#define	DOLCASE(x)	