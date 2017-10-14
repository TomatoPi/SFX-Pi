#ifndef DEF_POTAR_H
#define DEF_POTAR_H

#include <stdlib.h>
#include <wiringPi.h>
#include <wiringShift.h>
#include <mcp3004.h>

#include "../core/ModulesGraph.h"
#include "../modules/EndModule.h"

#include "io.h"

#define SPI_BASE 100
#define SPI_CHAN1 0
#define SPI_PMAX 8
#define SPI_MAX 1023.0f
#define SPI_HYSTERESIS 0.005f
#define SPI_POTAR_COUNT 6

/**
 * Function for init connection with MCP3008 registers
 */
void io_init_spi();

/**
 * Function for remap a value from an interval to another
 * Remap val from [fmin;fmax] to [tmin;tmax]
 * @return mapped value
 */
float io_map(float val, float fmin, float fmax, float tmin, float tmax);

/**
* Class used for store data about potentiometers.
* One instance of this class is created for each potentiometer ( actually 6 )
* Each potentiometer have a display name, a display plage and
* a vector of accessors, used for modulate module's params
*/
class IO_Potentiometer{

    public :

        static const int D_NAME; // Flag for display potentiometer Name
        static const int D_VAL;  // Flag used for display pot value
        static const int D_ALL;  // Flag used for display everything

        /**
        * Potentiometer Constructor.
        * @param idx potentiometer's idx , must be inferior than SPI_POTAR_COUNT
        * @param name potentiometer's name must be 11 charachters length max
        * @param min display min
        * @param max display max
        */
        IO_Potentiometer( int idx=0, std::string name=std::string("DPot"), float min=0, float max=999 );

        /**
        * Change potentiomer's index
        * @param idx new potentiometer's index
        */
        void set_index( int idx );

        /**
        * Change potentiomer's name
        * @param name new name, must be 11 characters length max
        */
        void set_name( std::string name );
        /**
        * Get Potentiometer's name
        * @return potentiomer's name
        */
        std::string get_name() const;

        /**
        * Change potentiometer's display plage
        * @param min display min
        * @param max display max
        */
        void set_plage( float min, float max );

        /**
        * Get minimal display value
        * @return minimal display value
        */
        float get_min() const;

        /**
        * Get maximal display value
        * @param maximal display value
        */
        float get_max() const;

        /**
        * Update potentiometer's value.
        * Read actual value using gpio and update it if it differs from the old one
        * If value has changed, update it's accessor array ( update module's params )
        * @param graph module's graph
        * @param disp flag used for specify what will be displayed
        */
        void update( int disp=0 );

        /**
        * Return current potentiometer's value.
        * @return current value mapped between -1 and 1;
        */
        float   get_value() const;

        /**
        * Add given accessor to accessor list
        * @param acc accessor to add
        */
        void add_accessor( Accessor acc );

        /**
        * Delete given accessor if exist
        * @param given accessor idx
        */
        void del_accessor( int idx );

        int get_accessor_count();

        std::vector<Accessor> get_accessor();


    private :

        void display( int flag );

        string  name_;
        int     idx_;

        float   min_, max_;

        int     value_;

        std::vector< Accessor > accs_;
};

class PotarArray : public Singleton<PotarArray>{

    public :


    private :


};

void io_init_potar_tab( IO_Potentiometer pot[SPI_POTAR_COUNT] );
void io_update_potar_tab( IO_Potentiometer pot[SPI_POTAR_COUNT] );

#endif
