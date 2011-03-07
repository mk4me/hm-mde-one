/********************************************************************
	created:  2010/07/29
	created:  29:7:2010   13:21
	filename: Picture.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD_VIDLIB__PICTURE_H__
#define __HEADER_GUARD_VIDLIB__PICTURE_H__

#include <vidlib/Export.h>
#include <vidlib/PixelFormat.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

//!
struct PictureContext;

//! Wsp�lne dane wszystkich obrazk�w.
struct VIDLIB_EXPORT PictureCommon
{
  //! Kontekst zdj�cia.
  PixelFormat format;
  //! Szeroko�� (tekstele).
  int width;
  //! Wysoko�� (tekstele).
  int height;

  //! \return Kontekst zdj�cia.
  const PictureContext * getContext() const;
};

//! Obrazek z wieloma warstwami.
struct VIDLIB_EXPORT PictureLayered : public PictureCommon
{
  //! Dane warstw.
  unsigned char * data[4];
  //! Szeroko�ci warstw (w bajtach).
  int dataWidth[4];
  //! Wysoko�� warstw (liczba wierszy)
  int dataHeight[4];

  //! \return ��czny rozmiar zaalokowanej pami�ci.
  inline int getAllocSize() const
  {
    int size = 0;
    for (int i = 0; i < 4; ++i) {
      size += dataWidth[i] * dataHeight[i];
    }
    return size;
  }
};

//! Obrazek z jedn� warstw� (lub ze spakowanymi danymi).
struct VIDLIB_EXPORT Picture : public PictureCommon
{
  //! Szeroko�� wiersza (w bajtach). W przypadku spakowanych danych
  //! mo�e odbiega� od szeroko�ci obrazka.
  int dataWidth;
  //! Liczba wierszy danych. W przypadku spakowanych danych mo�e odbiega�
  //! od wysoko�ci obrazka.
  int dataHeight;
  //! Dane.
  unsigned char * data;

  //! Tworzy instancj� obrazka.
  //! \param width Szeroko��.
  //! \param height Wysoko��.
  //! \param format Format.
  static Picture create(int width, int height, PixelFormat format);
  //! 
  //! \param width
  //! \param height
  //! \param format
  static int getAllocSize(int width, int height, PixelFormat format);

  //! Okre�la rozmiar potrzebnych danych oraz alokuje.
  void alloc();
  //! Zwalnia dane obrazka.
  void free();
  //! Zeruje dane obrazka.
  void zero();

  //! \return Rozmiar zaalokowanej pami�ci.
  inline int getAllocSize() const
  {
    return dataHeight * dataWidth;
  }

  //! \return Reprezentacja obrazka z wydzielonymi warstwami.
  PictureLayered getLayers();
  //! Kopiuje dane obrazka do obrazka docelowego.
  //! \param dst Obrazek docelowy.
  void copyTo(Picture & dst) const;
};

//! Obrazek z destruktorem; nadaje si� tam, gdzie kod ma wiele punkt�w wyj�cia
//! i r�czne zwalnianie mo�e by� problemem.
struct VIDLIB_EXPORT ScopedPicture : public Picture
{
    //! 
    //! \param width
    //! \param height
    //! \param format
    ScopedPicture(int width, int height, PixelFormat format)
    {
        reinterpret_cast<Picture&>(*this) = create(width, height, format);
    }
    //! 
    ~ScopedPicture()
    {
        free();
    }

private:
    //! Uniemo�liwone kopiowanie.
    ScopedPicture(const ScopedPicture& picture);
    //! Uniemo�liwione kopiowanie.
    ScopedPicture& operator=(const ScopedPicture& picture);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD_VIDLIB__PICTURE_H__