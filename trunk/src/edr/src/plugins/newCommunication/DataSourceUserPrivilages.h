/********************************************************************
    created:  2012/02/04
    created:  4:2:2012   22:42
    filename: DataSourceUserPrivilages.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCEUSERPRIVILAGES_H__
#define HEADER_GUARD___DATASOURCEUSERPRIVILAGES_H__

#include <plugins/newCommunication/IDataSourceUserPrivilages.h>

//! Implementacja opisuj¹ca uprawnienia u¿ytkownika
class UserPrivilages : public communication::IUserPrivilages
{
public:
    UserPrivilages();
    ~UserPrivilages();
};

#endif	//	HEADER_GUARD___DATASOURCEUSERPRIVILAGES_H__
