#include "Button.h"

using namespace std;

IO_Button::IO_Button( IO_PUSH_TYPE type, IO_BUTTON_FUNCTION func, IO_Adress adr ):
    type_(type),
    func_(func),
    adr_(adr)
{

}

void IO_Button::set_type( IO_PUSH_TYPE type ){

    type_ = type;
}

IO_PUSH_TYPE IO_Button::get_type() const{

    return type_;
}

void IO_Button::set_func( IO_BUTTON_FUNCTION func ){

    func_ = func;
}

IO_BUTTON_FUNCTION IO_Button::get_func() const{

    return func_;
}

void* IO_Button::compute(){

    if ( func_ == FUNC_MENU ){
        
        return NULL;
    }
    else if ( func_ == FUNC_BANK_NEXT ){

        Module_Node_List::Get().next_bank();
        
        string b = "Bank";
        string i = "";
        i += Module_Node_List::Get().get_bank();
        
        IOS::printm( b + i, IOS::DEFAULT_TEMP, IOS::TEMP );
        IOS::printm( i, IOS::DEFAULT_TEMP, IOS::TEMP | IOS::SEG7 );
    }
    else if ( func_ == FUNC_BANK_PREV ){
        
        Module_Node_List::Get().prev_bank();
        
        string b = "Bank";
        string i = "";
        i += Module_Node_List::Get().get_bank();
        
        IOS::printm( b + i, IOS::DEFAULT_TEMP, IOS::TEMP );
        IOS::printm( i, IOS::DEFAULT_TEMP, IOS::TEMP | IOS::SEG7 );
    }
    
    return NULL;
}

IO_Adress IO_Button::get_adr() const{

    return adr_;
}
