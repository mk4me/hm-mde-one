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
namespace utils
{
	class IThreadFactory;
}

////////////////////////////////////////////////////////////////////////////////
namespace df{
////////////////////////////////////////////////////////////////////////////////
	//! Forward declarations
	class IModelReader;
	class IDFLogger;
	class INode;

	//! Wyj¹tki

	//! Wyh¹tek weryfikacji modelu pod k¹tem mo¿liwoœci jego przetwarzania
	class ModelVerificationException : public std::exception
	{
	public:
		ModelVerificationException(const char * e) : std::exception(e) {}
	};

	//! Wyj¹tek inicjalizacji przetwarzania -> mapowania obiektów modelu na obiekty przetwarzaj¹ce
	class ModelRunInitializationException : public std::exception
	{
	public:
		ModelRunInitializationException(const char * e) : std::exception(e) {}
	};

	//! Wyj¹tek startu przetwarzania -> przygotowanie i uruchomienie w¹tków obd³uguj¹cych posdzczególne obiekty modelu
	class ModelStartException : public std::exception
	{
	public:
		ModelStartException(const char * e) : std::exception(e) {}
	};

	//! Wyj¹tek przetwarzania modelu -> coœ posz³o nie tak w dataflow
	class ModelRunningException : public std::exception
	{
	public:
		ModelRunningException(const char * e) : std::exception(e) {}
	};

	//! Klasa realizuj¹ca wielow¹tkowe, potokowe przetwarzanie danych. Ka¿dy wêze³ modelu jest wrapowany przez
	//! dedykowan¹ klasê wraz z obs³uguj¹cycm j¹ w¹tkiem. Przetwarzanie odbywa siê od Ÿróde³ do sinków lub procesorów
	//! z niepodpiêtymi wyjsciami. Tylko Ÿtód³¹ s¹ synchronizowane - czekaj¹ a¿ wszystkie razem skoñcz¹ produkowaæ dane
	//! do modelu, aby sprawdziæ czy s¹ w stanie wszystkie dostarczyæ nowych danych. Jeœli tak ponownie moga produkowac dane,
	//! w przeciwnym wypadku czekamy na przetworzenie danych ju¿ znajduj¹cych siê w modelu. 
	class DFModelRunner : public IDFPausable, public IDFJoinable
	{
	public:
		//! Domyœlny konstruktor
		DFModelRunner();
		//! Destruktor
		~DFModelRunner();

		//! Metoda startuj¹ca model, nieblokuj¹ca -> nale¿y uzyæ join by czekaæ na koniec modelu
		//! \param model Model kóry bêdziemy przetwarzaæ
		//! \param logger Obiekt loguj¹cy stan przetwarzania
		//! \param tFactory Fabryka w¹tków - gdy pusta u¿ywamy domyslnej wynikaj¹cej z implementacji
		void start(IModelReader * model, IDFLogger * logger, utils::IThreadFactory * tFactory = nullptr);

		//! Natychmiastowe przerwania przetwarzania
		//! Metoda blokuj¹ca - czeka na zakoñczenie wszystkich w¹tków
		void stop();

		//! \param node Wêze³ który chcemy pauzowaæ
		virtual void pause(INode * node);
		//! \param node Wêze³ który chcemy przywróciæ do pracy
		virtual void resume(INode * node);

		//! Pauzowanie przetwrarzania
		virtual void pause();
		//! Przywracanie przetwarzania
		virtual void resume();
		//! Czy przetwarzanie wstrzymane
		virtual const bool paused() const;

		//! Metoda blokuj¹ca, czeka a¿ model zakoñczy przetwarzanie w jakikolwiek sposób,
		//! poprawnie b¹dŸ z b³êdem - wtedy rzuca wyj¹tkiem i zwalnia blokadê.
		virtual void join();

		//! \param reader Model który potencjalnie ma byæ uruchomiony 
		//! \return Czy model mo¿na uruchomiæ za pomoca tego runnera
		static const bool verifyModel(const IModelReader * reader);

	private:
		//! Forward declaration
		class DFModelRunnerImpl;

	private:
		//! Hidden implementation
		DFModelRunnerImpl * privImpl;
	};

}

#endif	//	HEADER_GUARD___DFMODELRUNNER_H__
