#include "CommunicationPCH.h"
#include "SQLCipherStorage.h"
#include <boost/format.hpp>
#include <sqlite3.h>
#include <boost/shared_array.hpp>
#include <streambuf>
#include <boost/array.hpp>

class SQLitePreparedStatement
{
public:
	SQLitePreparedStatement(sqlite3 * db, const std::string & sql)
		: statement(nullptr)
	{
		sqlite3_stmt * q = nullptr;
		auto ret = sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &q, nullptr);

		if (ret == SQLITE_OK){
			statement = q;
		}
	}

	sqlite3_stmt * get() const
	{
		return statement;
	}

	sqlite3_stmt * release()
	{
		sqlite3_stmt * q = nullptr;
		std::swap(q, statement);
		return q;
	}

	~SQLitePreparedStatement()
	{
		if (statement != nullptr){
			sqlite3_finalize(statement);
		}
	}

private:
	//! Skompilowane zapytanie
	sqlite3_stmt * statement;
};

enum BufferSizeTraits {
	MinBufferSize = (1 << 8),		//! 256B
	DefaultBufferSize = (1 << 11),	//! 2KB
	MaxBufferSize = (1 << 20) * 10	//! 10MB
};

//! \tparam T Typ danych przechowywanych w buforze
//! \tparam Rozmiar bufora
//! Klasa realizuj¹ca politykê bufora o sta³ym rozmiarze
template<typename T, int Size = DefaultBufferSize>
class FixedBufferPolicy
{
private:
	//! Typ kolekcji przechowujacej dane
	typedef boost::array<T, Size> Data;

public:
	//! Domyœlny konsturktor
	FixedBufferPolicy() {}
	//! Destruktor niewirtualny
	~FixedBufferPolicy() {}

	T * begin() { return data->c_array();  }
	T * end() { return &data->c_array()[data->size()-1]; }

	const T * begin() const { return data->c_array(); }
	const T * end() const { return &data->c_array()[data->size() - 1]; }

	const int updateSize(const int size) { if (data == nullptr) { data.reset(new Data); } return Size; }

private:
	//! Faktyczne dane
	utils::shared_ptr<Data> data;
};

//! \tparam T Typ danych przechowywanych w buforze
//! Klasa realizuj¹ca politykê bufora o dynamicznie zwiekszanym rozmiarze przy zapisie
template<typename T>
class DynamicWriteBufferPolicy
{
private:
	//! Typ kolekcji przechowujacej dane
	typedef std::vector<T> Data;
	//! Wspó³czynnik procentowy opisuj¹cy rozmiar bufora wzglêdem rozmiaru strumienia
	static const float BufferFactor() { return 0.05f; }

public:
	//! Domyœlny konsturktor
	DynamicWriteBufferPolicy() {}
	//! Destruktor niewirtualny
	~DynamicWriteBufferPolicy() {}

	T * begin() { return &(data->front()); }
	T * end() { return &(data->back()); }

	const T * begin() const { return &(data->front()); }
	const T * end() const { return &(data->back()); }

	const int updateSize(const int size) {

		const auto newSize = std::min(std::max((int)(size * BufferFactor()), (int)MinBufferSize), (int)MaxBufferSize);

		if (data == nullptr) {
			data.reset(new Data);
		}

		if (data->capacity() < newSize){
			data->reserve(newSize);
		}

		return newSize;
	}

private:
	//! Faktyczne dane
	utils::shared_ptr<Data> data;
};

//! Wzorzec klasy obslugujacej sturmieniowe IO do blobow bazy sqlite
//! \tparam _Elem Elementy sk³adowane w blobach
//! \tparam _Traits Cechy obiektu typu _Elem
//! \tparam _BufferPolicy Polityka zarzadzania buforem danych
template<typename _Elem,
	typename _Traits,
	typename _BufferPolicy = FixedBufferPolicy<_Elem>>
class basic_sqliteblobbuf : public std::basic_streambuf<_Elem, _Traits>
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
	typedef basic_sqliteblobbuf<_Elem, _Traits> _Myt;
	//! Typ bufora strumienia
	typedef basic_streambuf<_Elem, _Traits> _Mysb;
	//! Typ polityki bufora
	typedef _BufferPolicy _MyBufferPolicy;
	//! Typ mojego stanu
	typedef typename _Traits::state_type _Myst;

private:

	//! \return Rozmiar bufora
	inline const int bufferSize() const
	{
		return buffer.end() - buffer.begin();
	}

	//! \param idx Globalny index
	//! \return Maksymalny index bufora uwzglêdniaj¹cy zakres strumienia i rozmiar bufora
	inline const int maxIDX(const int idx) const
	{
		return std::min(idx + bufferSize(), _Seekhigh);
	}

	//! \return Maksymalny index globalny bufora do odczytu
	inline const int readMaxIDX() const
	{
		return maxIDX(blobReadIDX);
	}

	//! \return Maksymalny index globalny bufora do zapisu
	inline const int writeMaxIDX() const
	{
		return maxIDX(blobWriteIDX);
	}

	//! \param globalBase Globalna baza
	//! \param localBase Baza lokalna bufora
	//! \param localIDX Lokalna pozycja bufora
	//! \return Globalna pozycja bufora
	inline static const int globalIDX(const int globalBase, const _Elem * localBase,
		const _Elem * localIDX)
	{
		return globalBase + (int)(localIDX - localBase);
	}

	//! \return Globalna pozycja dla zapisu
	inline const int globalWriteIDX() const
	{
		return globalIDX(blobWriteIDX, _Mysb::pbase(), _Mysb::pptr());
	}

	//! \return Globalna pozycja dla odczytu
	inline const int globalReadIDX() const
	{
		return globalIDX(blobReadIDX, _Mysb::eback(), _Mysb::gptr());		
	}

	inline void updateStreamEnd()
	{
		if (_Seekhigh < globalWriteIDX()){
			_Seekhigh = globalWriteIDX();
		}
	}

public:

	//! Destruktor wirtualny - zapisuje zmodyfikowane dane do bazy,
	//! zamyka i zwalnia wszystkie zasoby bazy
	virtual __CLR_OR_THIS_CALL ~basic_sqliteblobbuf()
	{
		//pozamykaæ wszystko i pozwalaniac zasoby bufora i sqlite
		if (_Myblob != nullptr){
			synch();
			sqlite3_blob_close(_Myblob);
		}

		if (update != nullptr){
			sqlite3_finalize(update);
		}
	}

	void synch()
	{

	}

	//! Konstruktor
	//! \param _Mode W jakim trybie otwarty jest strumieñ
	//! \param db Baza na której bêdziemy dzia³aæ
	//! \param dbName Nazwa bazy na której dzia³amy (https://sqlite.org/c3ref/blob_open.html)
	//! \param table Nazwa tabeli
	//! \param column Nazwa kolumny
	//! \param rowID Unikalny identyfikator wiersza w zadanej tabeli
	//! \param buffer Bufor uzywany wewnatrz
	basic_sqliteblobbuf(sqlite3 * db,
		const std::string & dbName, const std::string & table,
		const std::string & column, const sqlite3_int64 rowID,
		std::ios_base::openmode _Mode =	std::ios_base::in | std::ios_base::out)
		: _Mysb(), db(db), dbName(dbName), table(table), column(column),
		rowID(rowID), _Myblob(nullptr), update(nullptr),
		blobReadIDX(0), blobWriteIDX(0), blobSize(-1), _Seekhigh(-1)
	{
		//teraz otwieram bloba
		sqlite3_blob * b = nullptr;
		auto ret = sqlite3_blob_open(db, dbName.c_str(), table.c_str(),
			column.c_str(), rowID, _Mode & std::ios_base::out, &b);

		if (ret != SQLITE_OK){
			sqlite3_blob_close(b);
			throw std::runtime_error("Error initializing sqlite db blob stream");
		}

		_Myblob = b;

		_Mystate = _Getstate(_Mode);

		//pobieram rozmiar bloba ¿eby dobrac bufor na poczatek
		_Seekhigh = blobSize = sqlite3_blob_bytes(_Myblob);		

		//czy pierwsze wywolanie i tryb append?
		if (_Mystate & _Append)
		{
			blobWriteIDX = blobSize;
		}
		else{
			blobWriteIDX = 0;
		}		
	}

protected:

	virtual int_type __CLR_OR_THIS_CALL overflow(int_type _Meta =
		_Traits::eof())
	{	
		if ((_Mystate & _Constant) || _Myblob == nullptr){
			return (_Traits::eof()); // read only mode or no open blob, fail
		} else if (_Traits::eq_int_type(_Traits::eof(), _Meta)) {
			return (_Traits::not_eof(_Meta));	// EOF, return success code
		}else if ((buffer.begin() == nullptr) && (buffer.updateSize(std::max(blobSize, (int)DefaultBufferSize)) < 1)){
			return (_Traits::eof());	// could not initialize internal buffer, fail
		}

		//czy cos by³o pisane do bufora?
		if (_Mysb::pptr() != nullptr){
			// musze zrzucic bufor do strumienia
			//zapamietuje poprzedni rozmiar aby ewentualnie rozmiar bufora dopasowac
			const auto oldBlobSize = blobSize;

			// je¿eli cos czyta³em wczeœniej to muszê zapamiêtaæ i wyzerowaæ je¿li to nie ten sam obszar
			if (_Mysb::gptr() != nullptr){
				blobReadIDX += (_Mysb::gptr() - _Mysb::eback());
				_Mysb::setg(buffer.begin(), nullptr, buffer.end());
			}

			//zapisuje bufor w stumieniu
			if (flushBuffer(_Mysb::pbase(), _Mysb::pptr(), blobWriteIDX) == false){
				return (_Traits::eof());	// zapis sie nie powiodlo, fail
			}

			//czy zmieni³ siê rozmiar?
			if (oldBlobSize < blobSize){
				//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
				buffer.updateSize(blobSize);
			}
			//rozmiar sie nie zmieni³
			//sprawdzam gdzie teraz w strumieniu jestem
			else if (blobWriteIDX < blobSize){
				//ca³y czas piszê wewn¹trz bloba, moge odœwiezyæ zawartoœæ bufor ze strumienia
				auto s = refillBuffer(buffer.begin(), buffer.end() - buffer.begin(), _Myblob, blobWriteIDX, blobSize);

				//czy cos pobralem albo jakis blad?
				if (s < 1){
					return (_Traits::eof()); // nic nie pobralem wiec pewnie koniec
				}
			}

			//ustawiam nowy bufor i jego stan
			_Mysb::setp(buffer.begin(), buffer.begin(), buffer.end());
		}
		//nie pisa³em do bufora
		else if (_Mysb::gptr() != nullptr){
			//czyta³em coœ z bufora a chcê bufor do zapisu
			//sprawdzam czy obszary odczytu/zapisu siê nie zazêbiaj¹
			if (blobWriteIDX < blobReadIDX || blobWriteIDX >= std::min(blobReadIDX + bufferSize(), _Seekhigh)){
				
				//niestety, obszary s¹ roz³¹czne, muszê zapamiêtac stan bufora do odczytu i zresetowaæ jego obszar
				blobReadIDX += (_Mysb::gptr() - _Mysb::eback());
				_Mysb::setg(buffer.begin(), nullptr, buffer.end());

				//prze³adowuje bufor nowymi danymi, bêdê po nich pisa³
				auto s = refillBuffer(buffer.begin(), buffer.end() - buffer.begin(), _Myblob, blobWriteIDX, blobSize);

				//czy cos pobralem albo jakis blad?
				if (s < 1){
					return (_Traits::eof()); // nic nie pobralem wiec pewnie koniec
				}

				//ustawiam nowy bufor i jego stan
				_Mysb::setp(buffer.begin(), buffer.begin(), buffer.end());
			}
			else{
				//obszary pokrywaj¹ siê - wystarczy poprzestawiaæ wskaŸniki
				auto diff = blobWriteIDX - blobReadIDX;
				blobWriteIDX = blobReadIDX;
				//poprawiam obszar do zapisu
				_Mysb::setp(buffer.begin(), buffer.begin() + diff, buffer.end());
			}
		}
		else{
			//nie by³o zapisu i odczytu albo oba obszary nieaktyne po seek

			if (blobSize > 0 && blobWriteIDX != blobSize){
				//uzupelniam bufor
				auto s = refillBuffer(buffer.begin(), buffer.end() - buffer.begin(), _Myblob, blobWriteIDX, blobSize);

				//czy cos pobralem albo jakis blad?
				if (s < 1){
					return (_Traits::eof()); // nic nie pobralem wiec pewnie koniec
				}
			}			

			//ustawiam nowy bufor i jego stan
			setp(buffer.begin(), buffer.begin(), buffer.end());
		}

		//zapisujemy
		*_Mysb::_Pninc() = _Traits::to_char_type(_Meta);
		//aktualizuje koniec strumienia dla poprawnego odczytu ca³ego strumienia podczas zapisu
		updateStreamEnd();

		return (_Meta);
	}

	virtual int_type __CLR_OR_THIS_CALL underflow()
	{	// get an element from stream, point on it
		if (_Myblob == nullptr || blobSize < 1 || (_Mystate & _Noread) ||
			((buffer.begin() == nullptr) && (buffer.updateSize(std::max(blobSize, (int)DefaultBufferSize)) < 1))){
			return (_Traits::eof());	// no open blob or no data to read or failed to create buffer, fail
		}

		updateStreamEnd();

		//czy konieczne jest prze³adowanie bufora?
		bool refill = true;

		//sprawdzam czy byly zapisy
		if (_Mysb::pptr() != nullptr){
			//musze sprawdzic czy to gdzie sie chce znalezc wypada poza aktualny obszar - jezeli tak to musze zapisac dane, odswiezyc bufor
			if ((blobReadIDX < blobWriteIDX) || (globalWriteIDX() <= blobReadIDX)){
				//wypadam poza
				blobWriteIDX = globalWriteIDX();

				const auto oldBlobSize = blobSize;

				if (flushBuffer(_Mysb::pbase(), _Mysb::pptr(), blobWriteIDX) == false){
					return (_Traits::eof());	// zapis sie nie powiodlo, fail
				}

				if (oldBlobSize < blobSize){
					//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
					buffer.updateSize(blobSize);
				}

				_Mysb::setp(buffer.begin(), nullptr, buffer.end());
			}
			else{
				//zazêbiaj¹ siê obszary, wystarczy wska¿niki poprawiæ
				refill = false;
				//maks rozmiar do odczytu
				const auto size = ((blobWriteIDX + bufferSize()) <= blobSize) ? bufferSize() : _Seekhigh - globalWriteIDX();
				//poprawiam obszar do odczytu
				_Mysb::setg(buffer.begin(), buffer.begin() + blobReadIDX - blobWriteIDX, buffer.begin() + size);
				blobReadIDX = blobWriteIDX;
			}
		}

		if (refill == true){

			//uzupelnam bufor
			auto s = refillBuffer(buffer.begin(), buffer.end() - buffer.begin(), _Myblob, blobReadIDX, blobSize);

			//czy cos pobralem albo jakis blad?
			if (s < 1){
				return (_Traits::eof()); // nic nie pobralem wiec pewnie koniec
			}

			//inicjuje obszar do odczytu
			_Mysb::setg(buffer.begin(), buffer.begin(), buffer.begin() + s);
		}

		// no codecvt facet, just get it
		_Elem _Ch = *(_Mysb::gptr());
		return _Traits::to_int_type(_Ch);
	}

	virtual pos_type seekoff(off_type _Off,
		std::ios_base::seekdir _Way,
		std::ios_base::openmode _Which = std::ios_base::in | std::ios_base::out)
	{	// change position by _Off, according to _Way, _Mode

		updateStreamEnd();

		auto _OffPos = _Off;

		if (_Which & std::ios_base::in)
		{	// position within read buffer
			if (_Way == std::ios_base::end)
				_OffPos += (off_type)_Seekhigh;
			else if (_Way == std::ios_base::cur
				&& (_Which & std::ios_base::out) == 0)
				_OffPos += (off_type)((_Mysb::gptr() != nullptr) ? globalReadIDX() : blobReadIDX);
			else if (_Way != std::ios_base::beg)
				_OffPos = std::_BADOFF;

			if (0 <= _OffPos && _OffPos <= _Seekhigh)
			{	// change read position

				bool writeUpdated = false;

				if (_Mysb::gptr() == nullptr){
					blobReadIDX = _OffPos;
				}
				else if (blobReadIDX <= _OffPos && _OffPos < readMaxIDX()){
					_Mysb::gbump((int)(globalReadIDX() - _OffPos));
				}
				else{
					
					if (_Mysb::pptr() != nullptr){

						const auto oldBlobSize = blobSize;

						if (flushBuffer(_Mysb::pbase(), _Mysb::pptr(), blobWriteIDX) == false){
							return (_Traits::eof());	// zapis sie nie powiodlo, fail
						}

						if (oldBlobSize < blobSize){
							//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
							buffer.updateSize(blobSize);
						}

						_Mysb::setp(buffer.begin(), nullptr, buffer.end());
						writeUpdated = true;
					}

					blobReadIDX = _OffPos;
					_Mysb::setg(buffer.begin(), nullptr, buffer.end());
				}

				if ((_Which & std::ios_base::out) && writeUpdated == false){
					if (blobWriteIDX <= _OffPos && _OffPos < readMaxIDX()){
						_Mysb::pbump((int)(globalWriteIDX() - _OffPos));
					}
					else{

						const auto oldBlobSize = blobSize;

						if (flushBuffer(_Mysb::pbase(), _Mysb::pptr(), blobWriteIDX) == false){
							return (_Traits::eof());	// zapis sie nie powiodlo, fail
						}

						if (oldBlobSize < blobSize){
							//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
							buffer.updateSize(blobSize);
						}

						blobWriteIDX = _OffPos;
						_Mysb::setp(buffer.begin(), nullptr, buffer.end());
					}
				}
			}
			else
				_OffPos = std::_BADOFF;
		}
		else if (_Which & std::ios_base::out)
		{	// position within write buffer
			if (_Way == std::ios_base::end)
				_OffPos += (off_type)_Seekhigh;
			else if (_Way == std::ios_base::cur)
				_OffPos += (off_type)((_Mysb::pptr() != nullptr) ? globalWriteIDX() : blobWriteIDX);				
			else if (_Way != std::ios_base::beg)
				_OffPos = std::_BADOFF;

			if (0 <= _OffPos && _OffPos <= _Seekhigh){
				if (blobWriteIDX <= _OffPos && _OffPos < readMaxIDX()){
					_Mysb::pbump((int)(globalWriteIDX() - _OffPos));
				}
				else{

					if (_Mysb::gptr() != nullptr){
						blobReadIDX = globalReadIDX();
					}

					const auto oldBlobSize = blobSize;

					if (flushBuffer(_Mysb::pbase(), _Mysb::pptr(), blobWriteIDX) == false){
						return (_Traits::eof());	// zapis sie nie powiodlo, fail
					}

					if (oldBlobSize < blobSize){
						//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
						buffer.updateSize(blobSize);
					}

					if (_Mysb::gptr() != nullptr){
						_Mysb::setg(buffer.begin(), nullptr, buffer.end());
					}

					blobWriteIDX = _OffPos;
					_Mysb::setp(buffer.begin(), nullptr, buffer.end());
				}
			}
			else
				_OffPos = std::_BADOFF;
		}

#if _HAS_CPP0X
		else if (_OffPos != 0)

#else /* _HAS_CPP0X */
		else
#endif /* _HAS_CPP0X */

			_OffPos = std::_BADOFF;	// neither read nor write buffer selected, fail
		return (pos_type(_Off));
	}

	virtual pos_type seekpos(pos_type _Ptr,
		std::ios_base::openmode _Mode = std::ios_base::in | std::ios_base::out)
	{	// change position to _Pos, according to _Mode
		std::streamoff _Off = (std::streamoff)_Ptr;

		updateStreamEnd();

		if (_Off == std::_BADOFF)
			;
		else if (_Mode & std::ios_base::in && _Mysb::gptr() != 0)
		{	// position within read buffer
			if (0 <= _Off && _Off <= _Seekhigh)
			{	// change read position
				_Mysb::gbump((int)(_Mysb::eback() - _Mysb::gptr() + _Off));
				if (_Mode & std::ios_base::out && _Mysb::pptr() != 0)
					_Mysb::setp(_Mysb::pbase(), _Mysb::gptr(),
					_Mysb::epptr());	// change write position to match
			}
			else
				_Off = std::_BADOFF;
		}
		else if (_Mode & std::ios_base::out && _Mysb::pptr() != 0)
		{	// position within write buffer
			if (0 <= _Off && _Off <= _Seekhigh)
				_Mysb::pbump((int)(_Mysb::eback()
				- _Mysb::pptr() + _Off));	// change write position
			else
				_Off = std::_BADOFF;
		}
		else
			_Off = std::_BADOFF;	// neither read nor write buffer selected, fail
		return (std::streampos(_Off));
	}

	virtual int __CLR_OR_THIS_CALL sync()
	{	// synchronize C stream with external file
		return (_Myblob == 0
			|| _Traits::eq_int_type(_Traits::eof(), overflow())
			/*|| 0 <= fflush(_Myblob)*/ ? 0 : -1);
	}

private:

	//! Metoda wypelnia bufor zawartoscia bloba
	//! \param buffer Wskaznik poczatku bufora gdzie moge czytac dane z bloba
	//! \param bufferSize Ile maksymalnie moge wczytac do bufora
	//! \param blob Blob z ktorego czytam
	//! \param blobIDX Wskaznik w obrebie bloba od ktorego zaczynam czytac
	//! \param blobSize Rozmiar bloba
	//! \return Ilosc znakow odczytanych z bloba
	static const int refillBuffer(char * buffer, const int bufferSize,
		sqlite3_blob * blob, const int blobIDX, const int blobSize)
	{
		int toRead = std::min(bufferSize, blobSize - blobIDX);

		// jak mam jeszcze cos do czytania
		if (toRead > 0){

			//teraz czytam do mojego bufora
			auto ret = sqlite3_blob_read(blob, buffer, toRead, blobIDX);

			// jak sie nie udalo czytac z sqlite
			if (ret != SQLITE_OK){
				toRead = -1;
			}
		}

		return toRead;
	}

	//! Metoda wypycha lokalne zmiany do bloba - synchronizuje
	//! \return Czy udalo sie dane zapisac do bloba
	const bool flushBuffer(const char * base, const char * end,
		int & blobIDX)
	{
		// czy zapis sie udal?
		return save(base, end - base, blobIDX);
	}

	//! Metoda probuje tworzyc zapytanie aktualizujace bloba
	//! \return Czy udalo sie utworzyc zapytanie
	const bool tryCreateUpdateQuery()
	{
		if (update == nullptr) {

			auto updateq = (boost::format("UPDATE %1% SET %2% = ? WHERE iRow = %3%;")
				% updateTableName(dbName, table) % column % rowID).str();

			SQLitePreparedStatement u(db, updateq);

			if (u.get() != nullptr){
				update = u.release();
			}
			else {
				return false;	// prepare statement not created, fail
			}
		}

		return true;
	}

	//! Metoda zapisuje bufor do bloba
	//! \param buffer Bufor ktory zapisujemy
	//! \param bufferSize Ile z bufora zapisujemy
	//! \param blobIDX Gdzie w ramach bloba zpiasujemy zawartosc bufora
	//! \return Czy zapis sie powiód³
	const bool save(const char * buffer, const unsigned int bufferSize,
		int & blobIDX)
	{
		const unsigned int newSize = blobIDX + bufferSize;

		// zapis do bloba
		// czy dane jeszcze sie zmieszcza?
		if (newSize <= blobSize){
			//pisze bezposrednio do bloba
			auto ret = sqlite3_blob_write(_Myblob, buffer, bufferSize, blobIDX);

			if (ret != SQLITE_OK){
				return false;
			}
		}
		else {

			//jezeli jeszcze nie mam probuje utworzyc odpowiednie zapytanie aktualizujace bloba
			if (tryCreateUpdateQuery() == false){
				return false;
			}

			//bufor tymczasowy na nowy blob
			std::string tmpBuffer;

			try{

				//rezerwuje na nowy rozmiar
				tmpBuffer.reserve(newSize);

			}
			catch (...){
				return false; // failed to compose complete data, probably out of memory
			}

			//rozszerzam do aktualnego rozmiaru
			tmpBuffer.resize(blobIDX);
			//czytam calego bloba
			auto ret = sqlite3_blob_read(_Myblob, &tmpBuffer[0], blobIDX, 0);

			if (ret != SQLITE_OK){
				return false;
			}

			//zamykam bloba
			ret = sqlite3_blob_close(_Myblob);
			_Myblob = nullptr;

			if (ret != SQLITE_OK){
				return false;
			}

			//dopisuje to co mam
			tmpBuffer.append(buffer, bufferSize);

			//aktualizuje bloba
			ret = sqlite3_bind_blob(update, 1, tmpBuffer.c_str(), newSize, SQLITE_STATIC);

			if (ret != SQLITE_OK){
				return false; // failed to bind sqlite query
			}

			//odpalam update
			ret = sqlite3_step(update);

			if (ret != SQLITE_OK){
				return false; // failed to write to sqlite db
			}

			// wszystko ok
			// otwieram bloba ponownie
			ret = sqlite3_blob_open(db, dbName.c_str(), table.c_str(), column.c_str(), rowID, !(_Mystate & _Constant), &_Myblob);

			if (ret != SQLITE_OK){
				sqlite3_blob_close(_Myblob);
				_Myblob = nullptr;
				return false; // failed to open sqlite blob for io
			}

			blobSize = newSize;
		}

		blobIDX += bufferSize;

		return true;
	}

	//! Metoda generuje nazwe tabeli dla zapytaia aktualizujacego bloba
	//! \param dbName Nazwa bazy danych
	//! \param table Nazwa tabeli
	//! \return Nazwa tabeli dla zapytania update
	static const std::string updateTableName(const std::string & dbName,
		const std::string & table)
	{
		return (dbName.empty() == true) ? table : dbName + '.' + table;
	}

	//! \param _Mode tryb otwarcia strumienia
	//! \return Wewnêtrzny tryb otwarcia strumienia
	const _Strstate _Getstate(std::ios_base::openmode _Mode)
	{	// convert open mode to stream state bits
		_Strstate _State = (_Strstate)0;
		if (!(_Mode & std::ios_base::in))
			_State |= _Noread;
		if (!(_Mode & std::ios_base::out))
			_State |= _Constant;
		if (_Mode & std::ios_base::app)
			_State |= _Append;
		if (_Mode & std::ios_base::ate)
			_State |= _Atend;
		return (_State);
	}

private:

	//! ------------ OPIS DANYCH W SQLITE ------------------
	//! Baza danych
	sqlite3 * db;
	//! Nazwa bazy
	std::string dbName;
	//! Nazwa tabeli
	std::string table;
	//! Nazwa kolumny
	std::string column;
	//! Identyfikator wiersza
	sqlite3_int64 rowID;
	//! Ostwarty blob
	sqlite3_blob *_Myblob;
	//! Aktualny rozmiar bloba
	int blobSize;
	//! Zapytanie aktualizujace bloba
	sqlite3_stmt * update;

	//! ----------------- STAN BUFORÓW --------------------
	//! Bufor
	_MyBufferPolicy buffer;
	//! Aktualny indeks wewn¹trz bloba
	int blobIDX;
	int blobReadIDX;
	int blobWriteIDX;

	//! -------------- STAN STRUMIENIA ---------------------

	//const _Cvt *_Pcvt;	// pointer to codecvt facet (may be null)
	_Strstate _Mystate;	// the stream testa
	int _Seekhigh;	// the high-water pointer in character array
};

const bool initialize(sqlite3 * db)
{
	char * error;
	static const char sqlCreateTable[] = "CREATE TABLE files_table (file_name TEXT PRIMARY KEY, file BLOB, size UNSIGNED BIG INT);";
	const auto rc = sqlite3_exec(db, sqlCreateTable, NULL, NULL, &error);
	if (rc != SQLITE_OK){
		sqlite3_free(error);
		return false;
	}

	return true;
}

const bool checkStructure(sqlite3 * db)
{
	bool ret = false;

	const char* tail;
	sqlite3_stmt* res;
	static const char sqlCheckTable[] = "SELECT count(*) FROM sqlite_master WHERE type='table' AND name='files_table' ORDER BY name;";
	const auto rc = sqlite3_prepare_v2(db, sqlCheckTable, -1, &res, &tail);

	if (rc == SQLITE_OK && res != nullptr && sqlite3_step(res) == SQLITE_ROW){
		const auto count = sqlite3_column_int(res, 0);
		sqlite3_finalize(res);
		if (count >= 1){
			ret = true;
		}
	}

	return ret;
}

const bool checkIfEncrypted(sqlite3 * db, const std::string & key)
{
	bool ret = true;

	const auto rc = sqlite3_key(db, key.c_str(), key.size());

	if (rc != SQLITE_OK){
		ret = false;
	}

	return ret;
}

sqlite3 * verify_(const core::Filesystem::Path & path,
	const std::string & key)
{
	sqlite3 * db;
	auto rc = sqlite3_open_v2(path.string().c_str(), &db, SQLITE_OPEN_READWRITE, nullptr);

	if (rc != SQLITE_OK){
		//TODO
		//obs³uga kodów b³êdu sqlite
		sqlite3_close(db);
		db = nullptr;
	}
	//uda³o siê otworzyæ
	//sprawdzam czy zaszyfrowana
	//i czy struktura siê zgadza
	else if (checkStructure(db) == false){
		
		if (checkIfEncrypted(db, key) == false || checkStructure(db) == false){
			sqlite3_close(db);			
			db = nullptr;
		}	
	}

	return db;
}

class hmdbCommunication::SQLCipherStorage::SQLCipherStorageTransaction : public hmdbCommunication::IHMDBStorageOperations
{
public:
	SQLCipherStorageTransaction(SQLCipherStorage * storage)
		: storage(storage), lock(storage->sync_)
	{

	}

	virtual ~SQLCipherStorageTransaction()
	{

	}

	virtual const bool exists(const std::string & key) const
	{
		return storage->rawExists(key);
	}
	
	virtual std::iostream * get(const std::string & key)
	{
		return storage->rawGet(key);
	}
	
	virtual std::istream * get(const std::string & key) const
	{
		return storage->rawGet(key);
	}
	
	virtual const bool set(const std::string & key, std::istream * input)
	{
		return storage->rawSet(key, input);
	}
	
	virtual void remove(const std::string & key)
	{
		storage->rawRemove(key);
	}
	
	virtual const bool rename(const std::string & oldKey,
		const std::string & newKey, const bool overwrite = false)
	{
		return storage->rawRename(oldKey, newKey, overwrite);
	}
	
	virtual const Keys keys() const
	{
		return storage->rawKeys();
	}

private:
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock;
	SQLCipherStorage * const storage;

};


using namespace hmdbCommunication;

SQLCipherStorage::SQLCipherStorage() : db(nullptr)
{

}
SQLCipherStorage::~SQLCipherStorage()
{
	close();	
}


const bool SQLCipherStorage::rawExists(const std::string & key) const
{
	bool ret = false;

	const char* tail;
	sqlite3_stmt* res;

	std::string fileQuery((boost::format("SELECT count(*) FROM files_table WHERE file_name='%1%';") % key).str());

	auto rc = sqlite3_prepare_v2(db, fileQuery.c_str(), -1, &res, &tail);

	if (rc == SQLITE_OK && res != nullptr && sqlite3_step(res) == SQLITE_ROW){
		const auto count = sqlite3_column_int(res, 0);
		if (count == 1){
			ret = true;
		}
	}

	sqlite3_finalize(res);

	return ret;
}

std::iostream * SQLCipherStorage::rawGet(const std::string & key)
{
	//typ bufora strumienia
	typedef basic_sqliteblobbuf<char, std::char_traits<char>, DynamicWriteBufferPolicy<char>> SQLiteBuffer;

	//pobieramy dane z bazy
	const char* tail;
	sqlite3_stmt* res;

	std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name = '%1%';") % key).str());

	int rc = sqlite3_prepare_v2(db, fileQuery.c_str(), -1, &res, &tail);

	if (rc != SQLITE_OK){
		return nullptr;
	}

	if (sqlite3_step(res) == SQLITE_ROW){
		const auto rowID = sqlite3_column_int64(res, 0);

		std::auto_ptr<SQLiteBuffer> buf(new SQLiteBuffer(db, "main", "files_table", "file", rowID));

		sqlite3_finalize(res);

		return new std::iostream(buf.release());
	}

	return nullptr;
}

std::istream * SQLCipherStorage::rawGet(const std::string & key) const
{
	typedef basic_sqliteblobbuf<char, std::char_traits<char>, FixedBufferPolicy<char>> SQLiteBuffer;

	//pobieramy dane z bazy
	const char* tail;
	sqlite3_stmt* res;

	std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name = '%1%';") % key).str());

	int rc = sqlite3_prepare_v2(db, fileQuery.c_str(), -1, &res, &tail);

	if (rc != SQLITE_OK){
		return nullptr;
	}

	if (sqlite3_step(res) == SQLITE_ROW){
		const auto rowID = sqlite3_column_int64(res, 0);
		//http://www.sqlite.org/c3ref/blob_open.html
		std::auto_ptr<SQLiteBuffer> buf(new SQLiteBuffer(db, "main", "files_table", "file", rowID));

		sqlite3_finalize(res);

		return new std::iostream(buf.release());
	}

	return nullptr;
}

const bool SQLCipherStorage::rawSet(const std::string & key, std::istream * input)
{
	if (input == nullptr){
		return false;
	}

	input->seekg(0, std::ios::beg);
	auto begin = input->tellg();
	input->seekg(0, std::ios::end);
	auto end = input->tellg();
	const auto streamSize = end - begin;

	//wrzucam to do bazy danych

	const char* tail;
	sqlite3_stmt* res;
	std::string querry;

	const bool existed = exists(key);

	//najpierw sprawdzam czy plik istnieje
	if (existed == false){
		//insert
		querry = (boost::format("INSERT INTO files_table VALUES('%1%', ?, %2%);") % key % (streamSize * sizeof(char))).str();
	}
	else{
		//update
		querry = (boost::format("UPDATE files_table SET file = ?, size = %1% WHERE file_name = '%2%';") % (streamSize * sizeof(char)) % key).str();
	}

	int rc = sqlite3_prepare_v2(db, querry.c_str(), -1, &res, &tail);

	if (rc != SQLITE_OK || res == nullptr){
		sqlite3_finalize(res);
		return false;
	}

	input->seekg(0, std::ios::beg);

	sqlite3_bind_zeroblob(res, 1, streamSize);

	//sqlite3_bind_blob(res, 1, memblock.get(), size * sizeof(char), SQLITE_STATIC);

	auto ret = sqlite3_step(res);
	sqlite3_finalize(res);
	if (ret != SQLITE_DONE){
		return false;
	}

	sqlite3_int64 rowid = -1;

	if (existed == false){
		rowid = sqlite3_last_insert_rowid(db);
	}
	else{
		sqlite3_stmt* res;

		std::string fileQuery((boost::format("SELECT _rowid_ FROM files_table WHERE file_name='%1%';") % key).str());

		auto rc = sqlite3_prepare_v2(db, fileQuery.c_str(), -1, &res, nullptr);

		if (rc == SQLITE_OK && res != nullptr && sqlite3_step(res) == SQLITE_ROW){
			rowid = sqlite3_column_int64(res, 0);
		}

		sqlite3_finalize(res);
	}

	if (rowid == -1){
		remove(key);
		return false;
	}

	// Getting here means we have a valid file handle, f, and a valid db handle, db
	//   Also, a blank row has been inserted with key rowid
	sqlite3_blob *blob;
	rc = sqlite3_blob_open(db, "main", "files_table", "file", rowid, 1, &blob);

	if (SQLITE_OK != rc) {
		sqlite3_blob_close(blob);
		remove(key);
		return false;
	}

	const auto readSize = std::min(streamSize, (long long)MaxBufferSize);

	boost::shared_array<char> memblock(new char[readSize]);
	input->seekg(0, std::ios::beg);
	int offset = 0;
	while (input->eof() == false){
		input->read(memblock.get(), readSize);
		const auto rs = input->gcount();
		rc = sqlite3_blob_write(blob, memblock.get(), rs, offset);
		if (rc != SQLITE_OK){
			sqlite3_blob_close(blob);
			remove(key);
			return false;
		}

		offset += rs;
	}

	sqlite3_blob_close(blob);

	return true;
}

void SQLCipherStorage::rawRemove(const std::string & key)
{
	std::string fileDelete((boost::format("DELETE FROM files_table WHERE file_name = '%1%';") % key).str());
	char * error;
	int rc = sqlite3_exec(db, fileDelete.c_str(), nullptr, nullptr, &error);
	if (rc != SQLITE_OK) {
		sqlite3_free(error);
		throw std::runtime_error("Could not remove key from database");
	}
}

const bool SQLCipherStorage::rawRename(const std::string & oldKey,
	const std::string & newKey, const bool overwrite)
{
	if (rawExists(oldKey) == true){
		const char* tail;
		sqlite3_stmt* res;
		std::string querry((boost::format("UPDATE files_table SET file_name = %1% WHERE file_name = '%2%';") % newKey % oldKey).str());
		int rc = sqlite3_prepare_v2(db, querry.c_str(), -1, &res, &tail);

		if (rc != SQLITE_OK || res == nullptr){
			sqlite3_finalize(res);
			return false;
		}

		auto ret = sqlite3_step(res);
		sqlite3_finalize(res);
		if (ret != SQLITE_DONE){
			return false;
		}

		return true;
	}
	else{
		return false;
	}
}

const SQLCipherStorage::Keys SQLCipherStorage::rawKeys() const
{

	SQLCipherStorage::Keys ret;

	const char* tail;
	sqlite3_stmt* res;

	std::string fileQuery("SELECT file_name FROM files_table;");

	auto rc = sqlite3_prepare_v2(db, fileQuery.c_str(), -1, &res, &tail);

	if (rc == SQLITE_OK && res != nullptr){
		int r = -1;
		while ((r = sqlite3_step(res)) == SQLITE_ROW){
			const auto key = sqlite3_column_text(res, 0);
			
			ret.insert(std::string((const char *)key));
		}

		if (r != SQLITE_DONE){
			SQLCipherStorage::Keys().swap(ret);
		}
	}

	sqlite3_finalize(res);

	return ret;
}

void SQLCipherStorage::open(const core::Filesystem::Path & path,
	const std::string & key)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	if (db != nullptr){
		return;
	}

	if (core::Filesystem::pathExists(path) == true){
		db = verify_(path, key);
	}	
}

const bool SQLCipherStorage::create(const core::Filesystem::Path & path,
	const std::string & key)
{
	if (core::Filesystem::pathExists(path) == true){
		return false;
	}	

	bool ret = false;
	sqlite3 * db;

	auto rc = sqlite3_open_v2(path.string().c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

	if (rc == SQLITE_OK){
		rc = sqlite3_rekey(db, key.c_str(), key.size());
		if (rc == SQLITE_OK){
			ret = initialize(db);
		}
	}

	sqlite3_close(db);

	return ret;
}

const bool SQLCipherStorage::changeKey(const core::Filesystem::Path & path,
	const std::string & oldKey, const std::string & newKey)
{
	if (core::Filesystem::pathExists(path) == false){
		return false;
	}

	bool ret = false;
	sqlite3 * db;

	auto rc = sqlite3_open_v2(path.string().c_str(), &db, SQLITE_OPEN_READWRITE, nullptr);

	if (rc == SQLITE_OK){
		rc = sqlite3_key(db, oldKey.c_str(), oldKey.size());
		if (rc == SQLITE_OK){
			rc = sqlite3_rekey(db, newKey.c_str(), newKey.size());
			if (rc == SQLITE_OK){
				ret = true;
			}
		}
	}

	sqlite3_close(db);

	return ret;
}

const bool SQLCipherStorage::setKey(const core::Filesystem::Path & path,
	const std::string & key)
{
	if (core::Filesystem::pathExists(path) == false){
		return false;
	}

	bool ret = false;
	sqlite3 * db;

	auto rc = sqlite3_open_v2(path.string().c_str(), &db, SQLITE_OPEN_READWRITE, nullptr);

	if (rc == SQLITE_OK){
		rc = sqlite3_rekey(db, key.c_str(), key.size());
		if (rc == SQLITE_OK){
			ret = true;
		}
	}

	sqlite3_close(db);

	return ret;
}

const bool SQLCipherStorage::verify(const core::Filesystem::Path & path,
	const std::string & key)
{
	auto db = verify_(path, key);

	if (db == nullptr){
		return false;
	}

	sqlite3_close(db);

	return true;
}

const bool SQLCipherStorage::isOpened() const
{
	return db != nullptr;
}

const bool SQLCipherStorage::exists(const std::string & key) const
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawExists(key);
}

std::iostream * SQLCipherStorage::get(const std::string & key)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawGet(key);
}

std::istream * SQLCipherStorage::get(const std::string & key) const
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawGet(key);
}

const bool SQLCipherStorage::set(const std::string & key, std::istream * input)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawSet(key, input);
}

void SQLCipherStorage::remove(const std::string & key)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawRemove(key);
}

const bool SQLCipherStorage::rename(const std::string & oldKey,
	const std::string & newKey, const bool overwrite)
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawRename(oldKey, newKey, overwrite);
}

const SQLCipherStorage::Keys SQLCipherStorage::keys() const
{
	threadingUtils::ScopedLock<threadingUtils::RecursiveSyncPolicy> lock(sync_);
	return rawKeys();
}

const std::string SQLCipherStorage::name() const
{
	return std::string("SQLCipher");
}

const std::string SQLCipherStorage::protocol() const
{
	return std::string("sqlcipherstream://");
}

void SQLCipherStorage::close()
{
	if (db != nullptr){
		sqlite3_close(db);
		db = nullptr;
	}
}

const SQLCipherStorage::TransactionPtr SQLCipherStorage::transaction()
{
	return TransactionPtr(new SQLCipherStorageTransaction(this));
}

const SQLCipherStorage::TransactionConstPtr SQLCipherStorage::transaction() const
{
	return TransactionPtr(new SQLCipherStorageTransaction(const_cast<SQLCipherStorage*>(this)));
}