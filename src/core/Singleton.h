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
         * Function to Destroy Current Instance
         */
        static void Kill();

    protected :

        static void Get();

        static bool m_ok;    /**< True when object has been created */

    private :
    
        static T *m_instance;/**< Pointer to the only instance */

        /**
         * Prevent Operation as assignement of our unique object
         */
        const T& operator= (const T&){}
};

#include "Singleton.hpp"

#endif
