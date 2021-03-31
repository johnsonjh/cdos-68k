/************************************************************************
*									
*	RAM DISK- Concurrent 68K Ram Disc driver 		
*	====================================================
*							
*						
*  Copyright Digital Research Inc. 1985	
*
*  TBD:
*	handle requests for > 64k sectors (either error or correct).
*	headers in file
*	take care of kludge in init (init fat and dir)
*				
************************************************************************/

#define	OLDCODE	1		/* TRUE means don't impl changes yet	*/

#define Last_change	"18 Dec 85" 	/* M. Alexander			*/
#define Version		"00.02"		/*				*/


#include	"portab.h"
#include	"system.h" 
#include	"io.h"
#include	"btools.h"


/****************************************************************************
**	Ram Disk Configuration
****************************************************************************/

#define	RDNSECS		256	/* default nbr of sectors on 'disk'	*/
#define	RDNCLRCHK	3	/* nbr of chunks to clear for fat and dir*/

#define	RDBYTSEC	512	/* bytes per sector			*/
#define	RDSECTRK	8	/* sectors per track			*/
#define	RDBYTCHK	32768	/* nbr bytes per 'chunk' (area)		*/
#define	RDNCHKS		16	/* nbr of chunks for each disk		*/
#define	RDMAXUNITS	4	/* max nbr of units			*/

#define MSECTORSZ	RDBYTSEC /* memory disc standard sector size 	*/
#define MSECTRK		RDSECTRK /* memory disc sectors/track		*/
#define MNAREAS		RDNCHKS	/* max number of area pointers 		*/

#define	MSECAREA	(RDBYTCHK/RDBYTSEC)

/****************************************************************************
**	local defines 
****************************************************************************/

#define	SECTOR	LONG		/*  size of a sector data item		*/

#define	ES_DISK	0L		/*  for the sake of ICL code		*/
#define	RDREAD	0
#define	RDWRITE	1

#define DPF_HSCADDR	0x0002	/* rec is head sect cyl not logical sector */
#define DPF_VERIFY	0x0004	/* verify, do not read */
#define DPF_WRITE	0x0100	/* write (else read) */

#ifndef	min
#define	min(a,b)	( a < b ? a : b )
#endif


/****************************************************************************
**	local structure declarations
****************************************************************************/

/*
**  MDB - Media Descriptor Block
*/

#define MDB	struct MediaDescrBlock
MDB	
{				/* for SELECT return */
	WORD	md_secsize;	/* sector size bytes */
	WORD	md_1sec;	/* first sector of FAT from track 0 */
	LONG	md_nsecs;	/* total disc size sectors from track 0 */
	WORD	md_sectrk;	/* sectors/track */
	WORD	md_secblk;	/* sectors/block (cluster) */
	BYTE	md_nfats;	/* number of FATs */
	BYTE	md_fatid;	/* FAT identification byte (MDB) */
	WORD	md_nfrecs;	/* sectors/FAT */
	WORD	md_dirsize;	/* entries in ROOT directory */
	BYTE	md_nheads;	/* number of heads */
	UBYTE	md_format;	/* format - see below */
	LONG	md_nhidden;	/* number hidden sectors before partition */
	LONG	md_syssize;	/* bytes in system area */
};

/* format values */
#define MDFRAW		0	/* alien or CP/M media */
#define MDFPC		1	/* PC media - 12 bit FAT entries */
#define MDFPC16		2	/* PC media - 16 bit FAT entries */



/* 
**  HSCADDR - Hd / Sect / Cyl address block
*/

#define HSCADDR	struct HeadSectorCylinder
HSCADDR	
{
	UBYTE	hsc_head;	/* from 0 */
	UBYTE	hsc_sector;	/* from 1 */
	UWORD	hsc_cyl;	/* from 0 */
};


/*
**  SPECPB - Special Parm Block
*/

#define SPECPB	struct SpecialParamBlock
SPECPB	
{				/* Parameter block for special functions */
	UBYTE	sp_unitno;	/* unit number */
	UBYTE	sp_function;	/* function number */
	UWORD	sp_flags;	/* flags - see above, also below */
	BYTE	*sp_sbaddr;
	LONG	sp_pdaddr;	/* proc descr addr for MAPU */
	BYTE	*sp_buffer;	/* data buffer */
	LONG	sp_bufsiz;	/* data buffer size bytes */
	BYTE	*sp_pbuffer;	/* parameter buffer */
	LONG	sp_pbfsiz;	/* parameter buffer size bytes */
};

/* special functions */
#define SPF_RSYS	0	/* read system area */
#define SPF_WSYS	1	/* write system area */
#define SPF_FORMAT	3	/* format whole disc */
#define SPF_INITF	8	/* initialise for format */
#define SPF_RBR		54	/* diagnostic - read branches table */
#define SPF_RTR		55	/* diagnostic - read trace table */
#define SPF_DIAG	62	/* diagnostic facilities */
#define SPF_HEADPARK	63	/* head park all discs */
#define SPF_DWRITE	64	/* (add) data buffer write */
#define SPF_PWRITE	128	/* (add) param buffer write */

/* additional sp_flags definition */
#define SPF_DOUT	0	/* diagnostic output transfer */
#define SPF_DIN		1	/* diagnostic input transfer */
#define SPF_DNONE	2	/* diagnostic no transfer */
#define SPF_DVAL	3	/* mask for above */

/* 
**  FPBUF - Format parameter buffer 
*/

#define	FPBUF	struct FormatParmBuff
FPBUF	
{
	BYTE	fp_head;	/* head no		*/
	BYTE	fp_res;		/* reserved		*/
	WORD	fp_cyl;		/* cylinder no		*/
	BYTE	fp_ddens;	/* density		*/
	BYTE	fp_fill;	/* fill character	*/
	WORD	fp_bytsec;	/* bytes/sector		*/
	WORD	fp_sectrk;	/* sectors/track	*/
	WORD	fp_stsec;	/* starting sector	*/
	BYTE	fp_list;	/* list of sectors	*/
};


/*
**  GETRP - Get Return Parameters
**	The structure of the stuff that a get call is suppose to return.
*/

#define GETRP	struct GetReturnParams
GETRP	
{				/* GET return parameters */
	UWORD	gp_dtype;	/* type - see below */
	WORD	gp_rsmax;	/* max sector size */
	BYTE	*gp_addr;	/* address of open door byte */
	WORD	gp_fatrmax;	/* max sectors/FAT */
	WORD	gp_fatsizmax;	/* max FAT size bytes */
	WORD	gp_dirsizmax;	/* max entries in ROOT dir */
};

/* gp_type definition */
#define GPT_FIX		0	/* fixed media */
#define GPT_REM		1	/* removable media */
#define GPT_OPEN	2	/* open door support */
/* #define GPT_MEMORY	4	memory drive */


/*
**  GETPB - Get Parameter Block
*/

#define GETPB	struct GetParamBlock
GETPB	
{				/* GET parameter block */
	UBYTE	gp_unitno;	/* unit number */
	BYTE	gp_res[3];
	GETRP	gp_params;	/* return parameters */
};


/*
**  SELPB - Select Parm Block
*/

#define SELPB	struct SelectParamBlock
SELPB	{			/* SELECT parameter block */
	UBYTE	sp_unitno;	/* unit number */
	BYTE	sp_res;
	MDB	*sp_mdbp;	/* disc manager's MDB pointer */
	};



/*
**  MDDB - Ram Disk Desriptor Block
*/

#define MDDB	struct MDiscDescrBlock
MDDB	
{
	WORD	dd_nsecs;	/* number of sectors */
	WORD	dd_nfrecs;	/* number of sectors per FAT */
	WORD	dd_dirsize;	/* entries in ROOT directory */
};




/*
**  MDCB - ram disk control block
*/

#define MDCB	struct MDiscControlBlock
MDCB	
{
	MDDB	*mc_mddbp;		/* pointer to MDDB 		*/
	BYTE	*mc_memory[MNAREAS];	/* pointers to memory disc 	*/
	FLAGNO	mc_flagno;		/* flag number for FLAGEVENT/SET */
};

/* above should be changed to 64Kb (values 8, 128) if this works ok */



/****************************************************************************
*	Routines in this module & externals				    *
****************************************************************************/

LONG	rd_init();	/* initialise unit */
ERROR	rd_uninit();	/* uninitialise */
ERROR	rd_select();	/* select unit & return MDB */
ERROR	rd_flush();	/* flush any buffers */
EMASK	rd_read();	/* read sectors */
EMASK	rd_write();	/* write sectors */
ERROR	rd_get();	/* get unit information */
ERROR	rd_special();	/* special routines */
ERROR	rd_none();	/* non-implemented routine */

/* subroutines */

EMASK	readwrite() ;
LONG	getnsecs() ;

/****************************************************************************
* driver header - must be first item in data				    *
****************************************************************************/

GLOBAL DH	dh =
{
	DVR_DISK,	/* driver type */
	RDMAXUNITS,
	DHF_USYNC,	/* synchronise at unit level */
	rd_init,
	rd_none,	/* subdrv routine */
	rd_uninit,
	rd_select,
	rd_flush,
	rd_read,
	rd_write,
	rd_get,
	rd_none,	/* set routine */
	rd_special,
	0, 0, 0, 0, 0
};

/****************************************************************************
*  Miscellaneous data							    *
****************************************************************************/

char	rident[] = "Ram disk driver. Copyright Digital Research, Inc. 1985.";
char	rversion[]	= Version;
char	rlastchange[]	= Last_change;

/****************************************************************************
*  MDCB pointers							    *
****************************************************************************/

MDCB	*mdcbps [RDMAXUNITS] =	{ NULL, NULL };

/****************************************************************************
*  GETRP								    *
****************************************************************************/

GETRP	grpmem =
{
	GPT_FIX,	/* simulated fixed disc */
	MSECTORSZ,	/* max sector size */
	NULL,		/* memory area */
	3,		/* max sectors/FAT */
	(3 * MSECTORSZ),	/* max bytes/FAT */
	32		/* max ROOT directory entries */
};

/****************************************************************************
*  MDDBs								    *
****************************************************************************/

#define MAXMDDB		7	/* number of MDDBs */
MDDB	mddbs [MAXMDDB] =
{
	{		/* 8Kb memory disc */
	16,		/* disc size, sectors */
	1,		/* number of FAT records */
	16		/* directory size */
	},
	{32, 1, 16},
	{64, 1, 16},
	{128, 1, 16},
	{256, 1, 16},
	{512, 2, 32},
	{1024, 3, 32}	/* 512Kb memory disc */
};	



/****************************************************************************
**  rd_init - initialize ram disk unit
****************************************************************************/

LONG	rd_init (unitnbr)
	LONG	unitnbr;	/* install flags (ms word) + unit (ls word) */
{
	REG MDCB	*mdcb;
	ERROR	resp;
	LONG	nbytes ;	/* nbr of bytes in rd 			*/
	WORD	nch ;		/* nbr of chunks			*/
	REG WORD	unit ;		/* unit number */
	WORD	nsecs;		/* remaining sectors */
	REG WORD	i,j;


	unit = (WORD) unitnbr ;

	/* 
	**  check unit number 
	*/

	if (unit >= RDMAXUNITS)
		return( E_UNITNO ) ;
	if (mdcbps[unit])
		return( E_GENERAL ) ;

	/* 
	**  create MDCB & flag 
	*/

	if (!(mdcbps[unit] = mdcb = (MDCB *) salloc ((LONG) sizeof (MDCB)) ))
		return( E_POOL ) ;

	bzero( mdcb->mc_memory, MNAREAS * 4 ) ;	/* clear pointers */
	if( !(mdcb->mc_flagno = flagget() ) )
	{
		resp = E_POOL + ES_DISK;
		goto initerror;
	}

	nsecs = getnsecs() ;

	/* check to see if size is valid - locate MDDB for this size, 
		or ret error */

#if	OLDCODE
	i = 0;
	while( i < MAXMDDB)
		if (mddbs [i].dd_nsecs == nsecs)
			break;
		else
			i++;
#else
	for( i = 0 ; i < MAXMDDB  &&  mddbs[i].dd_nsecs != nsecs ; ++i )
		;
#endif

	if( i >= MAXMDDB)
	{
		resp = E_BADPB + ES_DISK;
		goto initerror;
	}

	mdcb->mc_mddbp = &mddbs[i];

	/* aquire the memory ( in MSECAREA chunks) necessary for the disc 
		size ; What we have is the number of sectors requested for 
		the disk.  Round this up to the next multiple of the chunk 
		size and divide the result by the chunk size.  This will give 
		us the number of chunks to allocate.  Be careful not to 
		allocate more chunks than we have pointers for. If we run out 
		of memory, abort  */

	nbytes = ( (LONG)nsecs * MSECTORSZ ) ;
	if( nbytes % RDBYTCHK )
		nbytes += RDBYTCHK ;
	nch = min( MNAREAS , nbytes/RDBYTCHK ) ;

	for( i = 0 ; i < nch ; ++i )
	{
		if(  !(mdcb->mc_memory[i] = (BYTE *) salloc((LONG)RDBYTCHK))  )
		{
			resp = E_POOL + ES_DISK;
			goto initerror;
		}
	}

	/*  initialise memory as empty disc -
		KLUDGE ALERT: currently just clearing the first n chunks,
		where n <= total number of chunks, and is large enough to
		clear the fat and root directory */

	for( i = 0 , j = min( RDNCLRCHK , nch ) ; i < j ; ++i )
		bzero( mdcb->mc_memory[i] , (UWORD)RDBYTCHK ) ;
	bfill(mdcb->mc_memory[0],0xf8,3);

	resp = (LONG) DVR_DISK;	/* driver type + no subdriver */
	goto initend;

initerror:
	rd_uninit (unitnbr);	/* release MDCB, flag & memory if allocated*/

initend:
	return (resp);
}

/****************************************************************************
**  rd_uninit - uninitialize the ram disk
****************************************************************************/

ERROR	rd_uninit (unitnbr)
LONG	unitnbr;
{
	REG WORD unit	= (WORD) unitnbr;
	MDCB	*mdcb;
	ERROR	resp	= SUCCESS;
	REG WORD i;

	mdcb = mdcbps[ unit ];
	if (!mdcb)
		return( E_UNITNO ) ;
	else	
	{
		if( mdcb->mc_flagno )
		{
			resp = flagrel(mdcb->mc_flagno);  /* release flag */
		}
		for (i = 0; i < MNAREAS && mdcb->mc_memory [i]; i++)
		{
			if( mdcb->mc_memory[i] )
				resp = sfree (mdcb->mc_memory[i]);
		}				/* release memory */
	}
	if( !resp  &&  mdcb )
	{
		resp = sfree (mdcb);		/* release MDCB */
		mdcbps[ unit ] = NULLL;
	}
	return (resp);
}

/****************************************************************************
**  rd_get - get unit specific information for ram disk
****************************************************************************/

ERROR	rd_get (pb)
GETPB	*pb;
{
	MDCB	*mdcb	= mdcbps[pb->gp_unitno];

	if (!mdcb)
		return( E_UNITNO ) ;

	bmove( &grpmem , &pb->gp_params , sizeof(GETRP) ) ;
	return( SUCCESS );
}

/****************************************************************************
**  rd_select - select ram disk unit and return MDB
****************************************************************************/

ERROR	rd_select (pb)
	SELPB	*pb;
{
	MDCB	*mdcb	= mdcbps [pb->sp_unitno];
	MDDB	*mddb;
	MDB	*mdb	= pb->sp_mdbp;	/* resource manager's MDB */

	if (!mdcb)
		return( E_UNITNO ) ;

	/* fill in MDB */
	bzero(mdb, sizeof (MDB));
	mdb->md_format = MDFPC;			/*  12 bit fats		*/
	mdb->md_nfats = 1;			/*  1 fat		*/
	mdb->md_nheads = 1;			/*  single sided	*/
	mdb->md_secblk = 1;			/*  one sec block	*/
	mdb->md_sectrk = MSECTRK;		/*  sec/track		*/
	mdb->md_secsize = MSECTORSZ;		/*  byte/sec		*/
	mdb->md_fatid = 0xF8;			/*  fat id byte: fixed	*/

	mddb = mdcb->mc_mddbp;			/*  other info from MDD	*/
	mdb->md_nfrecs = mddb->dd_nfrecs;	/*    nbr fats		*/
	mdb->md_dirsize = mddb->dd_dirsize;	/*    nbr root entries	*/
	mdb->md_nsecs = mddb->dd_nsecs;		/*    total nbr sectors	*/

	return( SUCCESS );
}

/****************************************************************************
**  rd_flush - flush any buffers in ram disk
****************************************************************************/

ERROR	rd_flush (pb)
DPBLK	*pb;
{
	return( pb->dp_unitno > RDMAXUNITS ? E_UNITNO : SUCCESS ) ;
}



/****************************************************************************
**  rd_read - read info from ram disk
****************************************************************************/

EMASK	rd_read (pb)
DPBLK	*pb;
{
	return (readwrite (RDREAD, pb));
}


/****************************************************************************
**  rd_write - write info to ram disk
****************************************************************************/

EMASK	rd_write (pb)
DPBLK	*pb;
{
	return( readwrite (RDWRITE, pb) );
}

/****************************************************************************
**  readwrite - common i/o logic
****************************************************************************/

static EMASK	readwrite (command, pb)
WORD	command;
DPBLK	*pb;
{
	union	{
		LONG	lhsc;
		HSCADDR	shsc;
	}	hsc;
	MDCB	*mdcb	= mdcbps [pb->dp_unitno];
	EMASK	mask;
	SECTOR	sector;
	SECTOR	lastsec ;
	SECTOR	nsecs;		/* remaining sectors */
	SECTOR	sarea;		/* sectors in this area */
	SECTOR	start;		/* start offset in this area */
	WORD	i;		
	BYTE	*buffer;	/* user buffer for this area */
	BYTE	*discarea;	/* disc area for this transfer */
	ERROR	resp;

	if (!mdcb)
		return( E_UNITNO ) ;		/* unit not initialised */

	if ((mask = flagevent (mdcb->mc_flagno, pb->dp_swi)) >0)
	{
		/* get and check sector number */

		if (pb->dp_flags & DPF_HSCADDR)
		{
			hsc.lhsc = pb->dp_offset ;
			sector = (hsc.shsc.hsc_cyl + hsc.shsc.hsc_head)
			    * MSECTRK + hsc.shsc.hsc_sector - 1;
		}
		else
			sector = pb->dp_offset ;

		/*  range check request  */

		nsecs = pb->dp_bufsiz;
		lastsec = sector + nsecs ;
		if( lastsec < 0  ||  lastsec > mdcb->mc_mddbp->dd_nsecs)
		{
			resp = E_BADOFFSET + ES_DISK;	/* over end of rdisc */
		}
		else
		{
			/* verify: just report OK */
			if (!(pb->dp_flags & DPF_VERIFY))	
			{
				/* not verify; perform transfer, breaking at 
					MDCB area boundaries if necessary */

				if (pb->dp_flags & DPF_UADDR)
				{
					mapu(pb->dp_pdaddr);
					buffer = (BYTE*)saddr( pb->dp_buffer );
				}
				else
					buffer = pb->dp_buffer;
					
				i = sector / MSECAREA;	/* start of disc area */
				start = sector % MSECAREA;

				do		
				{
					/* that part in first/next mdcb area */
					sarea = (start + nsecs > MSECAREA) ? 
					  MSECAREA - start : nsecs;
					discarea = 
					  mdcb->mc_memory[i] + start*MSECTORSZ;

					if (command == RDREAD)
					  lbmove(discarea ,
					    buffer , (LONG) sarea*MSECTORSZ ) ;
					else	
					  lbmove(buffer ,
					    discarea, (LONG) sarea*MSECTORSZ ) ;

					i++;
					start = 0;/* reset for next area */
					buffer += sarea * MSECTORSZ;
					nsecs -= sarea;
				}  while (nsecs > 0);

				if (pb->dp_flags & DPF_UADDR)
					unmapu ();
			}
			resp = SUCCESS;
		}

		flagset(mdcb->mc_flagno, *dh.dh_curpd, resp);
	}
	return(mask);	/* asynchrouous interface */
}

/****************************************************************************
**  rd_none - used for unimplemented routines
****************************************************************************/

ERROR	rd_none (pb)
DPBLK	*pb;
{
	return (E_IMPLEMENT + ES_DISK);
}



/****************************************************************************
** rd_special - special function routine
****************************************************************************/

ERROR	rd_special (pb)
SPECPB	*pb;
{
	return( E_IMPLEMENT );
}


/****************************************************************************/

#define	RDVBUFSIZ	20		/* size of value buffer		*/
#define	DEFFLAGS	3		/* return system table value	*/

char	vbuffer[ RDVBUFSIZ ] ;		/* store ascii val of RDSIZ	*/

/****************************************************************************
** getnsecs - return the number of sectors to assign to the 'disk'
****************************************************************************/

LONG	getnsecs()
{
	LONG	s_define() ;

	struct deftab
	{
		UWORD	dt_res1 ;
		UWORD	dt_flags ;
		LONG	dt_res2 ;
		BYTE	*dt_lname ;
		BYTE	*dt_vbuff ;
		LONG	dt_vbufsiz ;
	} dt ;

	LONG	r, nsecs, nbytes ;

	/*  do table lookup on RDSIZE  */

	dt.dt_res1 = dt.dt_res2 = 0 ;
	dt.dt_flags = DEFFLAGS ;
	dt.dt_lname = "RDSIZE" ;
	dt.dt_vbuff = vbuffer ;
	dt.dt_vbufsiz = RDVBUFSIZ ;;
	r = supif( F_DEFINE , &dt ) ;

	/*  if we got a value use it, otherwise assume default  */

	if( r != SUCCESS )
		nsecs = RDNSECS ;
	else
	{
		nbytes = ( (LONG) atoi(vbuffer) ) * 1024 ;
		if( nbytes % RDBYTSEC )
			nbytes += RDBYTSEC ;
		nsecs = nbytes / RDBYTSEC ;
	}
	
	return( nsecs ) ;
}


/****************************************************************************
** atoi - straight from K & R
****************************************************************************/

int	atoi( s )
	char	s[] ;
{
	int	i, n, sign ;

	for( i = 0 ; s[i] == ' '  ||  s[i] == '\n'  ||  s[i] == '\t' ; ++i )
		;	/* skip white space  */

	sign = 1 ;
	if( s[i] == '+' || s[i] == '-' )
		sign = ( s[i++] == '+' ) ? 1 : -1 ;

	for( n = 0 ; s[i] >= '0' && s[i] <= '9' ; ++i )
		n = 10 * n + s[i] - '0' ;

	return( sign * n ) ;
}

