#!/bin/bash
echo "Deconnection du client FX" $1 " au serveur Space_FX"

jack_disconnect --server Space_FX $1:out system:playback_1
jack_disconnect --server Space_FX $1:out system:playback_2