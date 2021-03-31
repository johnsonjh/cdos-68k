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

DISK5.DOC                               Developer Kit Disk 5 - SYSTEM LIBRARY
6 August 1986                                              CDOS 68K, ver. 1.2
-----------------------------------------------------------------------------

	This diskette contains the sources, linkable library, submit files,
and batch files for a library that will allow you to write almost normal 'C' 
programs that will execute under CDOS version 1.2 for the MC68000 family of 
microprocessors.  In addition to the files listed in the Release Note, the
following files are provided on this diskette:
     
     SYSLINK  BAT		COMPL68  BAT
     LD       C			BUILDL68 BAT
     STD40    H			LDIV     S
     LIBCALLS H                 LREM     S
     OSIF     O			LMUL     S
     GSTART   O			STRNCMP  C
     GSTART   C                 STRCAT   C
     OSIF     S			STRCPY   C                 

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

	LD.C 	  -- Does S_LOOKUPS on the Disk File Table looking
	for sub-directories.  LD may be invoked with or without a
	argument.  Wild cards are allowed.

