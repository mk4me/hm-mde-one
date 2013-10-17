/********************************************************************
	created:	2013/10/11
	created:	11:10:2013   10:13
	filename: 	AbstractSerie.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORELIB__ABSTRACTSERIE_H__
#define HEADER_GUARD_CORELIB__ABSTRACTSERIE_H__


#include <corelib/IVisualizer.h>

namespace plugin {

    class CORELIB_EXPORT AbstractSerie : public plugin::IVisualizer::ISerie
    {
    public:
        virtual ~AbstractSerie() {}

    public:
        virtual void setName( const std::string & name );
        virtual const std::string getName() const;
        virtual const core::ObjectWrapperConstPtr & getData() const;
        virtual const core::TypeInfo & getRequestedDataType() const;
        virtual void setData( const core::TypeInfo & requestedDataType, const core::ObjectWrapperConstPtr & data );

    public:
        virtual void setupData( const core::ObjectWrapperConstPtr & data ) = 0;

    protected:
        //! wrapper przekazany serii
        core::ObjectWrapperConstPtr data;
        utils::TypeInfo requestedType;
        //! nazwa serii
        std::string name;
    };
    DEFINE_SMART_POINTERS(AbstractSerie);
}
#endif
