#ifndef DEF_MAIN_MENU_H
#define DEF_MAIN_MENU_H

namespace FRONT_PANEL{

	typedef enum{
		
		WAIT_DEL_BANK,
		WAIT_DEL_PRESET,
		WAIT_EXIT_SELECTMOD,
		WAIT_EXIT_EDITBANK,
		WAIT_EXIT_PARAM,
		WAIT_NOTHING
		
	}Wait_status;

	typedef enum{
		
		MAIN_MENU,
		
			MAIN_CHANGE_PRESSET,
			MAIN_SELECT_MODULE,
		
				SELMOD,
				SELMOD_CHANGE_BANK,
				SELMOD_ADD_BANK,
				SELMOD_EDIT_BANK,
		
					SELMOD_EDITB_SELPAR,
						SELMOD_EDITB_EDITPAR,
		
			EDIT_ADD_MODULE,
				EDIT_CHOOSE_MODTYPE,
				
			EDIT_SELECT_MODULE,
			
				EDITMOD,
				EDITMOD_ADD_CONNECTION,
				
					EDITMOD_CHOOSE_CSOURCE,
					EDITMOD_CHOOSE_CTARGET,
				
				EDITMOD_ADD_ACCESSOR,
				
					EDITMOD_ACC_CHOOSE_TARGET_M,
					EDITMOD_ACC_CHOOSE_TARGET_P,
					EDITMOD_ACC_CHOOSE_POT,
					EDITMOD_ACC_CHOOSE_MIN,
					EDITMOD_ACC_CHOOSE_MAX,
					EDITMOD_ACC_CHOOSE_CURVE,
					EDITMOD_ACC_CHOOSE_ISDB,
					EDITMOD_ACC_CHOOSE_ISINV,
		
		INIT
		
	}Menu_status;

	typedef enum{
		
		MOVE_UP,
		MOVE_DOWN,
		
		MOVE_NEXT,
		MOVE_PREV,
		
		MOVE_ADD,
		MOVE_DEL,
		
		MOVE_ENTER,
		
		MOVE_ESC,
		MOVE_OK,
		
		MOVE_ENTER_EDIT,
		MOVE_EXIT_EDIT,
		
		MOVE_FOOT_PREV,
		MOVE_FOOT_NEXT,
		
		MOVE_NONE
		
	}Move_flag;
	
	/**
	 * Function called when a button is pressed.
	 * Core menu's function it compute button order regarding current status
	 */
	void func_update_menu(	Module_Node_List* graph,
							IO_Potentiometer pot[SPI_POTAR_COUNT] );
	/**
	 * Function called when menu is exited
	 */
	void func_exit_menu();
	
	/**
	* Function called for enter ChangePreset submenu
	*/
	void func_go_main_change_preset();

	/**
	* Function called for process Move order inside ChangePreset menu
	* @param flag move flag
	*/
	void func_do_main_change_preset( Move_flag flag );

	/**
	* Function called for enter Select Module submenu
	*/
	void func_go_main_select_module();
	
	/**
	 * Function called for process move order inside SelectModule menu
	 * @param flag move flag
	 */
	void func_do_main_select_module( Move_flag flag );
	
	/**
	 * Function called for enter addModule submenu
	 */
	void func_go_main_add_module();
	
	/**
	 * Function called for process move order inside AddModule menu
	 * @param flag move flag
	 */
	void func_do_add_module( Move_flag flag );
	
	/**
	 * Function called for enter EditGraph
	 */
	void func_go_main_edit_graph();
	
	/**
	 * Function called for process move order inside Editgrpah menu
	 * @param flag move flag
	 */
	void func_do_main_edit_graph( Move_flag flag );
	
	/**
	 * Function called for enter PotEdition submenu
	 */
	void func_go_main_edit_pot();
	
	/**
	 * Function called for process move order inside PotEdition menu
	 */
	void func_do_edit_pot( Move_flag flag );
	
	
}

#endif
