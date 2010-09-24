/********************************************************************
	created:  2010/04/07
	created:  7:4:2010   14:05
	filename: VideoInput.h
	author:	  Piotr Gwiazdowski

	purpose:
*********************************************************************/
#ifndef __AVINOUT_VIDEOINPUTADAPTER_H__
#define __AVINOUT_VIDEOINPUTADAPTER_H__

////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////

/**
 * Wrapper na prawdziw� implementacj� VideoStream, umo�liwiaj�cy lepsz� integracj�
 * z logik� programu. Mo�na wybra� spos�b zarz�dzania
 * pami�ci� itd.
 * Ca�o�� to praktyczne zastosowanie "Policy-based design". Lista zastosowanych
 * wytycznych:
 *  - PtrPolicy: metody initPtr, zeroPtr oraz isUnique; zagnie�d�ony szablon FrameTraits<T>;
 *    szczeg�y w Policies.h
 *  - Base: dowolna klasa bazowa, nie ma na ni� �adnych wymaga� (poza bezparametrycznym
 *    konstruktorem). Jaka� klasa bazowa mo�e by� wymagana, je�li chcemy, aby klasa poddawa�a
 *    si� wybranej metodzie zliczania referencji; przyk�adem mo�e by� tutaj wska�nik osg::ref_ptr
 *    i wymagana przez niego klasa bazowa osg::Referenced
 * \see http://en.wikipedia.org/wiki/Policy-based_design
 */
  /*
template <class PtrPolicy, class Base = utils::NullType>
class VideoInput : public PtrPolicy, public Base
{
//------------------------------------------------------------------------------
public:
  //!
  typedef typename Frame<typename PtrPolicy::Base> FrameType;
  //!
  typedef typename PtrPolicy::template Ptr<FrameType> PtrFrame;

//------------------------------------------------------------------------------
private:
  //! Prawdziwa implementacja.
  VideoStream * pStream;
  //! Bufor na ramk� BGRA.
  PtrFrame frameBGRA;
  //! Bufor na ramk� RGB.
  PtrFrame frameRGB;
  //! Bufor na ramk� YV12.
  PtrFrame frameYV12;

//------------------------------------------------------------------------------
public:
  //! \param source �r�d�o obrazu.
  VideoInput(const std::string& source)
  {
    pStream = VideoStream::create(source);
    if ( pStream == NULL ) {
      throw new std::runtime_error("No input.");
    }
    initPtr(frameBGRA);
    initPtr(frameRGB);
    initPtr(frameYV12);
  }
  //! Polimorficzny destruktor.
  virtual ~VideoInput()
  {
    zeroPtr(frameBGRA);
    zeroPtr(frameRGB);
    zeroPtr(frameYV12);
    utils::deletePtr(pStream);
  }

//------------------------------------------------------------------------------
public:
  //! \return Wska�nik do �r�d�a.
  VideoStream * getStream()
  {
    return pStream;
  }
  //! \return Wska�nik do �r�d�a.
  const VideoStream * getStream() const
  {
    return pStream;
  }
  //! \return Referencja do �r�d�a.
  VideoStream & operator*()
  {
    assert(pStream);
    return *pStream;
  }
  //! \return Referencja do �r�d�a.
  const VideoStream & operator*() const
  {
    assert(pStream);
    return *pStream;
  }
  //! \return Wska�nik do �r�d�a.
  VideoStream * operator->()
  {
    assert(pStream);
    return pStream;
  }
  //! \return Wska�nik do �r�d�a.
  const VideoStream * operator->() const
  {
    assert(pStream);
    return pStream;
  }

//------------------------------------------------------------------------------
public:
  //! Zapisuje bie��c� ramk� w przekazanej strukturze. Je�li ramka nie
  //! b�dzie zaalokowana, zostanie stworzona z rozmiarem r�wnym rozmiarowi
  //! strumienia.
  bool getFrame(Picture & frame)
  {
    return getFrameGeneric(frame);
  }

  //!
  //! 
  //! \param format
  const PtrFrame getFrameBuffer(PixelFormat format)
  {
    if ( format == PixelFormatBGRA ) {
      return getFrameBufferBGRA();
    } else if ( format == PixelFormatRGB24 ) {
      return getFrameBufferRGB();
    } else if ( format == PixelFormatYV12 ) {
      return getFrameBufferYV12();
    } else {
      PtrFrame nullFrame;
      initPtr(nullFrame);
      return nullFrame;
    }
  }

  //! \return Bie��ca ramka w formacie BGRA.
  const PtrFrameBGRA getFrameBufferBGRA()
  {
    return getFrameBufferGeneric(frameBGRA);
  }
  //! \return Bie��ca ramka w formacie RGB.
  const PtrFrameRGB getFrameBufferRGB()
  {
    return getFrameBufferGeneric(frameRGB);
  }
  //! \return Bie�aca ramka w formacie YV12.
  const PtrFrameYV12 getFrameBufferYV12()
  {
    return getFrameBufferGeneric(frameYV12);
  }

//------------------------------------------------------------------------------
private:

  //! \return Wska�nik do bufora o zadanym typie.
  typename PtrFrame& getFrameBufferGeneric(typename PtrFrame& bufferPtr, PixelFormat format)
  {
    assert(pStream);
    // je�li nie ma bufora to go tworzymy
    if ( bufferPtr == NULL ) {
      PtrPolicy::setPtr( bufferPtr, new FrameType );
      bufferPtr->width = pStream->getWidth();
      bufferPtr->height = pStream->getHeight();
      bufferPtr->format = format;
      bufferPtr->alloc();
      bufferPtr->timestamp = INVALID_TIMESTAMP;
    }
    // czy trzeba od�wie�y�?
    if ( bufferPtr->timestamp == INVALID_TIMESTAMP ) {
      if (convertOrZeroFrame(*bufferPtr)) {
        bufferPtr->timestamp = pStream->getFrameTimestamp();
      }
    }
    return bufferPtr;
  }

  //! Pomocnicza metoda uog�lniaj�ca pobieranie i buforowanie ramek.
  template <class T>
  inline bool convertOrZeroFrame(T & frame)
  {
    assert(pStream);
    // nie ma sensu uaktualnia� je�eli mamy tylko jedn�
    if ( pStream->isEndOfStream() ) {
      // zerujemy bufor
      frame.zero();
      return true;
    } else {
      // konwertujemy ramk�
      return pStream->getFrame(frame);
    }
  }

  //! Pomocnicza metoda uog�lniaj�ca pobieranie i buforowanie ramek.
  template <class T>
  bool getFrameGeneric(T & frame)
  {
    typename FrameTraits<T>::Ptr& buffer = getFrameBufferHelper( (T*)NULL );

    // sprawdzamy, czy ramka jest ju� zaalokowana
    if ( !frame.data ) {
      // alokujemy z domy�lnym rozmiarem
      frame.construct( pStream->getWidth(), pStream->getHeight() );
    }

    // sprawdzamy, czy ju� konwertowali�my
    if ( (buffer != NULL) && (&(*buffer) == &frame) ) {
      // pobieramy bufor!
      if ( buffer->timestamp != pStream->getFrameTimestamp() )  {
        return convertOrZeroFrame(frame);
      } else {
        // bufor jest aktualny
        return true;
      }
    } else {
      if ( buffer != NULL ) {
        // czy kopiujemy?
        bool copy = false;
        if ( buffer->timestamp == pStream->getFrameTimestamp() ) {
          copy = true;
        } else if ( !PtrPolicy::isUnique(buffer) ) {
          // nie ma sensu uaktualnia� je�eli mamy tylko jedn�
          if ( !convertOrZeroFrame(*buffer) ) {
            return false;
          }
          buffer->timestamp = pStream->getFrameTimestamp();
          copy = true;
        }
        // kopiujemy ramk�
        if ( copy ) {
          buffer->copyTo(&frame);
          return true;
        }
      }
      // pobieramy ramk�
      return convertOrZeroFrame(frame);
    }
  }

  //! Pomocnicza metoda zeruj�ca wska�nik.
  void zeroPtr( typename PtrFrame & ptr )
  {
    PtrPolicy::setPtr( ptr, (FrameType*)NULL );
  }
};*/

////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////



#endif  // __AVINOUT_VIDEOINPUTADAPTER_H__
