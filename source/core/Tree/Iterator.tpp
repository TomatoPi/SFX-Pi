
template <typename T, typename F>
typename Tree<T,F>::Iterator Tree<T,F>::Iterator::next(){

    if ( pos_ != NULL ){

        pos_ = pos_->next_;
    }
    return *this;
}

template <typename T, typename F>
typename Tree<T,F>::Iterator Tree<T,F>::Iterator::prev(){

    if ( pos_ != NULL ){

        pos_ = pos_->prev_;
    }
    return *this;
}

template <typename T, typename F>
bool Tree<T,F>::Iterator::has_child(){

    if ( pos_ != NULL ) return pos_->child_ != NULL;
    return false;
}

template <typename T, typename F>
typename Tree<T,F>::Iterator Tree<T,F>::Iterator::child(){

    if ( pos_ != NULL ){
        if ( pos_->child_ != NULL ){

            pos_ = pos_->child_;
        }
    }

    return *this;
}

template <typename T, typename F>
bool Tree<T,F>::Iterator::has_parent(){

    if ( pos_ != NULL ) return pos_->parent_ != NULL;
    return false;
}

template <typename T, typename F>
typename Tree<T,F>::Iterator Tree<T,F>::Iterator::parent(){

    if ( pos_ != NULL ){
        if ( pos_->parent_ != NULL ){

            pos_ = pos_->parent_;
        }
    }

    return *this;
}

template <typename T, typename F>
T Tree<T,F>::Iterator::get(){

    if ( pos_ != NULL ) return pos_->data_;
    return T();
}

template <typename T, typename F>
F Tree<T,F>::Iterator::id(){

    if ( pos_ != NULL ) return pos_->id_;
    return NULL;
}

template <typename T, typename F>
bool Tree<T,F>::Iterator::ok(){

    return pos_ != NULL;
}
