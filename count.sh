#!/bin/bash
echo Lignes
find src/ -type f | xargs cat | wc -l
echo Fichiers
find src/ -type f | wc -l
