
template <typename T>
LinkedList<T>::LinkedList():first_(NULL), size_(0)
{
}

template <typename T>
LinkedList<T>::~LinkedList(){

    clear();
}

template <typename T>
void LinkedList<T>::push_back( T el ){

    ListElement<T> *newe = new ListElement<T>( el );

    if ( first_ == NULL ){

        first_ = newe;
        last_ = newe;
    }
    else if ( last_ != NULL ){

        last_->next_ = newe;
        newe->prev_ = last_;
        last_ = newe;
    }
    else{

        last_ = this->get_last();

        if ( last_ == NULL ) return;
        
        last_->next_ = newe;
        newe->prev_ = last_;
        last_ = newe;
    }

    ++size_;
}

template <typename T>
void LinkedList<T>::push_front( T el ){

    ListElement<T> *newe = new ListElement<T>( el );

    if ( first_ == NULL ){

        first_ = newe;
        last_ = newe;
    }
    else{

        first_->prev_ = newe;
        newe->next_ = first_;
        first_ = newe;
    }

    ++size_;
}

template <typename T>
void LinkedList<T>::remove( size_t idx ){

    if ( idx < size_ ){

        size_t i = 0;
        ListElement<T> *cur = first_;

        // While targeted index not reached and current Element not NULL
        while ( i != idx && cur != NULL && cur->next_ != NULL){

            cur = cur->next_;
            i++;
        }
        // If targeted element was reached
        if ( i == idx ){

            // Save pointer to next element
            if ( cur->prev_ != NULL ){

                cur->prev_->next_ = cur->next_;
            }
            else if ( cur == first_ ){

                first_ = cur->next_;
            }
            // Save Next element's pointer to prev element
            if ( cur->next_ != NULL ){

                cur->next_->prev_ = cur->prev_;
            }
            else if ( cur == last_ ){

                last_ = cur->prev_;
            }

            delete cur;
            --size_;
        }
    }
}

template <typename T>
void LinkedList<T>::clear(){

    if ( first_ != NULL ){

        ListElement<T>* cur = first_;
        ListElement<T>* next = cur->next_;
        do{

            delete cur;
            
            cur = next;
            if ( next != NULL ){

                next = next->next_;
            }

        }while( cur != NULL );

        first_ = NULL;
        last_ = NULL;
        size_ = 0;
    }
}

template <typename T>
size_t LinkedList<T>::size() const{

    return size_;
}

template <typename T>
T LinkedList<T>::first(){

    if ( first_ == NULL ) return NULL;

    return first_->data_;
}

template <typename T>
T LinkedList<T>::last(){

    if ( last_ == NULL ) return NULL;
    
    return last_->data_;
}

template <typename T>
LinkedList<T>::ListElement<T>* LinkedList<T>::get_last(){

    if ( first_ == NULL) return NULL;
    
    ListElement<T> *cur = first_;

    while ( cur->next_ != NULL ){

        cur = cur->next_;
    }

    return cur;
}
