/********************************************************************
    created:  2012/12/11
    created:  11:12:2012   8:14
    filename: MRModelInterfaceVerifier.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___MRMODELINTERFACEVERIFIER_H__
#define HEADER_GUARD___MRMODELINTERFACEVERIFIER_H__

#include <algorithm>
#include <list>

//! Forward declarations 
namespace df{

	class IPin;
	class INode;

	class IInputPin;
	class IOutputPin;

	class ISourceNode;
	class ISinkNode;
	class IProcessingNode;

	class IDFInput;
	class IDFOutput;

	class IDFSource;
	class IDFSink;
	class IDFProcessor;

	class IModelReader;
}

//! Klasa weryfikuj�ca interfejsy element�w modelu pod k�tem przetwarzania danych.
//! Realizacja jest oparta o rzutowanie dynamic_castem element�w na konkretne interfejsy.
//! W wyniku mozemy dosta� wyniki rzutowania aby przy dalszym przetwarzaniu nie by�o trzeba
//! tej procedury powtarza�. Veryfikacja obejmuje jedynie pod��czone elementy
class MRModelInterfaceVerifier
{
public:
	//! Typ opisuj�cy interfejs wejscia pod k�tem przetwarzania danych
	typedef std::pair<df::IInputPin*, df::IDFInput*> InputVerificationData;
	//! Typ agreguj�cy opisy wej�� pod k�tem przetwarzania danych
	typedef std::list<InputVerificationData> InputVerification;

	//! Typ opisuj�cy interfejs wyjscia pod k�tem przetwarzania danych
	typedef std::pair<df::IOutputPin*, df::IDFOutput*> OutputVerificationData;
	//! Typ agreguj�cy opisy wyj�� pod k�tem przetwarzania danych
	typedef std::list<OutputVerificationData> OutputVerification;

	//! Typ opisuj�cy interfejs �r�d�� pod k�tem przetwarzania danych
	struct SourceVerificationData
	{
		//! W�ze� �r�d�owy
		df::ISourceNode * node;
		//! Interfejs �r�d�a do produkowania danych na potrzeby przetwarzania
		df::IDFSource * source;
		//! Weryfikacja pin�w wyj�ciowych
		OutputVerification outputVerication;
	};

	//! Typ opisuj�cy interfejs w�z�a terminuj�cego pod k�tem przetwarzania danych
	struct SinkVerificationData
	{
		//! W�ze� terminuj�cy
		df::ISinkNode * node;
		//! Interfejs w�z�a terminuj�cego do konsumowania danych
		df::IDFSink * sink;
		//! Weryfikacja pin�w wej�ciowych
		InputVerification inputVerication;
	};

	//! Typ opisuj�cy interfejs w�z�a przetwarzaj�cego pod k�tem przetwarzania danych
	struct ProcessorVerificationData
	{
		//! W�ze� przetwarzaj�cy
		df::IProcessingNode * node;
		//! Interfejs w�z�a przetwarzaj�cego na potrzeby przetwarzania danych
		df::IDFProcessor * processor;
		//! Weryfikacja pin�w wejsciowych
		InputVerification inputVerication;
		//! Weryfikacja pin�w wyjsciowych
		OutputVerification outputVerication;
	};

	//! Typ agreguj�cy weryfikacje �r�de�
	typedef std::list<SourceVerificationData> SourceVerification;
	//! Typ agreguj�cy weryfikacje sink�w
	typedef std::list<SinkVerificationData> SinkVerification;
	//! Typ agreguj�cy weryfikacje procesor�w
	typedef std::list<ProcessorVerificationData> ProcessorVerification;

	//! Typ agreguj�cy piny nie implementuj�ce wymaganych interfejs�w na potrzeby przetwarzania
	typedef std::list<df::IPin *> IncompatiblePins;
	//! Typ agreguj�cy w�z�y nie implementuj�ce wymaganych interfejs�w na potrzeby przetwarzania
	typedef std::list<df::INode *> IncompatibleNodes;

	//! Typ opisuj�cy model pod k�tem mozliwo�ci przetwarzania danych
	struct ModelVerificationData
	{
		//! Weryfikacja �r�de�
		SourceVerification sourceVerification;
		//! Weryfikacja sink�w
		SinkVerification sinkVerification;
		//! Weryfikacja procesor�w
		ProcessorVerification processorVerification;

		//! Piny nienadaj�ce si� do przetwarzania
		IncompatiblePins incompatiblePins;
		//! W�z�y nienadaj�ce si� do przetwarzania
		IncompatibleNodes incompatibleNodes;
	};

public:
	//! \param model Model do weryfikacji
	//! \param modelVerification Opis veryfikacji modelu
	//! \return Czy modle nadaje si� do przetwarzania
	static const bool verifyModelInterface(df::IModelReader * model, ModelVerificationData & modelVerification);

private:
	//! \param node W�ze� sink do weryfikacji
	//! \param inputVeryfication Opis weryfikacji wej�� w�z�a
	//! \param incompatiblePins Zbi�r pin�w nienadaj�cych sie do przetwarzania
	static void verifyInputPinsInterface(df::ISinkNode * node, InputVerification & inputVerification, IncompatiblePins & incompatiblePins);
	//! \param node W�ze� �r�d�o do weryfikacji
	//! \param outputVeryfication Opis weryfikacji wyj�� w�z�a
	//! \param incompatiblePins Zbi�r pin�w nienadaj�cych sie do przetwarzania
	static void verifyOutputPinsInterface(df::ISourceNode * node, OutputVerification & outputVerification, IncompatiblePins & incompatiblePins);
};

#endif	//	HEADER_GUARD___MRMODELVERIFIER_H__
