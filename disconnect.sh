#!/bin/bash
echo "Deconnection du client FX" $1 " au serveur Space_FX"

jack_disconnect --server Space_FX system:capture_1 $1:in_l
jack_disconnect --server Space_FX system:capture_1 $1:in_r

jack_disconnect --server Space_FX $1:out_l system:playback_1
jack_disconnect --server Space_FX $1:out_r system:playback_2
