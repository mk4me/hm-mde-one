/********************************************************************
	created:  2016/01/20
	filename: ValueCarrier.h
	author:	  Mateusz Janiak
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_UTILS__VALUECARRIER_H__
#define __HEADER_GUARD_UTILS__VALUECARRIER_H__

#include <utils/pointer_traits.h>

namespace utils
{
	//TODO - dodaæ obs³ugê go³ych wskaŸników? Czy ma to sens? Co z nimi robiæ?
	// kopiowaæ obiekty pod nimi? Trzymaæ tylko wskaŸniki?
	//! \tparam T Docelowy typ przechowywanych danych
	template<typename T>
	//! Klasa realizuje bezpieczne przechowywanie danych
	class ValueCarrier
	{
		//! \tparam U Typ przechowywany
		template<typename U, bool>
		struct ValueTypeImpl
		{
			using type = U;
		};

		//! \tparam SmartPtr Inteligentny wskaŸnik z którego wy³uskujemy przechoywany typ
		template<typename SmartPtr>
		struct ValueTypeImpl<SmartPtr, true>
		{
			using type = typename std::decay<typename pointed_type<SmartPtr>::type>::type;
		};

		template<typename T>
		struct ValueType : public ValueTypeImpl<T, is_like_smart_pointer<T>::value> {};

	public:
		//! Typ referencji jakiej dostarczamy
		using ref_type = typename std::add_reference<typename std::add_const<T>::type>::type;

	private:

		//! Forward declaration
		template<typename U, typename = void>
		class ValueCarrierImpl;

		//! Klasa bazowa gwarantuj¹ca dostêp do danych
		class ValueCarrierBase
		{
		public:
			//! Destruktor wirtualny
			virtual ~ValueCarrierBase() {}
			//! \return Referencja do przechowywanej wartoœci
			virtual ref_type ref() const = 0;
		};

		template<>
		//! Specjalizacja dla prostego przechowywania przez wartoœæ
		class ValueCarrierImpl<T, void> : public ValueCarrierBase
		{
		public:
			//! \param value Kopiowana wartoœæ do przechowania
			ValueCarrierImpl(const T & value) : value_(value) {}
			//! \param value Przenoszona wartoœæ do przechowania
			ValueCarrierImpl(T && value) : value_(std::move(value)) {}
			//! Destruktor wirtualny
			virtual ~ValueCarrierImpl() {}
			//! \return Referencja do przechowywanej wartoœci
			virtual ref_type ref() const override final { return value_; }

		private:
			//! Przechowywana wartoœæ
			const T value_;
		};


		//! \tparam U Typ przechowywany w tablicy
		//! \tparam N Rozmiar tablicy
		template<typename U, std::size_t N>
		//! Specjalizacja dla prostego przechowywania przez wartoœæ dla tablic
		class ValueCarrierImpl<U[N], void> : public ValueCarrierBase
		{
		public:
			//! Typ tablicy
			using LC = U[N];

		private:
			//! \param value Tablica któr¹ chcemy skopiowaæ
			//! \param dummy Statyczne rozpoznanie PODów
			//! \return WskaŸnik do skopiowanej tablicy
			static U* init(const LC & value, std::true_type)
			{
				utils::unique_ptr<LC> ret(new LC);
				std::memcpy(ret.get(), &value, sizeof(LC));
				return ret.release();
			}

			//! \param value Tablica któr¹ chcemy skopiowaæ
			//! \param dummy Statyczne rozpoznanie PODów
			//! \return WskaŸnik do skopiowanej tablicy
			static U* init(const LC & value, std::false_type)
			{
				utils::unique_ptr<LC> ret(new LC);
				static_assert(std::is_default_constructible<U>::value && std::is_copy_assignable<U>::value, "Type U must be default constructible and trivially assignable");
				auto a = value_.get();
				for (auto & val : value)
				{
					*a = val;
					++a;
				}

				return ret.release();
			}

		public:

			//! \param value Kopiowana tablica
			ValueCarrierImpl(const LC & value) : value_(init(value, std::is_pod<U>::value)) {}
			//! Destruktor wirtualny
			virtual ~ValueCarrierImpl() {}
			//! \return Referencja do przechowywanej wartoœci
			virtual ref_type ref() const override final { return *(value_.get()); }

		private:
			//! Przechowywana tablica (przydzielana dynamicznie)
			const utils::unique_ptr<const LC> value_;
		};

		//! \tparam SmartPtr Typ inteligentnego wskaŸnika
		template<typename SmartPtr>
		//! Wersja dla smart pointerów
		class ValueCarrierImpl<typename ValueType<SmartPtr>::type,
			//! TODO
			SmartPtr/*typename std::enable_if<supports_pointer_operator<SmartPtr>::value>::type*/>
			: public ValueCarrierBase
		{
		private:
			//! Sprawdzenie czy nie pusty wskaŸnik do kontenera
			inline void verify() const { if (ptr_.operator->() == nullptr) throw std::invalid_argument("Uninitialized container"); }

		public:
			//! \param ptr Kopiowany wskaŸnik
			ValueCarrierImpl(const SmartPtr & ptr) : ptr_(ptr) { verify(); }
			//! \param ptr Przenoszony wskaŸnik
			ValueCarrierImpl(SmartPtr && ptr) : ptr_(std::move(ptr)) { verify(); }
			//! Destruktor wirtualny
			virtual ~ValueCarrierImpl() {}
			//! \return Referencja do przechowywanej wartoœci
			virtual ref_type ref() const override final { return *(ptr_.operator->()); }

		private:
			//! WskaŸnik z danymi
			const SmartPtr ptr_;
		};

	public:
		//! \tparam U Typ noœnika danych docelowych
		template<typename U>
		//! \param value Kopiowany noœnik danych
		ValueCarrier(const U & value) : valueCarier_(new ValueCarrierImpl<U>(value)) {}
		//! \tparam U Typ noœnika danych docelowych
		template<typename U>
		//! \param value przenoszony noœnik danych
		ValueCarrier(U && value) : valueCarier_(new ValueCarrierImpl<U>(std::move(value))) {}
		//! Destruktor
		~ValueCarrier() {}
		//! \return Referencja do przechowywanej wartoœci
		inline ref_type ref() const { return valueCarier_->ref(); }

	private:
		//! Owrapowane dane
		const utils::shared_ptr<const ValueCarrierBase> valueCarier_;
	};	
}

#endif  // __HEADER_GUARD_UTILS__VALUECARRIER_H__