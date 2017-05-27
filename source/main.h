#ifndef DEF_MAIN_H
#define DEF_MAIN_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <bits/stdc++.h>
using namespace std;

#include <jack/jack.h>

#include "./modules/Drive.h"
#include "./modules/LFO.h"
#include "./modules/Ringmod.h"
#include "./modules/Delay.h"

int main_add_module(MODULE_TYPE mod);

int main_add_connection(Module *source, int is, Module *destination, int id);

#endif
