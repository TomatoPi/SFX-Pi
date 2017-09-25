#include "io.h"

#define NEGMOD(x, y) ( (x) = ( ((x) - 1) < 0 )? (y) - 1 : (x) -1 )
#define POSMOD(x, y) ( (x) = ( ((x) + 1) % (y) ) )
/*  
*   ---------------------------------------------------------------------------
*   ---------------------------------------------------------------------------
*                               MENU GRAPH
*   ---------------------------------------------------------------------------
*   ---------------------------------------------------------------------------
*   Navigation graph inside menus
*   Up and Down for navigate verticaly
*   Next and Prev for change seleted submenu
*   Enter for enter inside a subMenu
*   Add and Del for some specific subMenu
*   Esc for exit current sub menu without saving
*   Ok for exit with saving
*   
*
*   Main Menu : Edit Mode false
*   -> Change Presset                           (Next / Prev)
*
*   -> Select Module                            (Next / Prev) (Enter)
*       -> Change Current Param Bank            (Next / Prev)
*       -> Add / Remove Current Bank            (Add / Del)
*       -> Edit Current Bank                    (Enter)
*           -> Change curent parameter          (Next / Prev) (Enter)
*               ->  Up/Down:    +- 0.01 +- 10
*                   Next/Prev   +- 0.1  +- 100
*                   Add/Del     +- 1    +- 1000
*                   Enter       toggle between values
*               <-                              (Esc / Ok)
*           <-                                  (Esc / Ok)
*       <-                                      (Esc / Ok)
*/

/**
*   Store last buttons states
*/
static bool lastb[BUTTON_COUNT] = {};

const string BACK_SELECT_MODULE = string("tmp/tmp_sel_mod");
const string BACK_EDIT_BANK     = string("tmp/tmp_edit_bank");
const string BACK_EDIT_PARAM    = string("tmp/tmp_edit_param");

static Wait_status  W_STATUS = WAIT_NOTHING;
static Menu_status  M_STATUS = INIT;
static Move_flag    M_FLAG   = MOVE_NONE;

static string VERSION;

static const string PRESSET_NAME = string("Presset-");
static const string PRESSET_PATH = string("/home/sfx_pi/sfx/Files/");
static vector<string> PRESSET_LIST;
static vector<string>::iterator CURRENT_PRESSET = PRESSET_LIST.end();

Module_Node_List* GRAPH = 0;
Module *CURRENT_MODULE = NULL;
Module_List::iterator CURRENT_IDX;
int CURRENT_BANK = -1;
int CURRENT_PARAM = -1;
int MULTIPLIER = 1;

unsigned char last_reg_gpioa = 0;
unsigned char last_reg_gpiob = 0;

void func_update_menu();
void func_exit_menu();

void io_init_frontPanel(Module_Node_List* graph, string version){
    
    MCP0 = new mcp23017( HEX_MCP_0, string("/dev/i2c-1") );
    MCP1 = new mcp23017( HEX_MCP_1, string("/dev/i2c-1") );
    
    // Set buttons ports to output
    MCP0->writeReg( HEX_IODIRA, 0xff);//MASK_ADRRA, 0xff );
    MCP0->writeReg( HEX_IODIRB, 0xff);//MASK_ADRRB, 0xff );
    
    // Invert their state
    MCP0->writeReg( HEX_IPOLA, 0xff);//MASK_ADRRA, 0xff );
    MCP0->writeReg( HEX_IPOLB, 0xff);//MASK_ADRRB, 0xff );
    
    GRAPH = graph;
    CURRENT_IDX = GRAPH->list_.begin();
    
    VERSION = version;
    
    list_files( PRESSET_PATH, &PRESSET_LIST );
}

void io_update_frontPanel(){
    
    //Get MCP's registers
    unsigned char gpioa = 0;
    unsigned char gpiob = 0;
    
    MCP0->readReg( HEX_GPIOA, gpioa );
    MCP0->readReg( HEX_GPIOB, gpiob );
    
    // If one button has changed, registers will differ
    if ( (gpioa & MASK_ADRRA) != (last_reg_gpioa & MASK_ADRRA) || gpiob != last_reg_gpiob ){
        
        /*
        * Search witch button has been presset or released
        * set Move flag to appropriate state and update menu
        */
        if ( !!(gpiob & HEX_UP) != lastb[0] ){
            
            lastb[0] = !!( gpiob & HEX_UP );
            if ( lastb[0] ) M_FLAG = MOVE_UP;
        }
        else if ( !!(gpiob & HEX_DOWN) != lastb[1] ){
            
            lastb[1] = !!( gpiob & HEX_DOWN );
            if ( lastb[1] ) M_FLAG = MOVE_DOWN;
        }
        else if ( !!(gpiob & HEX_NEXT) != lastb[2] ){
            
            lastb[2] = !!( gpiob & HEX_NEXT );
            if ( lastb[2] ) M_FLAG = MOVE_NEXT;
        }
        else if ( !!(gpioa & HEX_PREV) != lastb[3] ){
            
            lastb[3] = !!( gpioa & HEX_PREV );
            if ( lastb[3] ) M_FLAG = MOVE_PREV;
        }
        else if ( !!(gpioa & HEX_ADD) != lastb[4] ){
            
            lastb[4] = !!( gpioa & HEX_ADD );
            if ( lastb[4] ) M_FLAG = MOVE_ADD;
        }
        else if ( !!(gpiob & HEX_DEL) != lastb[5] ){
            
            lastb[5] = !!( gpiob & HEX_DEL );
            if ( lastb[5] ) M_FLAG = MOVE_DEL;
        }
        else if ( !!(gpiob & HEX_ENTER) != lastb[6]){
            
            lastb[6] = !!( gpiob & HEX_ENTER );
            if ( lastb[6] ) M_FLAG = MOVE_ENTER;
        }
        else if ( !!(gpiob & HEX_ESC) != lastb[7] ){
            
            lastb[7] = !!( gpiob & HEX_ESC );
            if ( lastb[7] ) M_FLAG = MOVE_ESC;
        }
        else if ( !!(gpiob & HEX_OK) != lastb[8] ){
            
            lastb[8] = !!( gpiob & HEX_OK );
            if ( lastb[8] ) M_FLAG = MOVE_OK;
        }
        else if ( !!(gpiob & HEX_EDIT) != lastb[9] ){
            
            lastb[9] = !!( gpiob & HEX_EDIT );
            if ( lastb[9] ) M_FLAG = MOVE_ENTER_EDIT;
            else M_FLAG = MOVE_EXIT_EDIT;
        }
        else if ( !!(gpioa & HEX_FOOT_NEXT) != lastb[10] ){
            
            lastb[10] = !!( gpioa & HEX_FOOT_NEXT );
            if ( GRAPH != NULL ) GRAPH->next_bank();
        }
        else if ( !!(gpioa & HEX_FOOT_PREV) != lastb[11] ){
            
            lastb[11] = !!( gpioa & HEX_FOOT_PREV );
            if ( GRAPH != NULL ) GRAPH->prev_bank();
        }
        
        func_update_menu();
    }
    
    last_reg_gpioa = gpioa;
    last_reg_gpiob = gpiob;
}

void func_update_menu(){
    
    // if button is pressed
    if ( M_FLAG != MOVE_NONE ){
        
        char n[64];
        bool d = true;
            
        // If not waiting for confirm and if an order wait beeing processed, process order
        if ( W_STATUS == WAIT_NOTHING ){
            
                
            switch ( M_STATUS ){
                
                /*
                * Main menu, no option selected
                * User just opened menu, esc quit menu, other buttons go to first option
                */
                case MAIN_MENU :
                
                    // Esc for exit main menu
                    if ( M_FLAG == MOVE_ESC ){
                        
                        d = false;
                        func_exit_menu();
                    }
                    // Any button for enter
                    else{
                        
                        M_STATUS = MAIN_CHANGE_PRESSET;
                        sprintf( n, "Sel Preset" );
                    }
                    break;
                    
                /* 
                * Change preset submenu
                * Change current presset with next and prev, change menu with up and down
                * Add/Del : add new presset or remove current one
                * Enter : reload current presset
                * Quit main menu with esc
                */
                case MAIN_CHANGE_PRESSET :
                
                    // Up and down : change current menu, go to select module
                    if ( M_FLAG == MOVE_UP || M_FLAG == MOVE_DOWN ){
                        
                        CURRENT_MODULE = NULL;
                        CURRENT_BANK = -1;
                        CURRENT_PARAM = -1;
                        
                        M_STATUS = MAIN_SELECT_MODULE;
                        sprintf( n, "Sel Module" );
                    }
                    // Next : try go to next presset, if none found go back to presset 0
                    else if ( M_FLAG == MOVE_NEXT ){
                        
                        // If presset list empty try reaload it
                        if ( PRESSET_LIST.size() == 0 ){
                            
                            // If no presset found print error
                            if ( list_files( PRESSET_PATH, &PRESSET_LIST ) ){
                                
                                sprintf( n, "ErrnoPreset" );
                            }
                        }
                        // Navigate througt list if not empty
                        if ( PRESSET_LIST.size() != 0 ){
                            
                            CURRENT_MODULE = NULL;
                            CURRENT_BANK = -1;
                            CURRENT_PARAM = -1;
                        
                            // If list end reached, go back to begin
                            if ( CURRENT_PRESSET == PRESSET_LIST.end() ){
                                
                                CURRENT_PRESSET = PRESSET_LIST.begin();
                            }
                            else{
                                
                                if ( ++CURRENT_PRESSET == PRESSET_LIST.end() ){
                                    
                                    CURRENT_PRESSET = PRESSET_LIST.begin();
                                }
                            }
                            
                            // Try load presset
                            if ( load_preset( *(CURRENT_PRESSET), VERSION, GRAPH ) ){
                                
                                sprintf( n, "Err Preset" );
                            }
                            else{
                                
                                string p = string(*CURRENT_PRESSET);
                                p.erase( p.end() - 4, p.end() );
                                sprintf( n, "%s", p.c_str() );
                            }
                        }
                    }
                    // Prev : try go to previous presset, or bo back to last presset
                    else if ( M_FLAG == MOVE_PREV ){
                        
                        // If presset list empty try reaload it
                        if ( PRESSET_LIST.size() == 0 ){
                            
                            // If no presset found print error
                            if ( list_files( PRESSET_PATH, &PRESSET_LIST ) ){
                                
                                sprintf( n, "ErrnoPreset" );
                            }
                        }
                        // Navigate througt list if not empty
                        if ( PRESSET_LIST.size() != 0 ){
                            
                            CURRENT_MODULE = NULL;
                            CURRENT_BANK = -1;
                            CURRENT_PARAM = -1;
                            
                            // If list begin reached, go back to end
                            if ( CURRENT_PRESSET == PRESSET_LIST.begin() ){
                                
                                CURRENT_PRESSET = PRESSET_LIST.end() - 1;
                            }
                            else{
                                
                                --CURRENT_PRESSET;
                            }
                            
                            // Try load presset
                            if ( load_preset( *(CURRENT_PRESSET), VERSION, GRAPH ) ){
                                
                                sprintf( n, "Err Preset" );
                            }
                            else{
                                
                                string p = string(*CURRENT_PRESSET);
                                p.erase( p.end() - 4, p.end() );
                                sprintf( n, "%s", p.c_str() );
                            }
                        }
                    }
                    // Add add new preset
                    else if ( M_FLAG == MOVE_ADD ){
                        
                        // Try create a new presset
                        string p = PRESSET_NAME + to_string( PRESSET_LIST.size() ) + string(".txt");
                        if ( new_preset( p, VERSION ) ){
                            
                            sprintf( n, "Err create" );
                        }
                        // On success go to this presset and reload presset list
                        else{
                            
                            CURRENT_MODULE = NULL;
                            CURRENT_BANK = -1;
                            CURRENT_PARAM = -1;
                            
                            // Try load new preset
                            if ( load_preset( p, VERSION, GRAPH ) ){
                                
                                sprintf( n, "Err Load" );
                            }
                            // On success go to this preset and reload list
                            else{
                                
                                list_files( PRESSET_PATH, &PRESSET_LIST );
                                vector<string>::iterator itr = find( PRESSET_LIST.begin(), PRESSET_LIST.end(), p );
                                if ( itr != PRESSET_LIST.end() ){
                                    
                                    CURRENT_PRESSET = itr;
                                    string s = string(*CURRENT_PRESSET);
                                    s.erase( s.end() - 4, s.end() );
                                    sprintf( n, "%s", s.c_str() );
                                }
                                else{
                                    
                                    sprintf( n, "Err Find" );
                                }
                            }
                        }
                    }
                    // Del : delete current preset if selected
                    else if ( M_FLAG == MOVE_DEL ){
                        
                        // If a presset is selected
                        if ( CURRENT_PRESSET != PRESSET_LIST.end() ){
                            
                            // Ask for confirm for delete it
                            W_STATUS = WAIT_DEL_PRESET;
                            sprintf( n, "Confirm Y/N");
                        }
                        // Else ask select presset
                        else{
                            
                            sprintf( n, "Sel preset");
                        }
                    }
                    // Enter : reload current preset
                    else if ( M_FLAG == MOVE_ENTER ){
                        
                        // If no preset selected
                        if ( CURRENT_PRESSET == PRESSET_LIST.end() ){
                            
                            sprintf( n, "Sel Preset");
                        }
                        // else Try reload current presset
                        else if ( load_preset( *(CURRENT_PRESSET), VERSION, GRAPH ) ){
                            
                            sprintf( n, "Err Preset");
                        }
                        // If reload successful
                        else {
                            
                            sprintf( n, "Reload ok");
                        }
                    }
                    // Esc : exit main menu
                    else if ( M_FLAG == MOVE_ESC ){
                        
                        d = false;
                        func_exit_menu();
                    }
                    // Other buttons do nothing
                    else{
                        
                        d = false;
                    }
                    break;
                    
                /*
                * Select module submenu
                * Change menu with up and down
                * Change selected module with next and prev
                * If current module is non null Enter go to module edition menu
                * If current module is null esc quit menu, set current module to null otherwise
                */
                case MAIN_SELECT_MODULE :
                
                    // Up and Down : change current menu, go to Change presset
                    if ( M_FLAG == MOVE_UP || M_FLAG == MOVE_DOWN ){
                        
                        CURRENT_MODULE = NULL;
                        CURRENT_BANK = -1;
                        CURRENT_PARAM = -1;
                        
                        M_STATUS = MAIN_CHANGE_PRESSET;
                        
                        if ( CURRENT_PRESSET == PRESSET_LIST.end() ) sprintf( n, "Sel Preset" );
                        else{

                            string p = string(*CURRENT_PRESSET);
                            p.erase( p.end() - 4, p.end() );
                            sprintf( n, "%s", p.c_str() );
                        }
                    }
                    /*
                    * Next : try go to next module, if none found go back to first module
                    * Prev : try go to previous module, if none found go to last one
                    */
                    else if ( M_FLAG == MOVE_NEXT || M_FLAG == MOVE_PREV ){
                        
                        // Check if graph is not empty
                        if ( GRAPH->list_.size() != 0 ){
                            
                            // If no module is selected get first or last one
                            if ( CURRENT_MODULE == NULL ){
                                
                                if ( M_FLAG == MOVE_NEXT ){
                                    
                                    CURRENT_MODULE = (*( CURRENT_IDX = GRAPH->list_.begin() ))->get_module();
                                }
                                else{
                                    
                                    CURRENT_MODULE = (*( CURRENT_IDX = GRAPH->list_.end() -1))->get_module();
                                }
                            }
                            // Else Increment current position 
                            else{
                                
                                if ( M_FLAG == MOVE_NEXT ){
                                    
                                    // Increment current position or go back to first if no next
                                    if( ++CURRENT_IDX == GRAPH->list_.end() ){
                                        
                                        CURRENT_IDX = GRAPH->list_.begin();
                                    }
                                }
                                else{
                                    
                                    // Decrement current position or go back to last if no prev
                                    if( CURRENT_IDX == GRAPH->list_.begin() ){
                                        
                                        CURRENT_IDX = GRAPH->list_.end() -1;
                                    }else{
                                        
                                        --CURRENT_IDX;
                                    }
                                }
                                
                                CURRENT_MODULE = (*CURRENT_IDX)->get_module();
                            }
                            
                            sprintf( n, CURRENT_MODULE->get_name() );
                        }
                        // If graph is empty return an error
                        else{
                            
                            sprintf(n, "Err Graph");
                        }
                    }
                    // Enter : go to module edition module if a module is selected, do nothing if not
                    else if ( M_FLAG == MOVE_ENTER ){
                        
                        if ( CURRENT_MODULE != NULL ){
                            
                            CURRENT_BANK = -1;
                            CURRENT_PARAM = -1;
                            
                            M_STATUS = SELMOD;
                            sprintf( n, "Edit Params");
                        }
                        else{
                            
                            d = false;
                        }
                    }
                    // Esc : if a module is selected, unselect it, exit menu otherwise
                    else if ( M_FLAG == MOVE_ESC ){
                        
                        if ( CURRENT_MODULE != NULL ){
                            
                            CURRENT_MODULE = NULL;
                            CURRENT_BANK = -1;
                            CURRENT_PARAM = -1;
                            
                            M_STATUS = MAIN_SELECT_MODULE;
                            sprintf( n, "Sel Module" );
                        }
                        else{
                            
                            d = false;
                            func_exit_menu();
                        }
                    }
                    // Other buttons do nothing
                    else{
                        
                        d = false;
                    }
                    break;
                
                /*
                * Module 's param edition submenu
                * Press any button for enter 
                * Esc for go back to main menu
                */
                case SELMOD :
                    
                    // Esc for exit
                    if ( M_FLAG == MOVE_ESC ){
                        
                        CURRENT_MODULE = NULL;
                        CURRENT_BANK = -1;
                        CURRENT_PARAM = -1;
                        
                        M_STATUS = MAIN_MENU;
                        sprintf( n, "Main Menu" );
                    }
                    // Any button for enter, backup current module
                    else{
                        
                        if ( save_module( BACK_SELECT_MODULE, VERSION, CURRENT_MODULE) ){
                            
                            sprintf( n, "wrn nobakup" );
                        }
                        else{
                            
                            sprintf( n, "Sel Bank" );
                        }
                        
                        M_STATUS = SELMOD_CHANGE_BANK;
                    }
                    break;
                    
                /*
                * Module selected : Change current bank
                * Up and Down navigate throught module edition submenu
                * Next and prev change current bank if avaiable
                * Esc ask for exit edition without saving
                * Ok exit edition menu with saving
                */
                case SELMOD_CHANGE_BANK :
                
                    // Up : go to add bank submenu
                    if ( M_FLAG == MOVE_UP ){
                        
                        M_STATUS = SELMOD_ADD_BANK;
                        sprintf( n, "Add/DelBank" );
                    }
                    // Down : go to edit param submenu
                    else if ( M_FLAG == MOVE_DOWN ){
                        
                        CURRENT_PARAM = -1;
                        M_STATUS = SELMOD_EDIT_BANK;
                        sprintf( n, "Edit Param" );
                    }
                    // Next : go to next bank
                    else if ( M_FLAG == MOVE_NEXT ){
                        
                        if ( CURRENT_BANK == -1 ){
                            
                            CURRENT_MODULE->set_bank(0);
                        }
                        else{
                            
                            CURRENT_MODULE->next_bank();
                        }
                        CURRENT_BANK = CURRENT_MODULE->get_bank();
                        sprintf( n, "Bank-%d", CURRENT_BANK);
                    }
                    // Prev : go to prev bank
                    else if ( M_FLAG == MOVE_PREV ){
                        
                        if ( CURRENT_BANK == -1 ){
                            
                            CURRENT_MODULE->set_bank(0);
                        }
                        else{
                           
                            CURRENT_MODULE->prev_bank();
                        }
                        CURRENT_BANK = CURRENT_MODULE->get_bank();
                        sprintf( n, "Bank-%d", CURRENT_BANK);
                    }
                    // Esc : ask for exit without save
                    else if ( M_FLAG == MOVE_ESC ){
                        
                        W_STATUS = WAIT_EXIT_SELECTMOD;
                        sprintf( n, "Cancel Y/N");
                    }
                    // Ok : ask for exit saving changes
                    else if ( M_FLAG == MOVE_OK ){
                        
                        if ( save_preset( *(CURRENT_PRESSET), VERSION, GRAPH ) ){
                        
                            sprintf( n, "Save Fail");
                        
                        }else{
                            
                            sprintf( n, "Save ok" );
                        }
                        
                        M_STATUS = MAIN_MENU;
                        
                        CURRENT_MODULE = NULL;
                        CURRENT_BANK = -1;
                        CURRENT_PARAM = -1;
                    }
                    // Other buttons do nothing
                    else{
                        
                        d = false;
                    }
                    break;
                    
                /*
                * Add or remove a bank from current module
                * Up and down navigate throught module edition submenu
                * Add and remove for add / remove bank
                * Esc ask for exit edition without saving
                * Ok exit edition menu with saving
                */
                case SELMOD_ADD_BANK :
                
                    // Up : go to Edit bank submenu
                    if ( M_FLAG == MOVE_UP ){
                        
                        CURRENT_PARAM = -1;
                        M_STATUS = SELMOD_EDIT_BANK;
                        sprintf( n, "Edit Param" );
                    }
                    // Down : go to change bank submenu
                    else if ( M_FLAG == MOVE_DOWN ){
                        
                        M_STATUS = SELMOD_CHANGE_BANK;
                        if ( CURRENT_BANK == -1 ) sprintf( n, "Sel bank" );
                        else sprintf( n, "Bank-%d", CURRENT_BANK);
                    }
                    // Add : add a new bank to the module
                    else if ( M_FLAG == MOVE_ADD ){
                        
                        CURRENT_MODULE->add_bank();
                        sprintf( n, "Bank added" );
                    }
                    // Del : remove current bank from the module
                    else if ( M_FLAG == MOVE_DEL ){
                        
                        // Remove current bank if selected, tell select bank otherwise
                        if ( CURRENT_BANK != -1 ){
                            
                            W_STATUS = WAIT_DEL_BANK;
                            sprintf( n, "Del B%d y/n", CURRENT_BANK);
                        }
                        else{
                            
                            sprintf( n, "nobank sel");
                        }
                    }
                    // Esc : ask for exit module edition without saving
                    else if ( M_FLAG == MOVE_ESC ){
                        
                        W_STATUS = WAIT_EXIT_SELECTMOD;
                        sprintf( n, "Cancel Y/N");
                    }
                    // Ok : exit module edition saving changes
                    else if ( M_FLAG == MOVE_OK ){
                        
                        if ( save_preset( *(CURRENT_PRESSET), VERSION, GRAPH ) ){
                        
                            sprintf( n, "Save Fail");
                        
                        }else{
                            
                            sprintf( n, "Save ok" );
                        }
                        
                        M_STATUS = MAIN_MENU;
                        
                        CURRENT_MODULE = NULL;
                        CURRENT_BANK = -1;
                        CURRENT_PARAM = -1;
                    }
                    // Other buttons do nothing
                    else{
                        
                        d = false;
                    }
                    break;
                    
                /*
                * Edit bank submenu
                * Up and down navigate throught select mod menu
                * Enter for enter edit bank submenu
                * Esc ask for exit edition without saving
                * Ok exit edition menu with saving
                */
                case SELMOD_EDIT_BANK :
                
                    // Up : go to change current bank submenu
                    if ( M_FLAG == MOVE_UP ){
                        
                        M_STATUS = SELMOD_CHANGE_BANK;
                        if ( CURRENT_BANK == -1 ) sprintf( n, "Sel bank" );
                        else sprintf( n, "Bank-%d", CURRENT_BANK);
                    }
                    // Down : go to add bank submenu
                    else if ( M_FLAG == MOVE_DOWN ){
                        
                        M_STATUS = SELMOD_ADD_BANK;
                        sprintf( n, "Add/DelBank" );
                    }
                    // Enter : Enter submenu
                    else if ( M_FLAG == MOVE_ENTER ){
                        
                        if ( CURRENT_BANK == -1 ){
                            
                            sprintf( n, "No bank sel");
                        }
                        else{
                            
                            if ( save_module( BACK_EDIT_BANK, VERSION, CURRENT_MODULE ) ){
                                
                                sprintf( n, "wrn nobakup" );
                            }
                            else{
                            
                                sprintf( n, "Sel Param" );
                            }
                            
                            M_STATUS = SELMOD_EDITB_SELPAR;
                            CURRENT_PARAM = -1;
                        }
                    }
                    // Esc : ask for exit without save
                    else if ( M_FLAG == MOVE_ESC ){
                        
                        W_STATUS = WAIT_EXIT_SELECTMOD;
                        sprintf( n, "Cancel Y/N");
                    }
                    // Ok : exit saving changes
                    else if ( M_FLAG == MOVE_OK ){
                        
                        if ( save_preset( *(CURRENT_PRESSET), VERSION, GRAPH ) ){
                        
                            sprintf( n, "Save Fail");
                        
                        }else{
                            
                            sprintf( n, "Save ok" );
                        }
                        
                        M_STATUS = MAIN_MENU;
                        
                        CURRENT_MODULE = NULL;
                        CURRENT_BANK = -1;
                        CURRENT_PARAM = -1;
                    }
                    // Other buttons do nothing
                    else{
                        
                        d = false;
                    }
                    break;
                    
                /*
                * Bank edition submenu
                * Prev and next change selected param
                * Enter edit selected param, do nothing if none
                * Esc ask for exit bank edition without save
                * Ok exit bank edition with saving
                */
                case SELMOD_EDITB_SELPAR :
                
                    // Next : select next param or param 0 if none selected
                    if ( M_FLAG == MOVE_NEXT ){
                        
                        // select param 0 if none selected
                        if ( CURRENT_PARAM == -1 ){
                            
                            CURRENT_PARAM = 0;
                        }
                        // Else increment current apram
                        else{
                            
                            POSMOD( CURRENT_PARAM, CURRENT_MODULE->get_param_count() );
                        }
                        
                        sprintf( n, "%s", CURRENT_MODULE->get_formated_param( CURRENT_PARAM ).c_str() );
                    }
                    // Prev : select prev param or param 0 if none selected
                    else if ( M_FLAG == MOVE_PREV ){
                        
                        // select param 0 if none selected
                        if ( CURRENT_PARAM == -1 ){
                            
                            CURRENT_PARAM = 0;
                        }
                        // Else decrement current apram
                        else{
                            
                            NEGMOD( CURRENT_PARAM, CURRENT_MODULE->get_param_count() );
                        }
                        
                        sprintf( n, "%s", CURRENT_MODULE->get_formated_param( CURRENT_PARAM ).c_str() );
                    }
                    // Enter : start param edition or do nothing if no param selected
                    else if ( M_FLAG == MOVE_ENTER ){
                        
                        if ( CURRENT_PARAM != -1 ){
                            
                            if ( save_module( BACK_EDIT_PARAM, VERSION, CURRENT_MODULE ) ){
                                
                                sprintf( n, "Wrn nobakup");
                            }
                            else{
                                
                                sprintf( n, "Edit on" );
                            }
                            
                            M_STATUS = SELMOD_EDITB_EDITPAR;
                        }
                        else{
                            
                            d = false;
                        }
                    }
                    // Esc : ask for exit bank edition without save
                    else if ( M_FLAG == MOVE_ESC ){
                        
                        W_STATUS = WAIT_EXIT_EDITBANK;
                        sprintf( n, "Cancel Y/N");
                    }
                    // Ok : exit saving changes
                    else if ( M_FLAG == MOVE_OK ){
                        
                        if ( save_preset( *(CURRENT_PRESSET), VERSION, GRAPH ) ){
                        
                            sprintf( n, "Save Fail");
                        }
                        else{
                            
                            sprintf( n, "Save ok" );
                        }
                       
                        M_STATUS = SELMOD_EDIT_BANK;
                        CURRENT_PARAM = -1;
                    }
                    // Other buttons do nothing
                    else{
                        
                        d = false;
                    }
                    break;
                    
                /*
                * Param edition menu
                * Up and down   +- 1    or +- 1000
                * Next and prev +- 0.1  or +- 100
                * Add and del   +- 0.01 or +- 10
                * Enter switch between first and second values
                * Esc ask for exit param without save
                * Ok exit param edition with saving
                */
                case SELMOD_EDITB_EDITPAR :
                
                    // Up : +1 / +1000
                    if ( M_FLAG == MOVE_UP ){
                        
                        CURRENT_MODULE->set_param( 
                                CURRENT_PARAM, 
                                CURRENT_MODULE->get_param( CURRENT_PARAM ) + 1 * MULTIPLIER 
                                );
                        sprintf( n, "%s", CURRENT_MODULE->get_formated_param( CURRENT_PARAM ).c_str() );
                    }
                    // Down : -1 / -1000
                    else if ( M_FLAG == MOVE_DOWN ){
                        
                        CURRENT_MODULE->set_param( 
                                CURRENT_PARAM, 
                                CURRENT_MODULE->get_param( CURRENT_PARAM ) - 1 * MULTIPLIER 
                                );
                        sprintf( n, "%s", CURRENT_MODULE->get_formated_param( CURRENT_PARAM ).c_str() );
                    }
                    // Next : +0.1f / +100
                    else if ( M_FLAG == MOVE_NEXT ){
                        
                        CURRENT_MODULE->set_param( 
                                CURRENT_PARAM, 
                                CURRENT_MODULE->get_param( CURRENT_PARAM ) + 0.1f * MULTIPLIER 
                                );
                        sprintf( n, "%s", CURRENT_MODULE->get_formated_param( CURRENT_PARAM ).c_str() );
                    }
                    // Prev : -0.1f / -100
                    else if ( M_FLAG == MOVE_PREV ){
                        
                        CURRENT_MODULE->set_param( 
                                CURRENT_PARAM, 
                                CURRENT_MODULE->get_param( CURRENT_PARAM ) - 0.1f * MULTIPLIER 
                                );
                        sprintf( n, "%s", CURRENT_MODULE->get_formated_param( CURRENT_PARAM ).c_str() );
                    }
                    // Add : +0.01f / +10
                    else if ( M_FLAG == MOVE_ADD ){
                        
                        CURRENT_MODULE->set_param( 
                                CURRENT_PARAM, 
                                CURRENT_MODULE->get_param( CURRENT_PARAM ) + 0.01f * MULTIPLIER 
                                );
                        sprintf( n, "%s", CURRENT_MODULE->get_formated_param( CURRENT_PARAM ).c_str() );
                    }
                    // Del : -0.01f / -10
                    else if ( M_FLAG == MOVE_DEL ){
                        
                        CURRENT_MODULE->set_param( 
                                CURRENT_PARAM, 
                                CURRENT_MODULE->get_param( CURRENT_PARAM ) - 0.01f * MULTIPLIER 
                                );
                        sprintf( n, "%s", CURRENT_MODULE->get_formated_param( CURRENT_PARAM ).c_str() );
                    }
                    // Enter : switch values
                    else if ( M_FLAG == MOVE_ENTER ){
                        
                        if ( MULTIPLIER == 1 ){
                            
                            MULTIPLIER = 1000;
                            sprintf( n, "+1k 100 10" );
                        }else{
                            
                            MULTIPLIER = 1;
                            sprintf( n, "+f01 0f1 1f" );
                        }
                    }
                    // Esc : ask for exit bank edition without save
                    else if ( M_FLAG == MOVE_ESC ){
                        
                        W_STATUS = WAIT_EXIT_PARAM;
                        sprintf( n, "Cancel Y/N");
                    }
                    // Ok : exit saving changes
                    else if ( M_FLAG == MOVE_OK ){
                        
                        if ( save_preset( *(CURRENT_PRESSET), VERSION, GRAPH ) ){
                        
                            sprintf( n, "Save Fail");
                        }
                        else{
                            
                            sprintf( n, "Save ok" );
                        }
                           
                        M_STATUS = SELMOD_EDITB_SELPAR;
                        CURRENT_PARAM = -1;
                    }
                    // Other buttons do nothing
                    else{
                        
                        d = false;
                    }
                    break;
                    
                /*
                * Add new module submenu
                * Up and down for navigate throught edit menu
                * Add for add new module
                * Del for del selected module
                * Esc for exit menu
                */
                case EDIT_ADD_MODULE :
                
                    break;
                    
                /*
                * Choose new module type
                * Up and down for select type
                * Esc for cancel
                */
                case EDIT_CHOOSE_MODTYPE :
                
                    break;
                    
                /*
                * Select module submenu
                * Up and down for navigate throught edit menu
                * Next and prev for change selected menu
                * Enter for enter module management submenu
                * Esc deselect current module or leave menu
                */
                case EDIT_SELECT_MODULE :
                
                    break;
                    
                /*
                * Edit mod submenu
                * Any button for enter
                * Esc for cancel
                */
                case EDITMOD :
                
                    break;
                    
                /*
                * Manage conncetion submenu
                * Next and prev for select a connection to remove
                * Add for add a connection
                * Del for remove selected connection
                * Esc for deselect current connection or ask for exit submenu without save
                * Ok for exit with save
                */
                case EDITMOD_ADD_CONNECTION :
                
                    break;
                    
                // default is reached once when menu status is init
                default :
                    
                    M_STATUS = MAIN_MENU;
                    CURRENT_PRESSET = PRESSET_LIST.end();
                    CURRENT_MODULE = NULL;
                    CURRENT_BANK = -1;
                    CURRENT_PARAM = -1;
                    sprintf( n, "Main Menu" );
                    break;
            } // switch end
        } // wait nothing end
        // Else compute waiting order
        else{
            
            // Only ok or esc buttons work here
            if ( M_FLAG == MOVE_OK || M_FLAG == MOVE_ESC ){
                
                // c true if OK button pressed
                bool c = M_FLAG == MOVE_OK;
                
                switch ( W_STATUS ){
                    
                    /*
                    * Waiting for confirm delete current bank
                    */
                    case WAIT_DEL_BANK :
                    
                        // Delete current bank
                        if ( c ){
                            
                        }
                        else{
                            
                        }
                        break;
                        
                    /*
                    * Waiting for confirm delete current preset
                    */
                    case WAIT_DEL_PRESET :
                    
                        // Delete current preset
                        if ( c ){
                            
                        }
                        else{
                            
                        }
                        break;
                        
                    /*
                    * Waiting for exit module's param edition without save
                    */
                    case WAIT_EXIT_SELECTMOD :
                    
                        // Exit module's param edition submenu
                        if ( c ){
                            
                        }
                        else{
                            
                        }
                        break;
                        
                    /*
                    * Waiting for exit bank edition without saving
                    */
                    case WAIT_EXIT_EDITBANK :
                    
                        // Exit bank edition submenu
                        if ( c ){
                            
                        }
                        else{
                            
                        }
                        break;
                        
                    /*
                    * Waiting for exit param edition without saving
                    */
                    case WAIT_EXIT_PARAM :
                    
                        // Exit param edition submenu
                        if ( c ){
                            
                        }
                        else{
                            
                        }
                        break;
                    
                    /*
                    * Shouldn't be reach
                    */
                    default :
                    
                        d = false;
                        func_exit_menu();
                        break;
                } // switch end
                
                W_STATUS = WAIT_NOTHING;
            } // move ok or esc end
            // Other buttons do nothing
            else{
                
                d = false;
            }
        } // compute wait order end
        
        if ( d ) IOS::printm( string(n) , IOS::TEMP);
    } // Move != none end
    
    M_FLAG = MOVE_NONE;
}

// Exit main menu
void func_exit_menu(){
    
    M_STATUS = INIT;
    CURRENT_PRESSET = PRESSET_LIST.end();
    CURRENT_MODULE = NULL;
    CURRENT_BANK = -1;
    CURRENT_PARAM = -1;
    
    char n[] = "space-fx";
    IOS::printm( string(n) );
}