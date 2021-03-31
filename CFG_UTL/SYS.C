/***********************************************************************
*  SYS - copy system files from boot drive to destination drive
*
*  VERSION  DATE	WHO	CHANGE
*  =======  ====	===	======
*  1.1	    03/24/86	MA	Relink with new putsys.  Use s_exit()
*				to return properly.
*  1.0	    ??/??/??	MA	First version. 
***********************************************************************/


#include "portab.h"
#include "stdio.h"

EXTERN LONG putsys();

main(argc,argv)
WORD argc;
BYTE *argv[];
{
    LONG sysfiles;

    if (argc != 2)
    {
	printf("Usage: sys <drive name>");
	s_exit(1L);
    }
    sysfiles = putsys(argv[1]);
    if (sysfiles == 0)
	printf("No system files transferred\r\n");
    s_exit(0L);
/*  printf("%ld bytes of system files transferred\r\n");    */
}
