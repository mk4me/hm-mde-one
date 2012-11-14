/********************************************************************
    created:  2012/02/04
    created:  4:2:2012   22:41
    filename: DataSourceUserData.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCEUSERDATA_H__
#define HEADER_GUARD___DATASOURCEUSERDATA_H__

#include <plugins/newCommunication/IDataSourceUserData.h>

//! Implementacja zawierająca specyficzne dla użytkownika dane
class UserData : public communication::IUserData
{
public:
    UserData();
    ~UserData();
};



#endif	//	HEADER_GUARD___DATASOURCEUSERDATA_H__
