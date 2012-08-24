/********************************************************************
    created:  2011/05/20
    created:  20:5:2011   11:33
    filename: EDRDataFlow.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___EDRDATAFLOW_H__
#define HEADER_GUARD___EDRDATAFLOW_H__

#ifdef IN
#undef IN
#endif

#ifdef OUT
#undef OUT
#endif

#include <dfmlib/DFModel.h>

class EDRDataFlow : public dflm::DFModel
{
public:
    EDRDataFlow();

    ~EDRDataFlow();

    //! \param node W�ze� kt�rego kompatybilno�� z modelem sprawdzamy
    //! \return Czy w�ze� jest wspierany przez model
    virtual bool isNodeSupported(const dflm::NPtr & node) const;

protected:

    //! \param connection Po�aczenie do usuni�cia z modelu
    virtual void beforeRemoveConnection(const dflm::ConnPtr & connection);

    //! \param src Pin �r�d�owy (wyj�ciowy)
    //! \param src Pin docelowy (wej�ciowy)
    virtual void afterConnect(const dflm::ConnPtr & connection);

};

#endif  //  HEADER_GUARD___EDRDATAFLOW_H__