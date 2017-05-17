#!/bin/bash
echo "Connection du client FX" $1 " au serveur Space_FX"

jack_connect --server Space_FX $1:out system:playback_1
jack_connect --server Space_FX $1:out system:playback_2
