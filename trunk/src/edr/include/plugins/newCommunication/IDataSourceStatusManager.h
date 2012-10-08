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
// rev - publiczny naglowek wciaga prywatny
//#include "DataSourceStatus.h"
#include <plugins/newCommunication/IDataSourceStatus.h>

namespace communication
{

//! Interfejs zapewniający dostęp do statusu danych
class IDataSourceStatusManager
{
public:
    //! Destruktor wirtualny
    virtual ~IDataSourceStatusManager() {}

    //! \param disorderID Identyfikator schorzenia
    //! \return Status schorzenia
    // rev - DataStatus -> IDataStatus
    virtual const IDataStatus & disorderStatus(int disorderID) const = 0;
    virtual const IDataStatus & patientStatus(int patientID) const = 0;
    virtual const IDataStatus & subjectStatus(int subjectID) const = 0;
    virtual const IDataStatus & sessionStatus(int sessionID) const = 0;
    virtual const IDataStatus & motionStatus(int motionID) const = 0;
    virtual const IDataStatus & fileStatus(int fileID) const = 0;
};

}

#endif	//	HEADER_GUARD___IDATASOURCESTATUSMANAGER_H__
