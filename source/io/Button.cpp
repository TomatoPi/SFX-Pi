#include "io.h"

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

void* IO_Button::compute( void* data ){

    return data;
}

IO_Adress IO_Button::get_adr() const{

    return adr_;
}
