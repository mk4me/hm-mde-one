/********************************************************************
    created:  2012/02/05
    created:  5:2:2012   17:01
    filename: IDataSourceUserPrivilages.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCEUSERPRIVILAGES_H__
#define HEADER_GUARD___IDATASOURCEUSERPRIVILAGES_H__

namespace communication
{

//! Interfejs zapewniający dostęp do praw użytkownika względem danych w bazie danych
class IUserPrivilages
{
public:
    virtual ~IUserPrivilages() {}
};

}

#endif	//	HEADER_GUARD___IDATASOURCEUSERPRIVILAGES_H__
