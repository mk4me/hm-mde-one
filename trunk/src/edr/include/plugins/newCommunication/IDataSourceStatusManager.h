/********************************************************************
    created:  2012/02/06
    created:  6:2:2012   9:23
    filename: IDataSourceStatusManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCESTATUSMANAGER_H__
#define HEADER_GUARD___IDATASOURCESTATUSMANAGER_H__

#include <plugins/newCommunication/IDataSourceStatus.h>
#include "DataSourceStatus.h"

namespace communication
{

//! Interfejs zapewniaj¹cy dostêp do statusu danych
class IDataSourceStatusManager
{
public:
    //! Destruktor wirtualny
    virtual ~IDataSourceStatusManager() {}

    //! \param disorderID Identyfikator schorzenia
    //! \return Status schorzenia
    virtual const DataStatus & disorderStatus(int disorderID) const = 0;
    virtual const DataStatus & patientStatus(int patientID) const = 0;
    virtual const DataStatus & subjectStatus(int subjectID) const = 0;
    virtual const DataStatus & sessionStatus(int sessionID) const = 0;
    virtual const DataStatus & motionStatus(int motionID) const = 0;
    virtual const DataStatus & fileStatus(int fileID) const = 0;
};

}

#endif	//	HEADER_GUARD___IDATASOURCESTATUSMANAGER_H__
