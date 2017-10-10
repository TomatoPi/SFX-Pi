/**
 * Function For Instance the object
 */
template<class T> void Singleton<T>::Create(){

    if ( !ok_ ){

        i_ = new T();
        ok_ = true;
    }
    else{
        
        std::cout << "Error : Singleton Already Created" << std::endl;
    }
}

/**
 * Function For Get The Current Instance
 */
template<class T> T& Singleton<T>::Get(){

    if ( !ok_ ){

        Singleton<T>::Create();
        std::cout << "Warning Singleton Called Before Created" << std::endl;
    }

    return *i_;
}

/**
 * Function For Destroy Current Instance
 */
template<class T> void Singleton<T>::Kill(){

    if ( ok_ ){
        
        delete i_;
        ok_ = false;
        std::cout << "Singleton Object Deleted" << std::endl;
    }
}
