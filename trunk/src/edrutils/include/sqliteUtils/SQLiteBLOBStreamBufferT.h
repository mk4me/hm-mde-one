/********************************************************************
	created:  2014/09/24	16:30:03
	filename: SQLiteBLOBStreamBufferT.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_SQLITEUTILS__SQLITEBLOBSTREAMBUFFERT_H__
#define __HEADER_GUARD_SQLITEUTILS__SQLITEBLOBSTREAMBUFFERT_H__

#include <string>
#include <streambuf>
#include <iosfwd>
#include <sqliteUtils/BufferPolicyT.h>

namespace sqliteUtils
{
	//! Wzorzec klasy obslugujacej sturmieniowe IO do blobow bazy sqlite
	//! \tparam _Elem Elementy sk³adowane w blobach
	//! \tparam _Traits Cechy obiektu typu _Elem
	//! \tparam _BufferPolicy Polityka zarzadzania buforem danych
	template<typename _Elem = char,
		typename _Traits = std::char_traits<_Elem>,
		typename _BufferPolicy = FixedBufferPolicy<_Elem > >
	class SQLiteBLOBStreamBufferT : public std::basic_streambuf<_Elem, _Traits>
	{
	private:

		enum
		{	// constants for bits in stream state		
			_Constant = 2,	// set if character array nonmutable
			_Noread = 4,	// set if character array cannot be read
			_Append = 8,	// set if all writes are appends
			_Atend = 16		// set if initial writes are appends
		};

		//! Internal state
		typedef int _Strstate;

		//! Typ inta
		typedef typename _Traits::int_type int_type;
		//! Typ pozycji
		typedef typename _Traits::pos_type pos_type;
		//! Typ offsetu
		typedef typename _Traits::off_type off_type;

	public:
		//! Typ mojego bufora
		typedef SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy> _Myt;
		//! Typ bufora strumienia
		typedef basic_streambuf<_Elem, _Traits> _Mysb;
		//! Typ mojego stanu
		typedef typename _Traits::state_type _Myst;

	public:
		//! Konstruktor
		//! \param _Mode W jakim trybie otwarty jest strumieñ
		//! \param db Baza na której bêdziemy dzia³aæ
		//! \param dbName Nazwa bazy na której dzia³amy (https://sqlite.org/c3ref/blob_open.html)
		//! \param table Nazwa tabeli
		//! \param column Nazwa kolumny
		//! \param rowID Unikalny identyfikator wiersza w zadanej tabeli		
		SQLiteBLOBStreamBufferT(const std::string & path, const std::string & table,
			const std::string & column, const sqlite3_int64 rowID, const int initialSize,
			const std::string & dbName = "main", const std::string & key = "",
			std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);

		//! Destruktor wirtualny
		virtual ~SQLiteBLOBStreamBufferT();

	protected:

		virtual std::streamsize showmanyc();

		virtual int_type pbackfail(int_type = _Traits::eof());

		virtual int_type overflow(int_type _Meta = _Traits::eof());

		virtual int_type underflow();

		virtual pos_type seekoff(off_type _Off,
			std::ios_base::seekdir _Way,
			std::ios_base::openmode _Which = std::ios_base::in | std::ios_base::out);

		virtual pos_type seekpos(pos_type _Ptr,
			std::ios_base::openmode _Mode = std::ios_base::in | std::ios_base::out);

		virtual int sync();

	private:

		//! \return Rozmiar bufora
		const int streamBufferSize() const;
		//! \return Rozmiar bufora
		const int blobBufferSize() const;

		//! \param idx Globalny index
		//! \return Maksymalny index bufora uwzglêdniaj¹cy zakres strumienia i rozmiar bufora
		const int blobMaxIDX(const int idx) const;

		//! \return Maksymalny index globalny bufora do odczytu
		const int blobReadMaxIDX() const;

		//! \return Maksymalny index globalny bufora do zapisu
		const int blobWriteMaxIDX() const;

		//! \param blobBase Globalna baza
		//! \param bufferBase Baza lokalna bufora
		//! \param bufferPos Lokalna pozycja bufora
		//! \return Globalna pozycja strumienia
		static const int blobGlobalIDX(const int blobBase, const _Elem * bufferBase,
			const _Elem * bufferPos);

		//! \return Globalna pozycja dla zapisu
		const int blobGlobalWriteIDX() const;

		//! \return Globalna pozycja dla odczytu
		const int blobGlobalReadIDX() const;

		//! \param pos Pozycja we wspolrzednych bloba [B]
		//! \return Pozycja we wspolrzednych strumienia
		static const int toStreamPos(int pos);
		//! \param pos Pozycja we wspolrzednych strumienia [_Elem]
		//! \return Pozycja we wspolrzednych bloba
		static const int toBlobPos(int pos);

		//! Aktualizuje globalny indeks ostatniego elementu strumienia
		void updateBlobStreamEnd();

		//! Metoda wypelnia bufor zawartoscia bloba
		//! \param buffer Wskaznik poczatku bufora gdzie moge czytac dane z bloba
		//! \param bufferSize Ile maksymalnie moge wczytac do bufora
		//! \param blob Blob z ktorego czytam
		//! \param blobIDX Wskaznik w obrebie bloba od ktorego zaczynam czytac
		//! \param blobSize Rozmiar bloba
		//! \return Ilosc znakow odczytanych z bloba
		const int refillBuffer(const int blobIDX, _Elem * bufferBase);

		//! Metoda wypycha lokalne zmiany do bloba - synchronizuje
		//! \return Czy udalo sie dane zapisac do bloba
		const bool flushBuffer(const _Elem * base, const int count,
			const int blobIDX);

		//! Metoda generuje nazwe tabeli dla zapytaia aktualizujacego bloba
		//! \param dbName Nazwa bazy danych
		//! \param table Nazwa tabeli
		//! \return Nazwa tabeli dla zapytania update
		static const std::string updateTableName(const std::string & dbName,
			const std::string & table);

		//! \param _Mode tryb otwarcia strumienia
		//! \return Wewnêtrzny tryb otwarcia strumienia
		static const _Strstate _Getstate(std::ios_base::openmode _Mode);

	private:

		//! ------------ OPIS DANYCH W SQLITE ------------------
		const std::ios_base::openmode mode_;
		//! Œcie¿ka do bazy danych
		const std::string path;
		//! Has³o bazy
		const std::string key;
		//! Nazwa bazy
		std::string dbName;
		//! Nazwa tabeli
		std::string table;
		//! Nazwa kolumny
		std::string column;
		//! Identyfikator wiersza
		sqlite3_int64 rowID;
		//! Aktualny rozmiar bloba
		int blobSize;

		//! ----------------- STAN BUFORÓW --------------------
		//! Bufor
		_BufferPolicy buffer;
		//! Aktualny indeks wewn¹trz bloba dla bufora odczytu
		int blobReadBase;
		//! Aktualny indeks wewn¹trz bloba dla bufora zapisu
		int blobWriteBase;
		//! Czy by³ zapis przez putback
		bool putBackWrite;

		//! -------------- STAN STRUMIENIA ---------------------
		_Strstate Mystate;	// Stan strumienia
		int blobSeekhigh;		// Faktyczny rozmiar strumienia - uwzglêdnia rozmiar bloba i stan zape³nienia bufora
	};

	#include <sqliteUtils/SQLiteBLOBStreamBufferTImpl.h>
}

#endif	// __HEADER_GUARD_SQLITEUTILS__SQLITEBLOBSTREAMBUFFERT_H__