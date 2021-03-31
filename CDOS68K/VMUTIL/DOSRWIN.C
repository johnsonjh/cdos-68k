/*	@(#)dosrwin.c	2.5		*/
/*
 *	These are the physical disk I/O routines for the cross
 *	PC-DOS utility running under CP/M.
 *
 *	They drive RWIN disk controller on the VME/10.
 *
 *	ASSUMPTIONS:
 *
 *	- Floppy disks are 96 tpi, 512 byte DD sectors, 8 per track.
 *
 */

#include "portab.h"
#include "ibmdisk.h"

BYTE copyright[] = "Copyright 1984, Motorola Inc.";

#define NO_ASM_SUPPORT	1

/************************************************************************/
/* Define Disk I/O Addresses and Related Constants			*/
/************************************************************************/

#define	DISKCNTL	((DIO *) 0xf1c0d1)	/* controller address */

#define	DCXFER	128		/* bytes per dsk controller transfer request */

#define	DISKREAD		0x08	/* commands */
#define DISKWRITE		0x0a
#define DISKCONFG		0xc0

#define	CTL512DD	0x34	/* default command control byte: 48 tpi */
				/* IBM sn, DD, 512 bps, DS, no ci, blk drq */
#define CTL128SD	0x04

#define	BUSY		0x80	/* status bits */
#define	DRQ		0x08	/* data request - can xfer DCXFER bytes */

#define CORRDER		0x13	/* sense error code - correctable read error */

#define	TPI96	0x80	/* we must tell controller we have 96 tpi floppy */

#define RETRYCNT	10	/* number of retries on disk read if error */

/************************************************************************/
/*	Disk I/O Packets and Variables					*/
/************************************************************************/

typedef struct			/* disk controller registers */
{
	BYTE	cmdsns;		/* command/sense byte */
	BYTE	diofl1;		/* fill byte */
	BYTE	intstt;		/* interrupt/status byte */
	BYTE	diofl2;		/* fill byte */
	BYTE	rst;		/* reset */
	BYTE	diofl3;		/* fill byte */
	BYTE	ntusd;		/* not used */
	BYTE	diofl4;		/* fill byte */
	BYTE	data;		/* data */
} DIO;

typedef struct			/* sense packet */
{
	BYTE	ercode;		/* error code */
	BYTE	lun;		/* CP/M logical unit number */
	BYTE	status;		/* status includes controller lun */
	WORD	pcylnm;		/* physical cylinder number */
	BYTE	headnm;		/* head number */
	BYTE	sectnm;		/* sector number */
	BYTE	n;		/* number sectors left to process */
	BYTE	snsbt6;		/* sense packet byte 6 */
	BYTE	snsbt7;		/* sense packet byte 7 */
	BYTE	snsbt8;		/* sense packet byte 8 */
	BYTE	snsbt9;		/* sense packet byte 9 */
} DSNS;

DSNS sns;			/* last sense packet read from disk */

/************************************************************************/
/*	Send command packet						*/
/*									*/
/*	NOTE: These assignments must not be reordered			*/
/*	or optimized away.  We are writing to a single memory		*/
/*	mapped register and the order of data sent is significant.	*/
/************************************************************************/

MLOCAL VOID SndCmd(dsk,psn,n,cmd)
WORD n, dsk, cmd;
REG LONG psn;
{
	WORD	ctl;

	/* correction for reading or writing track 0 */
	/* track 0 is sd, 128 bytes/sector, 16 sectors */
	/* correction assumes reads and writes are done on a track basis */
	if ( (dsk == 2 || dsk == 3) &&
		((cmd == DISKREAD) || (cmd == DISKWRITE)) && psn == 0L)
	{
		ctl = CTL128SD;		/* it is a floppy and we do track 0 */
		n = 16;
	}
	else
		ctl = CTL512DD;

	/* write the packet to the controller */

	DISKCNTL->cmdsns = cmd;			/* command - byte 0 */

	/* following line assumes psn <= 21 bits long */
	DISKCNTL->cmdsns = (dsk << 5) | (psn >> 16);	/* byte 1 */
	DISKCNTL->cmdsns = (psn >> 8);			/* byte 2 */
	DISKCNTL->cmdsns = psn;				/* byte 3 */
	DISKCNTL->cmdsns = n;				/* byte 4 */

	/* byte 5 configuration information is IGNORED FOR HARD DISK */
	/* and we don't enable interrupts */
	DISKCNTL->cmdsns = ctl | TPI96;		/* byte 5 */
}


/************************************************************************/
/*	Send disk configuration packet					*/
/************************************************************************/

SndCnf(dsk, mxhd, mxcl, prcmp)
REG WORD dsk, mxhd, mxcl, prcmp;
{
	WORD zero;

	zero = 0;	/* so clr.b won't be generated for byte 5 */

	/* write the configuration packet to the controller */
	/* the DISKCNTL references must NOT be reordered */

	DISKCNTL->cmdsns = DISKCONFG;	/* command - byte 0 */
	DISKCNTL->cmdsns = (dsk << 5);	/* byte 1 */
	/* following line assumes mxcl <= 13 bits long */
	DISKCNTL->cmdsns = (mxhd << 5) | (mxcl >> 8);	/* byte 2 */
	DISKCNTL->cmdsns = mxcl;		/* byte 3 */
	DISKCNTL->cmdsns = prcmp;	/* byte 4 */
	DISKCNTL->cmdsns = zero;		/* byte 5 */

	while ( DISKCNTL->intstt & BUSY )
		;	/* wait while controller busy */
}


/************************************************************************/
/*	Get disk sense							*/
/************************************************************************/

MLOCAL VOID GetDiskSense()
{
	/* read the sense block from the controller */
	/* the DISKCNTL references must NOT be reordered */

	while ( DISKCNTL->intstt & BUSY )
		;	/* wait while controller busy */

	sns.ercode = DISKCNTL->cmdsns;
	sns.status = DISKCNTL->cmdsns;
	sns.lun = (sns.status >> 5) & 0x3;
	sns.pcylnm = DISKCNTL->cmdsns;
	sns.pcylnm = (sns.pcylnm << 8) + DISKCNTL->cmdsns;
	sns.headnm = DISKCNTL->cmdsns;
	sns.sectnm = sns.headnm & 0x1f;
	sns.headnm = sns.headnm >> 5;
	sns.n = DISKCNTL->cmdsns;
	sns.snsbt6 = DISKCNTL->cmdsns;
	sns.snsbt7 = DISKCNTL->cmdsns;
	sns.snsbt8 = DISKCNTL->cmdsns;
	sns.snsbt9 = DISKCNTL->cmdsns;
}


#if NO_ASM_SUPPORT

/************************************************************************/
/*	Disk read data transfer						*/
/************************************************************************/

MLOCAL WORD RdDataXfer(bp)
REG BYTE *bp;
{
	/* This routine should be written in assembly language later. */

	REG WORD cnt;

	for ( cnt = DCXFER; cnt--; )
		*bp++ = DISKCNTL->data;
}

/************************************************************************/
/*	Disk write data transfer					*/
/************************************************************************/

MLOCAL WORD WrDataXfer(bp)
REG BYTE *bp;
{
	/* This routine should be written in assembly language later. */

	REG WORD cnt;

	for ( cnt = DCXFER; cnt; cnt-- )
		DISKCNTL->data = *bp++;
}

#endif	/* NO_ASM_SUPPORT	*/

/************************************************************************/
/*	Disk Read with error correction					*/
/************************************************************************/

MLOCAL WORD RdDiskOnce(dsk, psn, pscnt, bufp)
WORD	dsk;
REG LONG  psn;
REG WORD  pscnt;
REG BYTE *bufp;
{
	LONG erofst;	/* offset from bp of location to correct */
	BYTE *bp;	/* address of last sector read - for correction */

	SndCmd(dsk, psn, pscnt, DISKREAD);
	while ( 1 )
	{
		while ( DISKCNTL->intstt & BUSY )
			if ( DISKCNTL->intstt & DRQ )
			{
				RdDataXfer(bufp);
				bufp += DCXFER;
			}
		GetDiskSense();	/* check for error */
		if ( sns.ercode != CORRDER )
			return (sns.ercode);
		else
		{		/* correct the data - winchester only */
			erofst = (sns.snsbt6 << 8) + sns.snsbt7;
			bp = (BYTE *)((LONG)bufp - 256);
			bp[erofst] ^= sns.snsbt8;
			bp[erofst+1] ^= sns.snsbt9;
			if ( sns.n )	/* more to read - reissue command */
				SndCmd(dsk, psn+pscnt-sns.n, sns.n, DISKREAD);
			else return OK;	/* done - no error to report */
		}
	}
}

/************************************************************************/
/*	Disk Transfers							*/
/************************************************************************/

/************************************************************************/
/*	Disk Sector Write						*/
/************************************************************************/

WriteSector(dsk,psn,nsecs,bufp)
WORD  dsk;
LONG  psn;
WORD  nsecs;
BYTE *bufp;
{
	WORD  rcnt;	/* retry count */
	BYTE *bp;	/* buffer pointer for retries */
	WORD  error;	/* error flag */
	WORD  scnt;	/* blocks to transfer */

	bp = bufp;		/* save buffer addr */
	rcnt = RETRYCNT;		/* retry count */
	if( psn == 0L && dsk == 2)	/* KLUDGE */
		scnt = 16;
	else
		scnt = (dsk == 2) ? (nsecs*4) : (nsecs*2);

	do			/* error retry loop */
	{
		SndCmd(dsk, psn, nsecs, DISKWRITE);
		while ( DISKCNTL->intstt & BUSY )
			if ( (DISKCNTL->intstt & DRQ) /* && scnt  */)
			{
				WrDataXfer(bufp);
				bufp += DCXFER;
				scnt--;
			}
		GetDiskSense();
		error = sns.ercode;
		bufp = bp;		/* restore buffer addr */
	} while (error && --rcnt);

	if( error )
		return ERROR;
	else
		return OK;
}

/************************************************************************/
/*	Disk Sector Read						*/
/************************************************************************/

ReadSector(dsk,psn,nsecs,bufp)
WORD  dsk;
LONG  psn;
WORD  nsecs;
BYTE *bufp;
{
	WORD  rcnt;	/* retry count */
	WORD  error;	/* error flag */

	rcnt = RETRYCNT;		/* retry count */

	do			/* error retry loop */
	{
		error = RdDiskOnce(dsk, psn, nsecs, bufp);
	} while (error && --rcnt);

	return error;
}


/************************************************************************/
/*	Disk Initialization						*/
/************************************************************************/

InitDisk()
{
	/* turn off controller interrupts */
	DISKCNTL->intstt = 0;
}
