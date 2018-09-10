/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IDManager.h
 * Author: Space-Computer
 *
 * Created on 4 juin 2018, 20:58
 */

#ifndef IDMANAGER_H
#define IDMANAGER_H

#include <set>

template <typename T>
class UIDManager
{
public :
    
    template <class name = void>
    UIDManager(const T& first):_firstValidUID(first),_usedUIDs(),_forbidenUIDs()
    {
    }
    template <typename F, typename ...Fs>
    UIDManager(const T& first, const F& f1, const Fs& ...f2):
    UIDManager(first, f2...)
    {
        forbidUID(f1);
    }
    
    /**
     * @brief method to get a new unique ID
     */
    T reserveUID()
    {
        if (_usedUIDs.find(_firstValidUID) == _usedUIDs.end())
        {
            _usedUIDs.insert(_firstValidUID);
            return _firstValidUID;
        }
        ++_firstValidUID;
        return reserveUID();
    }
    
    /**
     * @brief method to reserve a known ID
     * @warning this method doesn't garantee uniqueness of given id
     *  or if id is not forbidden
     */
    T reserveUID(const T& uid)
    {
        _usedUIDs.insert(uid);
        return uid;
    }
    
    /**
     * Method to call when an identifier is no longuer used
     * @param uid
     */
    void releaseUID(const T& uid)
    {
        if (_forbidenUIDs.find(uid) == _forbidenUIDs.end())
        {
            _usedUIDs.erase(uid);
            _firstValidUID = uid;
        }
    }
    
    void forbidUID(const T& uid)
    {
        _usedUIDs.insert(uid);
        _forbidenUIDs.insert(uid);
    }
    
private :
    
    T           _firstValidUID; /**< ID that will be used by the next Unit */
    std::set<T> _usedUIDs;      /**< Set of used ID's */
    std::set<T> _forbidenUIDs;  /**< Set of forbidden ID's */
};

#endif /* IDMANAGER_H */

