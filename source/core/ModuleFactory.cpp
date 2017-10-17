#include "ModuleFactory.h"

using namespace std;


MODULE_REG create_reg( string name, Module* (*builder)(int), const string* params, int pcount ){

    vector<string> pnames;
    for( int i = 0; i < MOD_COUNT; i++ ){

        pnames.push_back( MOD_PARAMS[i] );
    }
    for( int i = MOD_COUNT; i < pcount + MOD_COUNT; i++ ){

        pnames.push_back( params[i] );
    }

    MODULE_REG tmp = { builder, name, pnames, pcount + MOD_COUNT };
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
        cout << " : Formated Name ( " << r.name_ << " ) : Param Count ( ";
        cout << to_string( r.count_ ) << " )" << endl;

        if( PROG_CONST::FULL_LOG ) for( auto &names : r.params_ ){

            cout << "    " << names << endl;
        }
    }
    else{

        cout << "Warning : Module Type : ( " << type << " ) Already Registered" << endl;
    }
}

string ModuleFactory::get_name( int type ){

    if ( reg_.find( type ) != reg_.end() ){

        return reg_[type].name_;
    }

    return string("BadType");
}

vector<string> ModuleFactory::get_params( int type ){

    if ( reg_.find( type ) != reg_.end() ){

        return reg_[type].params_;
    }

    return vector<string>();
}

int ModuleFactory::get_pcount( int type ){

    if ( reg_.find( type ) != reg_.end() ){

        return reg_[type].count_;
    }

    return 0;
}
