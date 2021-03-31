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
 *   Version 1.2        IO.H                                     *
 *                      Structures and Defines for the Driver to *
 *                      System Interface.                        *
 *---------------------------------------------------------------*
 *    VERSION   DATE    BY      CHANGE/COMMENTS                  *
 *---------------------------------------------------------------*
 *    1.0     06/12/85 reb      rewritten/added to system        *
 *    1.1     07/17/85 jns	added doasr/nextasr defines	 *
 *				reformat PD to reflect struct.h  *
 *				removed okasr/noasr.		 *
 *    1.2     07/30/85 jns	added ASRMX defines.		 *
 *                                                               *
 *===============================================================*
 *  INCLUDES:                                                    */

/************************************************************************
 *                                    IO.H                              *
 *  General header file for drivers.                                    *
 *                                                                      *
 ************************************************************************/

/*
 *  structure name defines
 */

#define DH              struct _DriverHdr
                        /*  Driver Header                               */

#define DPBLK           struct _DvrPblk
                        /*  Driver Parameter Block.  Structure of Parm  */
                        /*  block passed to driver from Resource Mgrs.  */

#define PD              struct _ProcDescr
                        /*  Process Descriptonr structure.              */

#define PORT_ITAB       struct _PortInfoTable
                        /*  structure of Port Information table         */



/************************************************************************
 *  general type definitions
 ************************************************************************/

#define PROC            WORD
                        /*  this is subroutine type.  it is similar to  */
                        /*  VOID, in that it declares that a routine    */
                        /*  returns no value in particular; i.e., it is */
                        /*  a procedure, as opposed to a function.  It  */
                        /*  differs from VOID in that you can actually  */
                        /*  globally declare a procedure with PROC,     */
                        /*  whereas you can't with VOID ('cause someone */
                        /*  saw fit to define it as a comment, thus     */
                        /*  causing a syntax error in the following:    */
                        /*      VOID   funcname() ;                     */
                        /*  however, it will be legal to declare:       */
                        /*      PROC    funcname() ;                    */

#define ASR             WORD
                        /*  another subroutine type.  declares the rtn  */
                        /*  as an ASR, returning no value, and running  */
                        /*  asynchronously.                             */

#define ISR             BOOLEAN
                        /*  for interrupt service routines              */


#define ERROR           LONG            
                        /*  Error codes.                                */

#define EMASK           LONG
                        /*  Event Masks.                                */

#define ENUM            LONG
                        /*  Event numbers.                              */

#define MXID            LONG
                        /*  mutual exclusion id's                       */
                        /*  used with mxinit, mxevent, etc              */

#define FLAGNO          LONG
                        /*  flag numbers used with flagget, flagset, etc*/

#define NULLL           0L
                        /*  Long Nulls                                  */

#define SYSADDR         LONG
                        /*  System Address                              */

#define PHYSADDR        LONG
                        /*  Physical Address                            */

#define USRADDR         LONG
                        /*  User Address                                */



/***********************************************************************
*  useful constants
*/

#define MAXPOSLONG      0x7fffffffL



/***********************************************************************
*  useful code macros
*
*/


#define MGETBLK(x)      (x *) mgetblk(sizeof(x))
                        /*  calls the system mgetblk routine, passing   */
                        /*  size of the structure, and casting the      */
                        /*  value returned to the correct type          */

#define MFREEBLK(x)     mfreblk(x)
                        /*  calls the free block routine                */

#define MAX(x,y)        ( (x) > (y) ? (x) : (y) ) 
#define MIN(x,y)        ( (x) < (y) ? (x) : (y) ) 




/***********************************************************************
*  system call map
*       maps all calls to the correct system routines
*/

#define ASRMX		asrmx
#define MXINIT          mxinit
#define MXEVENT         mxevent
#define MXUNINIT        mxunint
#define MXREL           mxrel
#define FLAGCLR         flagclr
#define FLAGSET         flagset
#define FLAGEVENT       flagevent
#define FLAGGET         flagget
#define FLAGREL         flagrel

#define WAIT            mwait
#define MWAIT           mwait

#define ARET            aret

#define SALLOC          salloc
#define MAPU            mapu
#define MAPPHYS         mapphys
#define SADDR           saddr
#define UADDR           uaddr
#define PADDR           log2phy

#define NODISP          nodisp
#define OKDISP          okdisp
#define NOABORT         noabort
#define OKABORT         okabort
#define DOASR		doasr
#define NEXTASR		nextasr
#define SFREE           sfree
#define MLOCK           mlock
#define MUNLOCK         munlock
#define UNMAPU          unmapu

#define PCREATE         pcreate

#define OUTP            outp

/**********************************************************************
*
*  system externals
*/

                                         /*  mutual exclusion functions  */
EXTERN	MXID	ASRMX() ;
EXTERN  MXID    MXINIT() ;                      
EXTERN  MXID    MXEVENT() ;
EXTERN  MXID    MXUNINIT() ;
EXTERN  MXID    MXREL() ;

EXTERN  VOID    NODISP() ;
EXTERN  VOID    OKDISP() ;
EXTERN  VOID    NOABORT() ;
EXTERN  VOID    OKABORT() ;
EXTERN	VOID	DOASR() ;
EXTERN	VOID	NEXTASR() ;
                                        /*  flag system functions       */
EXTERN  ERROR   FLAGCLR() ;
EXTERN  ERROR   FLAGSET() ;
EXTERN  EMASK   FLAGEVENT() ;
EXTERN  FLAGNO  FLAGGET() ;
EXTERN  ERROR   FLAGREL() ;

EXTERN  EMASK   WAIT() ;
EXTERN  ERROR   ARET() ;
                                        /*  memory allocate functions   */
EXTERN  SYSADDR SALLOC() ;
EXTERN  ERROR   SFREE() ;
EXTERN  SYSADDR mgetblk() ;
EXTERN  ERROR   mfreeblk() ;
                                        /*  memory mapping functions    */
EXTERN  VOID    MAPU() ;
EXTERN  VOID    UNMAPU() ;
EXTERN  SYSADDR MAPPHYS() ;

EXTERN  SYSADDR SADDR() ;
EXTERN  USRADDR UADDR() ;
EXTERN  PHYSADDR PADDR() ;
                                        /*  misc memory mgmt functions  */
EXTERN  VOID    MLOCK() ;
EXTERN  VOID    MUNLOCK() ;
EXTERN  ERROR   MRANGE() ;
                                        /*  misc functions              */
EXTERN  EMASK   PCREATE() ;
EXTERN  VOID    SETVEC() ;
EXTERN  LONG    SUPIF() ;




/***********************************************************************
*  DH -
*       Driver Header
*
*/

DH
{       
        UWORD   dh_dtype        ;/* type of driver                      */
        UBYTE   dh_nbrunits     ;/* max number of units supported       */
        UBYTE   dh_flags        ;/* flag word                           */

        ERROR   (*dh_init)()    ;/* pointer to init code                */
        ERROR   (*dh_subdrv)()  ;/* pointer to subdrv code              */
        ERROR   (*dh_uninit)()  ;/* pointer to init code                */
        ERROR   (*dh_select)()  ;/* pointer to select code              */
        ERROR   (*dh_flush)()   ;/* pointer to flush code               */
        EMASK   (*dh_read)()    ;/* pointer to read code                */
        EMASK   (*dh_write)()   ;/* pointer to write code               */
        ERROR   (*dh_get)()     ;/* pointer to get code                 */
        ERROR   (*dh_set)()     ;/* pointer to set code                 */
        EMASK   (*dh_special)() ;/* pointer to special code             */

        /*
         *   all items below are filled in by the o/s and should be 
         *   initialized to 0L
         */

        LONG    dh_ctl0         ;/*  reserved                           */
        LONG    dh_ctl1         ;/* reserved                            */
        LONG    dh_sync         ;/*  mxid for syncing on driver         */
        PD      **dh_curpd      ;/*  pointer to current pd              */
        LONG    dh_sysfunc      ;/*  ptr to os function table           */
} ;

/*
 *  dh_flag values
 *
 */

#define DHF_DSYNC       0x01
        /*  sync at driver level                                        */
#define DHF_USYNC       0x02
        /*  sync at unit level                                          */


/*
 *  Driver Parameter Block
 *      This is a general outline of what all parameter blocks to driver
 *      functions will look like.  
 *
 */

DPBLK
{
        BYTE    dp_unitno       ;/*  unit number for request            */
        BYTE    dp_option       ;/*  option for request                 */
        UWORD   dp_flags        ;/*  flags for request (see below)      */
        LONG    dp_swi          ;/*  address of calling proc's swi      */
        LONG    dp_pdaddr       ;/*  address of calling proc's proc descr*/
        BYTE    *dp_buffer      ;/*  address of data buffer             */
        LONG    dp_bufsiz       ;/*  size of buffer (dev specific)      */
        LONG    dp_offset       ;/*  offset for i/o                     */
        LONG    dp_delim        ;/*  parm specifying for delimiters     */
        LONG    dp_parm9        ;/*  msc parm                           */
} ;

        /*  dp_flags definition  */

#define DPF_UADDR       0x8000  
                        /* addresses are from user space        */


/*
 *  Process Descriptor structure.  Details are not necessary, only the size
 *  and the rnid,rpid fields concern the driver.
 */

PD 
{
        LONG    p_stuff[23];
        WORD    p_rnid;
	WORD	p_rfid;
        LONG    p_rpid;
} ;



/***********************************************************************
*  error codes for drivers.
*/

/*
 *  disk error codes
 */

#define E_MAGICBASE     0x80000000L + ( ((ULONG)(DVR_DISK)) << 16)

#define E_WPROT         E_MAGICBASE + 0x00L
#define E_UNITNO        E_MAGICBASE + 0x01L
#define E_READY         E_MAGICBASE + 0x02L
#define E_INVCMD        E_MAGICBASE + 0x03L
#define E_CRC           E_MAGICBASE + 0x04L
#define E_BADPB         E_MAGICBASE + 0x05L
#define E_SEEK          E_MAGICBASE + 0x06L
#define E_UNKNOWNMEDIA  E_MAGICBASE + 0x07L
#define E_SEC_NOT_FOUND E_MAGICBASE + 0x08L
#define E_DKATTACH      E_MAGICBASE + 0x09L
#define E_WRFAULT       E_MAGICBASE + 0x0aL
#define E_RDFAULT       E_MAGICBASE + 0x0bL
#define E_GENERAL       E_MAGICBASE + 0x0cL
#define E_MISADDR       E_MAGICBASE + 0x0dL
#define E_RES1          E_MAGICBASE + 0x0eL
#define E_RES2          E_MAGICBASE + 0x0fL

                                                                                                    