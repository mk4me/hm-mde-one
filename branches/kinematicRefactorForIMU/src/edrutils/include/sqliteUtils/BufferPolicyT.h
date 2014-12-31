/********************************************************************
	created:  2014/09/23	18:47:30
	filename: BufferPolicyT.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_SQLITEUTILS__BUFFERPOLICYT_H__
#define __HEADER_GUARD_SQLITEUTILS__BUFFERPOLICYT_H__

//#include <utils/Utils.h>
//#include <utils/Debug.h>
#include <array>
#include <vector>
#include <utils/Utils.h>

namespace sqliteUtils
{
	enum BufferSizeTraits {
		MinBufferSize = (100 * (1 << 10)),		//! 100KB
		DefaultBufferSize = (2 * (1 << 20)),	//! 2MB
		MaxBufferSize = (10 * (1 << 20))	//! 10MB
	};

	//! \tparam T Typ danych przechowywanych w buforze
	//! \tparam Rozmiar bufora
	//! Klasa realizuj¹ca politykê bufora o sta³ym rozmiarze
	template<typename T, int Size = DefaultBufferSize>
	class FixedBufferPolicy
	{
		//static_assert((Size > 0), "Fixed buffer size must be greater than 0");

	private:
		//! Typ kolekcji przechowujacej dane
		typedef std::array<T, Size> Data;

	public:
		//! Domyœlny konsturktor
		FixedBufferPolicy() {}
		//! Destruktor niewirtualny
		~FixedBufferPolicy() {}

		T * begin() { return &(data.data()[0]); }
		T * end() { return &(data.data()[Size]); }

		const T * begin() const { return &(data.data()[0]); }
		const T * end() const { return &(data.data()[Size]); }

		const int updateSize(const int size) { return Size; }

	private:
		//! Faktyczne dane
		Data data;
	};

	//! \tparam T Typ danych przechowywanych w buforze
	//! Klasa realizuj¹ca politykê bufora o dynamicznie zwiekszanym rozmiarze przy zapisie
	template<typename T, int InitialSize = DefaultBufferSize>
	class DynamicWriteBufferPolicy
	{
	private:
		//! Typ kolekcji przechowujacej dane
		typedef std::vector<T> Data;
		//! Wspó³czynnik procentowy opisuj¹cy rozmiar bufora wzglêdem rozmiaru strumienia
		static const float BufferFactor() { return 0.05f; }

	public:
		//! Domyœlny konsturktor
		DynamicWriteBufferPolicy() { data.resize(InitialSize); }
		//! Destruktor niewirtualny
		~DynamicWriteBufferPolicy() {}

		T * begin() { return data.data(); }
		T * end() { return data.data() + data.size(); }

		const T * begin() const { return data.data(); }
		const T * end() const { return data.data() + data.size(); }

		const int updateSize(const int size) {

			const auto newSize = utils::clamp(size * BufferFactor(), (int)MinBufferSize, (int)MaxBufferSize);

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
