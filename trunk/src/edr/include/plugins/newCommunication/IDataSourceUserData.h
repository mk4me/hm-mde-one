/********************************************************************
    created:  2012/02/05
    created:  5:2:2012   17:00
    filename: IDataSourceUserData.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCEUSERDATA_H__
#define HEADER_GUARD___IDATASOURCEUSERDATA_H__

namespace communication
{
	
//! Interfejs zapewniający dostęp do specyficznych danych użytkownika zapisanych w bazie danych
class IUserData
{
public:
    virtual ~IUserData() {}
};

}

#endif	//	HEADER_GUARD___IDATASOURCEUSERDATA_H__
