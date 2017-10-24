#ifndef DEF_SINGLETON_H
#define DEF_SINGLETON_H

#include <iostream>

/**
 * Template Class to Single Instance Objects.
 * See for Singleton Pattern for more informations
 */
template<class T>
class Singleton{

    public :
    
        /**
         * Function to Instance the object
         */
        static void Create();
        
        /**
         * Function to Get The Current Instance
         */
        static T& Get();

        /**
         * Function to Destroy Current Instance
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
