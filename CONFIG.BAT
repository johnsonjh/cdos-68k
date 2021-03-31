define con = con:console
define path = default: system: default:/
define order = 68K REL BAT
define switchar = -
define wmanager = system:wmex.68K
define shell = system:command.68K
define home: = boot:
define -s a: = fd0:
define -s b: = fd1:
define -s c: = fd2:
define -s d: = fd3:
define -s protect = ON
define -s helplvl = 4
define -s version = DRI
security -o=rwed -g=re -w=re
define con: = con0:
back boot:logon
;define con: = con1:
;back boot:logon
