/********************************************************************
	created:  2012/12/17
	created:  17:12:2012   8:30
	filename: DFModelRunner.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___DFMODELRUNNER_H__
#define HEADER_GUARD___DFMODELRUNNER_H__

#include <dflib/IDFFeatures.h>
#include <stdexcept>

//! Forward declaration
namespace threadingUtils
{
	class IThreadPool;
}

////////////////////////////////////////////////////////////////////////////////
namespace df{
	////////////////////////////////////////////////////////////////////////////////
	//! Forward declarations
	class IModelReader;
	class IDFLogger;
	class INode;

	//! Wyj�tki

	//! Wyh�tek weryfikacji modelu pod k�tem mo�liwo�ci jego przetwarzania
	class ModelVerificationException : public std::runtime_error
	{
	public:
		ModelVerificationException(const char * e) : std::runtime_error(e) {}
	};

	//! Wyj�tek inicjalizacji przetwarzania -> mapowania obiekt�w modelu na obiekty przetwarzaj�ce
	class ModelRunInitializationException : public std::runtime_error
	{
	public:
		ModelRunInitializationException(const char * e) : std::runtime_error(e) {}
	};

	//! Wyj�tek startu przetwarzania -> przygotowanie i uruchomienie w�tk�w obd�uguj�cych posdzczeg�lne obiekty modelu
	class ModelStartException : public std::runtime_error
	{
	public:
		ModelStartException(const char * e) : std::runtime_error(e) {}
	};

	//! Wyj�tek przetwarzania modelu -> co� posz�o nie tak w dataflow
	class ModelRunningException : public std::runtime_error
	{
	public:
		ModelRunningException(const char * e) : std::runtime_error(e) {}
	};

//	//! Klasa realizuj�ca wielow�tkowe, potokowe przetwarzanie danych. Ka�dy w�ze� modelu jest wrapowany przez
//	//! dedykowan� klas� wraz z obs�uguj�cycm j� w�tkiem. Przetwarzanie odbywa si� od �r�de� do sink�w lub procesor�w
//	//! z niepodpi�tymi wyjsciami. Tylko �t�d�� s� synchronizowane - czekaj� a� wszystkie razem sko�cz� produkowa� dane
//	//! do modelu, aby sprawdzi� czy s� w stanie wszystkie dostarczy� nowych danych. Je�li tak ponownie moga produkowac dane,
//	//! w przeciwnym wypadku czekamy na przetworzenie danych ju� znajduj�cych si� w modelu.
//	class DFModelRunner : public IDFPausable, public IDFJoinable
//	{
//	public:
//		//! Domy�lny konstruktor
//		DFModelRunner();
//		//! Destruktor
//		~DFModelRunner();
//
//		//! Metoda startuj�ca model, nieblokuj�ca -> nale�y uzy� join by czeka� na koniec modelu
//		//! \param model Model k�ry b�dziemy przetwarza�
//		//! \param logger Obiekt loguj�cy stan przetwarzania
//		//! \param tFactory Fabryka w�tk�w - gdy pusta u�ywamy domyslnej wynikaj�cej z implementacji
//		void start(IModelReader * model, IDFLogger * logger, threadingUtils::IThreadPool * tPool);
//
//		//! Natychmiastowe przerwania przetwarzania
//		//! Metoda blokuj�ca - czeka na zako�czenie wszystkich w�tk�w
//		void stop();
//
//		//! \param node W�ze� kt�ry chcemy pauzowa�
//		virtual void pause(INode * node);
//		//! \param node W�ze� kt�ry chcemy przywr�ci� do pracy
//		virtual void resume(INode * node);
//
//		//! Pauzowanie przetwrarzania
//		virtual void pause();
//		//! Przywracanie przetwarzania
//		virtual void resume();
//		//! Czy przetwarzanie wstrzymane
//		virtual const bool paused() const;
//
//		//! Metoda blokuj�ca, czeka a� model zako�czy przetwarzanie w jakikolwiek spos�b,
//		//! poprawnie b�d� z b��dem - wtedy rzuca wyj�tkiem i zwalnia blokad�.
//		virtual void join();
//
//		//! \param reader Model kt�ry potencjalnie ma by� uruchomiony
//		//! \return Czy model mo�na uruchomi� za pomoca tego runnera
//		static const bool verifyModel(const IModelReader * reader);
//
//	private:
//		//! Forward declaration
//		class DFModelRunnerImpl;
//
//	private:
//		//! Hidden implementation
//		DFModelRunnerImpl * privImpl;
//	};
}

#endif	//	HEADER_GUARD___DFMODELRUNNER_H__
