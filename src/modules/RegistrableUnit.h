/**********************************************************************
 * @file RegistrableUnit.h
 * @author TomatoPi
 * @version 1.0
 *
 * Template class containing all stuff that an effect must register to
 * the UnitFactory
 **********************************************************************/
#ifndef DEF_REGISTRABLE_UNIT_H
#define DEF_REGISTRABLE_UNIT_H

template <class T>
class RegistrableUnit{

    public :
        /**
         * Consts for UnitFactory Register
         **/
        static const std::string NAME;
        
        static AbstractEffectUnit* BUILDER(uint8_t id, uint8_t type);

        static const std::string PARNAMES[];
        static const uint8_t PARCOUNT;

        static const std::string PORNAMES[];
        static const uint8_t AI;
        static const uint8_t AO;
        static const uint8_t MI;
        static const uint8_t MO;

    protected :

        static const uint16_t PARSIZE;
};

#endif
