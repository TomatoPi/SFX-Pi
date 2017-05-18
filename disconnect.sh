#!/bin/bash
echo "Deconnection du client FX" $1 " au serveur Space_FX"

jack_disconnect --server Space_FX system:capture_1 $1:in_L
jack_disconnect --server Space_FX system:capture_1 $1:in_R

jack_disconnect --server Space_FX $1:out_L system:playback_1
jack_disconnect --server Space_FX $1:out_R system:playback_2
