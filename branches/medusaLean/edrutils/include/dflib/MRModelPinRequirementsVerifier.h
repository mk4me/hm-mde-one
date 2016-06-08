/********************************************************************
    created:  2012/12/11
    created:  11:12:2012   8:48
    filename: MRModelPinRequirementsVerifier.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___MRMODELPINREQUIREMENTSVERIFIER_H__
#define HEADER_GUARD___MRMODELPINREQUIREMENTSVERIFIER_H__

#include <list>

//! Forward declarations
namespace df{

	class IExtendedInputPin;
	class IExtendedOutputPin;

	class ISinkNode;
	class ISourceNode;

	class IModelReader;
}

//! Klasa realizuj�ca weryfikacj� opisu przej�� w�z��w. Operacja ta obejmuje
//! sprawdzenie czy piny s� opisane rozszerzonymi interfejsami i czy te wymagania
//! s� spe�nione.
class MRModelPinRequirementsVerifier
{
public:
	//! Typ opisuj�cy agregat pin�w wej�ciowych z niespe�nionymi zale�no�ciami (wymagane a niepodpi�te)
	typedef std::list<df::IExtendedInputPin*> UnsatisfiedInputs;
	//! Typ opisuj�cy agregat pin�w wyj�ciowych z niespe�nionymi zale�no�ciami (niepidpi�te zale�no�ci a pin podpi�ty)
	typedef std::list<df::IExtendedOutputPin*> UnsatisfiedOutputs;

	//! Typ opisuj�cy weryfikacj� opisu przej��
	struct UnsatisfiedPins
	{
		//! Piny wej�ciowe z niespe�nionymi zale�no�ciami
		UnsatisfiedInputs unsatisfiedInputs;
		//! Piny wyj�ciowe z niespe�nionymi zale�no�ciami
		UnsatisfiedOutputs unsatisfiedOutputs;
	};

public:

	//! \param model Model do weryfikacji
	//! \param unsatisfiedPins [out] Stan weryfikacji opisu przejs� modelu
	//! \return Czy model jest poprawny pod k�tem opisu przej��
	static const bool verifyModelPinRequirements(df::IModelReader * model, UnsatisfiedPins & unsatisfiedPins);

private:
	//! \param node W�ze� sink do weryfikacji
	//! \param unsatisfiedInputs [out] Wej�cia z niespe�nonymi wymaganiami
	static void verifyInputPinRequirements(df::ISinkNode * node, UnsatisfiedInputs & unsatisfiedInputs);
	//! \param node W�ze� �r�d�owy do weryfikacji
	//! \param unsatisfiedOutputs [out] Wyj�cia z niespe�nonymi wymaganiami
	static void verifyOutputPinRequirements(df::ISourceNode * node, UnsatisfiedOutputs & unsatisfiedOutputs);
};

#endif	//	HEADER_GUARD___MRMODELPINREQUIREMENTSVERIFIER_H__
