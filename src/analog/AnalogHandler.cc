/**********************************************************************
 * @file AnalogHandler.h
 * @author TomatoPi
 * @version 1.0
 *
 * File providing program structure for Analog Inputs
 **********************************************************************/
#include "AnalogHandler.h"

#define SPI_BASE 100
#define SPI_CHAN1 0
#define SPI_HYSTERESIS 5.0f

using namespace SFXP;

AnalogHandler::AnalogHandler(bool headlessRun):AbstractHandler("AnalogHandler"),
    _headless(headlessRun)
    ,_preset(nullptr)
{
    try {

        #ifdef __ARCH_LINUX__
        if (!_headless) {
            if (wiringPiSetup() == -1)
                throw string("Failed Setup WiringPi");

            mcp3004Setup(SPI_BASE, SPI_CHAN1);
        }
        #endif
            
        for (usize_t i = 0; i < SFXP::MAX_POT; i++) {

            _potars[i] = new Potentiometer(i);
        }

        _status = HANDLER_OK;
        SFXPlog::log(_name) << "Builded Handler" << endl;
    }
    catch (string const& e){

        SFXPlog::err(_name) << e << endl;
        _status = ERRORED;
    }
}

AnalogHandler::~AnalogHandler() {

    this->clearAnalog();
    
    for (usize_t i = 0; i < SFXP::MAX_POT; i++) {

        delete _potars[i];
    }
}

/**
 * Function used to push an event to an handler
 * The event is imediatly processed
 **/
void AnalogHandler::pushEvent(SFXPEvent* event) {

    if (event->_type == SFXPEvent::Type::Event_PresetChanged) {

        this->eventPresetChanged(event);
    }
    else if (event->_type == SFXPEvent::Type::Event_InitAll) {

        if (!_preset) {

            SFXPlog::err(_name) << "No Preset Given to the Handler" << endl;
        }
        else {
                
            SFXPlog::log(_name) << "Handler Initialised and Active" << endl;
        }
    }
    else if (_preset) {

        SFXPlog::wrn(_name) << "Unhandled Event : " << (*event) << endl;
    }
}

/**
 * Function called at each main loop frame
 * Makes the Handler doing his job
 **/
void AnalogHandler::run() {

    if (_preset) {

    #ifdef __ARCH_LINUX__
    if (!_headless) {
        // For each potentiometer
        for (SFXP::usize_t i = 0; i < SFXP::MAX_POT; i++)
        {
        // If Links are connected to this Potentiometer
        if (_preset->_links[i]){

            float value = analogRead(SPI_BASE + i);
            float diff = value - _potars[i]->value();

            // If value has changed enougth
            if (    (diff > SPI_HYSTERESIS && diff > 0)
                ||  (diff < -SPI_HYSTERESIS && diff < 0))
            {
                // Update Potentiometer value
                _potars[i]->setValue(value);

                // Create the event
                SFXPEvent event = SFXPEvent(SFXPEvent::Type::Event_AnalogChanged);
                    
                // Send a notification for each link
                for (auto& l : (*_preset->_links[i])) {

                    float v = l->computeValue(value);
                    int status = l->isRelative()?IOEvent::RELATIVE:IOEvent::ABSOLUTE;
                    id2_t id = (i & SFXP::ID_MASK) | SFXP::ID_POT;

                    event._io = IOEvent(status, v, id);
                    _eventHandler->pushEvent(event);
                }
            }
        }
        }
    }
    #endif
    }
}

void AnalogHandler::clearAnalog() {

    if (_preset) {

        for (SFXP::usize_t i = 0; i < SFXP::MAX_POT; i++)
        {
            if (_preset->_links[i]) {

                for (auto& l : (*_preset->_links[i])) delete l;
                delete _preset->_links[i];
            }
            _preset->_links[i] = nullptr;
        }
    }
}

void AnalogHandler::eventPresetChanged(SFXPEvent* event) {

    if (Preset* np = (Preset*)event->_preset._preset) {

        this->clearAnalog();
        _preset = np;
    }
}
