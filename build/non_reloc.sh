#!/bin/bash

TARGET=$1
DXFILE=DxFloLdScript.txt

saddr=`readelf -s $TARGET | grep stext | awk '{print $2}'`
eaddr=`readelf -s $TARGET | grep etext | awk '{print $2}'`
raddr=`readelf -r $TARGET | grep R_ARM_RELATIVE | head -n1 | awk '{print $1}'`
saddr10=`echo $((0x$saddr))`
eaddr10=`echo $((0x$eaddr))`
raddr10=`echo $((0x$raddr))`
if [ $raddr10 -gt $eaddr10 ]; then
	enraddr10=`expr $eaddr10 - $saddr10`
else
	enraddr10=`expr $raddr10 - $saddr10`
fi
enraddr=`printf "%x" $enraddr10`

echo stext addr: 0x$saddr = $saddr10
echo etext addr: 0x$eaddr = $eaddr10
echo reloc addr: 0x$raddr = $raddr10
echo etext_non_reloc: 0x$enraddr = $enraddr10


echo "stext = ADDR(.text);" > $DXFILE
echo "SECTIONS {" >> $DXFILE
echo " 	 etext_non_reloc = stext + 0x$enraddr;" >> $DXFILE
echo "}" >> $DXFILE



