/**********************************************************************
 * @file ReminderTree.h
 * @author TomatoPi
 * @version 1.0
 *
 * Tree Data Structure addapted to store basic datas in hierarchical
 * order
 * Data stored must be self descriptive, because tree structure only
 * provide a memory function
 * Basicaly used to see if a specific sequence of Value already exist
 **********************************************************************/
#ifndef DEF_BASIC_TREE_H
#define DEF_BASIC_TREE_H

#include <map>
#include <vector>
#include <cstring>

/**
 * ReminderTree Template class
 * The two template arguments are :
 * - Val : type of data stored
 * - Depth : Length of a specific sequency
 * 
 * For Exemple, with <int, 3> :
 * -1 -1 -1
 * 
 * -2 -1 -1
 *    -2 -1
 * 
 * -3 -1 -1
 *       -2
 **/
template <typename Val, size_t Depth>
class ReminderTree{

    public :

        /**
         * Tree Constructor, take value or object that will be returned
         * as error and valid Value for all tree's functions
         **/
        ReminderTree( Val okVal, Val errVal );
        ~ReminderTree();

        void clear();

        /**
         * Function that return Value at given path, or return
         * valid Value if given sequence exist
         * error Value if not
         **/
        Val find( Val seq[Depth] );

        /**
         * Add given sequence to the tree
         * Return error value if given sequence already exist
         **/
        Val add( Val seq[Depth] );

        /**
         * Remove given sequence from the tree
         * Return error value if given sequence don't exist
         **/
        Val remove( Val seq[Depth], size_t depth=(Depth-1) );

        /**
         * Function used to get back list of all stored sequencies
         * @warning user have to delete val array to prevent memory leak
         **/
        std::vector<Val*> getBackTree() const;

        /**
         * Show current Tree Content
         **/
        void print() const;

    private :

        template <typename V>
        struct Node{

            Node():m_node(std::map<Val,Node*>()){}

            std::map<Val,Node*> m_node;
        };

        /**
         * Recursive function that print Tree from given node
         * Depth indication represent current depth inside tree and is
         * only used for output indentation
         **/
        void printNode( Node<Val> *node, size_t d ) const;

        /**
         * Recursive function used to get back list of stored sequencies
         * @param out output array containing recovered sequencies
         * @param prefix prefix containing current position inside tree
         * @param depth current depth inside tree, number of significant
         *  datas inside prefix array
         *
         * @return passed vector pushed front newly recovered sequencies
         **/
        std::vector<Val*> getNode( Node<Val> *node, std::vector<Val*> out, Val prefix[Depth], size_t depth ) const;

        void eraseNode( Node<Val> *node );
        
        Node<Val> *m_root;

        Val m_okValue;
        Val m_errValue;
};

#include "ReminderTree.hpp"

#endif
