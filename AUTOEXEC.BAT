echo off
rem
rem ***** Set up some alias command names *****
rem
define def = define
def d = dir -w
def ren = rename
def t = type
def sy = systab
def lo = logoff
def pa = path
def pmt = prompt
def help = define helplvl
def ps = process view -e-m-p
rem
rem ***** Set up path and prompt *****
rem
pa default: system:
a:
pmt $p$g 
