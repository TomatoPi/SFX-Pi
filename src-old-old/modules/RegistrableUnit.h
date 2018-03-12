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
        
        static AbstractEffectUnit* BUILDER(id1_t id, id1_t type);

        static const std::string PARNAMES[];
        static const usize_t PARCOUNT;

        static const std::string PORNAMES[];
        static const usize_t AI;
        static const usize_t AO;
        static const usize_t MI;
        static const usize_t MO;

    protected :

        static const usize_t PARSIZE;
};

#endif
