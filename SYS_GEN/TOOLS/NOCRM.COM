[sym,abs,text[b00]] cdosnocr.68k =

dispa10.obj,mmu10m1.obj,ommu10.obj,ommua10.obj,

abort.o,aflag.o,allocate.o,async.o,asynca.o,base.o,coffload.o,command.o,
cpmload.o,
dinit.o,dvrif.o,except.o,fe1.o,fe2.o,fe68.o,
feinit.o,femain.o,feu1.o,feu2.o,fpool68k.o,fprintf.o,free.o,
fscache.o,fsdm.o,fsname.o,fspath.o,fsvol.o,fswindow.o,
heap.o,hook.o,install.o,kfuncs.o,ktools.o,load68k.o,
lutils.o,m68.o,mgetblk.o,misman.o,nulldev.o,ofile.o,osmem.o,overlay.o,
panic68.o,pewter.o,pipe.o,pmget.o,process.o,rdelim.o,rtmif.o,sup.o,
sup68k.o,superman.o,swi.o,swi68k.o,tables.o,tools.o,

vm400.o,vmclk.o,
vmconfig.o,vmdiska.o,vmpanic.o,vmqueue.o,vmrwindk.o,
vprobe.o,
sdrv,

xlib
                                                                               