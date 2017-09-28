#ifndef DEF_FRONT_PANEL_H
#define DEF_FRONT_PANEL_H

#include "io.h"

#include "../presset/Presset.h"

#include "./menu/main_menu.h"

/*
*   ---------------------------------------------------------------------------
*   ---------------------------------------------------------------------------
*                                   Front panel
*   ---------------------------------------------------------------------------
*   ---------------------------------------------------------------------------
*/

/*
*   -----------------------------------
*            Buttons adresses
*   -----------------------------------
*/
#define HEX_NULL 	0x00	// Used for null flag

#define HEX_UP      0x08     // MCP0 GPIOB 0
#define HEX_DOWN    0x04     // MCP0 GPIOB 1
#define HEX_NEXT    0x02     // MCP0 GPIOB 2
#define HEX_PREV    0x40    // MCP0 GPIOA 3
#define HEX_ADD     0x80    // MCP0 GPIOA 4
#define HEX_DEL     0x01     // MCP0 GPIOB 5
#define HEX_ENTER   0x10    // MCP0 GPIOB 6
#define HEX_ESC     0x20    // MCP0 GPIOB 7
#define HEX_OK      0x40    // MCP0 GPIOB 8
#define HEX_EDIT    0x80    // MCP0 GPIOB 9

#define MASK_ADRRA 0xf0     // mask for select only PREV and ADD values
#define MASK_ADRRB 0xff     // mask for select others buttons values

#define HEX_FOOT_NEXT 0x20 // MCP0 GPIOA 10
#define HEX_FOOT_PREV 0x10 // MCP0 GPIOA 11

#define BUTTON_COUNT 12

/**
 * Fuction called for read registers values
 * @return true if a register ahs changed
 */
bool func_read_reg();

/**
*   Initialise front panel's buttons.
*   Set coresponding ports to inputs and reverse their state
*   @param version current program version
*/
void io_init_frontPanel( string version );
/**
*   Check if a front button has been pushed.
* 	@param graph pointer to process graph
* 	@param pot potentiometers array
*/
void io_update_frontPanel( 	Module_Node_List* graph,
							IO_Potentiometer pot[SPI_POTAR_COUNT] );

#endif
