/********************************************************************
	created:  2012/12/17
	created:  17:12:2012   8:30
	filename: DFModelRunner.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___STDFMODELRUNNER_H__
#define HEADER_GUARD___STDFMODELRUNNER_H__

#include <dflib/IDFFeatures.h>
#include <stdexcept>


////////////////////////////////////////////////////////////////////////////////
namespace df{
	////////////////////////////////////////////////////////////////////////////////
	//! Forward declarations
	class IModelReader;
	class IDFLogger;
	class INode;


	//! Klasa realizuj�ca wielow�tkowe, potokowe przetwarzanie danych. Ka�dy w�ze� modelu jest wrapowany przez
	//! dedykowan� klas� wraz z obs�uguj�cycm j� w�tkiem. Przetwarzanie odbywa si� od �r�de� do sink�w lub procesor�w
	//! z niepodpi�tymi wyjsciami. Tylko �t�d�� s� synchronizowane - czekaj� a� wszystkie razem sko�cz� produkowa� dane
	//! do modelu, aby sprawdzi� czy s� w stanie wszystkie dostarczy� nowych danych. Je�li tak ponownie moga produkowac dane,
	//! w przeciwnym wypadku czekamy na przetworzenie danych ju� znajduj�cych si� w modelu.
	class STDFModelRunner : public IDFPausable
	{
	public:
		//! Domy�lny konstruktor
		STDFModelRunner();
		//! Destruktor
		~STDFModelRunner();

		//! Metoda startuj�ca model, nieblokuj�ca -> nale�y uzy� join by czeka� na koniec modelu
		//! \param model Model k�ry b�dziemy przetwarza�
		//! \param logger Obiekt loguj�cy stan przetwarzania
		//! \param tFactory Fabryka w�tk�w - gdy pusta u�ywamy domyslnej wynikaj�cej z implementacji
		void start(IModelReader * model, IDFLogger * logger);

		//! Natychmiastowe przerwania przetwarzania
		//! Metoda blokuj�ca - czeka na zako�czenie wszystkich w�tk�w
		void stop();

		//! \param node W�ze� kt�ry chcemy pauzowa�
		virtual void pause(INode * node);
		//! \param node W�ze� kt�ry chcemy przywr�ci� do pracy
		virtual void resume(INode * node);

		//! Pauzowanie przetwrarzania
		virtual void pause();
		//! Przywracanie przetwarzania
		virtual void resume();
		//! Czy przetwarzanie wstrzymane
		virtual const bool paused() const;


		//! \param reader Model kt�ry potencjalnie ma by� uruchomiony
		//! \return Czy model mo�na uruchomi� za pomoca tego runnera
		static const bool verifyModel(const IModelReader * reader);

	private:
		//! Forward declaration
		class STDFModelRunnerImpl;

	private:
		//! Hidden implementation
		STDFModelRunnerImpl * privImpl;
	};
}

#endif	//	HEADER_GUARD___DFMODELRUNNER_H__
