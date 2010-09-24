/********************************************************************
	created:  2010/02/11
	created:  11:2:2010   13:12
	filename: Frame.h
	author:	  Piotr Gwiazdowski
	
	purpose:  Dostarcza informacji o typach ramek. Wszystkie typy ramek
            powinny mie� identyczny interfejs publiczny tak, aby
            mo�na by�o ich u�ywa� w szablonach.
*********************************************************************/
#ifndef __AVINOUT_FRAME_H__
#define __AVINOUT_FRAME_H__

#include <utils/Align.h>

////////////////////////////////////////////////////////////////////////////////
namespace avinout {
////////////////////////////////////////////////////////////////////////////////

//! Dane obrazka.
struct FrameLayered {
  //! Warstwy obrazka.
  unsigned char * planes[4];
  //! Szeroko�ci warstw (bajty).
  int pitches[4];
  //! Szeroko�� obrazka (teksele).
  int width;
  //! Wysoko�� obrazka (teksele).
  int height;
  //! Format obrazka.
  PixelFormat format;
};

//! Do ilu wyr�wnane s� ramki?
#define AVINOUT_PITCH_ALIGN 1

struct FrameData {
  //! Typ funkcji t�umacz�cej ramk� na posta� "surow�".
  typedef FrameLayered (*PictureTranslation)(FrameData * data);
  //! Dane ramki.
  unsigned char * data;
  //! Szeroko�� wiersza w bajtach - nie to samo, co szeroko�� ramki!
  int pitch;
  //! Wysoko�� w tekselach.
  int height;
  //! Szeroko�� w tekselach.
  int width;
  //! Format.
  PixelFormat format;
  //!
  PictureTranslation translation;

  //! Inicjalizacja
  //! \param pitch
  //! \param height
  //! \param width
  void init(int width, int height, int pitch, PictureTranslation translation)
  {
    this->pitch = UTILS_ALIGN(pitch, AVINOUT_PITCH_ALIGN);
    this->height = height;
    this->width = width;
    this->translation = translation;
    data = new unsigned char[this->pitch * this->height];
  }
  //! Kopiowanie danych.
  //! \param dest Ramka docelowa.
  void copyTo(FrameData * dest) const
  {
    ::memcpy( dest->data, data, pitch * height);
  }
  //! Kopiowanie danych.
  //! \param dest Ramka �r�d�owa.
  void copyFrom(const FrameData * src)
  {
    src->copyTo(this);
  }
  //! Zerowanie pami�ci.
  void zero()
  {
    ::memset(data, 0, pitch * height);
  }
  //! Zwolnienie pami�ci.
  void free()
  {
    utils::deletePtr(data);
    pitch = 0;
    height = 0;
    width = 0;
    translation = NULL;
  }
  //! \return Rozmiar w pami�ci.
  int getAllocatedSize()
  {
    return pitch * height;
  }

  //!
  bool translate(FrameLayered * target)
  {
    if ( translation ) {
      *target = translation(this);
      return true;
    } else {
      return false;
    }
  }
};

//! Uniwersalna struktura na rami w formacie RGB.
template<PixelFormat Format, int Bpp>
struct FrameOnePlane : public FrameData
{
  enum {
    //! Format danych.
    pixelFormat = Format,
    //! Bajty na piksel.
    bytesPerPixel = Bpp
  };

  static FrameLayered toPicture(FrameData * frame)
  {
    FrameLayered result = {
      { frame->data, NULL, NULL, NULL },
      { frame->pitch, 0, 0, 0 },
      frame->width, frame->height, frame->format
    };
    return result;
  }

  //! Inicjalizacja oraz zaalokowanie pami�ci. Zaalokowane dane b�d� mia�y
  //! rozmiar r�wny \c width * \c height * \c bytesPerPixel
  //! \param width Szeroko�� (w tekselach) ramki.
  //! \param height Wysoko�� (w tekselach) ramki.
  void construct(int width, int height)
  {
    init(width, height, width * bytesPerPixel, toPicture);
    format = Format;
  }

  

//   //! Konwersja do surowej postaci.
//   RawFrame toRawFrame()
//   {
//     RawFrame result = {
//       { data, NULL, NULL, NULL },
//       { pitch, NULL, NULL, NULL }
//     };
//     return result;
//   }
};

//------------------------------------------------------------------------------

//! Ramka RGB (24BPP) 
typedef FrameOnePlane<PixelFormatRGB24, 3> FrameRGB;
//! Ramka BGRA (32BPP)
typedef FrameOnePlane<PixelFormatBGRA, 4> FrameBGRA;

//------------------------------------------------------------------------------

//! Ramka w formacie YV12.
struct FrameYV12 : public FrameData
{
  enum {
    //! Format danych.
    pixelFormat = PixelFormatYV12
  };

  //! \return Wysoko�� warstwy Y.
  unsigned int heightY()
  {
    return height - heightUV();
  }
  //! \return Wysoko�� warstwy U i V.
  unsigned int heightUV()
  {
    return height / 3;
  }
  //! \return Warstwa Y.
  unsigned char * Y()
  {
    return data;
  }
  //! \return Warstwa U.
  unsigned char * U()
  {
    return data + pitch * heightY();
  }
  // \return Warstwa V.
  unsigned char * V()
  {
    return U() + (pitch>>1);
  }

  static FrameLayered toPicture(FrameData * frame)
  {
    FrameYV12 * casted = reinterpret_cast<FrameYV12*>(frame);
    FrameLayered result = {
      { casted->Y(), casted->U(), casted->V(), NULL },
      { casted->pitch, casted->pitch, casted->pitch, 0 },
      casted->width, casted->heightY(), casted->format
    };
    return result;
  }
 
  //! Inicjalizacja oraz zaalokowanie pami�ci.
  //! \param width Szeroko�� (w tekselach) ramki.
  //! \param height Wysoko�� (w tekselach) ramki.
  void construct(int width, int height)
  {
    // rozmiar musi by� podzielny przez dwa
    assert( !(height & 0x01) );
    assert( !(width & 0x01) );
    init( width, height + (height>>1), width, toPicture );
    format = PixelFormatYV12;
  }

//   //! Konwersja do surowej postaci.
//   RawFrame toRawFrame()
//   {
//     RawFrame result = {
//       { Y(), U(), V(), NULL },
//       { pitch, pitch, pitch, NULL }
//     };
//     return result;
//   }

};

//------------------------------------------------------------------------------

//! Wrapper na ramk� zapewniaj�cy automatyczne zarz�dzanie pami�ci�.
template <class Frame, class Base>
class FrameWrapper : public Frame, public Base
{
public:
  //! Timestamp ramki.
  double timestamp;
  //! Wrapowana ramka.
  typedef Frame Wrapped;

public:
  //! Konstruktor domy�lny, zeruj�cy wszystkie pola.
  FrameWrapper()
  {
    utils::zero(const_cast<Frame&>(*static_cast<const Frame*>(this)));
    timestamp = ::avinout::INVALID_TIMESTAMP;
  }
  //! Wirtualny destruktor, zwalnia wszystkie dane.
  virtual ~FrameWrapper()
  {
    this->free();
    timestamp = ::avinout::INVALID_TIMESTAMP;
  }
};

//------------------------------------------------------------------------------

template <class Base>
class Frame : public Picture, public Base
{
public:
  //! Timestamp ramki.
  double timestamp;
  //! Wrapowana ramka.
  typedef Picture Wrapped;

public:
  //! Konstruktor domy�lny, zeruj�cy wszystkie pola.
  Frame()
  {
    utils::zero(const_cast<Picture&>(*static_cast<const Picture*>(this)));
    timestamp = ::avinout::INVALID_TIMESTAMP;
  }
  //! Konstruktor alokuj�cy miejsce na ramk�.
  //! \param width
  //! \param height
  //! \param format
  Frame(int width, int height, PixelFormat format)
  {
    this->width = width;
    this->height = height;
    this->format = format;
    this->alloc();
    timestamp = ::avinout::INVALID_TIMESTAMP;
  }
  //! Wirtualny destruktor, zwalnia wszystkie dane.
  virtual ~Frame()
  {
    this->free();
    timestamp = ::avinout::INVALID_TIMESTAMP;
  }
};

//------------------------------------------------------------------------------




////////////////////////////////////////////////////////////////////////////////
} // namespace avinout
////////////////////////////////////////////////////////////////////////////////

#endif  // __AVINOUT_FRAME_H__