/**********************************************************************
 * @file UnitFactory.h
 * @author TomatoPi
 * @version 1.0
 *
 * File Providing Common stuff about effect units
 **********************************************************************/
#ifndef DEF_UNIT_FACTORY_H
#define DEF_UNIT_FACTORY_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "./core/AbstractEffectUnit.h"

/**
 * Class used for avoid all program knowing effect list.
 **/
class UnitFactory{

    /** SubClass Containing all informations about effects types  **/
    class UnitFactoryReg;

    /** Prototype for the builder function **/
    typedef AbstractEffectUnit* (*builder_f)(uint8_t id, uint8_t type);

    /** Alias For String Vector **/
    typedef std::vector<std::string> NameArray;

    public :

        /**
         * Function used to register a new Effect Type.
         * It take the unique number that will identify this type and
         * all type's constants :
         * name
         * The builder function ( static call of constructor )
         * params count and names
         * ports count and names
         **/
        static void registerEffect(uint8_t type,
                    std::string name, builder_f builder,
                    const std::string* paramNames, uint8_t paramCount,
                    const std::string* portNames, 
                    uint8_t ai, uint8_t ao, uint8_t mi, uint8_t mo);

        /**
         * Function that must be used by the program to create new effect.
         * It call the registered builder function and return pointer
         * to the created effect
         * @return NULL if given type is not registered
         **/
        static AbstractEffectUnit* createEffect(uint8_t type, uint8_t id);

        /**
         * Function used to get Default name of given type
         * @return empty string if invalid type
         **/
        static std::string getName(uint8_t type);

        /**
         * Get params name's array of given type
         * @return Empty vector if invalid type
         **/
        static NameArray getParamNames(uint8_t type);
        static uint8_t getParamCount(uint8_t type);

        /**
         * Get ports name's array of given type
         * @return Empty vector if invalid type
         **/
        static NameArray getPortNames(uint8_t type);
        static uint8_t getPortCount(uint8_t type, uint8_t ptype);

        /**
         * Function used to get complete name of a port
         * Same effect as jack_port_name function used to respect
         * encapsulation of JACKUnit.
         * @param pointer to port's owner
         * @param type effect's type code
         * @param ptype port type
         * @param idx index of wanted port
         *
         * @return effect_name:port_name if type ok, empty string otherwise
         **/
        static std::string buildPortName(AbstractEffectUnit* source,
                                            uint8_t type,
                                            uint8_t ptype, uint8_t idx);
        
    private :

        class UnitFactoryReg{

            public :

                UnitFactoryReg();
                UnitFactoryReg(std::string name, builder_f builder,
                    const std::string* paramNames, uint8_t paramCount,
                    const std::string* portNames,
                    uint8_t ai, uint8_t ao, uint8_t mi, uint8_t mo);

            public :
            
                std::string m_name;

                builder_f m_builder;

                NameArray m_paramNames;
                NameArray m_portNames;
                uint8_t   m_portCount[4];
        };

        static std::map<uint8_t, UnitFactoryReg> m_reg;
};

#endif
