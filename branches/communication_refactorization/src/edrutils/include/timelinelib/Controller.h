/********************************************************************
	created:  2011/07/19
	created:  19:7:2011   14:17
	filename: Controller.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_TIMELINE__CONTROLLER_H__
#define HEADER_GUARD_TIMELINE__CONTROLLER_H__

#include <OpenThreads/Thread>
#include <OpenThreads/Mutex>
#include <OpenThreads/ReentrantMutex>
#include <OpenThreads/ScopedLock>

#include <timelinelib/Types.h>
#include <timelinelib/View.h>
#include <timelinelib/Model.h>

#include <utils/SmartPtr.h>
#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <utils/ObserverPattern.h>

namespace timeline{
	class IController : public utils::Observable<State>
	{
	public:

		//! Sposób aktualizacji czasu
		enum TimeUpdateMode {
			AsynchTimeUpdate, //! Asynchroniczna aktualizacja czasu - przyjscie nowego czasu przerywa poprzednią aktualizację jeśli taka była i rozpoczyna aktualizację od nowa
			SynchTimeUpdate     //! Czeka aż skończy się poprzenia aktualizacja czasu, ustawia nowy czas do kolejnej aktualizacji
		};

		//! Kierunek odtwarzania timeline
		enum PlaybackDirection {
			PlayForward = 1,    // odtwarzanie do przodu
			PlayBackward = -1    // odtwarzanie do tyłu
		};

		IController(State * state) : utils::Observable<State>(state) {}

		//! \return Aktualny model kontrolera
		virtual const ModelConstPtr & getModel() const = 0;

		virtual double getNormalizedTime(double time) const = 0;
		virtual double getNormalizedLength(double length) const = 0;
		virtual std::pair<double, double> getNormalizedRange(double begin, double length) const = 0;

		virtual double getAbsoluteTime(double normalizedTime) const = 0;
		virtual double getAbsoluteLength(double normalizedLength) const = 0;
		virtual std::pair<double, double> getAbsoluteRange(double begin, double length) const = 0;

		//------------------------------- Konfiguracja aktualizacji czasu -----------------------

		//! \param timeUpdateMode Sposób aktualizacji czasu
		virtual void setTimeUpdateMode(TimeUpdateMode timeUpdateMode) = 0;

		//! \return Sposób aktualizacji czasu
		virtual TimeUpdateMode getTimeUpdateMode() const = 0;

		//! \param playbackDirection Kierunek aktualizacji czasu w timeline
		virtual void setPlaybackDirection(PlaybackDirection playbackDirection) = 0;

		//! \return Kierunek aktualizacji czasu w timeline
		virtual PlaybackDirection getPlaybackDirection() const = 0;

		//------------------------------- Zarzadzanie odtwarzaniem ------------------------------

		//! Uruchamia timeline - odtwarzanie
		virtual void play() = 0;

		//! Wstrzymuje odtwarzanie timeline
		virtual void pause() = 0;

		//! Zatrzymuje odtwarzanie, wraca do "początku" - w zależności od kierunku odtwarzania
		//virtual void stop() = 0;

		//! \return Czy timeline jest odtwarzany
		virtual bool isPlaying() const = 0;

		//! \return Czy stan timeline jest "brudny"
		virtual bool isDirty() const = 0;

		//! \return Czy czas timeline jest "brudny"
		virtual bool isTimeDirty() const = 0;

		//! \return Czy kontroler jest zajęty?
		virtual bool isBusy() const = 0;

		//------------------------------- Operacje na timeline -----------------------------------

		//! \return długość kanału w sekundach, uwzględniając skale
		virtual double getLength() const = 0;

		//! \return Aktualny czas kanału
		virtual double getTime() const = 0;

		//! \return Znormalizowany czas timeline
		virtual double getNormalizedTime() const = 0;

		//! \return Skala czasu kanału
		virtual double getTimeScale() const = 0;

		//! \param timeScale Nowa skala czasowa
		virtual void setTimeScale(double timeScale) = 0;

		//! propaguje zmiane na wszystkie aktywne podkanały
		//! sprawdza maske i offset
		//! \param time Aktualny czas timeline, 0 <= time <= length
		//! \param lockHolder Obiekt chcacy aktualizować czas, trzymający blokade
		virtual void setTime(double time) = 0;

		//! propaguje zmiane na wszystkie aktywne podkanały
		//! sprawdza maske i offset
		//! \param normTime Aktualny czas timeline, 0 <= time <= length
		//! \param lockHolder Obiekt chcacy aktualizować czas, trzymający blokade
		virtual void setNormalizedTime(double normTime) = 0;

		//! \param path Sciezka nowego kanału
		//! \param channel fatyczny kanał dotarczony przez klienta
		virtual void addChannel(const std::string & path, const IChannelPtr & channel) = 0;

		//! \param channels Kanały do dodawnia w formie ściezka -> kanał, kanał nie powiniem być nullptr
		virtual void addChannels(const std::map<std::string, IChannelPtr> & channels) = 0;

		//! \param path Sciezka kanału do usuniecia
		virtual void removeChannel(const std::string & path) = 0;

		virtual void removeChannels(const std::set<std::string> & paths) = 0;

		virtual void setChannelActive(const std::string & path, bool active) = 0;

		//! \param path Sciezka do szukanego kanału
		//! \return wskaźnik do szukanego kanału lub pusty wskaźnik jeśli nie znaleziono
		virtual Model::TChannelConstPtr findChannel(const std::string & path) const = 0;
	};

	class Controller : public IController, public OpenThreads::ReentrantMutex
	{
	protected:

		//! Typ lokalnego lockowania mutexa
		typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopedLock;

	private:

		typedef boost::function<double(Controller *, unsigned long int)> TimeGenerator;

		class Timer : public OpenThreads::Thread
		{
		public:
			Timer(unsigned long int delay)
				: delay(delay)
			{
				UTILS_ASSERT((delay != 0), "Zły czas pomiędzy tikami tegara");
			}

			virtual void run()
			{
				if (controller == nullptr){
					throw std::runtime_error("Wrong controller to update");
				}

				while (true){
					{
						// czy nie ma pauzy
						ScopedLock(controller->pauseMutex);
					}

					//aktualizujemy czas
					if (controller->setNextTime(delay) == true){
						return;
					}

					//zasypiamy
					OpenThreads::Thread::microSleep(delay);
				}
			}

			void setController(Controller * controller)
			{
				UTILS_ASSERT((controller != nullptr), "Błędny kontroler do aktualizacji");
				this->controller = controller;
			}

			void setDelay(unsigned long int delay)
			{
				UTILS_ASSERT((delay > 0), "Blędne opóźnienie zegara");
				this->delay = delay;
			}

			unsigned long int getDelay() const
			{
				return delay;
			}

		private:
			Controller * controller;
			unsigned long int delay;
		};

		friend class Timer;

		typedef utils::shared_ptr<Timer> TimerPtr;

	private:
		//! Model logiczny timeline
		ModelPtr model;
		//! Model logiczny timeline w wersji const
		ModelConstPtr constModel;

		//! Wewnętrzny timer
		TimerPtr timer;

		//! Sposób aktualizacji czasu
		TimeUpdateMode timeUpdateMode;

		//! Kierunek aktualizacji czasu
		PlaybackDirection playbackDirection;

		//! Mutex do synchronizacji edycji modelu (struktura kanałów)
		OpenThreads::ReentrantMutex modelMutex;

		//! Mutex do edycji odtwarzania timeline
		mutable OpenThreads::ReentrantMutex stateMutex;

		//! Mutex do zarzadzania odtwarzaniem timeline - wstrzymywania go
		OpenThreads::Mutex pauseMutex;

		//! Czy kontroler jest zajęty?
		volatile bool busy;

		//! Czy czas modelu i timeline są różne
		bool dirty;

		//! "Brudny" stan timeline
		State dirtyState;

		//! Czy czas kontrolera i modelu są rózne
		bool timeDirty;

		//! Generator nowego czasu dla timera
		TimeGenerator timeGenerator;

		//! Informacja czy jawnie zapauzowano odtwarzanie czy osiągnięto limit czasu (true - pauza, false - limit czasu)
		bool paused;

	public:

		Controller();
		virtual ~Controller();

		//------------------------------- Konfiguracja kontrolera ------------------------------

		//! \param model Model do ustawienia w kontrolerze
		//virtual void setModel(const timeline::ModelPtr & model);

		//! \return Aktualny model kontrolera
		virtual const ModelConstPtr & getModel() const;

		virtual double getNormalizedTime(double time) const;
		virtual double getNormalizedLength(double length) const;
		virtual std::pair<double, double> getNormalizedRange(double begin, double length) const;

		virtual double getAbsoluteTime(double normalizedTime) const;
		virtual double getAbsoluteLength(double normalizedLength) const;
		virtual std::pair<double, double> getAbsoluteRange(double begin, double length) const;

		//------------------------------- Konfiguracja aktualizacji czasu -----------------------

		//! \param timeUpdateMode Sposób aktualizacji czasu
		virtual void setTimeUpdateMode(TimeUpdateMode timeUpdateMode);

		//! \return Sposób aktualizacji czasu
		virtual TimeUpdateMode getTimeUpdateMode() const;

		//! \param playbackDirection Kierunek aktualizacji czasu w timeline
		virtual void setPlaybackDirection(PlaybackDirection playbackDirection);

		//! \return Kierunek aktualizacji czasu w timeline
		virtual PlaybackDirection getPlaybackDirection() const;

		//------------------------------- Zarzadzanie odtwarzaniem ------------------------------

		//! Uruchamia timeline - odtwarzanie
		virtual void play();

		//! Wstrzymuje odtwarzanie timeline
		virtual void pause();

		//virtual void stop();

		//! \return Czy timeline jest odtwarzany
		virtual bool isPlaying() const;

		//! \return Czy stan timeline jest "brudny"
		virtual bool isDirty() const;

		//! \return Czy czas timeline jest "brudny"
		virtual bool isTimeDirty() const;

		//! \return Czy kontroler jest zajęty?
		bool isBusy() const;

		//------------------------------- Operacje na timeline -----------------------------------

		//! \return długość kanału w sekundach, uwzględniając skale
		virtual double getLength() const;

		//! \return Aktualny czas kanału
		virtual double getTime() const;

		//! \return Znormalizowany czas timeline
		virtual double getNormalizedTime() const;

		//! \return Skala czasu kanału
		virtual double getTimeScale() const;

		//! \param timeScale Nowa skala czasowa
		virtual void setTimeScale(double timeScale);

		//! propaguje zmiane na wszystkie aktywne podkanały
		//! sprawdza maske i offset
		//! \param time Aktualny czas timeline, 0 <= time <= length
		//! \param lockHolder Obiekt chcacy aktualizować czas, trzymający blokade
		virtual void setTime(double time);

		//! propaguje zmiane na wszystkie aktywne podkanały
		//! sprawdza maske i offset
		//! \param normTime Aktualny czas timeline, 0 <= time <= length
		//! \param lockHolder Obiekt chcacy aktualizować czas, trzymający blokade
		virtual void setNormalizedTime(double normTime);

		//! \param path Sciezka nowego kanału
		//! \param channel fatyczny kanał dotarczony przez klienta
		virtual void addChannel(const std::string & path, const IChannelPtr & channel);

		//! \param channels Kanały do dodawnia w formie ściezka -> kanał, kanał nie powiniem być nullptr
		virtual void addChannels(const std::map<std::string, IChannelPtr> & channels);

		//! \param path Sciezka kanału do usuniecia
		virtual void removeChannel(const std::string & path);

		virtual void removeChannels(const std::set<std::string> & paths);

		//! \param path Sciezka do kanału
		//! \param active Czy kanał jest aktywny podczas operacji czasowych (odtwarzanie timeline)
		virtual void setChannelActive(const std::string & path, bool active);

		//! \param path Sciezka do szukanego kanału
		//! \return wskaźnik do szukanego kanału lub pusty wskaźnik jeśli nie znaleziono
		virtual Model::TChannelConstPtr findChannel(const std::string & path) const;

	private:

		//! \return Czy możliwy jest "czysty" zapis?
		bool isWriteEnabled() const;

		//! \return Bieżący stan. W zależności od rezultatu isWriteEnabled
		//! zwracany jest albo faktyczny stan modelu, albo jego "brudna" kopia.
		State getState() const;

		//! Ustawia bieżący stan. Zmienna dirtyState zawsze jest ustawiana,
		//! ewentualnie jeszcze (na podstawie rezultatu isWriteEnabled)
		//! ustawiana jest flaga dirty.
		//! \param state Bieżący stan.
		void setState(const State& state);

		//! \param controller Kontroler, którego czas aktualizujemy do przodu
		//! \param realTimeDelta Rzeczywisty krok czasu w milisekundach
		//! \return nowy czas do ustawienia
		static double forwardTimeUpdate(Controller * controller, unsigned long int realTimeDelta);

		//! \param controller Kontroler, którego czas aktualizujemy do tyłu
		//! \param realTimeDelta Rzeczywisty krok czasu w milisekundach
		//! \return nowy czas do ustawienia
		static double backwardTimeUpdate(Controller * controller, unsigned long int realTimeDelta);

		//! \param realTimeDelta Rzeczywisty krok czasu w milisekundach
		//! \return Informacja czy osiągnięto/przekroczono limit czasu timeline
		bool setNextTime(unsigned long int realTimeDelta);
	};
}

#endif  //  HEADER_GUARD_TIMELINE__CONTROLLER_H__
