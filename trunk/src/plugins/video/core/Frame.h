/********************************************************************
	created:  2010/02/11
	created:  11:2:2010   13:12
	filename: Frame.h
	author:	  Piotr Gwiazdowski
	
	purpose:  Dostarcza informacji o typach ramek. Wszystkie typy ramek
            powinny mieæ identyczny interfejs publiczny tak, aby
            mo¿na by³o ich u¿ywaæ w szablonach.
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
  //! Szerokoœci warstw (bajty).
  int pitches[4];
  //! Szerokoœæ obrazka (teksele).
  int width;
  //! Wysokoœæ obrazka (teksele).
  int height;
  //! Format obrazka.
  PixelFormat format;
};

//! Do ilu wyrównane s¹ ramki?
#define AVINOUT_PITCH_ALIGN 1

struct FrameData {
  //! Typ funkcji t³umacz¹cej ramkê na postaæ "surow¹".
  typedef FrameLayered (*PictureTranslation)(FrameData * data);
  //! Dane ramki.
  unsigned char * data;
  //! Szerokoœæ wiersza w bajtach - nie to samo, co szerokoœæ ramki!
  int pitch;
  //! Wysokoœæ w tekselach.
  int height;
  //! Szerokoœæ w tekselach.
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
  //! \param dest Ramka Ÿród³owa.
  void copyFrom(const FrameData * src)
  {
    src->copyTo(this);
  }
  //! Zerowanie pamiêci.
  void zero()
  {
    ::memset(data, 0, pitch * height);
  }
  //! Zwolnienie pamiêci.
  void free()
  {
    utils::deletePtr(data);
    pitch = 0;
    height = 0;
    width = 0;
    translation = NULL;
  }
  //! \return Rozmiar w pamiêci.
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

  //! Inicjalizacja oraz zaalokowanie pamiêci. Zaalokowane dane bêd¹ mia³y
  //! rozmiar równy \c width * \c height * \c bytesPerPixel
  //! \param width Szerokoœæ (w tekselach) ramki.
  //! \param height Wysokoœæ (w tekselach) ramki.
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

  //! \return Wysokoœæ warstwy Y.
  unsigned int heightY()
  {
    return height - heightUV();
  }
  //! \return Wysokoœæ warstwy U i V.
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
 
  //! Inicjalizacja oraz zaalokowanie pamiêci.
  //! \param width Szerokoœæ (w tekselach) ramki.
  //! \param height Wysokoœæ (w tekselach) ramki.
  void construct(int width, int height)
  {
    // rozmiar musi byæ podzielny przez dwa
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

//! Wrapper na ramkê zapewniaj¹cy automatyczne zarz¹dzanie pamiêci¹.
template <class Frame, class Base>
class FrameWrapper : public Frame, public Base
{
public:
  //! Timestamp ramki.
  double timestamp;
  //! Wrapowana ramka.
  typedef Frame Wrapped;

public:
  //! Konstruktor domyœlny, zeruj¹cy wszystkie pola.
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
  //! Konstruktor domyœlny, zeruj¹cy wszystkie pola.
  Frame()
  {
    utils::zero(const_cast<Picture&>(*static_cast<const Picture*>(this)));
    timestamp = ::avinout::INVALID_TIMESTAMP;
  }
  //! Konstruktor alokuj¹cy miejsce na ramkê.
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