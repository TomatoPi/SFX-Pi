#ifndef DEF_TREE_H
#define DEF_TREE_H

#include <iostream>
#include <stdarg.h>

/**
 * Linked list main structure.
 * Provide fuctions for Linked List managment
 */
template <typename T>
class LinkedList{
    
    public :

        /**
         * LinkedList constructor.
         * Create a list and set first index to null
         */
        LinkedList();
        ~LinkedList();

        /**
         * Add new Element at the end of the list.
         * @param el element to add
         */
        void push_back( T el );

        /**
         * Add new Element at the begining of the list.
         * @param el element to add
         */
        void push_front( T el );

        /**
         * Remove param at given index if exist
         * @param idx index
         */
        void remove( size_t idx );

        /**
         * Clear all list
         */
        void clear();

        /**
         * Get count of elements inside the list
         * @return elements count
         */
        size_t size() const;

        /**
         * Get first element
         * @return list first element
         */
        T first();

        /**
         * Get supposed list end
         * @return supposed list end, wrong if list is corrupted
         */
        T last();
        
    protected :

        /**
         * Subclass for List elements
         */
        template <typename S>
        class ListElement{

            public :

                ListElement( S el ):prev_(NULL), next_(NULL), data_(el)
                {
                }

                ListElement<S> *prev_;
                ListElement<S> *next_;
                S data_;
        };

        /**
         * Get Effective list end
         * Go throught the list from first ellement to the last reacheable
         * @return last element
         */
        ListElement<T>* get_last();

        ListElement<T> *first_;
        ListElement<T> *last_;

        size_t size_;
};

#include "LinkedList.tpp"



/**
 * Tree class
 * A tree is basicaly a linked list wherein each element also has a fork list
 * T represent Contained value type
 * F represent Nodes identifyer type
 */
template <typename T, typename F>
class Tree{

    public :

        /**
         * Tree constructor.
         * Set pointers to null and iterator to nothing
         */
        Tree():root_(NULL), size_(0)
        {
        }
        ~Tree();

        /**
         * Tree iterator class.
         * Provide an object for navigate trought the tree
         */
        class Iterator;

        /**
         * Add new Element at the end of main branch
         * @param el element to add
         * @param id new element id
         */
        void add( T el, F id );

        /**
         * Add new element after given Iterator
         * @param el element to add
         * @param id new element id
         * @param itr iterator after witch new ellement will be added
         * @param child true for add element as child of target node
         */
        void add( T el, F id, Iterator itr, bool child=false );

        /**
         * Add new element at given position
         * @param el element to add
         * @param id new element id
         * @param child true for add elements as child of target node
         * @param depth number of nodes in path
         * @param ... path to target node, list of id's in order
         */
        void add( T el, F id, bool child, int depth, ... );

        /**
         * Get iterator at given path
         * @param depth number of nodes in path
         * @param ... path to target node, list of id's in order
         */
        Iterator get( int depth, ... );

        /**
         * Get iterator at root.
         * @return iterator at root node or NULL if tree is empty
         */
        Iterator root();

        /**
         * Remove node at given Iterator, and Erase child branch if exist
         * @param itr Iterator at node to remove
         * @return iterator on next node, or on parent node if no next
         */
        Iterator remove( Iterator itr );

        /**
         * Remove node of given ID, and child branch if exist.
         * @param depth number of nodes in path
         * @param ... path to target node, list of id's in order
         * @return iterator on next node, or on parent node if no next
         */
        Iterator remove( int depth, ... );

        /**
         * Remove the all branch containing given node.
         * Also erase all childs sub-branches
         * @param itr iterator at branch to remove
         * @return iterator to branch's parent node
         * @warning use it one a node in main branch will erase the all tree
         */
        Iterator remove_branch( Iterator itr );
        
        /**
         * Remove the all branch containing given node.
         * Also erase all childs sub-branches
         * @param depth number of nodes in path
         * @param ... path to target node, list of id's in order
         * @return iterator to branch's parent node
         * @warning use it one a node in main branch will erase the all tree
         */
        Iterator remove_branch( int depth, ... );

        /**
         * Clear all Tree
         */
        void clear();
        
        /**
         * get total count of nodes inside the tree
         */
        size_t size(){ return size_; }
        
    protected :

        /**
         * Node class, subelements of Trees.
         * A node contain pointer to next node in the branch
         * To the previous or master node in the branch
         * To the childnode if one is present
         */
        template <typename N, typename S>
        class Node{

            public :

                Node( N el , S id , bool r=false ):prev_(NULL), next_(NULL),
                                    parent_(NULL), child_(NULL),
                                    id_(id),
                                    data_(el)
                {
                }

                Node<N,S> *prev_, *next_;
                Node<N,S> *parent_, *child_;

                S id_;
                N data_;
        };

        /**
         * Set given node as tree's root
         */
        void set_root( Node<T,F>* node );

        /**
         * Insert a node after another one
         * @param t target node
         * @param n node to insert
         */
        void insert_after( Node<T,F>* t, Node<T,F>* n );

        /**
         * Insert a node before another one
         * @param t target node
         * @param n node to insert
         */
        void insert_before( Node<T,F>* t, Node<T,F>* n );

        /**
         * Connect given nodes together
         * @param p first node, will be second node's prev node
         * @param n second node, will be first node's next node
         */
        void connect_nodes( Node<T,F>* p, Node<T,F>* n );

        /**
         * Search node in curent branch by it ID
         * @paran itr iterator in branch wherein search
         * @param f node's id to search
         * @return iterator on founded node, or null iterator if not found
         */
        Iterator search_id( Iterator itr, F f );

        Node<T,F>* root_;
        size_t size_;
};
#include "Tree.tpp"

template <typename T, typename F>
class Tree<T,F>::Iterator{

    public :

        friend class Tree<T, F>;

        /**
         * Move to next node in the curent branch
         * @return curent iterator
         */
        Iterator next();

        /**
         * Move to previous node in the curent branch
         * @return curent iterator
         */
        Iterator prev();

        /**
         * Verify curent node has child
         */
        bool has_child();

        /**
         * Move to child node if present, does nothing if not
         * @return curent iterator
         */
        Iterator child();

        /**
         * Verify if node has parent
         */
        bool has_parent();
        
        /**
         * Move to parent node if present, does nothing if not
         * @return curent iterator
         */
        Iterator parent();

        /**
         * Get obect at current iterator position
         */
        T get();

        /**
         * Get id of node at curent iterator position
         */
        F id();

        /**
         * Get validity of iterator
         * return true if position is not NULL
         */
        bool ok();

    protected :

        Iterator( Node<T,F>* pos=NULL ):pos_(pos)
        {
        }

        Node<T,F>* pos_;
};

#include "Iterator.tpp"

#endif
