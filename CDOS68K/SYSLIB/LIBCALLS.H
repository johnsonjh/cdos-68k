/*=========================================================*
 *	Declarations for calls made thru GMAIN.C           *
 *=========================================================*/

EXTERN	BYTE	to_prnt();
EXTERN	BYTE	fgetc();
EXTERN	BYTE	getchar();

EXTERN	int	clear();
EXTERN	int	ch_upper();
EXTERN	int	fread();
EXTERN	int	puts();
EXTERN	int	putchar();
EXTERN	int	strlen();
EXTERN	int	strcmp();
EXTERN  int	mikechar();
EXTERN	int	getnam();

EXTERN	LONG	getnum();
EXTERN	LONG	to_bin(),fflush();
EXTERN 	LONG	fcreat();
EXTERN	LONG	pmake();
EXTERN	LONG	fopen();
EXTERN	LONG	fclose();
EXTERN 	LONG	fputc();
EXTERN	LONG	fputs();
EXTERN	LONG	getrc();
EXTERN	LONG	stoi();
EXTERN		getstr(), err();

	WORD	EXIT = 1;
/*---------------------------------------------------------*
 *	External declarations for CDOS calls.              *
 *---------------------------------------------------------*/

EXTERN	LONG s_abort(),e_termevent();
EXTERN	LONG s_alter();
EXTERN	LONG s_bwait(),e_bwait();
EXTERN	LONG s_cancel();
EXTERN	LONG s_close();
EXTERN	LONG s_command(),e_command();
EXTERN	LONG s_control(),e_control();
EXTERN	LONG s_copy();
EXTERN	LONG s_create();
EXTERN	LONG s_vccreate();
EXTERN	LONG s_define();
EXTERN	LONG s_delete();
EXTERN	LONG s_disable();
EXTERN	LONG s_devlock();
EXTERN	LONG s_enable();
EXTERN	LONG s_exception();
EXTERN	LONG s_exit();
EXTERN	LONG s_get();
EXTERN	LONG s_give();
EXTERN	LONG s_gsx();
EXTERN	LONG s_install();
EXTERN	LONG s_kctrl();
EXTERN	LONG s_keyput(); 
EXTERN	LONG s_lock(),e_lock();
EXTERN	LONG s_lookup();
EXTERN	LONG s_malloc();
EXTERN	LONG s_mfree();
EXTERN	LONG s_open();
EXTERN	LONG s_order();
EXTERN	LONG s_overlay();
EXTERN	LONG s_read(),e_read();
EXTERN	LONG s_rdelim();
EXTERN	LONG s_rename();
EXTERN	LONG s_return();
EXTERN	LONG s_rwait(),e_rwait();
EXTERN	LONG s_seek();
EXTERN	LONG s_set();
EXTERN	LONG s_special(),e_special();
EXTERN	LONG s_status();
EXTERN	LONG s_swiret();
EXTERN	LONG s_timer(),e_timer();
EXTERN	LONG s_wait();
EXTERN	LONG s_write(),e_write();
EXTERN	LONG s_xlat();

/*=========================================================*
 * 		END OF DECLARATIONS                        *
 *=========================================================*/
