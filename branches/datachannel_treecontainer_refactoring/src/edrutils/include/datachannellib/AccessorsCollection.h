/********************************************************************
	created:	2011/07/04
	created:	4:7:2011   20:56
	filename: 	AccessorsCollection.h
	author:		Wojciech Kniec

	purpose:
	*********************************************************************/

#ifndef HEADER_GUARD_UTILS__ACCESSORSCOLLECTION_H__
#define HEADER_GUARD_UTILS__ACCESSORSCOLLECTION_H__

#include <datachannellib/Accessors.h>
#include <datachannellib/DescriptorFeature.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include <utils/SmartPtr.h>
#include <utils/Debug.h>
#include <boost/tuple/tuple.hpp>
////////////////////////////////////////////////////////////////////////////////
namespace dataaccessor {
	////////////////////////////////////////////////////////////////////////////////

	//! Klasa agreguje klasy DataChannel, wszystkie dodawane kanały powinny mieć tyle samo wpisow
	template <typename AccessorType>
	class AccessorsCollection
	{
		static_assert(std::is_base_of<IDiscreteAccessorT<typename AccessorType::value_type, typename AccessorType::argument_type>, AccessorType>::value,	"AccessorType must be derrived from IDiscreteAccessor");

	public:
		using value_type = typename AccessorType::value_type;
		using argument_type = typename AccessorType::argument_type;
		using Accessor = AccessorType;
		using AccessorPtr = utils::shared_ptr<Accessor>;
		using AccessorConstPtr = utils::shared_ptr<const Accessor>;
		typedef std::vector<AccessorPtr> Collection;
		typedef typename Collection::iterator iterator;
		typedef typename Collection::const_iterator const_iterator;
		typedef boost::tuple<value_type, argument_type, std::size_t> ValueArgumentIndex;
	protected:
		Collection accessors;
		//! numer konfiguracji
		int configurationID;

	public:

		AccessorsCollection() {}
		AccessorsCollection(const AccessorsCollection& dc)
			: accessors(dc.accessors), configurationID(dc.configurationID)
		{			
		}
		
		virtual ~AccessorsCollection() {}

	public:
		void addAccessor(const AccessorPtr & ptr)
		{
			UTILS_ASSERT(accessors.size() == 0 || ptr->size() == accessors[0]->size());
			accessors.push_back(ptr);
		}

		const AccessorPtr & getAccessor(int index)
		{
			UTILS_ASSERT(index >= 0 && index < static_cast<int>(accessors.size()));
			return accessors[index];
		}

		AccessorConstPtr getAccessor(int index) const
		{
			UTILS_ASSERT(index >= 0 && index < static_cast<int>(accessors.size()));
			return accessors[index];
		}

		AccessorConstPtr getAccessor(const std::string& name) const
		{
			for (auto & a : accessors)
			{
				auto f = a->feature<IDescriptorFeature>();
				if (f != nullptr && f->name() == name){
					return a;
				}
			}

			throw std::runtime_error("Unknown accessor name");
		}

		int getNumPointsPerAccessor() const
		{
			UTILS_ASSERT(accessors.size());
			return accessors[0]->size();
		}

		int getNumAccessors() const
		{
			return accessors.size();
		}

		//! \return długość kanału w sekundach; gdy nie ma danych zwracane jest zero, jest to odejście od konwecji, ale dzialanie całkiem naturalne.
		argument_type getLength() const
		{
			if (accessors.size() == 0) {
				return argument_type(0);
			}
			else {
				return accessors[0]->argument(getNumPointsPerAccessor() - 1);
			}
		}

		//! \param time czas, dla którego pobieramy wartości
		//! \return wartości wszystkich kanałów dla konkretnego czasu
		std::vector<value_type> getValues(const argument_type & argument) const
		{
			//TODO
			return std::vector<value_type>();
		}

		int getConfigurationID() const
		{
			return configurationID;
		}

		void setConfigurationID(int id)
		{
			configurationID = id;
		}

		//! \param index indeks kanału, dla którego pobieramy wartość
		//! \param time czas, dla którego będzie zwrócona wartość
		//! \return wartość kanału
		value_type getValue(int index, const argument_type & argument) const
		{
			//TODO
			return value_type();
		}

		//! \return krotka z maksymalną wartością, jej czasem i indeksem kanału w calej dziedzinie dla wszystkich kanałów
		template<class _Fun>
		ValueArgumentIndex getValue(_Fun f) const
		{
			std::vector<ValueArgumentIndex> values;
			
			for (int channelIdx = 0; channelIdx < accessors.size(); ++channelIdx) {
				auto chosenVal = accessors[channelIdx]->value(0);
				int chosenIdx = 0;
				int count = accessors[channelIdx]->size();
				for (int i = 1; i < count; ++i) {
					auto val = accessors[channelIdx]->value(i);
					if (f(chosenVal, val)) {
						chosenVal = val;
						chosenIdx = i;
					}
				}
				values.push_back(boost::make_tuple(
					accessors[channelIdx]->value(chosenIdx),
					accessors[channelIdx]->argument(chosenIdx),
					channelIdx));
			}

			const auto count = values.size();
			auto chosenVal = values[0].template get<0>();
			int chosenIdx = 0;
			for (int i = 1; i < count; ++i) {
				auto val = values[i].template get<0>();
				if (f(chosenVal, val)) {
					chosenIdx = i;
					chosenVal = val;
				}
			}

			return values[chosenIdx];
		}

		//! \return krotka z minimalną wartością, jej czasem i indeksem kanału w calej dziedzinie dla wszystkich kanałów
		ValueArgumentIndex getMinValue() const
		{
			return getValue([&](const value_type& p1, const value_type& p2) { return p2 < p1; });
		}

		//! \return krotka z maksymalną wartością, jej czasem i indeksem kanału w calej dziedzinie dla wszystkich kanałów
		ValueArgumentIndex getMaxValue() const
		{
			return getValue([&](const value_type& p1, const value_type& p2) { return p1 < p2; });
		}
	};
	////////////////////////////////////////////////////////////////////////////////
} //namespace utils
////////////////////////////////////////////////////////////////////////////////

#endif
