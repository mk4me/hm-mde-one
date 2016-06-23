/********************************************************************
    created:  2012/12/11
    created:  11:12:2012   9:35
    filename: MRModelLogicVerifier.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___MRMODELLOGICVERIFIER_H__
#define HEADER_GUARD___MRMODELLOGICVERIFIER_H__

#include <list>

//! Forward declarations
namespace df {

	class IModelReader;
	class INode;
	class IProcessingNode;
}

//! Klasa realizuj�ca weryfikacj� logiki modelu na potrzeby przetwarzania.
//! Model sprawdzany jest pod k�tem pod��czonych w�z��w (ka�dy w�ze� powinien
//! by� podpi�ty, elementy przetwarzaj�ce musz� mie� podpi�te wej�cie)
class MRModelLogicVerifier
{
public:
	//! Typ opisuj�cy weryfikacje logiki modelu
	struct ModelVerificationData
	{
		//! Lista w�z��w niepod��czonych
		std::list<df::INode*> unconnectedNodes;
		//! Lista element�w przetwarzaj�cych z niepodpi�tymi wej�ciami
		std::list<df::IProcessingNode*> artificialSources;
	};

public:

	//! \param mode Model do weryfikacji logiki
	//! \patam logicVerification [out] Stan weryfikacji logiki
	//! \return Czy logika modelu zako�czona sukcesem
	static const bool verifyModelLogic(df::IModelReader * model, ModelVerificationData & logicVerification);
};

#endif	//	HEADER_GUARD___MRMODELLOGICVERIFIER_H__
