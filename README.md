# Concurrent DOS 68K

Concurrent DOS for the Motorola MC68000 VME/10

```text
DISK1.DOC                       Developer Kit Disk 1 - Boot Disk and Executable
                                                       Utilities (part 1)
6 August 1986                                          CDOS 68K ver. 1.2
_______________________________________________________________________________


     In this release of Concurrent DOS 68K the operating system's function-
ality and characteristics are defined in two phases;  1) the initial hardware
configuration and the user environment are defined during system generation
(when the CDOS.SYS file is created) by the VMCONFIG.C file, and 2) at system
load time via the files CONFIG.BAT and AUTOEXEC.BAT.  Be sure to edit these
files to correspond to any changes you make to your hardware or software
organization.

     This version of Concurrent DOS is shipped with a system that includes
the CP/M 68K front end and may be booted from either the floppy or the hard
disk.  Instructions for booting from the hard disk are included in the
Concurrent DOS for the Motorola MC68000 Release Note.

     For the VME/10 the window manager keys are as follows:

Function            Key

<WINDOW>            SEL
<HELP>              PAD FUNC + HELP
<NEXT>              -->| (the tab key on the arrow keypad)
<PREV>              |<-- (the backtab key on the arrow keypad)

```
```text
DISK2.DOC                       Developer Kit Disk 2 - Executable Utilities
                                                       (part 2) and UNIX-like
                                                       Utilities (part 1)
6 August 1986                                          CDOS 68K ver. 1.2
_______________________________________________________________________________


     This disk contains the remainder of the CDOS native utilities and part
1 of a set of general purpose tools similar to some of the tools provided with
UNIX.  These UNIX-like tools are provided to help system developers and
application programmers create and maintain files.  These tools are documented
in the PROGRAMMER'S UTILITIES GUIDE SUPPLEMENT.



```
```text
DISK3.DOC                       Developer Kit Disk 3 - UNIX-like Utilities
                                                       (part 2), CP/M 68K Tools,
                                                       Special Utilities, and
                                                       Code Sample
6 August 1986                                          CDOS 68K ver. 1.2
_______________________________________________________________________________


     This disk contains the following programming tools and sample code:

Subdirectory          Explanation

U_TOOLS               Remainder of the UNIX-like tools.  Documented in the
                      PROGRAMMER'S UTILITIES GUIDE SUPPLEMENT.

TOOLS                 CP/M 68K programming tools; requires the CP/M 68k
                      front-end be present.  The utilities COMBINE and
                      EXPAND are CDOS native mode utilities that are
                      useful for conserving disk space used to store
                      small files.  The typical naming convention is to
                      give COMBINE'd files the extension ARC.  All of these
                      programs are documented in the PROGRAMMER'S UTILITIES
                      GUIDE.

SAMPLE                Some of the Window Manager source is provided as an
                      example of a program that implements windowing tech-
                      niques, message passing, and process creation.
                      In addition to the files listed in the release note
                      the following files are included in the SAMPLE
                      subdirectory:
                           CCUTLS.H
                           UTLERRS.H


```
```text
DISK4.DOC                       Developer Kit Disk 4 - Programmer's Utilities
                                                       and Tools and C Compiler
6 August 1986                                          CDOS 68K ver. 1.2
_______________________________________________________________________________


     This disk contains the assembler, C compiler, linkers, debugger, and
other programming tools and utilities.  These tools and utilities are all
documented in the PROGRAMMER'S UTILITIES GUIDE.  Supplemental discussions
of assembly language programming conventions, file load formats, and the use
of the COFF and CRUNCH utilities are contained in the CONCURRENT DOS
SUPPLEMENT FOR COMPUTERS BASED ON THE MOTOROLA MC68000 FAMILY OF
MICROPROCESSORS.

     The C library contains CP/M 68K code rather than CDOS 68K native mode
code and therefore, the executable files produced can only be run on CDOS 68K
systems that include the CP/M 68K front end.  CDOS native mode C programs can
be produced by using the SYSLIB.L68 library included on Developer Kit Disk 5.

```
```text
DISK5.DOC                               Developer Kit Disk 5 - SYSTEM LIBRARY
6 August 1986                                              CDOS 68K, ver. 1.2
-----------------------------------------------------------------------------

        This diskette contains the sources, linkable library, submit files,
and batch files for a library that will allow you to write almost normal 'C'
programs that will execute under CDOS version 1.2 for the MC68000 family of
microprocessors.  In addition to the files listed in the Release Note, the
following files are provided on this diskette:

     SYSLINK  BAT               COMPL68  BAT
     LD       C                 BUILDL68 BAT
     STD40    H                 LDIV     S
     LIBCALLS H                 LREM     S
     OSIF     O                 LMUL     S
     GSTART   O                 STRNCMP  C
     GSTART   C                 STRCAT   C
     OSIF     S                 STRCPY   C

     The following files listed in the Release Note have not been provided:

     CRCK.C
     QATOOLS.C
     PRINTF.C
     JPRT.C


          This library is designed for use with the CP/M 68K C compiler and
contains the necessary 'C' functions to permit extremely sophisticated
system programs however, it is by no means complete.  K&R standards were not
strictly adhered to.  Two sample programs have been included as an illustra-
tion of what can be done with this library and to allow you to learn from
what we have learned.

SUBMIT FILES:

        The submit files, which were originally provided for use in a CP/M
        68K crosss-developement mode, compile and link the library source
        into the file SYSLIB.L68.  Use the the batch files of the same names
        under CDOS 68K.

        The submit files are meant to be more than just tools; they are
        also valuable examples of how things can be accomplished.  Feel free
        to study and modify them to suit your needs.

        COMPL68.SUB  -- This file will compile and assemble all
        of the source modules for SYSLIB.L68.

        BUILDL68.SUB -- Invokes AR68 to archive all of the .O files
        to create the library.

BATCH FILE:

        COMPL68.BAT -- See COMPL68.SUB

        BUILDL68.BAT -- See BUILDL68.SUB

        SYSLINK.BAT -- An example of the linker command line to link
        a program with the system library.

SAMPLE PROGRAMS:

        CVT40.C   -- A   simple  program  to   perform   numeric
        conversions.   Hexadecimal  numbers  are proceeded by  a
        '0x',  octal numbers are prefixed with '0',  and decimal
        numbers  begin  with the decimal  digit.   This  program
        demonstrates the use of the SYSLIB functions.

        LD.C      -- Does S_LOOKUPS on the Disk File Table looking
        for sub-directories.  LD may be invoked with or without a
        argument.  Wild cards are allowed.

```
```text
DISK6.DOC               Developer Kit Supplement Disk 1 - Driver and System
                                                          Sources
6 August 1986                                          CDOS 68K ver. 1.2
_______________________________________________________________________________

     This diskette is organized into three subdirectories:

     DRV_SRC -- Contains all of the CDOS 68K driver modules in source form.
     In addition to the files listed in the release note, this subdirectory
     contains the file CPASM.BAT which is used to process the RAMDISK driver
     source component BTOOLS.PS.  To produce a loadable ramdisk driver, the
     linker output file RDD.68K must be processed by the CRUNCH utility.

     SYS_SRC -- Contains all of the CDOS 68K system configuration modules
     in source form.  Also, the file listed in the release note as OMMUA.S
     is named OMMUA.PS.  Files with the file extension "PS" are assembler
     files that use the #include directive and must be processed by the C
     preprocessor CP68.68k.

     HEADERS -- Contains all of the .h files used to compile the sources
     in DRV_SRC and SYS_SRC.

     In order to conserve disk space, some of the .h files that are used to
compile the SYSLIB sources may not have been duplicated on this disk.  Check
Developer Kit Disk 5 for additional .h files.


```
```text
DISK7.DOC               Developer Kit Supplement Disk 2 - System Debugger,
                                                          System Generation
                                                          Tools and Objects
6 August 1986                                          CDOS 68K ver. 1.2
_______________________________________________________________________________

     This diskette is organized into two subdirectories:

     DEBUGGER -- Contains all of the files necessary to debug a CDOS 68K
     system under CP/M 68K.  Instructions for using the cross-environment
     debugger are provided in the CDOS 68K Release Note.

     SYS_GEN -- Organized into two subdirectories:

          TOOLS --  Contains .BAT and .COM files to link a new system file.
          The different permutations of hardware and features are described
          in the CDOS 68K Release Note.  To link a new system file you must
          increase the stack size of LINK68.68K to F000h with the following
          command:

               SS -sf000 link68.68k

          OBJECTS -- Contains some of the objects files required to link a new
          system file.

NOTE:  The files BASE.O, COFFLOAD.O, and COMMAND.O are on Developer Kit
Supplement Disk 2 instead of Developer Kit Disk 3 as shown in the CDOS
68K Release Note.


```
```text
DISK8.DOC               Developer Kit Supplement Disk 3 - System Generation
                                                          Objects
6 August 1986                                          CDOS 68K ver. 1.2
_______________________________________________________________________________

     This diskette is contains the remainder of the object files required to\
generate a new system file.  In general the files are organized into the
following categories:

File System files:
FSCACHE.O
FSDM.O
FMNAME.O
FSPATH.O
FSVOL.O
FSWINDOW.O
HOOK.O

CP/M 68k Front End files:
FE1.O
FE2.O
FE68.O
FESTUB.O (used in systems with no front end)
FEINIT.O
FEMAIN.O
FE.O
FEU1.O
FEU2.O
FPOOL.O
FPOOL68K.O (used in systems with no front end)
FPRINTF.O
SUP68K.O
SUP68KNF.O (used in systems with no front end)

Console Driver files:
VCDRCOPY.O
VCDRTOOL.O
VCDRV.O
VCDRVWRT.O
IBMTOVM.O
VMCONASM.O
VMKB.O

Disk Driver files
VMDISKA.O
VMRWINDK.O

Serial Port Driver files
VM400.O
VT52.O
SDRV.O
DRT.O

Kernel, Resource Managers and Misc. files
ABORT.O         AFLAG.O         ALLOCATE.O      ASYNC.O
ASYNCA.O        BASE.O          COFFLOAD.O      COMMAND.O
CPMLOAD.O       CRMAN.O         CRMCONV8.O      CRMCOPY.O
CRMCREAT.O      CRMDEL.O        CRMREAD.O       CRMGSL.O
CRMINST.O       CRMKB.O         CRMMOUSE.O      CRMOPEN.O
CRMRDEL.O       CRMWIND.O       CRMWRITE.O      CRMXLAT.O
DINIT.O         DVRIF.O         EXCEPT.O        FREE.O
HEAP.O          INSTALL.O       KFUNCS.O        KTOOLS.O
LOAD68K.O       LUTILS.O        M68.O           MISMAN.O
MGETBLK.O       NULLDEV.O       OFILE.O         OSMEM.O
OVERLAY.O       PANIC68.O       PEWTER.O        PIPE.O
PMGET.O         PROCESS.O       RDELIM.O        RTMIF.O
SUPERMAN.O      SWI68K.O        TABLES.O        TOOLS.O
VMCLK.O         VMCONFIG.O      VMPANIC.O       VMQUEUE.O
VPROBE.O

NOTE:  The files BASE.O, COFFLOAD.O, and COMMAND.O are on Developer Kit
Supplement Disk 2 instead of Developer Kit Disk 3 as shown in the CDOS
68K Release Note.

```
```text
DISK9.DOC                OEM Redistribution Kit Disk 1 - Utility Objects
6 August 1986                                          CDOS 68K ver. 1.2
_______________________________________________________________________________

     This diskette contains the object files and batch files to build the
following utilities:

RECDIR
FSET
MORE
SORT
CHKDSK
PROCESS
CONFIG
RENAME
DATE
DIR
DISKSET
FIND
TIME
TREE
TYPE
COPY
VER
VOL
BACKUP
WMEX
RESTORE
LOGON
COMP
PASSWORD
RECFILE

```
```text
DISK10.DOC                OEM Redistribution Kit Disk 2 - System Libraries
                                                          Shell Objects, Booter
                                                          Source, and OEM
                                                          Configurable Utilities
                                                          Sources
6 August 1986                                          CDOS 68K ver. 1.2
_______________________________________________________________________________

     This diskette is organized into the following subdirectories:

L68S -- System libraries linked into the shell and the CDOS utilities.

SHEL_OBJ -- Shell object file and the linker input file SHELL.COM to
regenerate the shell.

BOOTER  --  Sources to the booter used with CDOS 68K for the VME/10.

CFG_UTL  --  Sources and linker input files for the following OEM
configurable utilities:

COFF
FORMAT
SYS
DISKCOMP
DISKCOPY
CRUNCH
SS

This subdirectory also includes the files PORTAB.H and STDIO.H.  These
are special versions the header files designed specifically for use when
compiling the BOOTER and other utilities.  Do not use other versions of
PORTAB.H or STDIO.H when compiling sources included on this disk.

```
```text
DISK11.DOC                OEM Redistribution Kit Disk 3 - System Message Source,
                                                          CP/M68K to CDOS File
                                                          Transfer Utility, and
                                                          Special Sources
6 August 1986                                          CDOS 68K ver. 1.2
_______________________________________________________________________________

     This diskette is organized into the following subdirectories:

CCMSGS --  Contains the sources to all of the general system messages in
COMBINE'd file format.  Convert to individual files using the EXPAND
utility.

UTL_MSGS -- Contains the sources to these utilities in COMBINE'd file
format.  Convert to individual files using the EXPAND utility.

RECDIR
FSET
MORE
SORT
CHKDSK
PROCESS
CONFIG
RENAME
DATE
DIR
DISKSET
FIND
TIME
TREE
TYPE
COPY
VER
VOL
BACKUP
WMEX
RESTORE
LOGON
COMP
PASSWORD
RECFILE

Source -- Contains the source to files which must be modified to run CDOS
68K on a Motorola 68020 microprocessor.  See the CDOS 68K Release Note
for additional information; these files are named DISPA.PS and MMU.PS and
should be assembled using the CPASM.BAT file found in the DRV_SRC directory
on Developer Kit Supplement Disk 1.  This subdirectory also contains the
files STRUCT.EQU, PANIC.EQU, and CPUMMU.EQU which are not shown on the list
of included files in the RELEASE NOTE.

VMUTIL --  Contains the source and executable file for the CP/M 68K to CDOS
68K file transfer utilitiy.

SHEL_MSG -- Contains the sources to the utilities built in to the shell in
COMBINE'd file  format.  Convert to individual files using the EXPAND utility.
```
