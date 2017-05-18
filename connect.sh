#!/bin/bash
echo "Connection du client FX" $1 " au serveur Space_FX"

jack_connect --server Space_FX system:capture_1 $1:in_L
jack_connect --server Space_FX system:capture_1 $1:in_R

jack_connect --server Space_FX $1:out_L system:playback_1
jack_connect --server Space_FX $1:out_R system:playback_2
