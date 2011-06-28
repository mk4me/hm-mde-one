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

// POPa zrobimy na koñcu, bo inaczej powstan¹ warningi zwi¹zane z tym, ¿e
// osg::ref_ptr nie ma DLL interface; OSG domyœlnie wy³¹cza te warningi w
// nag³ówkach, wiêc zdajemy siê na ichnie rozwi¹zania
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
 *	Strumieñ wideo.
 */
class VIDLIB_EXPORT VideoImageStream : public osg::ImageStream, public OpenThreads::Thread
{
public:
    /** Domyœlne implementacje */
    META_Object(vidlib, VideoImageStream);

    //! Domyœlny format.
    static const PixelFormat defaultFormat = PixelFormatBGRA;

protected:
    //! Wrapper na surowy strumieñ.
    typedef utils::PtrWrapper<VideoStream, utils::PtrPolicyOSG> Stream;
    //! Wrapper na mutex.
    typedef utils::Adapter<OpenThreads::Mutex, utils::PtrPolicyOSG> Mutex;
    //! Wrapper na timer.
    typedef utils::Adapter<osg::Timer, utils::PtrPolicyOSG> Timer;
    //! Typ blokady opartej na muteksie.
    typedef OpenThreads::ScopedLock<OpenThreads::Mutex> ScopedLock;
    //! Typ p³ytkich kopii strumienia.
    typedef utils::Adapter<std::set<osg::observer_ptr<VideoImageStream> >, utils::PtrPolicyOSG > Copies;

    //! Wewnêtrzy strumieñ.
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
    //! Maksymalna szerokoœæ.
    int maxWidth;
    /** Bufor na bie¿¹c¹ ramkê */
    Picture currentPicture;

public:
    /** Konstruktor zeruj¹cy */
    VideoImageStream();
    /** Konstruktor kopiuj¹cy */
    VideoImageStream(const VideoImageStream & image, const osg::CopyOp & copyop = osg::CopyOp::SHALLOW_COPY);
    /** */
    virtual ~VideoImageStream();

    //! Otwiera strumieñ
    //! \param filename Œcie¿ka do pliku.
    //! \param dummy Pozostawiæ wartoœæ domyœln¹.
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

    /** Odtwarzanie strumienia (nowy w¹tek!) */
    virtual void play();
    /** Zatrzymanie strumienia */
    virtual void pause();
    /** Odtwarzanie "do ty³u" */
    virtual void rewind();
    /** Wyszukiwanie w strumieniu */
    virtual void seek(double time);
    /** Zamkniêcie strumienia */
    virtual void quit(bool waitForThreadToExit = true);

    /** Czas utworzenia */
    virtual double getCreationTime() const;
    /** D³ugoœæ strumienia */
    virtual double getLength() const;
    /** Liczba klatek na sekundê */
    virtual double getFrameRate() const;
    /** Czas w strumieniu */
    virtual double getReferenceTime() const;
    /** Czas w strumieniu */
    virtual void setReferenceTime(double value) { seek(value); }
    /** Skala czasowa */
    virtual void setTimeMultiplier(double timescale);
    /** Skala czasowa */
    virtual double getTimeMultiplier() const;
    /** Czy wideo ma kana³ alpha? */
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

    //! \return Maksymalna szerokoœæ konwertowanego strumienia.
    inline int getMaxWidth() const
    { 
        return maxWidth;
    }
    //! \param maxWidth Maksymalna szerokoœæ konwertowanego strumienia.
    void setMaxWidth(int maxWidth);

protected:

    /** W¹tek odtwarzaj¹cy */
    virtual void run();
    /** Tryb zapêtlania */
    virtual void applyLoopingMode();
    /** Ustawia bie¿¹c¹ ramkê (jeœli jest nowsza ni¿ poprzednia)*/
    virtual void publishFrameAndNotify();
    /** Ustawia bie¿¹c¹ ramkê (jeœli jest nowsza ni¿ poprzednia)*/
    virtual void publishFrame();
    /** Ustawia bie¿¹cy czas */
    virtual void setStreamTime( double time );
    /** Ustawia bie¿¹c¹ ramkê */
    void reloadImage(Picture& picture);


    //! \param stream Wewnêtrzny strumieñ.
    void setStream( VideoStream* stream );

    //! \return Wewnêtrzny strumieñ.
    inline VideoStream* getStream()
    {
        return *(innerStream.get());
    }
    //! \return Wewnêtrzny strumieñ.
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