#!/bin/bash
echo Lignes
find source/ -type f | xargs cat | wc -l
echo Fichiers
find source/ -type f | wc -l
