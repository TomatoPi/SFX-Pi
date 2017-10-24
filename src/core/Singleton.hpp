/**
 * Function to Instance the object
 */

template<class T> T* Singleton<T>::i_ = NULL;
template<class T> bool Singleton<T>::ok_ = false;

template<class T>
void Singleton<T>::Create(){

    if ( !ok_ ){

        i_ = new T();
        ok_ = true;
    }
    else{
        
        std::cout << "Error : Singleton Already Created" << std::endl;
    }
}

/**
 * Function to Get The Current Instance
 */
template<class T>
T& Singleton<T>::Get(){

    if ( !ok_ ){

        Singleton<T>::Create();
        std::cout << "Warning Singleton Called Before Created" << std::endl;
    }

    return *i_;
}

/**
 * Function to Destroy Current Instance
 */
template<class T>
void Singleton<T>::Kill(){

    if ( ok_ ){
        
        delete i_;
        ok_ = false;
        //std::cout << "Singleton Object Deleted" << std::endl;
    }
}
