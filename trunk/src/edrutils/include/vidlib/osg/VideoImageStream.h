/********************************************************************
	created:  2011/01/26
	created:  26:1:2011   15:35
	filename: VideoImageStream.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDLIB__VIDEOIMAGESTREAM_H__
#define HEADER_GUARD_VIDLIB__VIDEOIMAGESTREAM_H__

#include <vidlib/Config.h>
#include <utils/Utils.h>

#include <set>

// POPa zrobimy na końcu, bo inaczej powstaną warningi związane z tym, że
// osg::ref_ptr nie ma DLL interface; OSG domyślnie wyłącza te warningi w
// nagłówkach, więc zdajemy się na ichnie rozwiązania
UTILS_PUSH_WARNINGS
#include <osg/ref_ptr>
#include <osg/observer_ptr>
#include <osg/ImageStream>
#include <OpenThreads/Thread>
#include <osg/Timer>
UTILS_POP_WARNINGS

#include <utils/PtrWrapper.h>
#include <utils/PtrPolicyOSG.h>

#include <vidlib/Export.h>
#include <vidlib/PixelFormat.h>
#include <vidlib/VideoStream.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////
UTILS_PUSH_WARNINGS
UTILS_DISABLE_DLL_INTERFACE_WARNING

/**
 *	Strumień wideo.
 */
class VIDLIB_EXPORT VideoImageStream : public osg::ImageStream, public OpenThreads::Thread
{
public:
    /** Domyślne implementacje */
    META_Object(vidlib, VideoImageStream);

    //! Domyślny format.
    static const PixelFormat defaultFormat = PixelFormatBGRA;

protected:
    //! Wrapper na surowy strumień.
    typedef utils::PtrWrapper<VideoStream, utils::PtrPolicyOSG> Stream;
    //! Wrapper na mutex.
    typedef utils::Adapter<OpenThreads::Mutex, utils::PtrPolicyOSG> Mutex;
    //! Wrapper na timer.
    typedef utils::Adapter<osg::Timer, utils::PtrPolicyOSG> Timer;
    //! Typ blokady opartej na muteksie.
    typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopedLock;
    //! Typ płytkich kopii strumienia.
    typedef utils::Adapter<std::set<osg::observer_ptr<VideoImageStream> >, utils::PtrPolicyOSG > Copies;

    //! Wewnętrzy strumień.
    osg::ref_ptr<Stream> innerStream; 
    /** Mutex */
    osg::ref_ptr<Mutex> mutex;
    /** Timer */
    osg::ref_ptr<Timer> timer;
    /** */
    osg::ref_ptr<Copies> copies;

    /** Skala czasowa */
    double timeScale;
    /** Poprzednia pozycja w strumieniu */
    double prevTimestamp;
    /** Format piksela */
    PixelFormat targetFormat;
    //! Maksymalna szerokość.
    int maxWidth;
    /** Bufor na bieżącą ramkę */
    Picture currentPicture;

public:
    /** Konstruktor zerujący */
    VideoImageStream();
    /** Konstruktor kopiujący */
    VideoImageStream(const VideoImageStream & image, const osg::CopyOp & copyop = osg::CopyOp::SHALLOW_COPY);
    /** */
    virtual ~VideoImageStream();

    //! Otwiera strumień
    //! \param filename Ścieżka do pliku.
    //! \param dummy Pozostawić wartość domyślną.
    template <class VideoStreamImpl>
    bool open(const std::string & filename, VideoStreamImpl* = 0)
    {
        try {
            setStream( new VideoStreamImpl(filename) );
            return true;
        } catch ( VideoError& error ) {
            OSG_FATAL<<"open("<<filename<<") reported an error: "<< error.what() << std::endl;
            return false;
        }
    }

    /** Odtwarzanie strumienia (nowy wątek!) */
    virtual void play();
    /** Zatrzymanie strumienia */
    virtual void pause();
    /** Odtwarzanie "do tyłu" */
    virtual void rewind();
    /** Wyszukiwanie w strumieniu */
    virtual void seek(double time);
    /** Zamknięcie strumienia */
    virtual void quit(bool waitForThreadToExit = true);

    /** Czas utworzenia */
    virtual double getCreationTime() const;
    /** Długość strumienia */
    virtual double getLength() const;
    /** Liczba klatek na sekundę */
    virtual double getFrameRate() const;
    /** Czas w strumieniu */
    virtual double getReferenceTime() const;
    /** Czas w strumieniu */
    virtual void setReferenceTime(double value) { seek(value); }
    /** Skala czasowa */
    virtual void setTimeMultiplier(double timescale);
    /** Skala czasowa */
    virtual double getTimeMultiplier() const;
    /** Czy wideo ma kanał alpha? */
    virtual bool isImageTranslucent() const;




    


    //!
    virtual bool requiresUpdateCall() const;
    //!
    virtual void update(osg::NodeVisitor* nv);

    //! \return Format do którego film jest konwertowany.
    inline PixelFormat getTargetFormat() const
    {
        return targetFormat;
    }
    //! \param targetFormat Format do którego film jest konwertowany.
    void setTargetFormat(PixelFormat targetFormat); 

    //! \return Maksymalna szerokość konwertowanego strumienia.
    inline int getMaxWidth() const
    { 
        return maxWidth;
    }
    //! \param maxWidth Maksymalna szerokość konwertowanego strumienia.
    void setMaxWidth(int maxWidth);

protected:

    /** Wątek odtwarzający */
    virtual void run();
    /** Tryb zapętlania */
    virtual void applyLoopingMode();
    /** Ustawia bieżącą ramkę (jeśli jest nowsza niż poprzednia)*/
    virtual void publishFrameAndNotify();
    /** Ustawia bieżącą ramkę (jeśli jest nowsza niż poprzednia)*/
    virtual void publishFrame();
    /** Ustawia bieżący czas */
    virtual void setStreamTime( double time );
    /** Ustawia bieżącą ramkę */
    void reloadImage(Picture& picture);


    //! \param stream Wewnętrzny strumień.
    void setStream( VideoStream* stream );

    //! \return Wewnętrzny strumień.
    inline VideoStream* getStream()
    {
        return *(innerStream.get());
    }
    //! \return Wewnętrzny strumień.
    inline const VideoStream* getStream() const
    {
        return *(innerStream.get());
    }
    //!
    inline OpenThreads::Mutex& getMutex()
    {
        return mutex->get();
    }
    //!
    inline osg::Timer& getTimer()
    {
        return timer->get();
    }

};

UTILS_POP_WARNINGS
////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////

#endif // HEADER_GUARD_VIDLIB__VIDEOIMAGESTREAM_H__
