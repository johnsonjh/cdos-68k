/* gstart.c - 68K equivalent of gstart.a86	*/

#define F_EXIT 25

_main()
{
    mainmain();
    __osif(F_EXIT,0L);
}
 