/*	@(#)rwinboot.c	1.5		*/
/*
 *	This is the physical disk read module for the
 *	RWIN disk controller on the VME/10.
 *
 *	ASSUMPTIONS:
 *
 *	- A big one: we assume TENbug has configured the
 *	  the controller for the boot disk.  i.e. we don't
 *	  send out any configuration command.
 *
 *	- Floppy disks are 96 tpi, 512 byte DD sectors, 8 per track.
 *
 */

#include "portab.h"
#include "cdosboot.h"

BYTE copyright[] = "Copyright 1984, Motorola Inc.";

WORD disknum;

/************************************************************************/
/* Define Disk I/O Addresses and Related Constants			*/
/************************************************************************/

#define	DISKCNTL	((DIO *) 0xf1c0d1)	/* controller address */

#define	DCXFER	128		/* bytes per dsk controller transfer request */

#define	DISKREAD		0x08	/* commands */

#define	CTL512DD	0x34	/* default command control byte: 48 tpi */
				/* IBM sn, DD, 512 bps, DS, no ci, blk drq */

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
/*	Send disk command packet					*/
/*									*/
/*	NOTE: These assignments must not be reordered			*/
/*	or optimized away.  We are writing to a single memory		*/
/*	mapped register and the order of data sent is significant.	*/
/************************************************************************/

MLOCAL VOID SndRdCmd(psn,n)
REG LONG psn;
WORD n;
{
	/* write the packet to the controller */

	DISKCNTL->cmdsns = DISKREAD;			/* command - byte 0 */

	/* following line assumes psn <= 21 bits long */
	DISKCNTL->cmdsns = (disknum << 5) | (psn >> 16);	/* byte 1 */
	DISKCNTL->cmdsns = (psn >> 8);				/* byte 2 */
	DISKCNTL->cmdsns = psn;					/* byte 3 */
	DISKCNTL->cmdsns = n;					/* byte 4 */

	/* byte 5 configuration information is IGNORED FOR HARD DISK */
	/* and we don't enable interrupts */
	DISKCNTL->cmdsns = CTL512DD | TPI96;			/* byte 5 */
}


/************************************************************************/
/*	Get disk sense							*/
/************************************************************************/

MLOCAL VOID GetDiskSense()
{
	/* read the sense block from the controller */
	/* the DISKCNTL references must NOT be reordered */

	while ( DISKCNTL->intstt & BUSY )	/* wait while controller busy */
		;

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

/************************************************************************/
/*	Disk Read with error correction					*/
/************************************************************************/

MLOCAL WORD RdDiskOnce(psn, pscnt, bufp)
REG LONG  psn;
REG WORD  pscnt;
REG BYTE *bufp;
{
	LONG erofst;	/* offset from bp of location to correct */
	REG BYTE *bp;	/* address of last sector read - for correction */

	SndRdCmd(psn, pscnt);
	while ( 1 )
	{
		while ( DISKCNTL->intstt & BUSY )
			if ( DISKCNTL->intstt & DRQ )
			{
				RdDataXfer(bufp,&DISKCNTL->data);
				bufp += DCXFER;
			}
		GetDiskSense();	/* check for error */
		if ( sns.ercode != CORRDER )
			return (sns.ercode);
		else
		{	/* correct the data - winchester only */
			erofst = (sns.snsbt6 << 8) + sns.snsbt7;
			bp = (BYTE *)((LONG)bufp - 256);
			bp[erofst] ^= sns.snsbt8;
			bp[erofst+1] ^= sns.snsbt9;
			if ( sns.n )	/* more to read - reissue command */
				SndRdCmd(psn+pscnt-sns.n, sns.n);
			else return OK;	/* done - no error to report */
		}
	}
}

/************************************************************************/
/*	Disk Transfer							*/
/************************************************************************/

VOID ReadSector(psn,nsecs,bufp)
LONG  psn;
WORD  nsecs;
BYTE *bufp;
{
	WORD  rcnt;	/* retry count */
	WORD  error;	/* error flag */

	rcnt = RETRYCNT;		/* retry count */

	do			/* error retry loop */
	{
		error = RdDiskOnce(psn, nsecs, bufp);
	} while (error && --rcnt);

	if (error)
	{
		PutMsg("Disk read error in booter");
		Exit();			/* no return */
	}
}


/************************************************************************/
/*	Disk Initialization						*/
/************************************************************************/

InitDisk()
{
	/* turn off controller interrupts */
	DISKCNTL->intstt = 0;
}
**