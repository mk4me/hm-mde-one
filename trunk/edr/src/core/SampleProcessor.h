/********************************************************************
    created:  2011/04/15
    created:  15:4:2011   15:19
    filename: SampleProcessor.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__SAMPLEPROCESSOR_H__
#define HEADER_GUARD_CORE__SAMPLEPROCESSOR_H__

#include <core/IDataProcessor.h>

typedef core::shared_ptr<int> IntPtr;
typedef core::shared_ptr<const int> IntConstPtr;

class Constant : public core::IDataProcessor
{
    IntPtr result;
    virtual void getInputInfo(int inputNo, InputInfo& info)
    {
    }
    virtual void getOutputInfo(ObjectWrapper::Types& outputInfo)
    {
        outputInfo.push_back(typeid(int));
    }
    virtual void process(IObjectSource* input, IObjectOutput* output)
    {
        if ( input->isChanged(0) ) {
            ::VideoStreamConstPtr stream = input->getObject(0);
            *result = static_cast<int>(stream->getDuration() / stream->getFrameDuration);
            output->setObject(0, result);
        }
    }
};

class FrameCounter : public core::IDataProcessor
{
private:
    IntPtr result;
    virtual void getInputInfo(int inputNo, InputInfo& info)
    {
        if ( inputNo == 0 ) {
            info.name = "stream";
            info.types.push_back(typeid(::VideoStream));
        }
    }
    virtual void getOutputInfo(ObjectWrapper::Types& outputInfo)
    {
        outputInfo.push_back(typeid(int));
    }
    virtual void process(IObjectSource* input, IObjectOutput* output)
    {
        if ( input->isChanged(0) ) {
            ::VideoStreamConstPtr stream = input->getObject(0);
            *result = static_cast<int>(stream->getDuration() / stream->getFrameDuration);
            output->setObject(0, result);
        }
    }
};




class RBFlipper : public core::IDataProcessor
{
private:
    //! Zdjêcie wyjœciowe.
    core::shared_ptr<vidlib::ScopedPictureRGB> outputPicture;
    IplImage* iplImage;

public:
    RBFlipper() : outputPicture(new vidlib::ScopedPictureRGB)  )
    {
        utils::zero(*outputPicture);
    }

// core::IDataProcessor
public:
    virtual void getInputInfo(int inputNo, InputInfo& info)
    {
        if ( inputNo == 0 ) {
            info.name = "picture";
            info.types.push_back(typeid(vidlib::ScopedPictureRGB));
        }
    }

    virtual void getOutputInfo(ObjectWrapper::Types& outputInfo)
    {
        outputInfo.push_back(typeid(vidlib::ScopedPictureRGB));
        outputInfo.push_back(typeid(IplImage));
    }

    virtual void process(IObjectSource* input, IObjectOutput* output)
    {

        core::shared_ptr<const vidlib::ScopedPictureRGB> source = input->getObject(0);

        // je¿eli zmieni³ siê rozmiar alokujemy ramkê na nowo
        if (outputPicture->dataWidth != source->dataWidth || 
            outputPicture->dataHeight != source->dataHeight ) {
            outputPicture->create( source->width, source->height );
        }

        // zamieniamy miejscami dwa kana³y
        int size = source->dataWidth * source->dataHeight;
        for (int i = 0; i < size; i+=3) {
            outputPicture->data[i] = source->data[i+2];
            outputPicture->data[i+1] = source->data[i+1];
            outputPicture->data[i+2] = source->data[i];
        }

        // wystawienie wyjœcia
        output->setObject(0, outputPicture);
    }
};

void test()
{
    IDataProcessor* processor;

    ObjectSource source;
    ObjectOutput output;


}

#endif  // HEADER_GUARD_CORE__SAMPLEPROCESSOR_H__