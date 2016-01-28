/********************************************************************
created:  2012/04/26
created:  26:4:2012   23:47
filename: Accessors.h
author:   Mateusz Janiak

purpose:
*********************************************************************/
#ifndef HEADER_GUARD_DATACHANNEL__ACCESSORS_H__
#define HEADER_GUARD_DATACHANNEL__ACCESSORS_H__

#include <datachannellib/Types.h>
#include <datachannellib/Features.h>
#include <utils/TypeInfo.h>
#include <utils/SmartPtr.h>
#include <utils/Utils.h>
#include <map>

namespace dataaccessor
{
	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Interfejs opisujący dostęp do danych kanału w formie ciągłej
	class IFunctionAccessorT;

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Interfejs opisujący dyskretny dostęp do danych kanału
	class IDiscreteAccessorT;

	//! Klasa bazowa wszystkich akcesorów
	class IAccessor
	{
	protected:

		//! Domyśłny konstruktor
		IAccessor() {}	
		//! Konstruktor kopiujący
		//! \param other Kopiowany obiekt
		IAccessor(const IAccessor & other) : features(other.features) {}
		//! Konstruktor przenoszący
		//! \param other Przenoszony obiekt
		IAccessor(IAccessor && other) : features(std::move(other.features)) {}	

	public:
		//! Desturktor wirtualny
		virtual ~IAccessor() {}

		//! \param id Identyfikator cechy
		//! \return Czy cecha istnieje
		inline bool featureExists(const int id) const { return features.find(id) != features.end(); }

		//! \param type Typ cechy o jaką pytamy
		//! \return Cecha o zadanym typie, nullptr jeśli nie ma takiej cechy
		inline IFeaturePtr feature(const int id) const
		{
			auto it = features.find(id);
			if (it != features.end()) {
				return it->second;
			}

			return IFeaturePtr();
		}

		//! \tparam FeatureT Typ cechy o jaką pytamy
		//! \tparam dummy
		template<typename FeatureT,
			typename std::enable_if<std::is_base_of<IFeature, FeatureT>::value>::type * = 0>
		//! \param dummy
		//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
		inline utils::shared_ptr<FeatureT> feature(FeatureT * dummy = nullptr) const
		{
			auto f = this->feature(FeatureT::ID);
			if (f != nullptr) {
				//Dodatkowe zabezpieczenie przed pobraniem cechy dla innego kanału (cecha może mieć dobre ID ale dotyczyć innych typów)
				return utils::dynamic_pointer_cast<FeatureT>(f);
			}

			return utils::shared_ptr<FeatureT>();
		}

		//! \param feature Cecha dopinana do akcesora
		//! \return Czy udało się dopiąć cechę czy nie - false kiedy cecha o danym typie już jest podpięta
		bool attachFeature(IFeaturePtr feature) const
		{
			if (feature == nullptr || !type().match(feature->type())) {
				return false;
			}

			auto it = features.find(feature->id());
			if (it == features.end()) {
				features.insert({ feature->id(), feature });
				return true;
			}

			return false;
		}

		//! \param type Cecha którą chcemy usunąć
		inline bool removeFeature(const int id) { return (features.erase(id) > 0); }

		//! \tparam FeatureT Cecha jaką chcemy usunąć
		//! \tparam dummy
		template<typename FeatureT,
			typename std::enable_if<std::is_base_of<IFeature, FeatureT>::value>::type * = 0>
		//! \param dummy
		//! \return Czy cecha została usunięta (była obecna)
		inline bool removeFeature(FeatureT * dummy = nullptr) { return removeFeature(FeatureT::ID); }

		//! \return Typ akcesora
		virtual AccessorType type() const = 0;

	private:
		//! Mapa cech kanału
		mutable std::map<int, IFeaturePtr> features;
	};

	//! \tparam ValueType Typ wartości akcesora dla których przechowujemy własności
	template<typename ValueType>
	//! Interfejs akcesora cech dla wartości
	class IValueAccessorT : public virtual IAccessor
	{
	public:
		//! Typ przechowywanych wartości
		using value_type = ValueType;
		//! Typ cechy wartości
		using ValueFeature = IValueFeatureT<value_type>;

	public:

		//! Destruktor wirtualny
		virtual ~IValueAccessorT() {}

		//! \return Typ akcesora
		inline static AccessorType myType() { return AccessorType(typeid(value_type), typeid(void)); }

		//! \tparam ValueFeatureT Typ cechy wartości o jaką pytamy
		//! \tparam dummy
		template<template<typename> class ValueFeatureT,
			typename std::enable_if<std::is_base_of<ValueFeature, ValueFeatureT<value_type>>::value>::type * = 0>
		//! \param dummy
		//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
		inline utils::shared_ptr<ValueFeatureT<value_type>> feature(ValueFeatureT<value_type> * dummy = nullptr) const { return IAccessor::feature<ValueFeatureT<value_type>>(); }	
	};

	//! \tparam ArgumentType Typ argumentów akcesora dla których przechowujemy własności
	template<typename ArgumentType>
	//! Intefejs akcesora dla cech argumentów
	class IArgumentAccessorT : public virtual IAccessor
	{
	public:
		//! Typ argumentów
		using argument_type = ArgumentType;
		//! Typ cechy argumentów
		using ArgumentFeature = IArgumentFeatureT<argument_type>;

	public:

		//! Destruktor wirtualny
		virtual ~IArgumentAccessorT() {}

		//! \return Typ akcesora
		inline static AccessorType myType() { return AccessorType(typeid(void), typeid(argument_type)); }

		//! \tparam ArgumentFeatureT Typ cechy argumentów o jaką pytamy
		//! \tparam dummy
		template<template<typename> class ArgumentFeatureT,
			typename std::enable_if<std::is_base_of<ArgumentFeature, ArgumentFeatureT<argument_type>>::value>::type * = 0>
		//! \param dummy
		//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
		inline utils::shared_ptr<ArgumentFeatureT<argument_type>> feature(ArgumentFeatureT<argument_type> * dummy = nullptr) const { return IAccessor::feature<ArgumentFeatureT<argument_type>>(); }	
	};

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Interfejs akcesora cech wartości i argumentów
	class IAccessorT : public virtual IAccessor,
		public IValueAccessorT<ValueType>,
		public IArgumentAccessorT<ArgumentType>
	{
	public:		
		//! Próbka: argument + wartość
		using sample_type = std::pair<argument_type, value_type>;
		//! Typ akcesorta dyskretnego
		using DiscreteAccessor = IDiscreteAccessorT<value_type, argument_type>;
		//! Typ akcesora dyskretnego
		using FunctionAccessor = IFunctionAccessorT<value_type, argument_type>;
		//! Typ cechy akcesora
		using AccessorFeature = IAccessorFeatureT<value_type, argument_type>;	

	public:
		//! Destruktor wirtualny
		virtual ~IAccessorT() {}
		//! \return Kanał jako dyskretny
		virtual DiscreteAccessor * asDiscrete() { return nullptr; };
		//! \return Kanał jako dyskretny
		virtual const DiscreteAccessor * asDiscrete() const { return nullptr; };
		//! \return Kanał jako ciągły
		virtual FunctionAccessor * asFunction() { return nullptr; };
		//! \return Kanał jako ciągły
		virtual const FunctionAccessor * asFunction() const { return nullptr; };
		//! \return Identyfikator kanału
		inline static AccessorType myType() { return AccessorType(typeid(value_type), typeid(argument_type)); }
		//! \return Identyfikator kanału
		virtual AccessorType type() const override final { return myType(); }

		using IAccessor::feature;
		using IValueAccessorT<ValueType>::feature;
		using IArgumentAccessorT<ArgumentType>::feature;
		
		//! \tparam FeatureT Typ cechy o jaką pytamy
		//! \tparam dummy
		template<typename FeatureT,
			typename std::enable_if < std::is_base_of<IFeature, FeatureT>::value>::type * = 0 >
		//! \param dummy
		//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
		inline utils::shared_ptr<FeatureT> getOrCreateFeature(FeatureT * dummy = nullptr) const
		{
			utils::shared_ptr<FeatureT> ret = IAccessor::feature<FeatureT>();

			if (ret == nullptr) {
				UTILS_ASSERT((asDiscrete() != nullptr) || (asDiscrete() != nullptr));
				ret.reset(FeatureT::create(asDiscrete(), asFunction()));
				IAccessor::attachFeature(ret);
			}

			return ret;
		}

		//! \tparam ValueFeatureT Typ cechy wartości o jaką pytamy
		//! \tparam dummy
		template<template<typename> class ValueFeatureT,
			typename std::enable_if<std::is_base_of<ValueFeature, ValueFeatureT<value_type>>::value>::type * = 0>
		//! \param dummy
		//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
		inline utils::shared_ptr<ValueFeatureT<value_type>> getOrCreateFeature(ValueFeatureT<value_type> * dummy = nullptr) const { return this->getOrCreateFeature<ValueFeatureT<value_type>>(); }

		//! \tparam ArgumentFeatureT Typ cechy argumentów o jaką pytamy
		//! \tparam dummy
		template<template<typename> class ArgumentFeatureT,
			typename std::enable_if<std::is_base_of<ArgumentFeature, ArgumentFeatureT<argument_type>>::value>::type * = 0>
		//! \param dummy
		//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
		inline utils::shared_ptr<ArgumentFeatureT<argument_type>> getOrCreateFeature(ArgumentFeatureT<argument_type> * dummy = nullptr) const { return this->getOrCreateFeature < ArgumentFeatureT<argument_type>>(); }

		//! \tparam AccessorFeatureT Typ cechy o jaką pytamy
		template<template<typename, typename> class AccessorFeatureT,
			typename std::enable_if<std::is_base_of<AccessorFeature, AccessorFeatureT<value_type, argument_type>>::value>::type * = 0>
		//! \param dummy
		//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
		inline utils::shared_ptr<AccessorFeatureT<value_type, argument_type>> feature(AccessorFeatureT<value_type, argument_type> * dummy = nullptr) const { return this->feature<AccessorFeatureT<value_type, argument_type>(); }

		//! \tparam AccessorFeatureT Typ cechy danych o jaką pytamy
		template<template<typename, typename> class AccessorFeatureT,
			typename std::enable_if<std::is_base_of<AccessorFeature, AccessorFeatureT<value_type, argument_type>>::value>::type * = 0>
		//! \param dummy
		//! \return Cecha o zadanym typie lub nullptr jeśli istnieje cecha o danym id ale to nie ta o jaką pytamy
		inline utils::shared_ptr<AccessorFeatureT<value_type, argument_type>> getOrCreateFeature(AccessorFeatureT<value_type, argument_type> * dummy = nullptr) const { return this->getOrCreateFeature<AccessorFeatureT<value_type, argument_type>>(); }		
	};

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do akcesora cech
	using AccessorPtr = utils::shared_ptr<IAccessorT<typename utils::remove_toplevel<ValueType>::type, typename utils::remove_toplevel<ArgumentType>::type>>;
	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do akcesora cech
	using AccessorConstPtr = utils::shared_ptr<const IAccessorT<typename utils::remove_toplevel<ValueType>::type, typename utils::remove_toplevel<ArgumentType>::type>>;

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Interfejs opisujący dostęp do danych akcesora w formie funkcji
	class IFunctionAccessorT : public IAccessorT<ValueType, ArgumentType>
	{
	public:
		//! Typ mojego akcesora
		using accessor_type = IFunctionAccessorT<ValueType, ArgumentType>;

	public:
		//! Destruktor wirtualny
		virtual ~IFunctionAccessorT() {}
		//! \param argument Argument dla któego odpytujemy o wartość
		//! \return Wartość dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const = 0;
		//! \param argument Argument dla któego odpytujemy o wartość
		//! \return Próbka dla zadanego argumentu
		virtual sample_type sample(const argument_type & argument) const = 0;
		//! \return Kanał jako ciągły
		virtual FunctionAccessor * asFunction() override final { return this; };
		//! \return Kanał jako ciągły
		virtual const FunctionAccessor * asFunction() const override final { return this; };
	};

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do danych akcesora w formie funkcji
	using FunctionAccessorPtr = utils::shared_ptr<IFunctionAccessorT<typename utils::remove_toplevel<ValueType>::type, typename utils::remove_toplevel<ArgumentType>::type>>;

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do danych akcesora w formie funkcji
	using FunctionAccessorConstPtr = utils::shared_ptr<const IFunctionAccessorT<typename utils::remove_toplevel<ValueType>::type, typename utils::remove_toplevel<ArgumentType>::type>>;

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do danych akcesora w formie funkcji
	class IGeneratedFunctionAccessorT : public IFunctionAccessorT<ValueType, ArgumentType>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IGeneratedFunctionAccessorT() {}
		//! \param argument Argument dla któego odpytujemy o wartość
		//! \return Próbka dla zadanego argumentu
		virtual sample_type sample(const argument_type & argument) const override { return std::make_pair(argument, value(argument)); }
	};

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do danych akcesora w formie funkcji
	class IOptimizedFunctionAccessorT : public IFunctionAccessorT<ValueType, ArgumentType>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IOptimizedFunctionAccessorT() {}
		//! \param argument Argument dla któego odpytujemy o wartość
		//! \return Wartość dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const override { return sample(argument).second; }
	};

	//! Baza kanałów dyskretnych
	class IDiscreteAccessor : public virtual IAccessor
	{	
	public:
		//! Typ ilości elementów w kanale
		using size_type = std::size_t;

	public:
		//! Destruktor wirtualny
		virtual ~IDiscreteAccessor() {}

		//! \return Ilość próbek w kanale
		virtual size_type size() const = 0;

		//! \return Czy kanał nie zawiera danych
		virtual bool empty() const { return (size() == 0); }
	};


	//! \tparam ValueType Typ wartości kanału danych	
	template<typename ValueType>
	//! Interfejs opisujący dyskretny dostęp do wartości akcesora
	class IDiscreteValueAccessorT : public virtual IDiscreteAccessor,
		public IValueAccessorT<ValueType>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IDiscreteValueAccessorT() {}
		//! \param idx Indeks próbki
		//! \return Wartość dla danego indeksu
		virtual value_type value(const size_type idx) const = 0;

		//! \tparam FeatureT Typ cechy o jaką pytamy
		//! \tparam dummy
		template<template<typename> class ValueFeatureT,
			typename std::enable_if < std::is_base_of<ValueFeature, ValueFeatureT<ValueType>>::value>::type * = 0 >
		//! \param dummy
		//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
		inline utils::shared_ptr<ValueFeatureT<ValueType>> getOrCreateFeature(ValueFeatureT<ValueType> * dummy = nullptr) const
		{
			utils::shared_ptr<ValueFeatureT<ValueType>> ret = IAccessor::feature<ValueFeatureT<ValueType>>();

			if (ret == nullptr) {				
				ret.reset(ValueFeatureT<ValueType>::create(*this));
				IAccessor::attachFeature(ret);
			}

			return ret;
		}
	};

	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ArgumentType>
	//! Interfejs opisujący dyskretny dostęp do argumentów akcesora
	class IDiscreteArgumentAccessorT : public virtual IDiscreteAccessor,
		public IArgumentAccessorT<ArgumentType>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IDiscreteArgumentAccessorT() {}
		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const = 0;

		//! \tparam FeatureT Typ cechy o jaką pytamy
		//! \tparam dummy
		template<template<typename> class ArgumentFeatureT,
			typename std::enable_if < std::is_base_of<ArgumentFeature, ArgumentFeatureT<ArgumentType>>::value>::type * = 0 >
		//! \param dummy
		//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
		inline utils::shared_ptr<ArgumentFeatureT<ArgumentType>> getOrCreateFeature(ArgumentFeatureT<ArgumentType> * dummy = nullptr) const
		{
			utils::shared_ptr<ArgumentFeatureT<ArgumentType>> ret = IAccessor::feature<ArgumentFeatureT<ArgumentType>>();

			if (ret == nullptr) {
				ret.reset(ArgumentFeatureT<ArgumentType>::create(*this));
				IAccessor::attachFeature(ret);
			}

			return ret;
		}
	};

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<class ValueType, class ArgumentType>
	//! Interfejs opisujący dyskretny akcesor danych
	class IDiscreteAccessorT : public virtual IDiscreteAccessor,
		public IAccessorT<ValueType, ArgumentType>,
		public IDiscreteValueAccessorT<ValueType>,
		public IDiscreteArgumentAccessorT<ArgumentType>				
	{
	public:

	public:
		//! Typ mojego akcesora
		using accessor_type = IDiscreteAccessorT<ValueType, ArgumentType>;

	public:
		//! Wirtualny destruktor
		virtual ~IDiscreteAccessorT() {};

		//! \param idx Indeks próbki
		//! \return Próbka dla danego indeksu
		virtual sample_type sample(const size_type idx) const = 0;

		//! \return Kanał jako dyskretny
		virtual DiscreteAccessor * asDiscrete() override final { return this; };
		//! \return Kanał jako dyskretny
		virtual const DiscreteAccessor * asDiscrete() const override final { return this; };
	};

	//! \tparam ValueType Typ wartości akcesora
	//! \tparam ArgumentType Typ argumentu akcesora
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do akcesora z danymi dyskretnymi
	using DiscreteAccessorPtr = utils::shared_ptr<IDiscreteAccessorT<typename utils::remove_toplevel<ValueType>::type, typename utils::remove_toplevel<ArgumentType>::type>>;
	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do akcesora z danymi dyskretnymi
	using DiscreteAccessorConstPtr = utils::shared_ptr<const IDiscreteAccessorT<typename utils::remove_toplevel<ValueType>::type, typename utils::remove_toplevel<ArgumentType>::type>>;

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<class ValueType, class ArgumentType>
	//! Interfejs opisujący dyskretny akcesor danych
	class IIndependentDiscreteAccessorT : public IDiscreteAccessorT<ValueType, ArgumentType>
	{
	public:
		//! Wirtualny destruktor
		virtual ~IIndependentDiscreteAccessorT() {};

		//! \param idx Indeks próbki
		//! \return Próbka dla danego indeksu
		virtual sample_type sample(const size_type idx) const override { return std::make_pair(argument(idx), value(idx)); }
	};

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<class ValueType, class ArgumentType>
	//! Interfejs opisujący dyskretny akcesor danych
	class IOptimizedDiscreteAccessorT : public IDiscreteAccessorT<ValueType, ArgumentType>
	{
	public:
		//! Wirtualny destruktor
		virtual ~IOptimizedDiscreteAccessorT() {};

		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override { return sample(idx).first; }

		//! \param idx Indeks próbki
		//! \return Wartość dla danego indeksu
		virtual value_type value(const size_type idx) const override { return sample(idx).second; }
	};
}

#endif	//	HEADER_GUARD_DATACHANNEL__ACCESSORS_H__
