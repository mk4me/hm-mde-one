/********************************************************************
	created:  2012/04/26
	created:  26:4:2012   23:56
	filename: DataChannelTimers.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_DATACHANNEL__DATACHANNELTIMERS_H__
#define HEADER_GUARD_DATACHANNEL__DATACHANNELTIMERS_H__

#include <utils/SmartPtr.h>
#include <atomic>

namespace dataaccessor {
	//! Interfejs do odczytu czasu timera
	template<typename ArgumentType>
	class IArgumentTrackerReader
	{
	public:
		typedef IArgumentTrackerReader<ArgumentType> ArgumentTrackerReaderType;
		typedef utils::shared_ptr<ArgumentTrackerReaderType> ArgumentTrackerReaderPtr;
		typedef utils::shared_ptr<const ArgumentTrackerReaderType> ArgumentTrackerReaderConstPtr;

	public:
		//! Destruktor wirtualny
		virtual ~IArgumentTrackerReader() {}
		//! \return Aktualny czas timera
		virtual ArgumentType currentArgument() const = 0;
	};

	//! Interfejs timera dla kanału - ustawia czas i czyta czas
	template<typename ArgumentType>
	class IArgumentTracker : public IArgumentTrackerReader<ArgumentType>
	{
	public:
		typedef IArgumentTracker<ArgumentType> ArgumentTrackerType;
		typedef utils::shared_ptr<ArgumentTrackerType> ArgumentTrackerPtr;
		typedef utils::shared_ptr<const ArgumentTrackerType> ArgumentTrackerConstPtr;

	public:
		//! \param time Aktualny czas timera
		virtual void setCurrentArgument(const ArgumentType argument) = 0;
	};

	//! \tparam ArgumentType Typ śledzonego argumentu
	template<typename ArgumentType>
	//! Implementacja trackera
	class ArgumentTracker : public IArgumentTracker<ArgumentType>
	{
	public:
		//! Konstruktor domyślny
		ArgumentTracker() : currentArgument_(ArgumentType()) {}

		//! Konstruktor
		//! \param tracker Tracker z którego kopiujemy aktualną wartość
		template<class U,
		typename std::enable_if<std::is_convertible<U, ArgumentType>::value>::type>
		ArgumentTracker(const IArgumentTrackerReader<U> & tracker) : currentArgument_(tracker.currentArgument()) {}

		//! Konstruktor
		//! \tparam U Typ innego argumentu który kopiujemy
		template<class U,
			typename std::enable_if<std::is_convertible<U, ArgumentType>::value>::type>
		//! \param argument Aktualna wartośc argumentu
		ArgumentTracker(const U & argument) : currentArgument_(argument) {}

		//! Destruktor wirtualny
		virtual ~ArgumentTracker() {}

		//! \param argument Aktualna wartośc argumentu
		virtual void setCurrentArgument(const ArgumentType & argument) override
		{
			currentArgument_ = argument;
		}

		//! \return Aktualna wartośc argumentu
		virtual ArgumentType currentArgument() const override
		{
			return currentArgument_;
		}

	private:
		//! Aktualna wartośc argumentu
		std::atomic<ArgumentType> currentArgument_;
	};
}

#endif	//	HEADER_GUARD_DATACHANNEL__DATACHANNELTIMERS_H__
