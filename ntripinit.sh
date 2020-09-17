count=`ps -ef | grep sendNema2.py | wc -l`

if [ $count -lt 2 ]
then
	python /home/pi/pi_ublox-ntrip/sendNema2.py &
fi

count=`ps -ef |grep ntrip_client | wc -l`
if [ $count -lt 2 ]
then 
	/home/pi/pi_ublox-ntrip/ntrip_client &
fi

count=`ps -ef | grep pppd | wc -l`
if [ $count -lt 2 ]
then 
	sudo /home/pi/quectel-pppd.sh /dev/ttyUSB3 ctnet ctnet@mycdma.cn vnet.mobi
fi

