/******************************************************************************
* SYSTEM.H - Global System Definitions				    Version 1.1
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
* V1.1    01/07/86 BVH	Add DVR_GSX to everything
* V1.0.04 10/02/85 BKS	Add DVR_NETMAN (0x7d) to driver types
* v1.0.03 09/12/85 FRH	Rename E_SWI to E_INSWI
* v1.0.02 09/12/85 FRH	E_UNDERRUN Error Added
* v1.0.01 08/21/85 gat	NMODS define removed
* v1.0.00 		Concurrent DOS 1.0 Release Version
******************************************************************************/

/****************************************************************/
/*								*/	
/*	Module numbers, used to access the various resource	*/
/*	managers.						*/
/*								*/
/****************************************************************/

#define MOD_KERN	0	/* Kernel			*/
#define MOD_PIPE	1	/* Pipe Resource Manager	*/
#define MOD_DISK	2	/* Disk Resource Manager	*/
#define MOD_CON		3	/* Console Resource Manager	*/
#define MOD_LOAD	4	/* Command (Load) Resource Mgr.	*/
#define MOD_EXT		5	/* Extension Resource Manager	*/
#define	MOD_GSX		5	/* Graphics Resource Manager	*/
#define MOD_NET		6	/* Networ Resource Manager	*/
#define	MOD_MSC		7	/* Misc. Driver Resoure Mgr.	*/
#define	MOD_SUP		8	/* Internal Supervisor Module	*/
#define	MOD_DOS		9	/* DOS Front End		*/
#define	MOD_CPM		10	/* CP/M Front End		*/

/****************************************************************/
/*								*/	
/*	Driver types						*/
/*								*/
/****************************************************************/

#define	DVR_TIME	0x01	/* Timer Driver			*/
#define	DVR_PIPE	0x11	/* Pipe Driver			*/
#define	DVR_DISK	0x21	/* Disk Driver			*/
#define	DVR_CON		0x31	/* Console Driver		*/
#define	DVR_GSX		0x51	/* Graphics Driver		*/
#define	DVR_PRN		0x71	/* Printer Driver		*/
#define DVR_SER		0x72	/* Serial Driver		*/

#define	DVR_NETMAN	0x7d	/* Network Resource Manager	*/

#define DVR_CLOCK	0x7e	/* DOS Clock Driver Emulator	*/
#define DVR_NULL	0x7f	/* Null Device			*/

#define	DVR_PORT	0x81	/* Port Driver			*/
#define	DVR_OEM		0x82	/* 1st OEM Driver		*/

/****************************************************************/
/*								*/	
/*	System information table types				*/
/*								*/
/****************************************************************/

#define	T_PROC		0x00		/* Process table		*/
#define	T_ENV		0x01		/* Environment table		*/
#define	T_TD		0x02		/* Time and date table		*/
#define	T_MEM		0x03		/* Memory table			*/
#define	T_PIPE		0x10		/* Pipe table			*/
#define	T_FILE		0x20		/* Disk File table		*/
#define	T_DISK		DVR_DISK	/* Disk table			*/
#define	T_CON		0x30		/* Screen table			*/
#define	T_PCON		DVR_CON		/* Physical Console table	*/
#define	T_VCON		0x32		/* Virtual Console table	*/
#define	T_MOUSE		0x33		/* Mouse table			*/
#define T_TOP		0x34		/* Top Border (no table)	*/
#define T_BOTTOM	0x35		/* Bottom Border (no table)	*/
#define T_LEFT		0x36		/* Left Border (no table)	*/
#define T_RIGHT		0x37		/* Right Border (no table)	*/
#define	T_SYS		0x40		/* System table			*/
#define	T_FNUM		0x41		/* File number table		*/
#define	T_SDEF		0x42		/* System Defines table		*/
#define	T_PDEF		0x43		/* Process Defines table	*/
#define	T_CMD		0x44		/* Command Environment table	*/
#define	T_DEV		0x45		/* Device table			*/
#define	T_PNAME		0x46		/* Pathname table		*/
#define T_SER		DVR_SER		/* Serial table			*/
#define	T_PRN		DVR_PRN		/* Printer table		*/
#define T_CLOCK		DVR_CLOCK	/* DOS Clock Driver table	*/
#define T_NULL		DVR_NULL	/* Null Device table		*/
#define	T_PORT		DVR_PORT	/* Port table			*/

	/* Note:  Tables from 0x82 -> 0xff are special tables */

/****************************************************************/
/*								*/
/*  User Function Numbers					*/
/*								*/
/****************************************************************/

#define	F_GET		0
#define	F_SET		1
#define	F_LOOKUP	2
#define	F_CREATE	3
#define	F_DELETE	4
#define	F_OPEN		5
#define	F_CLOSE		6
#define	F_READ		7
#define	F_WRITE		8
#define	F_SPECIAL	9
#define	F_RENAME	10
#define	F_DEFINE	11
#define	F_DEVLOCK	12
#define	F_INSTALL	13
#define	F_LOCK		14
#define	F_COPY		15
#define	F_ALTER		16
#define	F_XLAT		17
#define	F_RWAIT		18
#define	F_KCTRL		19
#define	F_ORDER		20
#define	F_KEYPUT	21
#define	F_GIVE		22
#define	F_BWAIT		23
#define	F_TIMER		24
#define	F_EXIT		25
#define	F_ABORT		26
#define	F_CANCEL	27
#define	F_WAIT		28
#define	F_STATUS	29
#define	F_RETURN	30
#define	F_EXCEPTION	31
#define	F_ENABLE	32
#define	F_DISABLE	33
#define	F_SWIRET	34
#define	F_MALLOC	35
#define	F_MFREE		36
#define	F_OVERLAY	37
#define	F_COMMAND	38
#define	F_CONTROL	39
#define	F_GSX		40
#define	F_SEEK		41

#define NUFUNCS		41	/* Highest function number 	*/

/****************************************************************/
/*								*/
/*  Internal Module Function Numbers				*/
/*								*/
/****************************************************************/

#define I_INIT		0 /* LOAD NET DISK CON MSC PIPE KERN	*/
#define	I_PCREATE	1 /* LOAD NET DISK CON MSC PIPE KERN	*/
#define	I_PABORT	2 /* LOAD NET DISK CON MSC PIPE KERN	*/

#define	I_OVERLAY	3 /* LOAD */
#define	I_COMMAND	4 /* LOAD */
#define	I_CONTROL	5 /* LOAD */

#define	I_GSX		3 /* */

#define	I_GET		3 /* NET DISK CON MSC PIPE KERN */
#define	I_SET		4 /* NET DISK CON MSC PIPE KERN */
#define	I_LOOKUP	5 /* NET DISK CON MSC PIPE KERN */
#define	I_OPEN		6 /* NET DISK CON MSC PIPE */
#define	I_CLOSE		7 /* NET DISK CON MSC PIPE */
#define	I_READ		8 /* NET DISK CON MSC PIPE */
#define	I_WRITE		9 /* NET DISK CON MSC PIPE */
#define	I_SEEK		10 /* NET DISK CON MSC PIPE */
#define	I_CREATE	11 /* NET DISK CON PIPE */
#define	I_DELETE	12 /* NET DISK CON PIPE */
#define	I_SPECIAL	13 /* NET DISK CON MSC */
#define	I_INSTALL	14 /* NET DISK CON MSC */
#define	I_DVRLOCK	15 /* NET DISK MSC */
#define	I_RENAME	16 /* NET DISK */
#define	I_LOCK		17 /* NET DISK */

#define	I_COPY		18 /* CON */
#define	I_ALTER		19 /* CON */
#define	I_XLAT		20 /* CON */
#define	I_RWAIT		21 /* CON */
#define	I_KCTRL		22 /* CON */
#define	I_ORDER		23 /* CON */
#define	I_KEYPUT	24 /* CON */
#define	I_GIVE		25 /* CON */
#define	I_BWAIT		26 /* CON */

#define	I_TIMER		6 /* KERN */
#define	I_EXIT		7 /* KERN */
#define	I_ABORT		8 /* KERN */
#define	I_CANCEL	9 /* KERN */
#define	I_WAIT		10 /* KERN */
#define	I_STATUS	11 /* KERN */
#define	I_RETURN	12 /* KERN */
#define	I_EXCEPTION	13 /* KERN */
#define	I_ENABLE	14 /* KERN */
#define	I_DISABLE	15 /* KERN */
#define	I_SWIRET	16 /* KERN */
#define	I_MALLOC	17 /* KERN */
#define	I_MFREE		18 /* KERN */

#define I_DEFINE	0  /* SUP */

/****************************************************************/
/*								*/
/*  Error Definitions						*/
/*								*/
/*	All error codes are negative numbers (LONGS).          	*/
/*	Error Code Format :					*/
/*								*/
/*      31|30   24|23    16|15             0 (bits)		*/
/*	+-+-------+--------+----------------+			*/
/*	|1|   0   | module |   Error Code   |			*/
/*	+-+-------+--------+----------------+			*/
/*	(high byte)		(low word)			*/
/*								*/
/*	Error Code	16 bit number as described below	*/
/*	Module		8 bit number indicating source Module	*/
/*	High Byte	high bit is 1, low 7 bits are reserved	*/
/*								*/
/*			00H - Kernel & Supervisor		*/
/*			10H - Pipe System			*/
/*			20H - Disk System			*/
/*			21H-2FH - Disk drivers			*/
/*			30H - Console System			*/
/*			31H-3FH - Console drivers		*/
/*			40H - Command				*/
/*			50H - Extension				*/
/*			51H-5FH - OEM Ext. Drivers		*/
/*			60H - Network System			*/
/*			61H-6FH - Network Drivers		*/
/*			70H - Misc. Resource Manager		*/
/*			71H-7FH - Misc. drivers			*/
/*			81H - Port Driver			*/
/*			Special driver ID's begin at 82H.	*/
/*			Driver ID's cannot have a zero in the	*/
/*			low nibble of ID byte.			*/
/*								*/
/*	Resource Managers return errors that they generated	*/
/*	by ORing the RM number with the Error Code.		*/
/*	ie.							*/
/*		return( EM_CON | E_EXISTS );			*/ 
/*	  (The Console RM is noting that a file already exists)	*/
/*								*/
/*	ALSO:  DON'T OR Module Number with E_SUCCESS...		*/
/*								*/
/****************************************************************/

/****************************************************************/
/*								*/	
/*	Module numbers, used to identify the source of the	*/
/*	error.							*/
/*								*/
/****************************************************************/

#define EM_KERN	0x80000000L	/* Kernel			*/
#define	EM_SUP	0x80000000L	/* Internal Supervisor Module	*/
#define EM_PIPE	0x80100000L	/* Pipe Resource Manager	*/
#define EM_DISK	0x80200000L	/* Disk Resource Manager	*/
#define EM_CON	0x80300000L	/* Console Resource Manager	*/
#define EM_LOAD	0x80400000L	/* Command (Load) Resource Mgr.	*/
#define EM_EXT	0x80500000L	/* Extension Resource Manager	*/
#define	EM_GSX	0x80500000L	/* Graphics Resource Manager	*/
#define EM_NET	0x80600000L	/* Networ Resource Manager	*/
#define	EM_MSC	0x80700000L	/* Misc. Driver Resoure Mgr.	*/


#define	ED_TIME	0x80010000L	/* Timer Driver			*/
#define	ED_PIPE	0x80110000L	/* Pipe Driver			*/
#define	ED_DISK	0x80210000L	/* Disk Driver			*/
#define	ED_CON	0x80310000L	/* Console Driver		*/
#define	ED_GSX	0x80510000L	/* Graphixcs Driver		*/
#define	ED_PRN	0x80710000L	/* Printer Driver		*/
#define ED_SER	0x80720000L	/* Serial Driver		*/
#define ED_CLOCK 0x807e0000L	/* DOS Clock Driver Emulator	*/
#define ED_NULL	0x807f0000L	/* Null Device			*/
#define	ED_PORT	0x80810000L	/* Port Driver			*/


/****************************************************************/
/*								*/
/*  Common Error Definitions - Range 0x4000 - 0x407F		*/
/*								*/
/*  These error codes are shared by more than one system module.*/
/*								*/
/****************************************************************/

#define	E_SUCCESS	0x0L	/* No Error				*/

#define	E_ACCESS	0x4001L	/* Cannot access file due to ownership.	*/
#define	E_CANCEL	0x4002L	/* Event Cancelled			*/
#define	E_EOF		0x4003L	/* End of File				*/
#define	E_EXISTS	0x4004L	/* File already exists			*/
				/* INSTALL - Device already exists	*/
#define	E_DEVICE	0x4005L	/* Device does not match or not found	*/
				/* RENAME on different devices.		*/
#define	E_DEVLOCK	0x4006L	/* Device is LOCKED			*/
#define	E_FILENUM	0x4007L	/* Bad File Number			*/
#define	E_FUNCNUM	0x4008L	/* Bad function number			*/
#define	E_IMPLEMENT	0x4009L	/* This function not implemented	*/
#define	E_INFOTYPE	0x400AL	/* Illegal Infotype for this file	*/
#define	E_INIT		0x400BL	/* Error on Initialization of Driver	*/
#define	E_CONFLICT	0x400CL	/* Cannot access file due to current usage */
				/* DELETE of open file.			*/
				/* DELETE of directory with files	*/
				/* INSTALL- Replace Driver in use	*/
#define	E_MEMORY	0x400DL	/* No enough memory available		*/
#define	E_MISMATCH	0x400EL	/* Function Mismatch.  Attempt to perform */
				/*    a function on a file that does not*/
				/*    support the function.		*/
				/* INSTALL- Subdrive type mismatch	*/
#define	E_NAME		0x400FL	/* Illegal file name specified		*/
#define	E_NO_FILE	0x4010L	/* File Not Found.			*/
#define	E_PARAM		0x4011L	/* Illegal Parameter specified		*/
				/* EXCEPTION- Illegal number		*/
#define	E_RECSIZE	0x4012L	/* Record Size does not match request.	*/
#define	E_SUBDEV	0x4013L	/* INSTALL - Sub-drive required		*/
#define	E_FLAG		0x4014L	/* Bad Flag Number			*/

#define	E_NOMEM		0x4015L	/* Non-existant memory			*/
#define	E_MBOUND	0x4016L	/* Memory Bound error			*/
#define E_EBOUNDEX	E_MBOUND
#define	E_ILLINS	0x4017L	/* Illegal instruction			*/
#define	E_DIVZERO	0x4018L	/* Divide by zero			*/
#define	E_BOUND		0x4019L	/* Bound exception			*/
#define	E_OFLOW		0x401AL	/* Overflow exception			*/
#define	E_PRIV		0x401BL	/* Privilege violation			*/
#define	E_TRACE		0x401CL	/* Trace				*/
#define	E_BRKPT		0x401DL	/* Breakpoint				*/
#define	E_FLOAT		0x401EL	/* Floating point exception		*/
#define	E_STACK		0x401FL	/* Stack fault				*/
#define	E_NOTON286	0x4020L	/* Exception not caught by 286		*/
#define E_EM1		0x4021L	/* emulated instruction group 1		*/

/****************************************************************/
/*								*/
/*  Supervisor - Range 0x4080 - 0x40FF				*/
/*								*/
/****************************************************************/

#define	E_ASYNC		0x4080L	/* Asynchronous I/O not supported on	*/
				/*   function.				*/
#define	E_LOAD		0x4082L	/* Bad Load Format			*/
#define	E_LOOP		0x4083L	/* Infinite recursion (99 times) on	*/
				/*    Prefix Substitution.		*/
				/* INSTALL- Subdrive type mismatch	*/
#define	E_FULL		0x4084L	/* File number table full		*/
#define	E_DEFINE	0x4085L	/* DEFINE - Illegal name		*/

#define E_UNIT		0x4086L	/* Too Many Driver Units		*/
#define E_UNWANTED	0x4087L	/* Driver does not need Subdriver	*/
#define E_DVRTYPE	0x4088L	/* Driver returns bad Driver type	*/
#define E_LSTACK	0x4089L /* No stack group specified in cmd hdr  */

/****************************************************************/
/*								*/
/*  Memory Error Definitions - Range 0x4100 - 0x417F		*/
/*								*/
/****************************************************************/

#define	E_POOL		0x4100L	/* Out of memory pool			*/
#define	E_BADADD	0x4101L	/* Bad address specified to free	*/

/****************************************************************/
/*								*/
/*  Kernel Error Definitions - Range 0x4180 - 0x41FF		*/
/*								*/
/****************************************************************/


#define	E_OVERRUN	0x4180L	/* Flag alread set			*/
#define	E_FORCE		0x4181L	/* Return code of process being aborted	*/
#define	E_PDNAME	0x4182L	/* Process ID not found on abort	*/
#define	E_PROCINFO	0x4183L	/* COMMAND - no procinfo specified	*/
#define	E_LOADTYPE	0x4184L	/* COMMAND - invalid loadtype		*/
#define	E_ADDRESS	0x4185L	/* CONTROL - invalid memory access	*/
#define	E_EMASK		0x4186L	/* Invalid event mask			*/
#define	E_COMPLETE	0x4187L	/* Event has not completed		*/
#define	E_SRTL		0x4188L	/* The required SRTL could no be found	*/
#define	E_ABORT		0x4189L	/* Process cannot be terminated		*/
#define	E_CTLC		0x418AL	/* Process aborted by a control C	*/
#define E_GO		0x418BL /* CONTROL - Slave process running	*/
#define E_INSWI		0x418CL /* SWIRET - not a swi process		*/
#define E_UNDERRUN	0x418DL /* Flag already pending			*/

/****************************************************************/
/*								*/
/*  Pipe, Misc Error Definitions - Range 0x4200 - 0x427F	*/
/*								*/
/****************************************************************/

/* No unique error definitions at this time */

/****************************************************************/
/*								*/
/*  Console Error Definitions - Range 0x4280 - 0x42FF		*/
/*								*/
/****************************************************************/

/* No unique error definitions at this time */

/****************************************************************/
/*								*/
/*  Disk Error Definitions - Range 0x4300 - 0x437F		*/
/*								*/
/****************************************************************/

#define	E_SPACE		0x4300L	/* No block or directory entries	*/
				/* available				*/
#define E_MEDIACHANGE	0x4301L	/* Media change occured			*/
#define E_MEDCHGERR	0x4302L	/* Detected media change after a write	*/
#define E_PATH		0x4303L	/* Bad path				*/
#define E_DEVCONFLICT	0x4304L	/* Devices locked exclusively		*/
#define E_RANGE         0x4305L	/* Address out of range			*/
#define E_READONLY	0x4306L	/* Rename or delete on R/O file		*/
#define E_DIRNOTEMPTY	0x4307L	/* Delete of non-empty directory	*/
#define E_BADOFFSET	0x4308L	/* Badoffset in read, write or seek	*/
#define E_CORRUPT	0x4309L	/* Corrupted Fat			*/
#define E_PENDLK        0x430AL	/* Cannot unlock a pending lock		*/
#define E_RAWMEDIA	0x430BL	/* Media does not support file system.	*/
				/* Not DOS media.			*/
#define	E_FILECLOSED	0x430CL	/* File was closed before async lock	*/
				/* could be completed.			*/
#define	E_LOCK		0x430DL	/* Lock access conflict			*/
#define	E_FATERR	0x430EL	/* Error while reading the FAT.		*/

/****************************************************************/
/*								*/
/*  Driver Error Definitions - Range 0x0 - 0x3FFF		*/
/*								*/
/****************************************************************/

/****************************************************************/
/*								*/
/*  Note the order and definition of the following 16 error	*/
/*  codes must not change.  These error are to be returned to 	*/
/*  the appropriate resource manager.				*/
/*								*/
/****************************************************************/

#define	E_WPROT		0x00L	/*  write protect violation	*/
#define	E_UNITNO	0x01L	/*  illegal unitnumber		*/
#define	E_READY		0x02L	/*  drive not ready		*/
#define	E_INVCMD	0x03L	/*  invalid command issued	*/
#define	E_CRC		0x04L	/*  crc error on i/o		*/
#define	E_BADPB		0x05L	/*  bad parameter block		*/
#define	E_SEEK		0x06L	/*  seek operation failed	*/
#define	E_UNKNOWNMEDIA	0x07L	/*  unknown media present	*/
#define	E_SEC_NOTFOUND	0x08L	/*  req'd sector not found	*/
#define	E_DKATTACH	0x09L	/*  attchmt didn't respond	*/
#define	E_WRITEFAULT	0x0AL	/*  write fault			*/
#define	E_READFAULT	0x0BL	/*  read fault			*/
#define	E_GENERAL	0x0CL	/*  general failure		*/
#define	E_MISADDR	0x0DL	/*  missing address mark	*/
#define	E_RES1		0x0EL	/*  reserved			*/
#define	E_RES2		0x0FL	/*  reserved			*/


/*	END OF SYSTEM.H						*/
