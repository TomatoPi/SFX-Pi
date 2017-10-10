#ifndef DEF_FRONT_PANEL_H
#define DEF_FRONT_PANEL_H

#include "mcp23017.h"

#include "../core/ModulesGraph.h"

#include "./menu/Menu.h"

#include "Button.h"
#include "Potar.h"
#include "io.h"

/**
*   Initialise front panel's buttons.
*   Set coresponding ports to inputs and reverse their state
*   @param version current program version
*/
int io_init_frontPanel();

/**
*   Check if a front button has been pushed.
*   @param graph pointer to process graph
*   @param pot potentiometers array
*/
void io_update_frontPanel();

#endif
