
/*****************************************************************
 * "Copyright (C) 1985, Digital Research, Inc.  All Rights       *
 * Reserved.  The Software Code contained in this listing is     *
 * proprietary to Digital Research Inc., Monterey, California    *
 * and is covered by U.S. and other copyright protection.        *
 * Unauthorized copying, adaptation, distribution, use or        *
 * display is prohibited and may be subject to civil and         *
 * criminal penalties.  Disclosure to others is prohibited.  For *
 * the terms and conditions of software code use refer to the    *
 * appropriate Digital Research License Agreement."              *
 *****************************************************************/

/*===============================================================*
 *   Version 1.02       DH.H                                     *
 *                      Driver Header and Driver List Entry      *
 *                      Definitions.                             *
 *---------------------------------------------------------------*
 *    VERSION   DATE    BY      CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *                                                               *
 *   1.01     04/19/85 pjp      changed dh_code to dh_rsrv       *
 *   1.02     03/20/86 DR-K	define DHF_DELIM for OEM supplied*
 *				Read Delimited routine		 *
 *===============================================================*
 */

#define	DLE		struct dlestruct
#define	DH		struct dhstruct
#define DPBLK		struct dpblkstruct

#define	DLNSIZE	10	/*  number of characters in name field		*/
#define	FIRSTDLE 0x100	/*  FIRST DRIVER LIST ENTRY LOOKID		*/

DLE 
{
	DLE	*dl_next;
	UWORD	dl_id;		/* unique id		*/
	BYTE	dl_name[DLNSIZE]; /* name		*/

	UWORD	dl_type;	/* driver type		*/
	UBYTE	dl_unitno;	/* unit number		*/
	UBYTE	dl_state;	/* state of driver	*/
	UWORD	dl_own;		/* owner - RM number	*/
	UWORD	dl_access;	/* access flags		*/
	DH	*dl_dhp;	/* Driver Header-unused	*/
	DLE	*dl_sib;	/* Sibling-Unused	*/

	LONG	dl_ctrl1;	/* First Control Field	*/
	LONG	dl_ctrl2;	/* Second Control Field	*/
	LONG	*dl_lpb;	/* Load Memory Array	*/
};


	/*  values for dl_state */

#define	DLS_NOTINSTALLED	0x00	/*  not installed yet		*/
#define	DLS_NEEDSD	0x01		/*  needs sub-drive		*/
#define	DLS_RMOWNED	0x02		/*  owned by Resource Manager	*/
#define	DLS_DVROWNED	0x03		/*  owned by another driver unit*/

/*
 *  Driver Header
 *
 */

DH
{	/*  driver header  */

	UWORD	dh_reserve;	/* type of driver			*/
	UBYTE	dh_nbrunits;	/* max number of units supported	*/
	UBYTE	dh_flags;	/* flag word				*/
	LONG	(*dh_init)();	/* pointer to init code			*/
	LONG	(*dh_subdrv)();	/* pointer to subdrv code		*/
	LONG	(*dh_uninit)();	/* pointer to init code			*/
	LONG	(*dh_select)();	/* pointer to select code		*/
	LONG	(*dh_flush)();	/* pointer to flush code		*/
	LONG	(*dh_read)();	/* pointer to read code			*/
	LONG	(*dh_write)();	/* pointer to write code		*/
	LONG	(*dh_get)();	/* pointer to get code			*/
	LONG	(*dh_set)();	/* pointer to set code			*/
	LONG	(*dh_special)();/* pointer to special code		*/

	/*  all items below are initialized by o/s			*/

        DLE     *dh_1sib;	/* root of sibling list		*/
	LONG	dh_rsrv;	/* reserved			*/
	LONG	dh_mxid;	/* holds driver level sync mxid	*/
	LONG	dh_rlr;		/* address of Ready List Root		*/
	LONG	dh_functab;	/* ptr to os function table		*/
};

#define DHF_DSYNC	0x01	/* Sync at driver level */
#define DHF_USYNC	0x02	/* Sync at Unit level */
#define DHF_RSYNC	0x04	/* Sync at RM level */
#define DHF_WORD	0x08	/* WORD device, 0=BYTE device */
#define DHF_DELIM	0x10	/* has it's own delimited read function */

/*
 *  Driver Parameter Block
 *
 */

DPBLK
{
	BYTE	dp_unitno;	/*  unit number for request		*/
	BYTE	dp_option;	/*  option for request			*/
	UWORD	dp_flags;	/*  flags for request (see below)	*/
	LONG	dp_swi;		/*  address of calling proc's swi	*/
	LONG	dp_pdaddr;	/*  address of calling proc's proc descr*/
	LONG	dp_buffer;	/*  address of data buffer		*/
	LONG	dp_bufsiz;	/*  size of buffer (dev specific)	*/
	LONG	dp_offset;	/*  offset for i/o			*/
	LONG	dp_delim;	/*  parm specifying for delimiters	*/
};
buffer (dev 
cific)	*/
	LONG	dp_offset;	/*  offset for i/o			*/
	LONG	dp_delim;	/*  parm specifying for delimiters	*/
};
buffer (dev 