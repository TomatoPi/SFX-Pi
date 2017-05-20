#!/bin/bash
echo "Connection du client " $1 " au client " $2

jack_connect --server Space_FX $1:out_L $2:in_L
jack_connect --server Space_FX $1:out_R $2:in_R
