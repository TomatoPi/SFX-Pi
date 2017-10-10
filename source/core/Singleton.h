#ifndef DEF_SINGLETON_H
#define DEF_SINGLETON_H

#include <iostream>

/**
 * Template Class For Single Instance Objects.
 * It Derived class from it MUST have private constructor and declare
 * Singleton class as friend
 * 
 * See for Singleton Pattern for more informations
 */
template<class T>
class Singleton{

    public :
    
        /**
         * Function For Instance the object
         */
        static void Create();
        
        /**
         * Function For Get The Current Instance
         */
        static T& Get();

        /**
         * Function For Destroy Current Instance
         */
        static void Kill();

    protected :

        static bool ok_;    /**< True when object has been created */
        static T *i_;       /**< Pointer to the only instance */

    private :

        /**
         * Prevent Operation as assignement of our unique object
         */
        const T& operator= (const T&){}
};

#include "Singleton.hpp"

#endif
