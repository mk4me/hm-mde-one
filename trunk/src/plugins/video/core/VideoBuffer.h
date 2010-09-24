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
  //! Typ listy chunkÛw.
  typedef std::list<VideoBufferChunk*> Chunks;

private:
  //! Chunki bufora.
  Chunks chunks;
  //! Wszystkie ramki.
  std::vector<Picture*> buffer;
  //! Nieuøywne, a zaalokowane ramki
  std::stack<Picture*> unused;
  //! Maksymalny rozmiar buforu.
  const int maxSize;
  //! SzerokoúÊ ramki.
  int width;
  //! WysokoúÊ ramki.
  int height;
  //! Ile ramek maksymalnie moøna zbuforowaÊ?
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
  //! Destruktor, zwalnia ca≥y bufor.
  ~VideoBuffer();

  //! \return
  inline PixelFormat getFormat() const
  { 
    return format;
  }

  //! \return Wskaünik na bufor ramki albo NULL, jeøeli wyczerpa≥o siÍ miejsce w buforze.
  Picture * pop();
  //! \param frame Ramka, ktÛra ma byÊ zwrÛcona do bufora. Wywo≥ywaÊ TYLKO, jeúli po uøyciu
  //!              metody pop ramka nie zosta≥a uøyta.
  void push(Picture * frame);

  //! Zaznacza, øe ramka jest uøyta.
  //! \param frame
  //! \param startTime Poczπtek przedzia≥u czasu.
  //! \param endTime Koniec przedzia≥u czasu.
  bool notifyUsed(Picture * frame, double startTime, double endTime);
  //! Zwalania ramkÍ w zadanym punkcie czasowym.
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
  //! \return Zbuforowana ramka dla zdanego czasu bπdü NULL, jeøeli nie jest zbuforowana.
  const Picture * getFrame(double time, double * timestamp = NULL, double * finish = NULL);
  //! \param time
  //! \param timestamp
  //! \param finish
  //! \return Zbuforowana ramka dla zdanego czasu bπdü NULL, jeøeli nie jest zbuforowana.
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