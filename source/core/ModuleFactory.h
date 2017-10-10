#ifndef DEF_MODULE_FACTORY_H
#define DEF_MODULE_FACTORY_H

#include <map>

#include "Modules.h"

/**
*   Enum of all avaiable modules
*/

/**
 * Struct Used by Module Factory for register modules
 * It take a pointer to a builder function
 * And a formated Name that will be used for identify this module type
 * inside the log and outputs
 */
typedef struct{

    Module* (*builder_)(int);
    std::string name_;

}MODULE_REG;

MODULE_REG create_reg( std::string, Module* (*builder)(int) );

class ModuleFactory{

    public :

        /**
         * Function Used for Create a New module of Given Type.
         * It call the Function passed at Module's registration
         * and return Created Module
         * If given Type is Invalid, It Will return an NULL Pointer
         */
        static Module* create_module( int type, int id );

        /**
         * Function Used For Register a new Module Type.
         * It take the unique number that will identify this module type
         * durring all program run
         * Second argument is the function that will be used for create
         * a module of given type
         * Ignore registration call if given number is already Used
         */
        static void register_module( int type, MODULE_REG r );

        /**
         * Function Used for get Formated Name Of given module type
         * Return "BadType" if given type identifier is invalid
         */
        static std::string get_name( int type );

    private :

        static std::map<int, MODULE_REG> reg_;
};

#endif
