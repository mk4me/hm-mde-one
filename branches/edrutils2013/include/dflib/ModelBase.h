/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   11:25
    filename: ModelBase.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___MODELBASE_H__
#define HEADER_GUARD___MODELBASE_H__

#include <dflib/INode.h>
#include <dflib/IPin.h>

////////////////////////////////////////////////////////////////////////////////
namespace df{
////////////////////////////////////////////////////////////////////////////////

	//! Forward declaration
	class INode;

	//! Implementacja bazy modelu
	class ModelBase
	{
	public:
		//! Publiczny virtualny destruktor
		virtual ~ModelBase();

	protected:
		//! Chroniony domy�lny konstruktor
		ModelBase();

		//! \param node W�ze� dodawany do modelu
		void attachNode(INode * node);

		//! \param node W�ze� usuwany z modelu
		void detachNode(INode * node);

		//! \param pin Pin do kt�rego dodajemy po��czenie
		//! \param connection Po��czenie dodawane do pinu
		static void addConnection(IPin * pin, IConnection * connection);

		//! \param pin Pin z kt�rego usuwamy po��czenie
		//! \param connection Po��czenie usuwane z pinu
		static void removeConnection(IPin * pin, IConnection * connection);

		//! \param node W�ze� sink kt�rego pin wej�ciowy jest ��czony
		//! \param pin Pin wej�ciowy kt�ry jest ��czony
		static void connectInput(ISinkNode * node, IInputPin * pin);

		//! \param node W�ze� sink kt�rego pin wej�ciowy jest roz�anczany
		//! \param pin Pin wej�ciowy roz�anczany
		static void disconnectInput(ISinkNode * node, IInputPin * pin);

		//! \param node W�ze� �r�d�owy kt�rego pin wyj�ciowy jest ��czony
		//! \param pin Pin wyj�ciowy kt�ry jest ��czony
		static void connectOutput(ISourceNode * node, IOutputPin * pin);

		//! \param node W�ze� �r�d�owy kt�rego pin wyj�ciowy jest roz�anczany
		//! \param pin Pin wyj�ciowy kt�ry jest roz�anczany
		static void disconnectOutput(ISourceNode * node, IOutputPin * pin);
	};

}

#endif	//	HEADER_GUARD___MODELBASE_H__
