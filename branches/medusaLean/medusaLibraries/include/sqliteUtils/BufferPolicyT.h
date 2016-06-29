/********************************************************************
	created:  2014/09/23	18:47:30
	filename: BufferPolicyT.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_SQLITEUTILS__BUFFERPOLICYT_H__
#define __HEADER_GUARD_SQLITEUTILS__BUFFERPOLICYT_H__

#include <array>
#include <vector>
#include <utils/Clamping.h>
#include <utils/SmartPtr.h>

namespace sqliteUtils
{
	//! Opis domy�lnych warto�ci dla rozmiar�w bufora
	enum BufferSizeTraits {
		MinBufferSize = (512 * 1024),		//! 512KB Minimalny rozmiar
		DefaultBufferSize = (2 * 1024 * 1024),	//! 2MB Domy�lny rozmiar
		MaxBufferSize = (10 * 1024 * 1024)	//! 10MB Maksymalny rozmiar
	};

	//! Interfejs obs�uguj�cy bufory danych
	//! \tparam T Typ danych w buforze
	template<typename T = uint8_t>
	class IBufferPolicyT
	{
	public:
		//! Destruktor wirtualny
		virtual ~IBufferPolicyT() {}
		//! \return Pocz�tek bufora
		virtual T * begin() = 0;
		//! \return Pierwszy element za buforem
		virtual T * end() = 0;
		//! \return Pocz�tek bufora
		virtual const T * begin() const = 0;
		//! \return Pierwszy element za buforem
		virtual const T * end() const = 0;
		//! \param size Nowy rozmiar danych do kt�rych powinien si� dopasowa� bufor
		//! \return Nowy faktyczny rozmiar bufora
		virtual std::size_t updateSize(const std::size_t size) = 0;
	};	

	//! \tparam T Typ danych przechowywanych w buforze	
	//! Klasa realizuj�ca polityk� bufora o sta�ym rozmiarze
	template<typename T = uint8_t>
	class FixedBufferPolicy : public IBufferPolicyT<T>
	{
	private:
		//! Typ kolekcji przechowujacej dane
		using Data = std::vector<T>;

	public:
		//! Domy�lny konsturktor
		FixedBufferPolicy(const std::size_t size = DefaultBufferSize) : data(size) {}
		//! Destruktor niewirtualny
		virtual ~FixedBufferPolicy() {}

		virtual T * begin() override { return data.data(); }
		virtual T * end() override { return data.data() + data.size(); }

		virtual const T * begin() const override { return data.data(); }
		virtual const T * end() const override { return data.data() + data.size(); }

		virtual std::size_t updateSize(const std::size_t size) override { return data.size(); }

	private:
		//! Faktyczne dane
		Data data;
	};

	//! \tparam T Typ danych przechowywanych w buforze
	//! \tparam Rozmiar bufora
	//! Klasa realizuj�ca polityk� bufora o sta�ym rozmiarze
	template<typename T = uint8_t, std::size_t Size = DefaultBufferSize>
	class FixedBufferPolicyT : public IBufferPolicyT<T>
	{
		static_assert((Size > 0), "Fixed buffer size must be greater than 0");

	private:
		//! Typ kolekcji przechowujacej dane
		using Data = std::array<T, Size>;

	public:
		//! Domy�lny konsturktor
		FixedBufferPolicyT() {}
		//! Destruktor niewirtualny
		virtual ~FixedBufferPolicyT() {}

		virtual T * begin() override { return &(data.data()[0]); }
		virtual T * end() override { return &(data.data()[Size]); }

		virtual const T * begin() const override { return &(data.data()[0]); }
		virtual const T * end() const override { return &(data.data()[Size]); }

		virtual std::size_t updateSize(const std::size_t size) override { return Size; }

	private:
		//! Faktyczne dane
		Data data;
	};

	//! \tparam T Typ danych przechowywanych w buforze
	//! Klasa realizuj�ca polityk� bufora o dynamicznie zwiekszanym rozmiarze przy zapisie
	template<typename T = uint8_t>
	class DynamicWriteBufferPolicy : public IBufferPolicyT<T>
	{
	private:
		//! Typ kolekcji przechowujacej dane
		using Data = std::vector<T> ;
		//! Wsp�czynnik procentowy opisuj�cy rozmiar bufora wzgl�dem rozmiaru strumienia
		static float BufferFactor() { return 1.1f; }

	public:
		//! Domy�lny konsturktor
		DynamicWriteBufferPolicy(const std::size_t InitialSize = DefaultBufferSize,
			const std::size_t MinSize = MinBufferSize,
			const std::size_t MaxSize = MaxBufferSize)
			: data(InitialSize), MinSize(MinSize), MaxSize(MaxSize)
		{
			if (InitialSize < MinSize || InitialSize > MaxSize){
				throw std::runtime_error("Invalid buffer range");
			}
		}
		//! Destruktor niewirtualny
		virtual ~DynamicWriteBufferPolicy() {}

		virtual T * begin() override { return data.data(); }
		virtual T * end() override { return data.data() + data.size(); }

		virtual const T * begin() const override { return data.data(); }
		virtual const T * end() const override { return data.data() + data.size(); }

		virtual std::size_t updateSize(const std::size_t size) override
		{
			const auto newSize = utils::clamp<std::size_t>(size * BufferFactor(), MinSize, MaxSize);

			if (data.size() < newSize){
				data.resize(newSize);
			}

			return newSize;
		}

	private:
		//! Faktyczne dane
		Data data;
		const std::size_t MinSize;
		const std::size_t MaxSize;
	};

	//! \tparam T Typ danych przechowywanych w buforze
	//! Klasa realizuj�ca polityk� bufora o dynamicznie zwiekszanym rozmiarze przy zapisie
	template<typename T = uint8_t, std::size_t InitialSize = DefaultBufferSize,
		std::size_t MinSize = MinBufferSize, std::size_t MaxSize = MaxBufferSize>
	class DynamicWriteBufferPolicyT : public IBufferPolicyT<T>
	{
	private:
		//! Typ kolekcji przechowujacej dane
		using Data = std::vector<T>;
		//! Wsp�czynnik procentowy opisuj�cy rozmiar bufora wzgl�dem rozmiaru strumienia
		static float BufferFactor() { return 0.05f; }

	public:
		//! Domy�lny konsturktor
		DynamicWriteBufferPolicyT() { data.resize(InitialSize); }
		//! Destruktor niewirtualny
		virtual ~DynamicWriteBufferPolicyT() {}

		virtual T * begin() override { return data.data(); }
		virtual T * end() override { return data.data() + data.size(); }

		virtual const T * begin() const override { return data.data(); }
		virtual const T * end() const override { return data.data() + data.size(); }

		virtual std::size_t updateSize(const std::size_t size) override
		{
			const auto newSize = utils::clamp(size * BufferFactor(), (std::size_t)MinSize, (std::size_t)MaxSize);

			if (data.size() < newSize){
				data.resize(newSize);
			}

			return newSize;
		}

	private:
		//! Faktyczne dane
		Data data;
	};
}

#endif	// __HEADER_GUARD_SQLITEUTILS__BUFFERPOLICYT_H__
