echo off
echo *-----------------------------------------------------*
echo *                 RAMDISK.BAT                         *
echo *   This Batch file will install the loadable Memory  *
echo * Disk driver in your CDOS 1.2 operating sytem.  The  *
echo * installation will default a disk size of 64K if no  *
echo * parameter is specified when invoking this batch     *
echo * file.  At present, this version (0.03), can only    *
echo * only be installed in sizes of 64k, 128k, 256k, and  *
echo * 512K.  The Ram Disk is physical device "rd0:" and   *
echo * logical device "m:".                                *
echo *-----------------------------------------------------*
rem
rem     *** First check to see if rd0: has already been
rem                      installed.
rem
if exist rd0: goto done1
rem
rem     *** Was the size specified ?
rem
if @%1 == @  goto dosize
	define -s RDSIZE = %1
	goto install
:dosize
	define -s RDSIZE = 64
rem 	
:install
	dvrload rd0: ramdsk.drv lnrws
	if exist rd0: goto done
	echo *************Ram Disk Installation Failure******************
	goto fini
:done1
	echo *--------------Ram Disk already Installed ------------------*
	goto fini
:done
	define -s m:=rd0:
	echo *-----------------Ram Disk installed -----------------------*
	goto fini
:fini
	echo *------------------------DONE-------------------------------*
	echo on
