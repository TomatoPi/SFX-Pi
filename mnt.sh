#!/bin/bash
#Liste des dossiers où faire un script directement executable
#echo $PATH

argOK=''
ports=('' '')
destDir=''

# Verification du premier argument
if [ -z $1 ] || ([ $1 != "in" ] && [ $1 != "out" ])
then
	argOK="[err : Invalid First Arg]"
fi

# Verification du second argument
if [ -z $2 ]
then
	# Montage du disque par défault
	if [ -e "/mnt/disk/" ]
	then
		destDir='/mnt/disk/records/'
		echo "Utilisation de la destination par default : ${destDir}"
	else
		echo "Montage du disque externe requis ..."
		su
		argOK= mount -t ntfs /dev/sda1 /mnt/disk/
		exit
	fi
else
	# Verification de la validité du dossier indiqué
	if [ -e $destDir ]
	then
		destDir=$2
	else
		argOK=$argOK + "[err : Invalid Destination Dir]"
	fi
fi

# Verification que tout est Bon
if [ -z $argOK ]
then
	#Recuperation des noms des ports
	if [ $1 == 'in' ]
	then
		ports[0]='system:capture_1'
		ports[1]='system:capture_2'
	else
		ports[0]='system:playback_1'
		ports[1]='system:playback_2'
	fi

	echo Lancement de l enregistrement
	#echo "jack_capture --port ${ports[0]} --port ${ports[1]} --format wav --filename-prefix $destDir record_ -z 1"
	jack_capture --port ${ports[0]} --port ${ports[1]} --format wav --filename-prefix ${destDir}record_ -z 1

else
	echo "Usage : sfx_record in|out [Destination_Dir]"
fi
