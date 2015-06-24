/********************************************************************
	created:  2014/09/24	17:20:37
	filename: SQLiteBLOBStreamBufferTImpl.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/

#ifndef __HEADER_GUARD_SQLITEUTILS__SQLITEBLOBSTREAMBUFFERT_H__
	#error "This header should not be included directly without SQLiteBLOBStreamBufferT. This is just implementation"
#endif

#ifndef __HEADER_GUARD_SQLITEUTILS__SQLITEBLOBSTREAMBUFFERTIMPL_H__
#define __HEADER_GUARD_SQLITEUTILS__SQLITEBLOBSTREAMBUFFERTIMPL_H__

#include <sqliteUtils/SQLiteDB.h>
#include <sqliteUtils/SQLiteBLOB.h>
#include <sqliteUtils/SQLitePreparedStatement.h>
#include <sqliteUtils/SQLiteBLOBStreamBufferT.h>
#include <streambuf>
#include <iosfwd>
//#include <boost/format.hpp>


template<typename _Elem, typename _Traits, typename _BufferPolicy>
SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::SQLiteBLOBStreamBufferT(const std::string & path, const std::string & table,
	const std::string & column, const sqlite3_int64 rowID, const int initialSize,
	const std::string & dbName, const std::string & key,
	std::ios_base::openmode mode) : _Mysb(), path(path), key(key), mode_(mode),
	dbName(dbName), table(table), column(column), rowID(rowID), blobReadBase(0),
	blobWriteBase(mode & std::ios_base::app ? initialSize : 0),
	blobSize(initialSize), blobSeekhigh(initialSize), Mystate(_Getstate(mode)),
	putBackWrite(false)
{

}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::~SQLiteBLOBStreamBufferT()
{
	sync();
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
typename SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::int_type SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::pbackfail(typename SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::int_type _Meta)
{
	if ((blobReadBase == 0 && (_Mysb::gptr() == nullptr || _Mysb::gptr() == _Mysb::eback()))
		|| (!_Traits::eq_int_type(_Traits::eof(), _Meta)
		&& !_Traits::eq(_Traits::to_char_type(_Meta), _Mysb::gptr()[-1])
		&& Mystate & _Constant))
		return (_Traits::eof());	// can't put back, fail
	else
	{	// back up one position and store put-back character
		const auto pos = toStreamPos(blobGlobalReadIDX());
		auto newPos = std::max(pos - streamBufferSize(), 0);
		seekpos(newPos, std::ios_base::in);
		if (_Mysb::gptr() == nullptr){
			underflow();
		}
		seekpos(pos-1, std::ios_base::in);
		if (!_Traits::eq_int_type(_Traits::eof(), _Meta)){
			*_Mysb::gptr() = _Traits::to_char_type(_Meta);
			putBackWrite = true;
		}
		return (_Traits::not_eof(_Meta));
	}
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
typename SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::int_type SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::overflow(int_type _Meta)
{
	if ((Mystate & _Constant) || _Traits::eq_int_type(_Traits::eof(), _Meta) == true ||
		((buffer.begin() == nullptr) && (buffer.updateSize(std::max(toStreamPos(blobSize), (int)DefaultBufferSize)) < 1))){
		return (_Traits::eof()); // read only mode or no open blob, or EOF or faile dto create buffer
	}

	//czy cos by�o pisane do bufora?
	if (_Mysb::pptr() != nullptr){
		// musze zrzucic bufor do strumienia
		//zapamietuje poprzedni rozmiar aby ewentualnie rozmiar bufora dopasowac
		const auto oldBlobSize = blobSize;

		const auto offset = std::min(_Mysb::epptr() - _Mysb::pbase(), toStreamPos(blobSeekhigh - blobWriteBase));

		//zapisuje bufor w stumieniu
		if (flushBuffer(_Mysb::pbase(), offset, blobWriteBase) == false){
			return (_Traits::eof());	// zapis sie nie powiodlo, fail
		}

		//czy zmieni� si� rozmiar?
		if (oldBlobSize < blobSize){
			//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
			buffer.updateSize(toStreamPos(blobSize));
		}
		//rozmiar sie nie zmieni�
		//sprawdzam gdzie teraz w strumieniu jestem
		else if (blobWriteBase < blobSize){
			//ca�y czas pisz� wewn�trz bloba, moge od�wiezy� zawarto�� bufor ze strumienia
			auto s = refillBuffer(blobWriteBase, buffer.begin());

			//czy cos pobralem albo jakis blad?
			if (s < 1){
				return (_Traits::eof()); // nic nie pobralem wiec pewnie koniec
			}
		}

		// je�eli cos czyta�em wcze�niej to musz� zapami�ta� i wyzerowa� je�li to nie ten sam obszar
		if (_Mysb::gptr() != nullptr){
			blobReadBase += toBlobPos(std::distance(_Mysb::eback(), _Mysb::gptr()));
			_Mysb::setg(buffer.begin(), nullptr, buffer.end());
		}

		blobWriteBase += toBlobPos(offset);

		//ustawiam nowy bufor i jego stan
		_Mysb::setp(buffer.begin(), buffer.end());
	}
	//nie pisa�em do bufora zapisu
	else{

		if (putBackWrite == true){
			// musze zrzucic bufor do strumienia
			//zapamietuje poprzedni rozmiar aby ewentualnie rozmiar bufora dopasowac
			const auto oldBlobSize = blobSize;

			//zapisuje bufor w stumieniu
			if (flushBuffer(_Mysb::eback(), std::min(_Mysb::egptr() - _Mysb::eback(), toStreamPos(blobSeekhigh - blobReadBase)), blobReadBase) == false){
				return (_Traits::eof());	// zapis sie nie powiodlo, fail
			}

			//czy zmieni� si� rozmiar?
			if (oldBlobSize < blobSize){
				auto ss = streamBufferSize();
				//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
				if (ss < buffer.updateSize(toStreamPos(blobSize))){
					blobReadBase = blobGlobalReadIDX();
					_Mysb::setg(buffer.begin(), nullptr, buffer.end());
				}
			}
		}

		if (_Mysb::gptr() != nullptr){
			//czyta�em co� z bufora a chc� bufor do zapisu
			//sprawdzam czy obszary odczytu/zapisu si� nie zaz�biaj�
			if (blobWriteBase < blobReadBase || blobWriteBase >= blobReadMaxIDX()){

				//prze�adowuje bufor nowymi danymi, b�d� po nich pisa�
				auto s = refillBuffer(blobWriteBase, buffer.begin());

				//czy cos pobralem albo jakis blad?
				if (s < 1){
					return (_Traits::eof()); // nic nie pobralem wiec pewnie koniec
				}

				//niestety, obszary s� roz��czne, musz� zapami�tac stan bufora do odczytu i zresetowa� jego obszar
				blobReadBase += toBlobPos(std::distance(_Mysb::eback(), _Mysb::gptr()));
				_Mysb::setg(buffer.begin(), nullptr, buffer.end());

				//ustawiam nowy bufor i jego stan
				_Mysb::setp(buffer.begin(), buffer.end());
			}
			else{
				//obszary pokrywaj� si� - wystarczy poprzestawia� wska�niki
				auto diff = blobWriteBase - blobReadBase;
				blobWriteBase = blobReadBase;
				//poprawiam obszar do zapisu
				_Mysb::setp(buffer.begin() + diff, buffer.end());
			}
		}
		else{
			//nie by�o zapisu i odczytu albo oba obszary nieaktyne po seek

			if (blobSize > 0 && blobWriteBase != blobSize){
				//uzupelniam bufor
				auto s = refillBuffer(blobWriteBase, buffer.begin());

				//czy cos pobralem albo jakis blad?
				if (s < 1){
					return (_Traits::eof()); // nic nie pobralem wiec pewnie koniec
				}
			}

			//ustawiam nowy bufor i jego stan
			setp(buffer.begin(), buffer.end());
		}
	}
	//zapisujemy
	*_Mysb::pptr() = _Traits::to_char_type(_Meta);
	 _Mysb::pbump(1);
	//aktualizuje koniec strumienia dla poprawnego odczytu ca�ego strumienia podczas zapisu
	updateBlobStreamEnd();

	return (_Meta);
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
typename SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::int_type SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::underflow()
{
	// get an element from stream, point on it
	if (blobSeekhigh < 1 || (Mystate & _Noread) ||
		((buffer.begin() == nullptr) && (buffer.updateSize(std::max(toStreamPos(blobSize), (int)DefaultBufferSize)) < 1))){
		return (_Traits::eof());	// no open blob or no data to read or failed to create buffer, fail
	}
	auto oldSeekhigh = blobSeekhigh;
	updateBlobStreamEnd();

	//czy konieczne jest prze�adowanie bufora?
	bool refill = true;

	//sprawdzam czy byly zapisy
	if (_Mysb::pptr() != nullptr){
		//musze sprawdzic czy to gdzie sie chce znalezc wypada poza aktualny obszar - jezeli tak to musze zapisac dane, odswiezyc bufor
		if ((blobReadBase < blobWriteBase) || (blobGlobalWriteIDX() <= blobReadBase)){

			const auto oldBlobSize = blobSize;

			if (flushBuffer(_Mysb::pbase(), std::min(_Mysb::epptr() - _Mysb::pbase(), toStreamPos(blobSeekhigh - blobWriteBase)), blobWriteBase) == false){
				blobSeekhigh = oldSeekhigh;
				return (_Traits::eof());	// zapis sie nie powiodlo, fail
			}

			if (oldBlobSize < blobSize){
				//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
				buffer.updateSize(blobSize);
			}

			//wypadam poza
			blobWriteBase = blobGlobalWriteIDX();

			_Mysb::setp(buffer.begin(), buffer.end());
		}
		else{
			//zaz�biaj� si� obszary, wystarczy wska�niki poprawi�
			refill = false;
			//maks rozmiar do odczytu
			const auto size = ((blobWriteBase + blobBufferSize()) <= blobSeekhigh) ? blobBufferSize() : blobSeekhigh - blobGlobalWriteIDX();
			//poprawiam obszar do odczytu
			_Mysb::setg(_Mysb::pbase(), _Mysb::pbase() + toStreamPos(blobReadBase - blobWriteBase), buffer.begin() + toStreamPos(size));
			blobReadBase = blobWriteBase;
		}
	}
	else if (putBackWrite == true){
		// musze zrzucic bufor do strumienia
		//zapamietuje poprzedni rozmiar aby ewentualnie rozmiar bufora dopasowac
		const auto oldBlobSize = blobSize;

		//zapisuje bufor w stumieniu
		if (flushBuffer(_Mysb::eback(), std::min(_Mysb::egptr() - _Mysb::eback(), toStreamPos(blobSeekhigh - blobReadBase)), blobReadBase) == false){
			blobSeekhigh = oldSeekhigh;
			return (_Traits::eof());	// zapis sie nie powiodlo, fail
		}

		//czy zmieni� si� rozmiar?
		if (oldBlobSize < blobSize){
			//mam nowego bloba, moge uaktualnic rozmiar bufora do zapisu
			buffer.updateSize(toStreamPos(blobSize));
		}
	}

	if (refill == true){

		auto readIDX = blobReadBase;

		if (_Mysb::gptr() != nullptr){
			readIDX += toBlobPos(std::distance(_Mysb::eback(), _Mysb::egptr()));
		}

		//uzupelnam bufor
		auto s = refillBuffer(readIDX, buffer.begin());

		//czy cos pobralem albo jakis blad?
		if (s < 1){
			blobSeekhigh = oldSeekhigh;
			return (_Traits::eof()); // nic nie pobralem wiec pewnie koniec
		}

		if (readIDX > blobReadBase){
			blobReadBase = readIDX;
		}

		//inicjuje obszar do odczytu
		_Mysb::setg(buffer.begin(), buffer.begin(), buffer.begin() + toStreamPos(s));
	}

	// no codecvt facet, just get it
	_Elem _Ch = *(_Mysb::gptr());
	return _Traits::to_int_type(_Ch);
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
std::streamsize SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::showmanyc()
{
	std::streamsize ret = 0;

	if (blobSeekhigh == 0){
		return -1;
	}

	if (_Mysb::gptr() == nullptr){
		ret = std::min(streamBufferSize(), toStreamPos(blobSeekhigh - blobReadBase));
	}
	else if(_Mysb::gptr() == _Mysb::egptr()){
		if (blobReadBase == blobSize || (blobReadBase + toBlobPos(std::distance(_Mysb::eback(), _Mysb::egptr()))) == blobSeekhigh){
			ret = -1;
		}
		else{
			ret = std::min(streamBufferSize(), std::max(toStreamPos(blobSeekhigh - blobReadBase - blobBufferSize()), 0));
			if (ret == 0){
				ret = -1;
			}
		}
	}
	else{
		ret = std::distance(_Mysb::gptr(), _Mysb::egptr());
	}

	return ret;
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
typename SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::pos_type SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::seekoff(off_type _Off,
	std::ios_base::seekdir _Way, std::ios_base::openmode _Which)
{	

	SQLiteBLOBStreamBufferT::pos_type ret(_BADOFF);

	if (_Off != _BADOFF){

		switch (_Way)
		{
		case std::ios_base::beg:
			break;

		case std::ios_base::end:

			_Off += toStreamPos(blobSeekhigh);
			break;

		case std::ios_base::cur:

			if ((_Which & std::ios_base::in) && (_Which & std::ios_base::out) &&
				(blobGlobalReadIDX() != blobGlobalWriteIDX())){
				_Off = _BADOFF;
			}
			else{
				_Off += toStreamPos(blobGlobalReadIDX());
			}

			break;
		default:
			break;
		}

		if (_Off != _BADOFF){
			ret = seekpos(_Off, _Which);
		}
	}

	return ret;
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
typename SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::pos_type SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::seekpos(pos_type _Ptr,
	std::ios_base::openmode _Mode)
{
	// aktualizuje seek high - ci�gle mog�y by� zapisy
	auto oldSeekhigh = blobSeekhigh;
	updateBlobStreamEnd();
	// pozycja bezwgledna
	std::streamoff _StreamPos = (std::streamoff)_Ptr;
	std::streamoff _BlobPos = toBlobPos(_StreamPos);

	//! sprawdzam czy pozycja jest prawid�owa, czy wybrano jakis bufor
	if (_BlobPos == _BADOFF || _BlobPos < 0 || _BlobPos > blobSeekhigh ||
		!(_Mode & std::ios_base::in || _Mode & std::ios_base::out)){
		return _BADOFF;
	}
	else{

		if (_Mode & std::ios_base::out)
		{	// position within write buffer
			if ((_Mysb::pptr() == nullptr) || (blobWriteBase > _BlobPos) || (_BlobPos > blobWriteMaxIDX())){
				if (_Mysb::pptr() != nullptr){
					if (sync() < 0){
						return _BADOFF;
					}
				}
				blobWriteBase = _BlobPos;
				_Mysb::setp(buffer.begin(), buffer.end());
			}
			else{
				_Mysb::pbump((int)(_StreamPos - toStreamPos(blobGlobalWriteIDX())));
			}
		}

		if (_Mode & std::ios_base::in)
		{	// position within read buffer
			if ((_Mysb::gptr() == nullptr) || (blobReadBase > _BlobPos) || (_BlobPos > blobReadMaxIDX())){
				if (_Mysb::gptr() != nullptr){
					if (sync() < 0){
						return _BADOFF;
					}
				}

				blobReadBase = _BlobPos;
				_Mysb::setg(buffer.begin(), nullptr, buffer.end());
			}
			else{
				_Mysb::gbump((int)(_StreamPos - toStreamPos(blobGlobalReadIDX())));
			}
		}
	}

	return pos_type(_StreamPos);
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
int SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::sync()
{
	int ret = 0;

	if (!(Mystate & _Constant)){

		if (_Mysb::pptr() != nullptr){
			auto oldBlobSize = blobSize;
			if (flushBuffer(_Mysb::pbase(), std::min(_Mysb::epptr() - _Mysb::pbase(), toStreamPos(blobSeekhigh - blobWriteBase)), blobWriteBase) == true){
				if (oldBlobSize < blobSize){
					auto ss = streamBufferSize();
					if (ss < buffer.updateSize(toStreamPos(blobSize))){
						blobWriteBase = blobGlobalWriteIDX();
						_Mysb::setp(buffer.begin(), buffer.end());
						blobReadBase = blobGlobalReadIDX();
						_Mysb::setg(buffer.begin(), nullptr, buffer.end());
					}
				}
			}
			else{
				ret = -1;
			}
		}
		else if (putBackWrite == true){
			auto oldBlobSize = blobSize;
			if (flushBuffer(_Mysb::eback(), std::min(_Mysb::egptr() - _Mysb::eback(), toStreamPos(blobSeekhigh - blobReadBase)), blobReadBase) == true){
				if (oldBlobSize < blobSize){
					auto ss = streamBufferSize();
					if (ss < buffer.updateSize(toStreamPos(blobSize))){
						blobWriteBase = blobGlobalWriteIDX();
						_Mysb::setp(buffer.begin(), buffer.end());
						blobReadBase = blobGlobalReadIDX();
						_Mysb::setg(buffer.begin(), nullptr, buffer.end());
					}
				}
			}
			else{
				ret = -1;
			}
		}
	}

	return ret;
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
const int SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::blobBufferSize() const
{
	return toBlobPos(streamBufferSize());
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
const int SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::streamBufferSize() const
{
	return std::distance(buffer.begin(), buffer.end());
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
const int SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::blobMaxIDX(const int idx) const
{
	return std::min(idx + blobBufferSize(), blobSeekhigh);
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
const int SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::blobReadMaxIDX() const
{
	return blobMaxIDX(blobReadBase);
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
const int SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::blobWriteMaxIDX() const
{
	return blobMaxIDX(blobWriteBase);
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
const int SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::toStreamPos(int pos)
{
	return pos / sizeof(_Elem);
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
const int SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::toBlobPos(int pos)
{
	return pos * sizeof(_Elem);
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
const int SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::blobGlobalIDX(const int blobBase, const _Elem * bufferBase,
	const _Elem * bufferPos)
{
	return blobBase + (int)((bufferPos == nullptr) ? 0 : toBlobPos(std::distance(bufferBase, bufferPos)));
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
const int SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::blobGlobalWriteIDX() const
{
	return blobGlobalIDX(blobWriteBase, _Mysb::pbase(), _Mysb::pptr());
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
const int SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::blobGlobalReadIDX() const
{
	return blobGlobalIDX(blobReadBase, _Mysb::eback(), _Mysb::gptr());
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
void SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::updateBlobStreamEnd()
{
	blobSeekhigh = std::max(blobSeekhigh, blobGlobalWriteIDX());
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
const int SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::refillBuffer(const int blobIDX, _Elem * bufferBase)
{
	const int toRead = std::min(toBlobPos(std::distance(bufferBase, buffer.end())), blobSize - blobIDX);
	using namespace sqliteUtils;
	// jak mam jeszcze cos do czytania
	if (toRead > 0){
		SQLiteDB::Wrapper db(SQLiteDB::open(path, key, SQLITE_OPEN_READONLY | SQLITE_OPEN_SHAREDCACHE),
			SQLiteDB::Close(true, 10, 100));

		if (db == nullptr){
			return -1;
		}

		SQLiteBLOB::Wrapper blob(SQLiteBLOB::open(db, table, column, rowID,
			0, dbName), SQLiteBLOB::Close(100));

		if (blob == nullptr){
			return -1;
		}

		//teraz czytam do mojego bufora
		auto ret = sqlite3_blob_read(blob, bufferBase, toRead, blobIDX);

		// jak sie nie udalo czytac z sqlite
		if (ret != SQLITE_OK){
			return -1;
		}
	}

	return toRead;
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
const bool SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::flushBuffer(const _Elem * base, const int count,
	const int blobIDX)
{
	const unsigned int buffSize = toBlobPos(count);
	const unsigned int newSize = blobIDX + buffSize;
	using namespace sqliteUtils;
	SQLiteDB::Wrapper db(SQLiteDB::open(path, key, SQLITE_OPEN_READWRITE | SQLITE_OPEN_SHAREDCACHE),
		SQLiteDB::Close(true, 10, 100));

	if (db == nullptr){
		return false;
	}

	// zapis do bloba
	// czy dane jeszcze sie zmieszcza?
	if (newSize <= blobSize){	

		SQLiteBLOB::Wrapper blob(SQLiteBLOB::open(db, table, column, rowID,
			1, dbName), SQLiteBLOB::Close(100));

		if (blob == nullptr){
			return false;
		}

		//pisze bezposrednio do bloba
		return sqlite3_blob_write(blob, base, buffSize, blobIDX) == SQLITE_OK;
	}
	// dane sie juz nie mieszcza
	else {

		SQLiteBLOB::Wrapper blob(SQLiteBLOB::open(db, table, column, rowID,
			0, dbName), SQLiteBLOB::Close(100));

		if (blob == nullptr){
			return false;
		}

		//przygotowuje update
		static const auto updateQuery = (boost::format("UPDATE %1% SET %2% = ? WHERE iRow = %3%;")
			% updateTableName(dbName, table) % column % rowID).str();

		SQLitePreparedStatement::Wrapper update(SQLitePreparedStatement::prepare(db, updateQuery),
			SQLitePreparedStatement::Finalizer(100));

		if (update == nullptr){
			return false;
		}


		//TODO - dodac polityke update oparta na kopiowaniu do plikow przy du�ych blobach
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
		auto ret = sqlite3_blob_read(blob, (void*)tmpBuffer.c_str(), blobIDX, 0);

		if (ret != SQLITE_OK){
			return false;
		}

		//zamykam bloba
		blob.reset();

		//dopisuje to co mam
		tmpBuffer.append(base, buffSize);

		//aktualizuje bloba
		ret = sqlite3_bind_blob(update, 1, tmpBuffer.c_str(), newSize, SQLITE_STATIC);

		if (ret != SQLITE_OK){
			return false; // failed to bind sqlite query
		}

		//odpalam update			
		if (SQLitePreparedStatement::step(update) != SQLITE_DONE){
			return false; // failed to write to sqlite db
		}

		blobSize = newSize;
	}

	putBackWrite = false;

	return true;
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
const std::string SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::updateTableName(const std::string & dbName,
	const std::string & table)
{
	return (dbName.empty() == true) ? table : dbName + '.' + table;
}

template<typename _Elem, typename _Traits, typename _BufferPolicy>
const typename SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::_Strstate SQLiteBLOBStreamBufferT<_Elem, _Traits, _BufferPolicy>::_Getstate(std::ios_base::openmode _Mode)
{
	_Strstate _State = (_Strstate)0;
	if (!(_Mode & std::ios_base::in))
		_State |= _Noread;
	if (!(_Mode & std::ios_base::out))
		_State |= _Constant;
	if (_Mode & std::ios_base::app)
		_State |= _Append;
	if (_Mode & std::ios_base::ate)
		_State |= _Atend;
	return _State;
}

#endif	// __HEADER_GUARD_SQLITEUTILS__SQLITEBLOBSTREAMBUFFERTIMPL_H__
