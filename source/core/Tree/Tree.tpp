
template <typename T, typename F>
Tree<T,F>::~Tree(){

    clear();
}

template <typename T, typename F>
void Tree<T,F>::add( T el, F id ){

    Node<T,F> *newn = new Node<T,F>( el, id );

    // If tree is empty add new element as tree's root
    // Connect it next and prev pointers to itself
    if ( root_ == NULL ){

        set_root( newn );
    }
    //Else add new node as root's previous node ( aka "last" branch's node )
    else{

        // Insert New node between last node and root node
        insert_before( root_, newn );
    }

    ++size_;
}

template <typename T, typename F>
void Tree<T,F>::add( T el, F id, Iterator itr, bool child ){

    Node<T,F> *newn = new Node<T,F>( el, id );

    // If iterator pos is NULL and if tree is empty, add new elemnt as root
    if ( itr.pos_ == NULL ){

        if ( root_ == NULL ){

            set_root( newn );
        }
        // Invalid iterator : do nothing
        else{

            std::cout << "TreeError : Invalid Iterator" << std::endl;
            delete newn;
            
            return;
        }
    }
    // Else if new object is added as a child of given node
    else if ( child ){

        // If target already has a child node
        // Insert new node at the end of child branch ( before branch root )
        if ( itr.has_child() ){

            insert_before( itr.pos_->child_, newn );
            newn->parent_ = itr.pos_;
        }
        // Else set new node as child branch root
        else{

            // Create branch loop
            newn->prev_ = newn;
            newn->next_ = newn;

            // Add branch
            itr.pos_->child_ = newn;
            newn->parent_ = itr.pos_;
        }
    }
    // Else object is added after iterator
    else{

        insert_after( itr.pos_, newn );
    }

    ++size_;
}

template <typename T, typename F>
void Tree<T,F>::add( T el, F id, bool child, int depth, ... ){

}

template <typename T, typename F>
typename Tree<T,F>::Iterator Tree<T,F>::get( int depth, ... ){
    
    Tree<T,F>::Iterator rtn = root();

    if ( rtn.pos_ != NULL ){

        va_list v;
        va_start( v, depth );

        for ( int i = 0; i < depth; i++ ){
            
            rtn = search_id( rtn, va_arg( v, F ) );
            if ( rtn.pos_ == NULL ) break;
        }
        
        va_end(v);
    }
    else{

        rtn = Tree<T,F>::Iterator();
    }

    return rtn;
}

template <typename T, typename F>
typename Tree<T,F>::Iterator Tree<T,F>::root(){

    return Iterator( root_ );
}

template <typename T, typename F>
typename Tree<T,F>::Iterator Tree<T,F>::remove( Tree<T,F>::Iterator itr ){

    // If iterator is valid
    if ( itr.pos_ != NULL ){

        bool is_alone = itr.pos_->next_ == itr.pos_;
        Tree<T,F>::Iterator rtn = Tree<T,F>::Iterator();

        // If node has child, erase child branch
        if ( itr.has_child() ){

            remove_branch( Tree<T,F>::Iterator(itr.pos_->child_) );
        }

        // If node is root,  set next node as root
        if ( itr.pos_ == root_ ){

            // If node is the only one inside tree
            if ( is_alone ){

                root_ = NULL;
                size_ = 0;
                
                delete itr.pos_;
            }
            // Else transfert root node
            else{

                root_ = itr.pos_->next_;
                connect_nodes( itr.pos_->prev_, itr.pos_->next_ );

                rtn = Tree<T,F>::Iterator( itr.pos_->next_ );

                delete itr.pos_;
                --size_;
            }
        }
        // Else if node is in a sub branch
        else if ( itr.pos_->parent_ != NULL ){

            // If node is alone
            if ( is_alone ){

                itr.pos_->parent_->child_ = NULL;

                rtn =  Tree<T,F>::Iterator( itr.pos_->parent_ );
                
                delete itr.pos_;
                --size_;
            }
            // Else If node is the branch's root, transfert childness to next
            else if ( itr.pos_->parent_->child_ == itr.pos_ ){

                itr.pos_->parent_->child_ = itr.pos_->next_;
                connect_nodes( itr.pos_->prev_, itr.pos_->next_ );
                
                rtn =  Tree<T,F>::Iterator( itr.pos_->next_ );
                
                delete itr.pos_;
                --size_;
            }
            // Else simply delete node
            else{

                connect_nodes( itr.pos_->prev_, itr.pos_->next_ );

                rtn = Tree<T,F>::Iterator( itr.pos_->next_ );

                delete itr.pos_;
                --size_;
            }
        }
        // Else simply delete node
        else{

            connect_nodes( itr.pos_->prev_, itr.pos_->next_ );

            rtn = Tree<T,F>::Iterator( itr.pos_->next_ );

            delete itr.pos_;
            --size_;
        }

        return rtn;
    }
    
    return itr;
}

template <typename T, typename F>
typename Tree<T,F>::Iterator Tree<T,F>::remove( int depth, ... ){

    Tree<T,F>::Iterator rtn = root();

    if ( rtn.pos_ != NULL ){

        va_list v;
        va_start( v, depth );

        for ( int i = 0; i < depth; i++ ){
            
            rtn = search_id( rtn, va_arg( v, F ) );
            if ( rtn.pos_ == NULL ) break;
        }
        
        va_end(v);
    }
    else{

        rtn = Tree<T,F>::Iterator();
    }

    remove( rtn );

    return rtn;
}

template <typename T, typename F>
typename Tree<T,F>::Iterator Tree<T,F>::remove_branch( Tree<T,F>::Iterator itr ){
    
    if ( itr.pos_ != NULL ){

        Tree<T,F>::Iterator rtn = Tree<T,F>::Iterator( itr.pos_->parent_ );
        
        Tree<T,F>::Iterator itr2 = itr;
        itr2.next();

        // If branch has parent, set parent child to null
        if ( itr.has_parent() ){

            itr.pos_->parent_->child_ = NULL;
        }

        // If itr2 == itr, node is alone in the branch
        if ( itr.pos_ == itr2.pos_ ){

            if ( itr.has_child() ){

                remove_branch(  Tree<T,F>::Iterator( itr.pos_->child_ ) );
            }
            
            delete itr.pos_;
            --size_;
        }
        // Else erase branch
        else{

            itr.pos_->prev_->next_ = NULL;
            
            // While branch contain nodes
            while ( itr.pos_ != NULL ){

                std::cout << "Node Founded : Delete it" << std::endl;

                // If node has child delete it
                if ( itr.has_child() ){

                    remove_branch(  Tree<T,F>::Iterator( itr.pos_->child_ ) );
                }

                if ( itr.pos_ != NULL ){
                    
                    // Remove pointers to node that will be deleted
                    if ( itr.pos_->next_ != NULL ){
                        
                        itr.pos_->next_->prev_ = NULL;
                    }

                    // Delete curent node
                    delete itr.pos_;
                    --size_;
                }

                // Move to next node
                itr = itr2;
                itr2.next();
            }
        }

        return rtn;
    }

    return itr;
}

template <typename T, typename F>
typename Tree<T,F>::Iterator Tree<T,F>::remove_branch( int depth, ... ){
    
    Tree<T,F>::Iterator rtn = root();

    if ( rtn.pos_ != NULL ){

        va_list v;
        va_start( v, depth );

        for ( int i = 0; i < depth; i++ ){
            
            rtn = search_id( rtn, va_arg( v, F ) );
            if ( rtn.pos_ == NULL ) break;
        }
        
        va_end(v);
    }
    else{

        rtn = Tree<T,F>::Iterator();
    }

    remove_branch( rtn );

    return rtn;
}

template <typename T, typename F>
void Tree<T,F>::clear(){

    if ( root_ != NULL ){
        
        remove_branch( this->root() );
    }
}

template <typename T, typename F>
void Tree<T,F>::set_root( Node<T,F>* node ){

    root_ = node;
    root_->next_ = root_;
    root_->prev_ = root_;
}

template <typename T, typename F>
void Tree<T,F>::insert_after( Node<T,F>* t, Node<T,F>* n ){

    t->next_->prev_ = n;
    n->next_ = t->next_;
    n->prev_ = t;

    t->next_ = n;
}

template <typename T, typename F>
void Tree<T,F>::insert_before( Node<T,F>* t, Node<T,F>* n ){

    t->prev_->next_ = n;
    n->prev_ = t->prev_;
    n->next_ = t;

    t->prev_ = n;
}

template <typename T, typename F>
void Tree<T,F>::connect_nodes( Node<T,F>* p, Node<T,F>* n ){

    p->next_ = n;
    n->prev_ = p;
}

template <typename T, typename F>
typename Tree<T,F>::Iterator Tree<T,F>::search_id( Tree<T,F>::Iterator itr, F f ){

    Tree<T,F>::Iterator rtn = Tree<T,F>::Iterator();

    if ( itr.pos_ != NULL ){

        rtn = itr;
        
        while( rtn.id() != f ){

            rtn.next();
            if ( rtn.pos_ == itr.pos_ ){

                std::cout << "TreeFail : Node Not Found" << std::endl;
                break;
            }
        }

        // If node hasn't be found, return null iterator
        if ( rtn.id() != f ){

            rtn = Tree<T,F>::Iterator();
        }
    }

    return rtn;
}
