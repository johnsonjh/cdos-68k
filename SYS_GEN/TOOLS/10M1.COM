[sym,abs,text[b00]] cdos10m1.68k =

dispa10.obj,mmu10m1.obj,ommu10.obj,ommua10.obj,

abort.o,aflag.o,allocate.o,async.o,asynca.o,base.o,coffload.o,command.o,
cpmload.o,crman.o,crmconv8.o,crmcopy.o,crmcreat.o,crmdel.o,crmgsl.o,
crminst.o,crmkb.o,crmmouse.o,crmopen.o,crmrdel.o,crmread.o,crmwind.o,
crmwrite.o,crmxlat.o,dinit.o,dvrif.o,except.o,fe1.o,fe2.o,fe68.o,
feinit.o,femain.o,feu1.o,feu2.o,fpool.o,fprintf.o,free.o,
fscache.o,fsdm.o,fsname.o,fspath.o,fsvol.o,fswindow.o,
heap.o,hook.o,install.o,kfuncs.o,ktools.o,load68k.o,
lutils.o,m68.o,mgetblk.o,misman.o,nulldev.o,ofile.o,osmem.o,overlay.o,
panic68.o,pewter.o,pipe.o,pmget.o,process.o,rdelim.o,rtmif.o,sup.o,
sup68k.o,superman.o,swi.o,swi68k.o,tables.o,tools.o,

vcdrcopy.o,vcdrtool.o,vcdrv.o,vcdrvwrt.o,ibmtovm.o,vm400.o,vmclk.o,
vmconasm.o,vmconfig.o,vmdiska.o,vmkb.o,vmpanic.o,vmqueue.o,vmrwindk.o,
vprobe.o,vt52.o,sdrv.o,

xlib
m.o,vm400.o,vmclk.o,
vmconasm.o,vmconfig.o,vmdiska.o,vmkb.o,vmpanic.o,vmqueue.o,vmrwindk.o,
vprobe.o,vt52.o,sd