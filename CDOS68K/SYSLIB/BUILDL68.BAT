
;
;	This submit file builds SYSLIB.L68 from scratch.
;
ERASE SYSLIB.OLD
REN SYSLIB.L68 SYSLIB.OLD
ar68 r syslib.l68 gmain.o tools.o fprintf.o stoi.o
ar68 r syslib.l68 ebwait.o ecommand.o econtrol.o elock.o eread.o erwait.o
ar68 r syslib.l68 especial.o eterm.o etimer.o ewrite.o sabort.o salter.o
ar68 r syslib.l68 sbwait.o scancel.o sclose.o scommand.o scontrol.o scopy.o
ar68 r syslib.l68 screate.o sdefine.o sdelete.o sdevlock.o sdisable.o
ar68 r syslib.l68 senable.o sexcept.o sexit.o sget.o sgive.o sgsx.o 
ar68 r syslib.l68 sinstall.o skctrl.o slock.o slookup.o smalloc.o smctrl.o
ar68 r syslib.l68 smfree.o sopen.o sorder.o soverlay.o srdelim.o sread.o
ar68 r syslib.l68 srename.o sreturn.o srwait.o sseek.o sset.o sspecial.o
ar68 r syslib.l68 sstatus.o sswiret.o stimer.o svccreat.o swait.o swrite.o
ar68 r syslib.l68 sxlat.o zerofil.o osif.o ldiv.o lmul.o lrem.o strcat.o
ar68 r syslib.l68 strcpy.o strncmp.o
;
;	Done.
;
