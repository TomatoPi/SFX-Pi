/**
 * Function to Instance the object
 */

template<class T> T* Singleton<T>::m_instance = NULL;
template<class T> bool Singleton<T>::m_ok = false;

template<class T>
void Singleton<T>::Create(){

    if ( !m_ok ){

        m_instance = new T();
        m_ok = true;
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

    if ( !m_ok ){

        Singleton<T>::Create();
        std::cout << "Warning Singleton Called Before Created" << std::endl;
    }

    return *m_instance;
}

/**
 * Function to Destroy Current Instance
 */
template<class T>
void Singleton<T>::Kill(){

    if ( m_ok ){
        
        delete m_instance;
        m_ok = false;
        //std::cout << "Singleton Object Deleted" << std::endl;
    }
}
