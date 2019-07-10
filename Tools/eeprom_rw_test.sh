#!/bin/sh

# Enable eeprom in I2C1
eeprom_init()
{
gpio c 2 1 | exec 2>/dev/null
gpio s 2 0 | exec 2>/dev/null
echo "Set GPIO to connect eeprom" >> /tmp/dbg.eeprom.rw.record
rm -rf /tmp/eeprom.*
echo "Clear temporary files..." >> /tmp/dbg.eeprom.rw.record
#i2cdetect -y -r 1
#i2cget -f -y 1 $1 $2 | tee /tmp/eeprom.before | exec 2>/dev/null
#i2cset -f -y 1 $1 $2 $3 | echo $3 | tee -a /tmp/eeprom.record 1>&2
}

#Set "0xff" to the requested register
eeprom_clear()
{
for index in $(seq $2 $3)
do
	HEX_INDEX=`printf '%x' $index`
	i2cset -f -y 1 $1 0x$HEX_INDEX 0xff
done
}

#set data to the requested register in different I2C address
eeprom_rw()
{
for index in $(seq $2 $3)
do
        HEX_INDEX=`printf '%x' $index`
        i2cget -f -y 1 $1 0x$HEX_INDEX | tee -a /tmp/eeprom.$1.before | exec 2>/dev/null
        if [ $index -le 15 ]; then
        	i2cset -f -y 1 $1 0x$HEX_INDEX 0x$HEX_INDEX | echo 0x0$HEX_INDEX >> /tmp/eeprom.$1.write
        else
        	i2cset -f -y 1 $1 0x$HEX_INDEX 0x$HEX_INDEX | echo 0x$HEX_INDEX >> /tmp/eeprom.$1.write
        fi
        i2cget -f -y 1 $1 0x$HEX_INDEX | tee -a /tmp/eeprom.$1.after | exec 2>/dev/null
done	
}

#compare the register data before/after eeprom_rw
eeprom_compare()
{
	diff /tmp/eeprom.$1.write /tmp/eeprom.$1.after | tee /tmp/eeprom.result
	if [ -s /tmp/eeprom.result ]; then
	        echo "EEPROM $1 R/W test fail..."
	        exit
	else
	        echo "EEPROM $1 R/W test pass!"
	fi
}

#Test purpose...
eeprom_random()
{
# The number provided by $RANDOM changes in each iteration so each run of the awk program gets a different seed.
	random_num=`printf '%x' $(awk -v seed=$RANDOM -v min=0 -v max=255 'BEGIN{srand(seed); print int(min+rand()*(max-min+1))}')`
	echo $random_num
}

#set the sequential data(0x0-0xff) to all registers(0x0-0xff) in different I2C address(0x50-0x57)
eeprom_full_rw()
{
for address in $(seq 50 57)
do
                HEX_ADDRESS="0x$address"
                eeprom_clear $HEX_ADDRESS 0 255
                eeprom_rw $HEX_ADDRESS 0 255
                eeprom_compare $HEX_ADDRESS
                
done
}

#set a random data to the random register(data value=register value 0x00-0xff) in different I2C address(0x50-0x57)
eeprom_random_rw()
{
for address in $(seq 50 57)
do
	random_num=$(awk -v seed=$RANDOM -v min=0 -v max=255 'BEGIN{srand(seed); print int(min+rand()*(max-min+1))}')
	HEX_RANDOM_NUM=`printf '%x' $random_num`
	echo $address $HEX_RANDOM_NUM
	HEX_ADDRESS="0x$address"
	eeprom_clear $HEX_ADDRESS $random_num $random_num
	eeprom_rw $HEX_ADDRESS $random_num $random_num
	eeprom_compare $HEX_ADDRESS
done
}

eeprom_usage()
{
	echo "=================EEPROM R/W Test menu================="
	echo " * 1: Random R/W Test (-s)"
	echo " * 2: Full R/W Test   (-f)"
	echo " * 3: Read EEPROM     (-r [I2C address] [Reg])"
	echo " * 4: Write EEPROM    (-w [I2C address] [Reg] [Data])"
	echo " * 5: Dump EEPROM     (-d [I2C address])"
	echo " * 6: List I2C device (-c [I2C Bus_Num])"
	echo " * 7: Usage           (-h)"
}

eeprom_init

while getopts "sfr:w:d:c:h?" opt
do
    case $opt in
    s)  eeprom_random_rw 
    	exit
    	;; 
    f)  eeprom_full_rw 
    	exit 
    	;; 
    r)	echo "Read Address=$2 Reg=$3 Data=$(i2cget -f -y 1 $2 $3)"
    	exit
    	;;
    w)  echo "Write-$2 Reg-$3 Data-$4"
    	i2cset -f -y 1 $2 $3 $4
    	exit
    	;;
    d)  i2cdump -f -y 1 $2
        exit
        ;;
    c)  i2cdetect -y -r $2
    	exit
    	;;
    h|?)  eeprom_usage 
    	exit 
    	;;
    esac
done
