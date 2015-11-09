/********************************************************************
    created:  2012/04/26
    created:  26:4:2012   23:59
    filename: Descriptor.h
    author:   Mateusz Janiak

    purpose:
*********************************************************************/
#ifndef HEADER_GUARD_DATACHANNEL__DESCRIPTOR_H__
#define HEADER_GUARD_DATACHANNEL__DESCRIPTOR_H__

#include <datachannellib/Accessors.h>

namespace datachannel {
	
	//! Interfejs do czytania opisu kanału - osi x i y oraz nazwy kanału.
	class IDescriptor : public FeatureHelper<Description>
	{
	public:
		//! Destruktor wirtualny
		virtual ~IDescriptor() {}

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
	};
	
	//! Implementacja interfejsu IChannelDescriptor
	class Descriptor : public IDescriptor
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

		Descriptor(const std::string & name,
			const std::string & valueType,
			const std::string & valueUnit,
			const std::string & argumentType,
			const std::string & argumentUnit)
			: name_(name), valueUnit_(valueUnit),
			argumentUnit_(argumentUnit), valueType_(valueType),
			argumentType_(argumentType)
		{

		}		

		Descriptor(const Descriptor & Other)
			: Descriptor(Other.name_, Other.valueUnit_,
			Other.argumentUnit_, Other.valueType_, Other.argumentType_)
		{

		}		

		Descriptor(Descriptor && Other)
			: name_(std::move(Other.name_)),
			valueUnit_(std::move(Other.valueUnit_)),
			argumentUnit_(std::move(Other.argumentUnit_)),
			valueType_(std::move(Other.valueType_)),
			argumentType_(std::move(Other.argumentType_))
		{

		}

		Descriptor(const IDescriptor & Other)
			: Descriptor(Other.name(), Other.valueUnit(),
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
		static inline Descriptor * create(const std::string & name,
			const std::string & valueUnit,
			const std::string & argumentUnit,
			ValueType * dummyValue = nullptr,
			ArgumentType * dummyArgument = nullptr) {
			return new Descriptor(name, typeid(ValueType).name(), valueUnit,
				typeid(ArgumentType).name(), argumentUnit);
		}

		//! Destruktor
		virtual ~Descriptor() {}

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

#endif	//	HEADER_GUARD_DATACHANNEL__DESCRIPTOR_H__