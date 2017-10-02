#ifndef DEF_MAIN_MENU_H
#define DEF_MAIN_MENU_H

namespace FRONT_PANEL{


	
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
