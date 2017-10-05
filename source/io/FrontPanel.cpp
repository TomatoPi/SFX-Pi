#include "FrontPanel.h"

namespace{

    using namespace HEX;
    using namespace std;

    typedef unsigned char hex_reg;

    /******************************************************************
     *                          Menu Buttons
     ******************************************************************/
    IO_Button But_up    = IO_Button( PUSH, FUNC_MENU,
                                IO_Adress( HEX_UP, true, false ) );
    IO_Button But_down  = IO_Button( PUSH, FUNC_MENU,
                                IO_Adress( HEX_DOWN, true, false ) );
    IO_Button But_next  = IO_Button( PUSH, FUNC_MENU,
                                IO_Adress( HEX_NEXT, true, false ) );
    IO_Button But_prev  = IO_Button( PUSH, FUNC_MENU,
                                IO_Adress( HEX_PREV, false, false ) );
                                    
    IO_Button But_add   = IO_Button( PUSH, FUNC_MENU,
                                IO_Adress( HEX_ADD, false, false ) );
    IO_Button But_del   = IO_Button( PUSH, FUNC_MENU,
                                IO_Adress( HEX_DEL, true, false ) );
                                    
    IO_Button But_enter = IO_Button( PUSH, FUNC_MENU,
                                IO_Adress( HEX_ENTER, true, false ) );
                                
    IO_Button But_ok    = IO_Button( PUSH, FUNC_MENU,
                                IO_Adress( HEX_OK, true, false ) );
    IO_Button But_esc   = IO_Button( PUSH, FUNC_MENU,
                                IO_Adress( HEX_ESC, true, false ) );

    const int Buts_menu_count = 9;
    const IO_Button Buts_menu[Buts_menu_count] = { But_up, But_down,
                                    But_next, But_prev,
                                    But_add, But_del, But_enter,
                                    But_ok, But_esc };

    /******************************************************************
     *                      Footswitch Adresses
     ******************************************************************/
    IO_Button But_f_next= IO_Button( PUSH, FUNC_BANK_NEXT,
                                IO_Adress( HEX_FOOT_NEXT, false, false ) );
    IO_Button But_f_prev= IO_Button( PUSH, FUNC_BANK_PREV,
                                IO_Adress( HEX_FOOT_PREV, false, false ) );

    /******************************************************************
     *                      Potentiometers Stuff
     ******************************************************************/
    IO_Potentiometer MAIN_POTAR_TAB[SPI_POTAR_COUNT];
    
    /******************************************************************
     *                          Menu Stuff
     ******************************************************************/
    MenuTree MAIN_MENU;
    MenuIterator MENU_POS = MAIN_MENU.root();

    /******************************************************************
     *                       Registers Stuff    
     ******************************************************************/
    mcp23017 *MCP0;

    hex_reg mcp0_reg_gpioa = 0;
    hex_reg mcp0_reg_gpiob = 0;

    hex_reg mcp0_l_reg_gpioa = 0;
    hex_reg mcp0_l_reg_gpiob = 0;
    
    mcp23017 *MCP1;
    
    hex_reg mcp1_reg_gpioa = 0;
    hex_reg mcp1_reg_gpiob = 0;
    
    hex_reg mcp1_l_reg_gpioa = 0;
    hex_reg mcp1_l_reg_gpiob = 0;

    /******************************************************************
     *                      Internal Functions
     ******************************************************************/
    /**
     * Fuction called for read registers values
     * @return true if a register has changed
     */
    bool func_read_reg(){
	
        // Read Registers
        MCP0->readReg( HEX_GPIOA, mcp0_reg_gpioa );
        MCP0->readReg( HEX_GPIOB, mcp0_reg_gpiob );
        
        // Read Registers
        MCP1->readReg( HEX_GPIOA, mcp1_reg_gpioa );
        MCP1->readReg( HEX_GPIOB, mcp1_reg_gpiob );

        bool rtn = false;
        rtn |= mcp0_reg_gpioa != mcp0_l_reg_gpioa;
        rtn |= mcp0_reg_gpiob != mcp0_l_reg_gpiob;
        
        rtn |= mcp1_reg_gpioa != mcp1_l_reg_gpioa;
        rtn |= mcp1_reg_gpiob != mcp1_l_reg_gpiob;
        
        return rtn;
    }
    
    /**
     * Function for compare two registers with given mask
     * @param reg1 first register
     * @param reg2 second register
     * @param mask mask for select only specific bits
     * @return true if masked registers are equals
     */
    bool func_compare_reg( hex_reg reg1, hex_reg reg2, hex_reg mask=0xff )
    {
        return ( reg1 & mask ) == ( reg2 & mask );
    }

    /**
     * Function for extract a specific bit of given register
     * @return bit's value
     */
    bool func_get_reg( hex_reg reg, hex_reg mask )
    {
        return reg & mask;
    }
}
    
void io_init_frontPanel(){

    cout << "Configure MCP23017 registers :" << endl;
    
    MCP0 = new mcp23017( HEX_MCP_0, string("/dev/i2c-1") );
    MCP1 = new mcp23017( HEX_MCP_1, string("/dev/i2c-1") );
	
	// Set Menu buttons ports to output
	MCP0->writeReg( HEX_IODIRA, MASK_ADRRA_MENU, 0xff );
	MCP0->writeReg( HEX_IODIRB, MASK_ADRRB_MENU, 0xff );
	
	// Invert their state
	MCP0->writeReg( HEX_IPOLA, MASK_ADRRA_MENU, 0xff );
	MCP0->writeReg( HEX_IPOLB, MASK_ADRRB_MENU, 0xff );
	
	// Set buttons ports to output
	MCP1->writeReg( HEX_IODIRA, 0x00, 0xff );
	MCP1->writeReg( HEX_IODIRB, 0x00, 0xff );
	
	// Invert their state
	MCP1->writeReg( HEX_IPOLA, 0x00, 0xff );
	MCP1->writeReg( HEX_IPOLB, 0x00, 0xff );

    // get registers values
    func_read_reg();

    cout << endl << "Build Main Menu" << endl;
    
    menu_init_main_menu( &MAIN_MENU );

    MENU_POS = MAIN_MENU.root();

    cout << endl << "Configure MCP3008 AD Converters" << endl;
    
	io_init_spi();
    
	io_init_potar_tab(MAIN_POTAR_TAB);
}

void io_update_frontPanel( Module_Node_List* & graph ){

    /*
     * Update Potar Tab
     */
    io_update_potar_tab( MAIN_POTAR_TAB, graph );

    /*
     * Update Buttons
     */
    func_read_reg();
    
    // If a Menu's button has been pressed
    if ( !func_compare_reg( mcp0_reg_gpioa, mcp0_l_reg_gpioa, MASK_ADRRA_MENU )
        || !func_compare_reg( mcp0_reg_gpiob, mcp0_l_reg_gpiob, MASK_ADRRB_MENU ) )
    {
        for ( int i = 0; i < Buts_menu_count; i++ ){

            // Get correct register
            IO_Adress cur_adr = Buts_menu[i].get_adr();
            hex_reg cur_reg = (cur_adr.gpiob_)?mcp0_reg_gpiob:mcp0_reg_gpioa;
            hex_reg l_reg = (cur_adr.gpiob_)?mcp0_l_reg_gpiob:mcp0_l_reg_gpioa;

            // If button state has changed and button is now up
            if ( !func_compare_reg( cur_reg, l_reg, cur_adr.adr_ )
                && func_get_reg( cur_reg, cur_adr.adr_) )
            {

                //Update menu
                MENU_POS = (*MENU_POS.get().do_)( static_cast<Move_flag>(i), MENU_POS, graph, MAIN_POTAR_TAB );
                break;
            }
        }
    }

    /*
     * Keep Registers Values
     */
    mcp0_l_reg_gpioa = mcp0_reg_gpioa;
    mcp0_l_reg_gpiob = mcp0_reg_gpiob;

    mcp1_l_reg_gpioa = mcp1_reg_gpioa;
    mcp1_l_reg_gpiob = mcp1_reg_gpiob;
}
