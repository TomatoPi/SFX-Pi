#ifndef DEF_MODULES_GRAPH_H
#define DEF_MODULES_GRAPH_H

#include "Modules.h"
#include "ModuleFactory.h"

#include "../modules/EndModule.h"
#include "../consts.h"

#include "Singleton.h"

/**
*   Connection struct.
*   Store the signature of a JACK connection between two modules
*/
typedef struct{

    short s;    /**< Source's index */
    short sp;   /**< Source's port index */
    short t;    /**< Target's index */
    short tp;   /**< Target's port index */
}Connection;

/**
*   Alias for vector of Connection
*/
typedef std::vector<Connection> Connection_List;
typedef Connection_List::iterator Connection_iterator;

/**
*   Alias for vector of Module nodes
*/
typedef std::vector<Module*> Module_List;
typedef Module_List::iterator Module_iterator;

#define END_NODE -55
#define BEGIN_NODE -98

/**
*   Main graph class.
*/
class Module_Node_List : public Singleton<Module_Node_List>{

    friend class Singleton<Module_Node_List>;

    public :

        /**
        *   Main function used to create new module.
        *   Instantiate a new module of given type and add it to the graph
        *
        *   @param mod type of module to add_accessor
        *   @return 0 on success
        */
        int add_module(int type, int id);

        /**
        *   Remove module at idx.
        *
        *   @param idx index of module to remove
        *   @return 0 on success
        */
        int del_module(int idx);

        /**
        *   Function for connect two module nodes.
        *
        *   source_idx = -1 for System_Capture ports
        *   target_idx = -1 for System_Playback ports
        */
        int add_connection(short source_id, short is, short target_id, short id);
        int add_connection( Connection c );
        int del_connection(short source_id, short is, short target_id, short id);

        /**
        *   Disable output from graph.
        *   @param m true for mute graph ( set output volume to 0 )
        */
        void mute( bool m );
        /**
        *   @return true if graph is muted
        */
        bool mute();

        /**
        *   Cycle to next bank for all modules
        */
        void next_bank();

        /**
        *   Cycle to prev bank for all modules
        */
        void prev_bank();

        /**
         * Get Current Bank Index
         */
        int get_bank();

        /**
        * Change output volume
        */
        void set_out_volume( float vol );

        /**
        * Get node by it id
        * It id it given at node creation and will never change
        * @param id module's id
        */
        Module* get( int id );

        /**
        *   Add, remove or get connection at i
        */
        void            connection_add(Connection c);
        void            connection_remove(int i);
        Connection      connection_get(int i) const;
        Connection_List connection_get_list();

        void clear_graph();

        Module_List list_;
        int count_;

        Module *begin_, *end_;

    protected :

        bool mute_;

        float outvl_, outvr_;
        float outv_;

        bool tmp_;

        Connection_List connection_list_;

        int bank_;

    private :

        /**
         * Module Graph Constructor.
         * @param tmp : true for create a temp graph, without input and output nodes
         */
        Module_Node_List();

        ~Module_Node_List();
};

#endif
