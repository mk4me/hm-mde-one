/********************************************************************
    created:  2012/04/26
    created:  26:4:2012   23:59
    filename: DescriptorFeature.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD_DATACHANNEL__DESCRIPTORFEATURE_H__
#define HEADER_GUARD_DATACHANNEL__DESCRIPTORFEATURE_H__

#include <datachannellib/Features.h>

namespace dataaccessor {
	
	class DescriptorFeature;

	//! Interfejs do czytania opisu kanału - osi x i y oraz nazwy kanału.
	class IDescriptorFeature : public FeatureHelperT<Description>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IDescriptorFeature() {}

		//! \return Co reprezentuje kanał (np. "Zależność prędkości od czasu",
		//! coś na kształt opisu/tytułu wykresu)
		virtual std::string name() const = 0;

		//! \return Nazwa jednostki dla argumentów
		virtual std::string argumentType() const = 0;

		//! \return Nazwa jednostki dla argumentów
		virtual std::string argumentUnit() const = 0;

		//! \return Nazwa jednostki dla wartości
		virtual std::string valueType() const = 0;

		//! \return Nazwa jednostki dla wartości
		virtual std::string valueUnit() const = 0;

		template<typename ValueType, typename ArgumentType>
		inline static DescriptorFeature * create(
			const IDiscreteAccessorT<ValueType, ArgumentType> * discrete,
			const IFunctionAccessorT<ValueType, ArgumentType> * function)
		{
			return DescriptorFeature::create<ValueType, ArgumentType>("", "", "");
		}
	};
	
	//! Implementacja interfejsu IChannelDescriptor
	class DescriptorFeature : public IDescriptorFeature
	{
	private:
		//! Opis kanału
		const std::string name_;
		//! Jednostka czasu
		const std::string argumentUnit_;
		//! Jednostka czasu
		const std::string argumentType_;
		//! Jendostka wartości
		const std::string valueUnit_;
		//! Jendostka wartości
		const std::string valueType_;

	public:		

		DescriptorFeature(const std::string & name,
			const std::string & valueType,
			const std::string & valueUnit,
			const std::string & argumentType,
			const std::string & argumentUnit)
			: name_(name), valueUnit_(valueUnit),
			argumentUnit_(argumentUnit), valueType_(valueType),
			argumentType_(argumentType)
		{

		}		

		DescriptorFeature(const DescriptorFeature & Other)
			: DescriptorFeature(Other.name_, Other.valueUnit_,
			Other.argumentUnit_, Other.valueType_, Other.argumentType_)
		{

		}		

		DescriptorFeature(DescriptorFeature && Other)
			: name_(std::move(Other.name_)),
			valueUnit_(std::move(Other.valueUnit_)),
			argumentUnit_(std::move(Other.argumentUnit_)),
			valueType_(std::move(Other.valueType_)),
			argumentType_(std::move(Other.argumentType_))
		{

		}

		DescriptorFeature(const IDescriptorFeature & Other)
			: DescriptorFeature(Other.name(), Other.valueUnit(),
			Other.valueType(), Other.argumentUnit(),
			Other.argumentType())
		{

		}		

		//! \tparam ValueType Typ wartości
		//! \tparam ArgumentType Typ argumentu
		template<typename ValueType, typename ArgumentType>
		//! \param valueUnit Jendostka wartości
		//! \param argumentUnit Jednostka argumentu
		//! \return Obiekt deskryptora kanału danych
		static inline DescriptorFeature * create(const std::string & name,
			const std::string & valueUnit,
			const std::string & argumentUnit,
			ValueType * dummyValue = nullptr,
			ArgumentType * dummyArgument = nullptr) {
			return new DescriptorFeature(name, typeid(ValueType).name(), valueUnit,
				typeid(ArgumentType).name(), argumentUnit);
		}

		//! Destruktor
		virtual ~DescriptorFeature() {}

		//! \return
		virtual std::string name() const override { return name_; }

		//! \return
		virtual std::string argumentUnit() const override { return argumentUnit_; }

		//! \return
		virtual std::string valueUnit() const override { return valueUnit_; }

		//! \return
		virtual std::string argumentType() const override { return argumentType_; }

		//! \return
		virtual std::string valueType() const override { return valueType_; }
	};
}

#endif	//	HEADER_GUARD_DATACHANNEL__DESCRIPTORFEATURE_H__