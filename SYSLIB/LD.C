/*#define EBUG */
/*=========================================================*
 *	LD.C  --   ---------------------------------       *
 *       Program to display the Sub-directories on a disk  *
 *---------------------------------------------------------*
 *   VERSION   DATE       AUTHOR    COMMENTS               *
 *---------------------------------------------------------*
 *    1.10     08/05/85   cpg       LD now lists only      *
 *                                    subdirectories.      *
 *---------------------------------------------------------*
 *  INCLUDES:                                              */
#include "std40.H"
#include "libcalls.H"

#ifdef EBUG
#define TRACE printf
#else
#define TRACE   /**/
#endif

#define FILENUM_T 65
#define CONSOLE_T 0x30
#define DSK_FIL_T 0x20
#define SUB_DIR   0x0010

struct disk_file
    {
	LONG	key;
	char	name[18];
	UWORD	attr;
	UWORD	rec_siz;
	UBYTE	user_id;
	UBYTE	grp_id;
	UWORD	sec_word;
	UWORD	res[3];
	LONG	size;
	LONG	date;
	LONG	time;
    } d_table;

	BYTE *fname[4];

main(argc,argv)
	int argc;
	char *argv[];
{

	if( argc == 2 )
	{
	   fname[0] = argv[1];
	} else
	   fname[0] = "*";

	TRACE("\n\rTRACE: argc = %02d ",argc);
	TRACE("\n\rTRACE: fname[0] = %s -- argv[%d] = %s  ",
			fname[0],(argc - 1),argv[argc - 1]);

	show_dfile( fname[0] );
	return(0L);
}
/*---------------------------------------------------------*
 *                   UTILITY ROUTINES                      *
 *---------------------------------------------------------*/
show_dfile( ln )
	BYTE *ln; 
{
	LONG	nfound;
	LONG	ret,next;
	WORD	t_knt = 0;

	printf("\n\rSub-Directory entries follow:");

	if( (nfound = s_lookup(DSK_FIL_T,0x0f,ln,&d_table,
		(LONG)sizeof(d_table),(LONG)sizeof(d_table),0L)) < 0L) 
	{
		printf("\n\rERROR on F_LOOKUP...ret = %08lx.",nfound);
		return(-1L);
	}

	while (nfound > 0L)
	{
	   do_title1();
           if( d_table.attr & SUB_DIR ) 
	   {
	     printf("\n\r\t/%-18s    %04x   %02x/%02x    %04x ",
			d_table.name,d_table.attr,d_table.user_id,
			d_table.grp_id,d_table.sec_word);
	     t_knt++;
	   }

	   next = d_table.key;

	   nfound = s_lookup(DSK_FIL_T,0x0f,ln,&d_table,
			(LONG)sizeof(d_table),(LONG)sizeof(d_table),
			  next);
	}

	printf("\n\r\nEnd of Directory.  %d Sub-directories found.\n\r",t_knt);

	if( nfound < 0L ) 
	{
		printf("\n\rERROR: code = &08lx",nfound);
	}

}

do_title1() 
{
   static    BOOLEAN done = FALSE;
   int	i;

   if(done == TRUE) return(0);

   printf("\n\r\tNAME                  ATTRIB: US/GP    SECURITY");
   printf("\n\r\t");
   for (i = 0; i < 28; i++) printf("--"); 

   done = TRUE;
   return(0);	
}
/* ------------------ End of LD.C ------------------------*/
