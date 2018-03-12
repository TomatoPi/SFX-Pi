/**********************************************************************
 * @file JACKUnit.h
 * @author TomatoPi
 * @version 1.1
 *
 * File Containing JACK Side of Effect Units
 **********************************************************************/
#ifndef DEF_JACK_UNIT_H
#define DEF_JACK_UNIT_H

#include <iostream>
#include <string>

#include <jack/jack.h>

#include "Const.h"

/**
 * Class Containing JACK Related stuff for units.
 * It contains Client and Unit's Ports
 * And function for correctly setup unit for processing
 **/
class JACKUnit{

    public :

        /**
         * Enum of available Ports Type
         **/
        enum PortType{
            AudioIn =0,
            AudioOut=1,
            MidiIn  =2,
            MidiOut =3
        };

        /**
         * Function to get Jack's string equivalent of port type
         **/
        static const char* portTypeString(PortType t);

        /**
         * JACKUnit Constructor.
         * @throw string if client setup failed
         **/
        JACKUnit(std::string server, std::string name);
        
        /**
         * JACKUnit Destructor.
         * @warning JACKUnit must be deactivated before calling destructor
         **/
        ~JACKUnit();
    
        /**
         * Function Used to register unit's ports.
         * Must be called by Unit Constructor before activating client
         * Parameters are ports counts and array containing names
         * 
         * @throw string if port registration failed
         **/
        void registerPorts(const std::string** names,const SFXP::usize_t* count);

        /**
         * Function used to register unit's callback function.
         * Must be called by Unit Constructor before activating client
         * Passed function must be suitable for realtime shedulding
         **/
        void registerCallback(int (*callback)(jack_nframes_t, void*), void *arg);

        /**
         * Function used to activate JACK client.
         * Must be called after port and callback registration
         * Will cause the unit start to process samples
         *
         * @throw string if client activation failed
         **/
        void activateClient();
        void deactivateClient();

        /**
         * Function used to add connection to Unit's client.
         * It take source and target port's names as parameters
         * Program convention require the connection to be added to
         * source's client
         *
         * @throw string if ports are invalid
         **/
        void connect(std::string source, std::string target);
        void disconnect(std::string source, std::string target);

        /**
         * Get given port's
         **/
        jack_port_t*** getPortList();
        jack_port_t*   getPort(PortType type, SFXP::usize_t idx);

        SFXP::usize_t getPortCount(PortType type);

        jack_client_t* getClient();
        
        std::string getName() const;

        float getSamplerate() const;

    private :

        jack_client_t   *m_client;

        SFXP::usize_t*  m_portCount;
        jack_port_t***  m_ports;

    private :

        SFXP::hex_t m_status;
};

#endif
