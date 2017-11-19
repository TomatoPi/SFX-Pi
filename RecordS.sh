#!/bin/bash
#Script pour lancer le record des ouputs et du port de capture 2

echo Lancement de l enregistrement

./jack_capture --port system:playback_1 --port system:playback_2 --format wav --filename-prefix /mnt/disk/records/solo_ -z 1
