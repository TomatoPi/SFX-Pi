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
#include "./modules/Tonestack.h"

#include "./io/io.h"

int main_add_module(MODULE_TYPE mod);
int main_del_module(int idx);

char** get_ports_names(Module *source, int is, Module *target, int id);
int main_add_connection(Module *source, int is, Module *target, int id);
int main_del_connection(Module *source, int is, Module *target, int id);

int main_add_accessor(int target, int param_idx, int potentiometer, float min, float max, int is_db, int is_inv);
int main_del_accessor(int idx);

#endif
