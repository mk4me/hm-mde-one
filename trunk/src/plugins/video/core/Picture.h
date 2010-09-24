/********************************************************************
	created:  2010/07/29
	created:  29:7:2010   13:21
	filename: Picture.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__VM_IMAGE_H__
#define __HEADER_GUARD__VM_IMAGE_H__

////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////

using namespace vm;

struct PictureContext;

//! Wspólne dane wszystkich obrazków.
struct PictureCommon
{
  //! Kontekst zdjêcia.
  PixelFormat format;
  //! Szerokoœæ (tekstele).
  int width;
  //! Wysokoœæ (tekstele).
  int height;

  //! \return Kontekst zdjêcia.
  const PictureContext * getContext() const;
};

//! Obrazek z wieloma warstwami.
struct PictureLayered : public PictureCommon
{
  //! Dane warstw.
  unsigned char * data[4];
  //! Szerokoœci warstw (w bajtach).
  int dataWidth[4];
  //! Wysokoœæ warstw (liczba wierszy)
  int dataHeight[4];

  //! \return £¹czny rozmiar zaalokowanej pamiêci.
  int getAllocSize() const
  {
    int size = 0;
    for (int i = 0; i < 4; ++i) {
      size += dataWidth[i] * dataHeight[i];
    }
    return size;
  }
};

//! Obrazek z jedn¹ warstw¹ (lub ze spakowanymi danymi).
struct Picture : public PictureCommon
{
  //! Szerokoœæ wiersza (w bajtach). W przypadku spakowanych danych
  //! mo¿e odbiegaæ od szerokoœci obrazka.
  int dataWidth;
  //! Liczba wierszy danych. W przypadku spakowanych danych mo¿e odbiegaæ
  //! od wysokoœci obrazka.
  int dataHeight;
  //! Dane.
  unsigned char * data;

  //! Tworzy instancjê obrazka.
  //! \param width Szerokoœæ.
  //! \param height Wysokoœæ.
  //! \param format Format.
  static Picture create(int width, int height, PixelFormat format);
  //! 
  //! \param width
  //! \param height
  //! \param format
  static int getAllocSize(int width, int height, PixelFormat format);

  //! Okreœla rozmiar potrzebnych danych oraz alokuje.
  void alloc();
  //! Zwalnia dane obrazka.
  void free();
  //! Zeruje dane obrazka.
  void zero();

  //! \return Rozmiar zaalokowanej pamiêci.
  int getAllocSize() const
  {
    return dataHeight * dataWidth;
  }

  //! \return Reprezentacja obrazka z wydzielonymi warstwami.
  PictureLayered getLayers();
  //! Kopiuje dane obrazka do obrazka docelowego.
  //! \param dst Obrazek docelowy.
  void copyTo(Picture & dst) const;
};

////! 
//struct PictureYV12 : public Picture
//{
//  //! \return Wysokoœæ warstwy Y.
//  int heightY()
//  {
//    return dataHeight - heightUV();
//  }
//  //! \return Wysokoœæ warstwy U i V.
//  int heightUV()
//  {
//    return dataHeight / 3;
//  }
//  //! \return Warstwa Y.
//  unsigned char * Y()
//  {
//    return data;
//  }
//  //! \return Warstwa U.
//  unsigned char * U()
//  {
//    return data + dataWidth * heightY();
//  }
//  // \return Warstwa V.
//  unsigned char * V()
//  {
//    return U() + (dataWidth>>1);
//  }
//};

////////////////////////////////////////////////////////////////////////////////
} // namespace vm
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__VM_IMAGE_H__