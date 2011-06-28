#include "PCH.h"
#include <algorithm>
#include <math.h>
#include <float.h>
#include "VidLibPrivate.h"

#include <utils/Align.h>
#include <vidlib/BufferedVideoStream.h>
#include <vidlib/VideoBuffer.h>
#include "Converter.h"
#include <iostream>

#include <utils/Utils.h>

////////////////////////////////////////////////////////////////////////////////
namespace vidlib {
////////////////////////////////////////////////////////////////////////////////

#define FACTOR 4

BufferedVideoStream::BufferedVideoStream( VideoStream * innerStream, int maxBufferSize /*= INT_MAX*/ ) :
VideoStream(), 
innerStream(innerStream), maxBufferSize(maxBufferSize),
wantedTime(0.0)
{
    VIDLIB_FUNCTION_PROLOG;
    // skalowanie do potêgi dwójki
    //int width = innerStream->getWidth();
 // int height = innerStream->getHeight();

    // inicjalizacja
    onAfterInit( std::string("Buffered: ") + innerStream->getSource(), 
        innerStream->getFramerate(),
        innerStream->getDuration(),
        innerStream->getPixelFormat(),
        // rozmiar musi byæ potêg¹ dwójki - jeœli nie jest jest zmniejszany do najbli¿ej odpowiedniej wartoœci
        utils::align(innerStream->getWidth() / FACTOR - 1, 2),
        utils::align(innerStream->getHeight() / FACTOR - 1, 2),
        innerStream->getAspectRatio());

    convertedFrames = new VideoBuffer(1024*1024*300, getWidth(), getHeight(), PixelFormatYV12 );
    pendingFrames = new VideoBuffer(1024*1024*25, innerStream->getWidth(), innerStream->getHeight(), innerStream->getPixelFormat());

    innerStream->setCallback(this);
}

BufferedVideoStream::~BufferedVideoStream()
{
    VIDLIB_FUNCTION_PROLOG;
    // zwolnienie wskaŸnika
    utils::deletePtr(convertedFrames);
    utils::deletePtr(pendingFrames);
    utils::deletePtr(innerStream);
}

double BufferedVideoStream::getTime() const
{
    VIDLIB_FUNCTION_PROLOG;
    //return innerStream->getTime();
    return wantedTime;
}

double BufferedVideoStream::getFrameTimestamp() const
{
    VIDLIB_FUNCTION_PROLOG;
    //return innerStream->getFrameTimestamp();
    //return getFrame(wantedTime)->timestamp;
    double timestamp = 0.0;
    convertedFrames->getNearestFrame(wantedTime, &timestamp);
    return timestamp;
}


double BufferedVideoStream::getNextFrameTimestamp() const
{
    VIDLIB_FUNCTION_PROLOG;
    double timestamp = 0.0;
    convertedFrames->getNearestFrame(wantedTime, NULL, &timestamp);
    return timestamp;
}


bool BufferedVideoStream::setTime( double time )
{
    VIDLIB_FUNCTION_PROLOG;
    /*double prevTime = wantedTime;*/
    wantedTime = time;

    // czy ramka jest przekonwertowana?
    if ( !convertedFrames->getFrame(time) ) {
        // mo¿e jest w oczekuj¹cym buforze?
        double startTime, endTime;
        Picture* target = NULL;
        if ( const Picture * pending = pendingFrames->getFrame(time, &startTime, &endTime) ) {
            // konwertujemy ramkê
            target = popBuffer(time);
            converter->convert(*pending, *target);
            // zwalniamy z bufora oczekuj¹cego
            pendingFrames->freeFrame(time);
        } else {
            // nie ma, trzeba zmieniæ czas strumienia
            innerStream->setTime(time);
            startTime = innerStream->getFrameTimestamp();
            endTime = innerStream->getNextFrameTimestamp();
            // ustawianie czasu dla niektórych strumieni mo¿e siê nie powieœæ, dlatego trzeba
            // jeszcze raz sprawdziæ
            if ( !convertedFrames->getFrame(startTime) ) {
                // pobieramy ramkê
                target = popBuffer(time);
                innerStream->getFrame( *target );
            } else {
                int test = 0;
            }
        }

        // zaznaczamy u¿ycie
        if ( target ) {
            convertedFrames->notifyUsed( target, startTime, endTime == getDuration() ? DBL_MAX : endTime );
        }
    }

    return true;
    //return returnOrCopyError(innerStream->setTime(time));
}


bool BufferedVideoStream::getData( Picture & dst )
{
    VIDLIB_FUNCTION_PROLOG;
    const Picture * data = convertedFrames->getFrame(wantedTime);
    if ( data ) {
        dst = *data;
        return true;
    } else {
        // czyœcimy ramkê
        data = convertedFrames->getNearestFrame(wantedTime);
        if ( data ) {
            dst = *data;
            return true;
        }

        //TODO:
        //obs³uga

        // musimy zaktualizowaæ bufor
//         Picture * frame = convertedFrames->pop();
//         if ( !frame ) {
//             convertedFrames->freeFirstFrame();
//             frame = convertedFrames->pop();
//         }
// 
//         innerStream->setTime(wantedTime);
//         double timestamp = innerStream->getFrameTimestamp();
//         innerStream->getFrame(*frame);
//         innerStream->readNext();
// 
//         convertedFrames->notifyUsed( frame, timestamp, innerStream->getFrameTimestamp() );
// 
//         dst = *frame;
    }

    return false;
}


bool BufferedVideoStream::returnOrCopyError( bool result )
{
    VIDLIB_FUNCTION_PROLOG;
    if ( !result ) {
        return notifyError( *innerStream->getLastError() );
    } else {
        return true;
    }
}


bool BufferedVideoStream::readNext()
{
    return false;
}

void BufferedVideoStream::frameRead( Picture * /*picture*/, PictureLayered * /*layers*/, double timestamp, double endTimestamp )
{
    VIDLIB_FUNCTION_PROLOG;
    //return;
    if ( wantedTime >= timestamp && wantedTime < endTimestamp ) {
        // nie ma sensu, zaraz j¹ sobie odczytamy inaczej
        return;
    } else if ( pendingFrames->getFrame(timestamp) || convertedFrames->getFrame(timestamp) ) {
        // ramka ju¿ jest odczytana
        return;
    }
        
    //std::cout << "Missed: " << timestamp << " " << endTimestamp << " wanted " << wantedTime << std::endl;

    // ramka z bufora
    Picture * target = pendingFrames->pop();
    if ( !target ) {
        // usuwamy najstarsz¹ ramkê
        pendingFrames->freeFirstFrame();
        target = pendingFrames->pop();
    }
    // pobieramy
    innerStream->getFrame(*target);
    // dodajemy
    pendingFrames->notifyUsed(target, timestamp, endTimestamp);
}

Picture* BufferedVideoStream::popBuffer( double time )
{
    VIDLIB_FUNCTION_PROLOG;
    // czy da siê po prostu pobraæ?
    Picture * frame = convertedFrames->pop();
    if ( !frame ) {
        // bufor pe³ny, trzeba zwolniæ któr¹œ ramkê
        double latest, oldest;
        convertedFrames->getLastFrame(&latest);
        convertedFrames->getFirstFrame(&oldest);
        if ( fabs(time - oldest) > fabs(latest - time) ) {
            convertedFrames->freeFirstFrame();
        } else {
            convertedFrames->freeLastFrame();
        }
        frame = convertedFrames->pop();
    }
    return frame;
}



////////////////////////////////////////////////////////////////////////////////
} // namespace vidlib
////////////////////////////////////////////////////////////////////////////////