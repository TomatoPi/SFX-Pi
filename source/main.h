#ifndef DEF_MAIN_H
#define DEF_MAIN_H

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <bits/stdc++.h>
#include <vector>
using namespace std;

#include <jack/jack.h>

#include "./modules/Drive.h"
#include "./modules/LFO.h"
#include "./modules/Ringmod.h"
#include "./modules/Delay.h"
#include "./modules/Tonestack.h"
#include "./modules/Reverb.h"

#include "./io/io.h"

#include "./presset/Presset.h"

/*
*	Main function used to create new module
*/
int main_add_module(MODULE_TYPE mod, int v);

/*
*	Remove module at idx
*/
int main_del_module(int idx);

/*
*	Return the name of source's port number is, or System Capture port [is] if source is NULL
*/
const char* get_source_name(Module *source, int v, int is, Module *target);

/*
*	Return the name of target's port number is, or System Playback port [is] if target is NULL
*/
const char* get_target_name(Module *target, int v, int id, Module *source);

/*
*	Connect module[source_idx]'s port[is] to module[target_idx]'s port[id]
*	source_idx = -1 for System_Capture ports
*	target_idx = -1 for System_Playback ports
*/
int main_add_connection(short source_idx, short source_v, short is, short target_idx, short target_v, short id);
int main_del_connection(short source_idx, short source_v, short is, short target_idx, short target_v, short id);

/*
*	Create a link between a potentiometer and param[param_idx] of module[target]
*	See io/io.h --> IO_Accessor
*/
int main_add_accessor(int target, int target_voice, int param_idx, int potentiometer, float min, float max, IO_CURVE curve, int is_db, int is_inv);

#endif