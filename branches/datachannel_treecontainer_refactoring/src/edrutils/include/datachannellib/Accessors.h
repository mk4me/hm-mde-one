/********************************************************************
created:  2012/04/26
created:  26:4:2012   23:47
filename: Accessors.h
author:   Mateusz Janiak

purpose:
*********************************************************************/
#ifndef HEADER_GUARD_DATACHANNEL__ACCESSORS_H__
#define HEADER_GUARD_DATACHANNEL__ACCESSORS_H__

#include <utils/TypeInfo.h>
#include <datachannellib/Types.h>
#include <utils/SmartPtr.h>
#include <map>

namespace datachannel
{
	//! Interfejs cech
	class IFeature
	{
	public:
		//! Typ identyfikujący typy kanałów
		using ChannelID = std::pair<utils::TypeInfo, utils::TypeInfo>;

	public:
		//! Destruktor wirtualny
		virtual ~IFeature() {}
		//! \return Typ cechy danych kanału
		virtual int type() const = 0;
		//! \return Typ obsługiwanego kanału
		virtual ChannelID channelID() const { return ChannelID(typeid(void), typeid(void)); };
	};

	//! \tparam Type
	//! \tparam Feature
	template<int TypeT, typename Feature = IFeature>
	//! Klasa pomagająca w implementacji cech kanałów
	class FeatureHelper : public Feature
	{
		//! Weryfikacja interfejsu, któy musi dziedziczyć po IFuture
		static_assert(std::is_same<IFeature, Feature>::value || std::is_base_of<IFeature, Feature>::value, "Feature must be derived from IFeature");

	public:
		//! Typ cechy
		enum { Type = TypeT };

	public:
		//! Destruktor wirtualny
		virtual ~FeatureHelper() {}
		//! \return Typ cechy
		virtual int type() const override final { return TypeT; }
	};

	//! \tparam ArgumentType Typ argumentu
	template<typename ArgumentType>
	//! Klasa opisująca cechy argumentów kanału
	class IArgumentFeature : public virtual IFeature
	{
	public:
		//! Destruktor wirtualny
		virtual ~IArgumentFeature() {}
		//! \return Typ obsługiwanego kanału
		virtual ChannelID channelID() const override final{ return ChannelID(typeid(void), typeid(ArgumentType)); };
	};

	//! \tparam ValueType Typ wartości
	template<typename ValueType>
	//! Klasa opisująca cechy wartości kanału
	class IValueFeature : public virtual IFeature
	{
	public:
		//! Destruktor wirtualny
		virtual ~IValueFeature() {}
		//! \return Typ obsługiwanego kanału
		virtual ChannelID channelID() const override{ return ChannelID(typeid(ValueType), typeid(void)); };
	};

	//! \tparam ValueType Typ wartości
	//! \tparam ArgumentType Typ argumentu
	template<typename ValueType, typename ArgumentType>
	//! Klasa opisująca cechy argumentó i wartości kanału
	class IChannelFeature : public IValueFeature<ValueType>, public IArgumentFeature<ArgumentType>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IChannelFeature() {}
		//! \return Typ obsługiwanego kanału
		virtual ChannelID channelID() const override { return ChannelID(typeid(ValueType), typeid(ArgumentType)); };
	};

	DEFINE_SMART_POINTERS(IFeature);

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Interfejs opisujący dostęp do danych kanału w formie ciągłej
	class IFunctionAccessor;

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Interfejs opisujący dyskretny dostęp do danych kanału
	class IDiscreteAccessor;

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	class IAccessor
	{
	public:
		//! Typ przechowywanych wartości
		using value_type = ValueType;
		//! Typ argumentów
		using argument_type = ArgumentType;
		//! Próbka: argument + wartość
		using sample_type = std::pair<argument_type, value_type>;
		//! Typ kanały dyskretnego
		using DiscreteAccessor = IDiscreteAccessor<value_type, argument_type>;
		//! Typ kanały dyskretnego
		using FunctionAccessor = IFunctionAccessor<value_type, argument_type>;
		//!
		using ChannelFeature = IChannelFeature<value_type, argument_type>;
		//!
		using ValueFeature = IValueFeature<value_type>;
		//!
		using ArgumentFeature = IArgumentFeature<argument_type>;

		//! Identyfikator kanału
		using ChannelID = IFeature::ChannelID;

	public:
		//! Destruktor wirtualny
		virtual ~IAccessor() {}
		//! \return Kanał jako dyskretny
		virtual DiscreteAccessor * asDiscrete() { return nullptr; };
		//! \return Kanał jako dyskretny
		virtual const DiscreteAccessor * asDiscrete() const { return nullptr; };
		//! \return Kanał jako ciągły
		virtual FunctionAccessor * asFunction() { return nullptr; };
		//! \return Kanał jako ciągły
		virtual const FunctionAccessor * asFunction() const { return nullptr; };
		//! \return Identyfikator kanału
		inline static ChannelID channelID() { return ChannelID(typeid(value_type), typeid(argument_type)); }

		//! \param type Typ cechy o jaką pytamy
		//! \return Cecha o zadanym typie, nullptr jeśli nie ma takiej cechy
		IFeaturePtr feature(const int type) const
		{
			auto it = features.find(type);
			if (it != features.end()){
				return it->second;
			}

			return IFeaturePtr();
		}

		//! \tparam FeatureT Typ cechy o jaką pytamy
		template<typename FeatureT,
			typename std::enable_if<std::is_base_of<IFeature, FeatureT>::value>::type * = 0>
			//! \param dummy
			//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline utils::shared_ptr<FeatureT> feature(FeatureT * dummy = nullptr) const
		{
			auto f = feature(FeatureT::Type);
			if (f != nullptr){
				return utils::dynamic_pointer_cast<FeatureT>(f);
			}

			return utils::shared_ptr<FeatureT>();
		}

		//! \tparam ChannelFeatureT Typ cechy o jaką pytamy
		template<template<typename, typename> class ChannelFeatureT,
			typename std::enable_if<std::is_base_of<ChannelFeature, ChannelFeatureT<value_type, argument_type>>::value>::type * = 0>
			//! \param dummy
			//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline utils::shared_ptr<ChannelFeatureT<value_type, argument_type>> feature(ChannelFeatureT<value_type, argument_type> * dummy = nullptr) const
		{
			auto f = feature(ChannelFeatureT<value_type, argument_type>::Type);
			if (f != nullptr){
				return utils::dynamic_pointer_cast<ChannelFeatureT<value_type, argument_type>>(f);
			}

			return utils::shared_ptr<ChannelFeatureT<value_type, argument_type>>();
		}

		//! \tparam ValueFeatureT Typ cechy wartości o jaką pytamy
		template<template<typename> class ValueFeatureT,
			typename std::enable_if<std::is_base_of<ValueFeature, ValueFeatureT<value_type>>::value>::type * = 0>
			//! \param dummy
			//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline utils::shared_ptr<ValueFeatureT<value_type>> valueFeature(ValueFeatureT<value_type> * dummy = nullptr) const { return feature<ValueFeatureT<value_type>>(); }

		//! \tparam ArgumentFeatureT Typ cechy argumentów o jaką pytamy
		template<template<typename> class ArgumentFeatureT,
			typename std::enable_if<std::is_base_of<ArgumentFeature, ArgumentFeatureT<argument_type>>::value>::type * = 0>
			//! \param dummy
			//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline utils::shared_ptr<ArgumentFeatureT<argument_type>> argumentFeature(ArgumentFeatureT<argument_type> * dummy = nullptr) const { return feature<ArgumentFeatureT<argument_type>>(); }

		//! \tparam FeatureT Typ cechy o jaką pytamy
		template<typename FeatureT,
			typename std::enable_if<std::is_base_of<IFeature, FeatureT>::value>::type * = 0>
			//! \param dummy
			//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline utils::shared_ptr<FeatureT> getOrCreateFeature(FeatureT * dummy = nullptr) const
		{
			utils::shared_ptr<FeatureT> ret;
			auto it = features.find(FeatureT::Type);
			if (it != features.end()){
				if (it->second != nullptr){
					ret = utils::dynamic_pointer_cast<FeatureT>(it->second);
				}
			}
			else{
				UTILS_ASSERT((asDiscrete() != nullptr) || (asDiscrete() != nullptr));
				ret.reset(FeatureT::create(asDiscrete(), asFunction()));
				features.insert({ FeatureT::Type, ret });
			}

			return ret;
		}

		//! \tparam ValueFeatureT Typ cechy wartości o jaką pytamy
		template<template<typename> class ValueFeatureT,
			typename std::enable_if<std::is_base_of<ValueFeature, ValueFeatureT<value_type>>::value>::type * = 0>
			//! \param dummy
			//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline utils::shared_ptr<ValueFeatureT<value_type>> getOrCreateValueFeature(ValueFeatureT<value_type> * dummy = nullptr) const { return getOrCreateFeature<ValueFeatureT<value_type>>(); }
		//! \tparam ArgumentFeatureT Typ cechy argumentów o jaką pytamy
		template<template<typename> class ArgumentFeatureT,
			typename std::enable_if<std::is_base_of<ArgumentFeature, ArgumentFeatureT<argument_type>>::value>::type * = 0>
			//! \param dummy
			//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline utils::shared_ptr<ArgumentFeatureT<argument_type>> getOrCreateArgumentFeature(ArgumentFeatureT<argument_type> * dummy = nullptr) const { return getOrCreateFeature < ArgumentFeatureT<argument_type>>(); }
		//! \tparam ChannelFeatureT Typ cechy danych o jaką pytamy
		template<template<typename, typename> class ChannelFeatureT,
			typename std::enable_if<std::is_base_of<ChannelFeature, ChannelFeatureT<value_type, argument_type>>::value>::type * = 0>
			//! \param
			//! \return Cecha o zadanym typie lub nullptr jeśli takiej nie ma
			inline utils::shared_ptr<ChannelFeatureT<value_type, argument_type>> getOrCreateFeature(ChannelFeatureT<value_type, argument_type> * dummy = nullptr) const { return getOrCreateFeature<ChannelFeatureT<value_type, argument_type>>(); }
		//! \param feature Cecha dopinana do akcesora, nie da się jej odpiąć
		//! \param override Czy chcemy nadpisać dodaną już cechę
		//! \return Czy udało się dopiąć cechę czy nie - false kiedy cecha o danym typie już jest podpięta
		bool attachFeature(IFeaturePtr feature, bool override = false)
		{
			if (feature == nullptr || verifyChannelID(feature->channelID())){
				return false;
			}

			auto it = features.find(feature->type());
			if ((it == features.end()) || (override == true) || (it->second == nullptr)){
				features.insert({ feature->type(), feature });
				return true;
			}

			return false;
		}

		//! \param channelID Identyfikator typów kanału
		//! \return Czy kanał zgodny z naszym kanałem
		inline bool verifyChannelID(const ChannelID & channelID) const
		{
			if ((channelID.first == typeid(void)) && (channelID.second == typeid(void))){
				return true;
			}
			else if (channelID.first == typeid(void)){
				return channelID.second == typeid(argument_type);
			}
			else if (channelID.second == typeid(void)){
				return channelID.first == typeid(value_type);
			}
			else{
				return channelID == this->channelID();
			}
		}

	private:
		//! Mapa cech kanału
		mutable std::map<int, IFeaturePtr> features;
	};

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do akcesora danego kanału
	using AccessorPtr = utils::shared_ptr<IAccessor<typename std::decay<ValueType>::type, typename std::decay<ArgumentType>::type>>;

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Interfejs opisujący dostęp do danych kanału w formie ciągłej
	class IFunctionAccessor : public IAccessor<ValueType, ArgumentType>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IFunctionAccessor() {}
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
	//! Typ smart pointera do akcesora danego kanału funckyjnego
	using FunctionAccessorPtr = utils::shared_ptr<IFunctionAccessor<typename std::decay<ValueType>::type, typename std::decay<ArgumentType>::type>>;

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Interfejs opisujący dostęp do danych kanału w formie ciągłej
	class IGeneratedFunctionAccessor : public IFunctionAccessor<ValueType, ArgumentType>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IGeneratedFunctionAccessor() {}
		//! \param argument Argument dla któego odpytujemy o wartość
		//! \return Próbka dla zadanego argumentu
		virtual sample_type sample(const argument_type & argument) const override { return std::make_pair(argument, value(argument)); }
	};

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Interfejs opisujący dostęp do danych kanału w formie ciągłej
	class IOptimizedFunctionAccessor : public IFunctionAccessor<ValueType, ArgumentType>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IOptimizedFunctionAccessor() {}
		//! \param argument Argument dla któego odpytujemy o wartość
		//! \return Wartość dla zadanego argumentu
		virtual value_type value(const argument_type & argument) const override { return sample(argument).second; }
	};

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<class ValueType, class ArgumentType>
	//! Interfejs opisujący dyskretny dostęp do danych kanału
	class IDiscreteAccessor : public IAccessor<ValueType, ArgumentType>
	{
	public:
		//! Typ ilości elementów w kanale
		using size_type = std::size_t;

	public:
		//! Wirtualny destruktor
		virtual ~IDiscreteAccessor() {};

		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const = 0;

		//! \param idx Indeks próbki
		//! \return Wartość dla danego indeksu
		virtual value_type value(const size_type idx) const = 0;

		//! \param idx Indeks próbki
		//! \return Próbka dla danego indeksu
		virtual sample_type sample(const size_type idx) const = 0;

		//! \return Ilość próbek w kanale
		virtual size_type size() const = 0;

		//! \return Czy kanał nie zawiera danych
		virtual bool empty() const { return (size() == 0); }
		//! \return Kanał jako dyskretny
		virtual DiscreteAccessor * asDiscrete() override final { return this; };
		//! \return Kanał jako dyskretny
		virtual const DiscreteAccessor * asDiscrete() const override final { return this; };
	};

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<typename ValueType, typename ArgumentType>
	//! Typ smart pointera do akcesora danego kanału dyskretnego
	using DiscreteAccessorPtr = utils::shared_ptr<IDiscreteAccessor<typename std::decay<ValueType>::type, typename std::decay<ArgumentType>::type>>;

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<class ValueType, class ArgumentType>
	//! Interfejs opisujący dyskretny dostęp do danych kanału
	class IIndependentDiscreteAccessor : public IDiscreteAccessor<ValueType, ArgumentType>
	{
	public:
		//! Wirtualny destruktor
		virtual ~IIndependentDiscreteAccessor() {};

		//! \param idx Indeks próbki
		//! \return Próbka dla danego indeksu
		virtual sample_type sample(const size_type idx) const override { return std::make_pair(argument(idx), value(idx)); }
	};

	//! \tparam ValueType Typ wartości kanału danych
	//! \tparam ArgumentType Typ argumentu kanału danych
	template<class ValueType, class ArgumentType>
	//! Interfejs opisujący dyskretny dostęp do danych kanału
	class IOptimizedDiscreteAccessor : public IDiscreteAccessor<ValueType, ArgumentType>
	{
	public:
		//! Wirtualny destruktor
		virtual ~IOptimizedDiscreteAccessor() {};

		//! \param idx Indeks próbki
		//! \return Argument dla danego indeksu
		virtual argument_type argument(const size_type idx) const override { return sample(idx).first; }

		//! \param idx Indeks próbki
		//! \return Wartość dla danego indeksu
		virtual value_type value(const size_type idx) const override { return sample(idx).second; }
	};
}

#endif	//	HEADER_GUARD_DATACHANNEL__ACCESSORS_H__
