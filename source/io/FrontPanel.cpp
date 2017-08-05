#include "io.h"

#define NEGMOD(x, y) ( (x) = ( --(x) < 0 )? (y) - 1 : (x) )
#define POSMOD(x, y) ( (x) = ( ++(x) % (y) ) )
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
*   Store curent position inside menu.
*   first number is Main menu pos
*   second number is first submenu pos
*   third is third submenu pos
*/
static int CURRENT_MENU[3] = {};

#define MAINNORSIZE         2
    #define CHANGEPRESSET   0
    #define SELECTMODULE    1
        #define SELECTMODULESIZE    3
            #define CHANGEBANK      0
            #define ADDBANK         1
            #define EDITBANK        2
                #define EDITBANKSIZE    1
                    #define CHANGEPARAM 0

static bool lastb[10] = {};

Module_Node_List *GRAPH = NULL;
Module *CURRENT_MODULE = NULL;
Module_List::iterator CURRENT_IDX;
int CURRENT_BANK = -1;
int CURRENT_PARAM = -1;
bool EDIT_PARAM = false;

typedef enum{
    
    WAIT_DEL_BANK,
    WAIT_EXIT_SELECTMOD,
    WAIT_EXIT_EDITBANK,
    WAIT_EXIT_PARAM,
    DEFAULT
}Wait_Satus;
Wait_Satus W_STATUS = DEFAULT;
bool WAIT_CONFIRM = false;

int MULTIPLIER = 1;

static string VERSION;

static int CURRENT_PRESSET = 0;
static const string PRESSET_NAME = string("Presset-");

const string BACK_SELECT_MODULE = string("tmp_sel_mod");
const string BACK_EDIT_BANK     = string("tmp_edit_bank");
const string BACK_EDIT_PARAM    = string("tmp_edit_param");

void func_update_menu();

void io_init_frontPanel(Module_Node_List *graph, string version){
    
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
    
    CURRENT_MENU[1] = -1;
    CURRENT_MENU[2] = -1;
    
    VERSION = version;
}

void io_update_frontPanel(){
    
    //Get MCP's registers
    
    unsigned char gpioa = 0;
    unsigned char gpiob = 0;
    
    MCP0->readReg( HEX_GPIOA, gpioa );
    MCP0->readReg( HEX_GPIOB, gpiob );
    
    if ( !!(gpiob & HEX_UP) != lastb[0] ){
        
        func_button_up( gpiob & HEX_UP );
    }
    else if ( !!(gpiob & HEX_DOWN) != lastb[1] ){
        
        func_button_down( gpiob & HEX_DOWN );
    }
    else if ( !!(gpiob & HEX_NEXT) != lastb[2] ){
        
        func_button_next( gpiob & HEX_NEXT );
    }
    else if ( !!(gpioa & HEX_PREV) != lastb[3] ){
        
        func_button_prev( gpioa & HEX_PREV );
    }
    else if ( !!(gpioa & HEX_ADD) != lastb[4] ){
        
        func_button_add( gpioa & HEX_ADD );
    }
    else if ( !!(gpiob & HEX_DEL) != lastb[5] ){
        
        func_button_del( gpiob & HEX_DEL );
    }
    else if ( !!(gpiob & HEX_ENTER) != lastb[6]){
        
        func_button_enter( gpiob & HEX_ENTER );
    }
    else if ( !!(gpiob & HEX_ESC) != lastb[7] ){
        
        func_button_esc( gpiob & HEX_ESC );
    }
    else if ( !!(gpiob & HEX_OK) != lastb[8] ){
        
        func_button_ok( gpiob & HEX_OK );
    }
    else if ( !!(gpiob & HEX_EDIT) != lastb[9] ){
        
        func_button_edit( gpiob & HEX_EDIT );
    }
}

void func_button_up(bool s){
    
    lastb[0] = s;
    //cout << "button up : " << s << endl;
    
    bool d = true;
    
    if ( s && !WAIT_CONFIRM ){
       
        if ( CURRENT_MENU[0] == CHANGEPRESSET ){
           
            POSMOD( CURRENT_MENU[0], MAINNORSIZE );
        }
        else if ( CURRENT_MENU[0] == SELECTMODULE){
           
            if ( CURRENT_MENU[1] == -1 ){
               
                POSMOD( CURRENT_MENU[0], MAINNORSIZE );
            }
            else if ( CURRENT_MENU[2] == -1 ){
               
                POSMOD( CURRENT_MENU[1], SELECTMODULESIZE );
            }
            else if ( CURRENT_MENU[1] == EDITBANK && CURRENT_MENU[2] == CHANGEPARAM ){
               
                if( CURRENT_MODULE != NULL && CURRENT_PARAM != -1 && EDIT_PARAM){
                   
                    CURRENT_MODULE->set_param( 
                        CURRENT_PARAM, 
                        CURRENT_MODULE->get_param( CURRENT_PARAM ) + 0.01f * MULTIPLIER 
                        );
                }
            }
        }
        
        if ( d ) func_update_menu();
    }
}

void func_button_down(bool s){
    
    lastb[1] = s;
    //cout << "button down : " << s << endl;
    
    bool d = true;
    
    if ( s && !WAIT_CONFIRM ){
       
        if ( CURRENT_MENU[0] == CHANGEPRESSET ){
           
            NEGMOD( CURRENT_MENU[0], MAINNORSIZE );
        }
        else if ( CURRENT_MENU[0] == SELECTMODULE){
           
            if ( CURRENT_MENU[1] == -1 ){
               
                NEGMOD( CURRENT_MENU[0], MAINNORSIZE );
            }
            else if ( CURRENT_MENU[2] == -1 ){
               
                NEGMOD( CURRENT_MENU[1], SELECTMODULESIZE );
            }
            else if ( CURRENT_MENU[1] == EDITBANK && CURRENT_MENU[2] == CHANGEPARAM ){
               
                if( CURRENT_MODULE != NULL && CURRENT_PARAM != -1 && EDIT_PARAM){
                   
                    CURRENT_MODULE->set_param( 
                        CURRENT_PARAM, 
                        CURRENT_MODULE->get_param( CURRENT_PARAM ) - 0.01f * MULTIPLIER 
                        );
                }
            }
        }
       
       if ( d ) func_update_menu();
    }
}

void func_button_next(bool s){
    
    lastb[2] = s;
    //cout << "button next : " << s << endl;
    
    bool d = true;
    
    if ( s && !WAIT_CONFIRM ){
        
        if ( CURRENT_MENU[0] == CHANGEPRESSET ){
            
            // Try load next presset
            string p = string(PRESSET_NAME);
            if( load_preset( p += to_string(++CURRENT_PRESSET), VERSION, GRAPH ) ){
                
                // If presset load has failed , assume current presset is the last, go to presset 0
                p = string(PRESSET_NAME);
                if( load_preset( p += to_string(CURRENT_PRESSET = 0), VERSION, GRAPH ) ){
                    
                    // If presset 0 not found, no presset are avaiable
                    const char n[] = "Err Presset";
                    MAIN_SCREEN->print( n, 0 );
                    d = false;
                }
            }
            // If presset successfully changed, set current module to null
            if ( d ){
                
                CURRENT_MODULE = NULL;
            }
        }
        else if ( CURRENT_MENU[0] == SELECTMODULE ){
            
            if ( CURRENT_MENU[1] == -1 ){
                
                // If current graph not empty navigate trought it
                if ( GRAPH->list_.size() != 0 ){
                    
                    // If no module selected, get fist one
                    if ( CURRENT_MODULE == NULL ){
                        
                        //cout << "Current module null , get first module : ";
                        CURRENT_MODULE = (*( CURRENT_IDX = GRAPH->list_.begin() ))->get_module();
                        //cout << CURRENT_MODULE->get_name() << endl;
                    }
                    else{
                        
                        // Iterate current position or go back to first if no next
                        if( ++CURRENT_IDX == GRAPH->list_.end() ){
                            
                            CURRENT_IDX = GRAPH->list_.begin();
                            //cout << "Graph ended , go to begin" << endl;
                        }
                        
                        //cout << "Current module not null, get next module : ";
                        CURRENT_MODULE = (*CURRENT_IDX)->get_module();
                        //cout << CURRENT_MODULE->get_name() << endl;
                    }
                }
                else {
                    
                    const char n[] = "No Modules";
                    MAIN_SCREEN->print( n, 0 );
                    d = false;
                }
            }
            else if ( CURRENT_MENU[1] == CHANGEBANK ){
                
                if ( CURRENT_MODULE != NULL ){
                    
                    CURRENT_MODULE->next_bank();
                    CURRENT_BANK = CURRENT_MODULE->get_bank();
                }
                else{
                    
                    const char n[] = "Err Module";
                    MAIN_SCREEN->print( n, 0 );
                    d = false;
                }
            }
            else if ( CURRENT_MENU[1] == EDITBANK ){
                
                if ( CURRENT_MENU[2] != -1 ){
                    
                    if ( CURRENT_MODULE != NULL ){
                        
                        if ( EDIT_PARAM && CURRENT_PARAM != -1){
                            
                            CURRENT_MODULE->set_param( 
                                CURRENT_PARAM, 
                                CURRENT_MODULE->get_param( CURRENT_PARAM ) + 0.1f * MULTIPLIER 
                                );
                        }
                        else if ( CURRENT_PARAM == -1 ){
                            
                            CURRENT_PARAM = 0;
                            //cout << "Current param selected now : " << CURRENT_PARAM << endl;
                        }
                        else{
                            
                            POSMOD( CURRENT_PARAM, CURRENT_MODULE->get_param_count() );
                            //cout << "Current param incremented now : " << CURRENT_PARAM << endl;
                        }
                    }
                    else{
                        
                        const char n[] = "Err Module";
                        MAIN_SCREEN->print( n, 0 );
                        d = false;
                    }
                }
            }
        }
        
        if ( d ) func_update_menu();
    }
    
}

void func_button_prev(bool s){
    
    lastb[3] = s;
    //cout << "button prev : " << s << endl;
        bool d = true;
    
    if ( s && !WAIT_CONFIRM ){
        
        if ( CURRENT_MENU[0] == CHANGEPRESSET ){
            
            // Try load previous presset or 
            string p = string(PRESSET_NAME);
            if ( CURRENT_PRESSET > 0 ){
                
                if ( load_preset( p += to_string(--CURRENT_PRESSET), VERSION, GRAPH ) ){
                    
                    // If presset 0 not found, no presset are avaiable
                    const char n[] = "Err Presset";
                    MAIN_SCREEN->print( n, 0 );
                    d = false;
                }
            }
            else{
                
                d = false;
            }
            // If presset successfully changed, set current module to null
            if ( d ){
                
                CURRENT_MODULE = NULL;
            }
        }
        else if ( CURRENT_MENU[0] == SELECTMODULE ){
            
            if ( CURRENT_MENU[1] == -1 ){
                
                // If current graph not empty navigate trought it
                if ( GRAPH->list_.size() != 0 ){
                    
                    // If no module selected, get fist one
                    if ( CURRENT_MODULE == NULL ){
                        
                        //cout << "Current module null , get first module : ";
                        CURRENT_MODULE = (*( CURRENT_IDX = GRAPH->list_.end() -1))->get_module();
                        //cout << CURRENT_MODULE->get_name() << endl;
                    }
                    else{
                        
                        // Iterate current position or go back to first if no next
                        if( CURRENT_IDX == GRAPH->list_.begin() ){
                            
                            CURRENT_IDX = GRAPH->list_.end() -1;
                            //cout << "Graph ended , go to begin" << endl;
                        }else{
                            
                            --CURRENT_IDX;
                        }
                        
                        //cout << "Current module not null, get next module : ";
                        CURRENT_MODULE = (*CURRENT_IDX)->get_module();
                        //cout << CURRENT_MODULE->get_name() << endl;
                    }
                }
                else {
                    
                    const char n[] = "No Modules";
                    MAIN_SCREEN->print( n, 0 );
                    d = false;
                }
            }
            else if ( CURRENT_MENU[1] == CHANGEBANK ){
                
                if ( CURRENT_MODULE != NULL ){
                    
                    CURRENT_MODULE->prev_bank();
                    CURRENT_BANK = CURRENT_MODULE->get_bank();
                }
                else{
                    
                    const char n[] = "Err Module";
                    MAIN_SCREEN->print( n, 0 );
                    d = false;
                }
            }
            else if ( CURRENT_MENU[1] == EDITBANK ){
                
                if ( CURRENT_MENU[2] != -1 ){
                    
                    if ( CURRENT_MODULE != NULL ){
                        
                        if ( EDIT_PARAM && CURRENT_PARAM != -1 ){
                            
                            CURRENT_MODULE->set_param( 
                                CURRENT_PARAM, 
                                CURRENT_MODULE->get_param( CURRENT_PARAM ) - 0.1f * MULTIPLIER 
                                );
                        }
                        else if ( CURRENT_PARAM != -1 ){
                            
                            CURRENT_PARAM = CURRENT_MODULE->get_param_count() -1;
                            //cout << "Current param selected now : " << CURRENT_PARAM << endl;
                        }
                        else{
                            
                            NEGMOD( CURRENT_PARAM, CURRENT_MODULE->get_param_count() ); 
                            //cout << "Current param decremented now : " << CURRENT_PARAM << endl;
                        }
                    }
                    else{
                        
                        const char n[] = "Err Module";
                        MAIN_SCREEN->print( n, 0 );
                        d = false;
                    }
                }
            }
        }
        
        if ( d ) func_update_menu();
    }
}

void func_button_add(bool s){
    
    lastb[4] = s;
    //cout << "button add : " << s << endl;
    
    bool d = true;
    
    if ( s && !WAIT_CONFIRM ){
        
        if ( CURRENT_MENU[0] ==  SELECTMODULE ){
            
            if ( CURRENT_MENU[1] == ADDBANK && CURRENT_MODULE != NULL ){
                
                CURRENT_MODULE->add_bank();
            }
            else if ( CURRENT_MENU[1] == EDITBANK ){
                
                if ( CURRENT_MENU[2] == CHANGEPARAM && EDIT_PARAM ){
                    
                    CURRENT_MODULE->set_param( 
                                CURRENT_PARAM, 
                                CURRENT_MODULE->get_param( CURRENT_PARAM ) + 1.0f * MULTIPLIER 
                                );
                }
                else{
                    
                    d = false;
                }
            }
            else{
                
                d = false;
            }
        }
        else{
            
            d = false;
        }
        
        if ( d ) func_update_menu();
    }
}

void func_button_del(bool s){
    
    lastb[5] = s;
    //cout << "button del: " << s << endl;
    
    bool d = true;
    
    if ( s && !WAIT_CONFIRM ){
        
        if ( CURRENT_MENU[0] ==  SELECTMODULE ){
            
            if ( CURRENT_MENU[1] == ADDBANK && CURRENT_MODULE != NULL ){
                
                W_STATUS = WAIT_DEL_BANK;
                WAIT_CONFIRM = true;
            }
            else if ( CURRENT_MENU[1] == EDITBANK ){
                
                if ( CURRENT_MENU[2] == CHANGEPARAM && EDIT_PARAM ){
                    
                    CURRENT_MODULE->set_param( 
                                CURRENT_PARAM, 
                                CURRENT_MODULE->get_param( CURRENT_PARAM ) - 1.0f * MULTIPLIER 
                                );
                }
                else{
                    
                    d = false;
                }
            }
            else{
                
                d = false;
            }
        }
        else{
            
            d = false;
        }
        
        if ( d ) func_update_menu();
    }
}

void func_button_enter(bool s){
    
    lastb[6] = s;
    //cout << "button enter : " << s << endl;
    
    bool d = true;
    
    if ( s && !WAIT_CONFIRM ){
     
        if ( CURRENT_MENU[0] == SELECTMODULE && CURRENT_MODULE != NULL){
            
            if ( CURRENT_MENU[1] == -1 ){
                
                CURRENT_MENU[1] = CHANGEBANK;
                save_preset( BACK_SELECT_MODULE, VERSION, *GRAPH );
                
            }
            else if ( CURRENT_MENU[1] == EDITBANK && CURRENT_MENU[2] == -1){
                
                CURRENT_MENU[2] = CHANGEPARAM;
                save_preset( BACK_EDIT_BANK, VERSION, *GRAPH );
            }
            else if ( CURRENT_MENU[2] == CHANGEPARAM && CURRENT_PARAM != -1){
                
                if( EDIT_PARAM ){
                    
                    MULTIPLIER = ( MULTIPLIER == 1 )? 1000 : 1;
                    char n[20];
                    sprintf ( n, "x%d", MULTIPLIER );
                    MAIN_SCREEN->print( n, 0 );
                    d = false;
                }
                else{
                    
                    EDIT_PARAM = true;
                    save_preset( BACK_EDIT_PARAM, VERSION, *GRAPH );
                }
            }
            else{
                
                d = false;
            }
        }else{
            
            d = false;
        }
        if ( d ) func_update_menu();
    }
}

void func_button_esc(bool s){
    
    lastb[7] = s;
    //cout << "button esc : " << s << endl;
    
    bool d = true;
    
    if ( s ){
        
        if ( WAIT_CONFIRM ){

            // Pushing esc button when wait for confirmation cancel order
            WAIT_CONFIRM = false;
            W_STATUS = DEFAULT;
            
        }
        else{
            
            // If selecting presset, reinit current preset
            if ( CURRENT_MENU[0] == CHANGEPRESSET ){
                
                CURRENT_PRESSET = 0;
            }
            // If selecting a module
            else if ( CURRENT_MENU[0] == SELECTMODULE ){
                
                    // If selecting a module, deselect it
                    if ( CURRENT_MENU[1] == -1 ){
                        
                        CURRENT_MODULE = NULL;
                    }
                    // If Editing current module's params
                    else if ( CURRENT_MENU[1] == EDITBANK ){
                        
                        // If not changing param, ask for exit submenu
                        // and go back to main menu
                        if ( CURRENT_MENU[2] == -1 ){
                            
                            WAIT_CONFIRM = true;
                            W_STATUS = WAIT_EXIT_SELECTMOD;
                        }
                        // If changing parameters 
                        else if ( CURRENT_MENU[2] == CHANGEPARAM ){
                            
                            // If editing a param ask for exit without save 
                            // and go back to param selection
                            if ( EDIT_PARAM ){
                                
                                WAIT_CONFIRM = true;
                                W_STATUS = WAIT_EXIT_PARAM;
                            }
                            // Else ask for exit witout saving and go back
                            // to module gestion submenu
                            else{
                                
                                WAIT_CONFIRM = true;
                                W_STATUS = WAIT_EXIT_EDITBANK;
                            }
                        }
                        // This part shouldn't be reached
                        else{
                            
                            const char n[] = "Err Exit";
                            MAIN_SCREEN->print( n, 0 );
                            d = false;
                        }
                    }
                    // else ask for go back to main menu without saving
                    else{
                        
                        WAIT_CONFIRM = true;
                        W_STATUS = WAIT_EXIT_SELECTMOD;
                    }
            }
            // This part shouldn't be reached
            else{
                
                const char n[] = "Err Exit";
                MAIN_SCREEN->print( n, 0 );
                d = false;
            }
        }
        
        if ( d ) func_update_menu();
    }
}

void func_button_ok(bool s){
    
    lastb[8] = s;
    //cout << "button ok : " << s << endl;
    
    bool d = true;
    
    if ( s ){
        
        if ( WAIT_CONFIRM ){
            
            WAIT_CONFIRM = false;
            
            switch ( W_STATUS ){
                
                // Delete current bank and set current bank to none
                case WAIT_DEL_BANK :
                    
                    CURRENT_MODULE->remove_bank();
                    
                    d = false;
                    char n[20];
                    sprintf( n, "Bank %d Del", CURRENT_BANK);
                    MAIN_SCREEN->print( n, 0 );
                    
                    CURRENT_BANK = -1;
                    break;
                    
                // Exit Module edition menu ( Select Module )
                case WAIT_EXIT_SELECTMOD :
                
                    CURRENT_MENU[1] = -1;
                    CURRENT_MENU[0] = SELECTMODULE;
                    //load_preset( BACK_SELECT_MODULE, VERSION, GRAPH);
                    break;
                    
                // Exit bank edition menu (Edit Current Bank)
                case WAIT_EXIT_EDITBANK :
                
                    CURRENT_MENU[1] = CHANGEBANK;
                    CURRENT_MENU[2] = -1;
                    //load_preset( BACK_EDIT_BANK, VERSION, GRAPH);
                    break;
                    
                // Exit param edition menu (Change Current Parameter)
                case WAIT_EXIT_PARAM :
                
                    EDIT_PARAM = false;
                    //load_preset( BACK_EDIT_PARAM, VERSION, GRAPH);
                    break;
                    
                default :
                    break;
            }
            
            
        }
        else{
            
            // If selecting a module
            if ( CURRENT_MENU[0] == SELECTMODULE ){
                
                    // If Editing current module's params
                    if ( CURRENT_MENU[1] == EDITBANK ){
                        
                        // If not changing param, go back to main menu and save changes
                        if ( CURRENT_MENU[2] == -1 ){
                            
                            string p = string(PRESSET_NAME);
                            save_preset( p += to_string(CURRENT_PRESSET), VERSION, *GRAPH );
                            CURRENT_MENU[1] = -1;
                            CURRENT_MENU[0] = SELECTMODULE;
                            
                        }
                        // If changing parameters 
                        else if ( CURRENT_MENU[2] == CHANGEPARAM ){
                            
                            // If editing a param go back to param selection and save changes
                            if ( EDIT_PARAM ){
                                                
                                string p = string(PRESSET_NAME);
                                save_preset( p += to_string(CURRENT_PRESSET), VERSION, *GRAPH );
                                EDIT_PARAM = false;
                                
                            }
                            // Else go back to module gestion submenu and save changes
                            else{
                                                     
                                string p = string(PRESSET_NAME);
                                save_preset( p += to_string(CURRENT_PRESSET), VERSION, *GRAPH );
                                CURRENT_MENU[1] = CHANGEBANK;
                                CURRENT_MENU[2] = -1;
                            }
                        }
                        // This part shouldn't be reached
                        else{
                            
                            const char n[] = "Err Exit";
                            MAIN_SCREEN->print( n, 0 );
                            d = false;
                        }
                    }
                    // else go back to main menu and save changes
                    else{
                        
                        string p = string(PRESSET_NAME);
                        save_preset( p += to_string(CURRENT_PRESSET), VERSION, *GRAPH );
                        CURRENT_MENU[1] = -1;
                        CURRENT_MENU[0] = SELECTMODULE;
                    }
            }
        }
        if ( d ) func_update_menu();
    }
}

void func_button_edit(bool s){
    
    lastb[9] = s;
    cout << "button edit : " << s << endl;
}

void func_update_menu(){
    
    char n[20];
        
    if ( WAIT_CONFIRM ){
        
        switch ( W_STATUS ){
                        
            case WAIT_DEL_BANK :
                
                sprintf( n, "delbank Y/N" );
                break;
                
            case WAIT_EXIT_SELECTMOD :
                
                sprintf( n, "Cancel  Y/N" );
                break;
                
            case WAIT_EXIT_EDITBANK :
                
                sprintf( n, "Cancel  Y/N" );
                break;
                
            case WAIT_EXIT_PARAM :
                
                sprintf( n, "Cancel  Y/N" );
                break;
                
            default :
                
                sprintf( n, "Err" );
                break;
        }
    }
    else{
        
        // If edit mode true
        if ( lastb[9] ){
            
        }
        // Else menu is non edit mode
        else{
            
            
            cout << "Current pos : " << CURRENT_MENU[0] << " " << CURRENT_MENU[1];
            cout << " " << CURRENT_MENU[2] << endl;
            
            if ( CURRENT_MENU[0] == CHANGEPRESSET ){
                
                sprintf( n, "Presset-%d", CURRENT_PRESSET );
            }
            else if ( CURRENT_MENU[0] == SELECTMODULE){
                
                if ( CURRENT_MENU[1] == -1 ){
                    
                    if ( CURRENT_MODULE == NULL ){
                        
                        sprintf( n, "Sel Module");
                    }
                    else{
                        
                        sprintf( n, CURRENT_MODULE->get_name());
                    }
                }
                else if ( CURRENT_MODULE != NULL ){
                    
                    if ( CURRENT_MENU[1] == CHANGEBANK ){
                        
                        if ( CURRENT_BANK == -1 ){
                            
                            sprintf( n, "Sel Bank");
                        }
                        else{
                                
                            sprintf( n, "Bank-%d", CURRENT_BANK );
                        }
                    }
                    else if ( CURRENT_MENU[1] == ADDBANK ){
                        
                        if ( lastb[4] ){
                            
                            sprintf( n, "Bank Added" );
                        }
                        else if ( lastb[5] ){
                            
                            sprintf( n, "Bank Rmvd" );
                        }
                        else{
                            
                            sprintf( n, "Add Bank" );
                        }
                    }
                    else if ( CURRENT_MENU[1] == EDITBANK ){
                        
                        if ( CURRENT_MENU[2] == -1 || CURRENT_PARAM == -1){
                            
                            sprintf( n, "Edit param" );
                        }
                        else{
                            
                            sprintf( n, "%s", CURRENT_MODULE->get_formated_param( CURRENT_PARAM ).c_str() );
                        }
                    }
                }
            }
        }
    }
    MAIN_SCREEN->print( n, 0 );
}