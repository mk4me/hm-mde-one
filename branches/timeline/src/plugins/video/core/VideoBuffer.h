/********************************************************************
	created:  2010/07/28
	created:  28:7:2010   14:40
	filename: VideoBuffer.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__FRAMEBUFFER_H__
#define __HEADER_GUARD__FRAMEBUFFER_H__

////////////////////////////////////////////////////////////////////////////////
namespace vm {
////////////////////////////////////////////////////////////////////////////////

class VideoBufferChunk;

//! Bufor ramek o maksymalnym zadanym rozmiarze.
class VideoBuffer
{
private:
  //! Typ listy chunk�w.
  typedef std::list<VideoBufferChunk*> Chunks;

private:
  //! Chunki bufora.
  Chunks chunks;
  //! Wszystkie ramki.
  std::vector<Picture*> buffer;
  //! Nieu�ywne, a zaalokowane ramki
  std::stack<Picture*> unused;
  //! Maksymalny rozmiar buforu.
  const int maxSize;
  //! Szeroko�� ramki.
  int width;
  //! Wysoko�� ramki.
  int height;
  //! Ile ramek maksymalnie mo�na zbuforowa�?
  size_t maxBufferSize;
  //! Format ramki
  PixelFormat format;

public:
  //! 
  //! \param maxSize
  //! \param width
  //! \param height
  //! \param format
  VideoBuffer(int maxSize, int width, int height, PixelFormat format);
  //! Destruktor, zwalnia ca�y bufor.
  ~VideoBuffer();

  //! \return
  inline PixelFormat getFormat() const
  { 
    return format;
  }

  //! \return Wska�nik na bufor ramki albo NULL, je�eli wyczerpa�o si� miejsce w buforze.
  Picture * pop();
  //! \param frame Ramka, kt�ra ma by� zwr�cona do bufora. Wywo�ywa� TYLKO, je�li po u�yciu
  //!              metody pop ramka nie zosta�a u�yta.
  void push(Picture * frame);

  //! Zaznacza, �e ramka jest u�yta.
  //! \param frame
  //! \param startTime Pocz�tek przedzia�u czasu.
  //! \param endTime Koniec przedzia�u czasu.
  bool notifyUsed(Picture * frame, double startTime, double endTime);
  //! Zwalania ramk� w zadanym punkcie czasowym.
  bool freeFrame(double time);
  //!
  bool freeFirstFrame();
  //!
  bool freeLastFrame();

  //! \param timestamp
  //! \param finish
  inline const Picture * getLastFrame(double * timestamp = NULL, double * finish = NULL)
  {
    return chunks.empty() ? NULL : (*--chunks.end())->getLastFrame(timestamp, finish);
  }
  //! \param timestamp
  //! \param finish
  inline const Picture * getFirstFrame(double * timestamp = NULL, double * finish = NULL)
  {
    return chunks.empty() ? NULL : (*chunks.begin())->getFirstFrame(timestamp, finish);
  }

  //! \param time
  //! \param timestamp
  //! \param finish
  //! \return Zbuforowana ramka dla zdanego czasu b�d� NULL, je�eli nie jest zbuforowana.
  const Picture * getFrame(double time, double * timestamp = NULL, double * finish = NULL);
  //! \param time
  //! \param timestamp
  //! \param finish
  //! \return Zbuforowana ramka dla zdanego czasu b�d� NULL, je�eli nie jest zbuforowana.
  const Picture * getNearestFrame(double time, double * timestamp = NULL, double * finish = NULL);



private:
 
  Chunks::iterator getChunk( double time );
  Chunks::const_iterator getChunk( double time ) const;

  template <class Iter>
  Iter getChunk( double time, Iter begin, Iter end ) const;

};


////////////////////////////////////////////////////////////////////////////////
} // namespace vmOSGPlugin
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__FRAMEBUFFER_H__