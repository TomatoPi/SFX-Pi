#include "ModuleFactory.h"

using namespace std;


MODULE_REG create_reg( std::string name, Module* (*builder)(int) ){

    MODULE_REG tmp = { builder, name };
    return tmp;
}

map<int, MODULE_REG> ModuleFactory::reg_ = map<int, MODULE_REG>();

Module* ModuleFactory::create_module( int type, int id ){

    if ( reg_.find( type ) != reg_.end() ){

        return (*reg_[type].builder_)( id );
    }

    return NULL;
}

void ModuleFactory::register_module( int type, MODULE_REG r ){

    if ( reg_.find( type ) == reg_.end() ){

        reg_[type] = r;

        string t = ""; t += to_string(type); t.resize(2);
        cout << "ModuleFactory : Registered TC( " << t << " )";
        cout << " : Formated Name ( " << r.name_ << " )" << endl;
    }
    else{

        cout << "Warning : Module Type : ( " << type << " ) Already Registered" << endl;
    }
}

std::string ModuleFactory::get_name( int type ){

    if ( reg_.find( type ) != reg_.end() ){

        return reg_[type].name_;
    }

    return string("BadType");
}
