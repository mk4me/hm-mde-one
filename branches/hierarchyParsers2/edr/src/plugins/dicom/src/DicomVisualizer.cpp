#include "DicomPCH.h"
#include <QtGui/QPixmap>
#include "DicomVisualizer.h"
#include <QtGui/QPixmap>
#include <QtGui/QLabel>
#include <dcmtk/ofstd/ofbmanip.h>


DicomVisualizer::DicomVisualizer()
{
    label = new QLabel();
}

DicomVisualizer::~DicomVisualizer()
{

}

plugin::IVisualizer* DicomVisualizer::create() const
{
    return new DicomVisualizer();
}

plugin::IVisualizer::ISerie * DicomVisualizer::createSerie( const utils::TypeInfo & requestedType, const core::ObjectWrapperConstPtr& data )
{
    auto serie = new DicomSerie();
    serie->setData(requestedType, data);
    label->setPixmap(serie->getPixmap());
    return serie; 
}

plugin::IVisualizer::ISerie * DicomVisualizer::createSerie( const plugin::IVisualizer::ISerie * serie )
{
    return nullptr;
}

void DicomVisualizer::removeSerie( plugin::IVisualizer::ISerie *serie )
{

}

void DicomVisualizer::setActiveSerie( plugin::IVisualizer::ISerie * serie )
{

}

const plugin::IVisualizer::ISerie * DicomVisualizer::getActiveSerie() const
{
    return nullptr;
}

QWidget* DicomVisualizer::createWidget()
{
    return label;
}

QIcon* DicomVisualizer::createIcon()
{
    return new QIcon();
}

int DicomVisualizer::getMaxDataSeries() const
{
    return -1;
}

void DicomVisualizer::getSupportedTypes( core::TypeInfoList & supportedTypes ) const
{
    supportedTypes.push_back(typeid(DcmDataset));
    supportedTypes.push_back(typeid(DicomImage));
}

void DicomVisualizer::update( double deltaTime )
{

}

QPixmap DicomVisualizer::takeScreenshot() const
{
    return QPixmap();
}


void DicomSerie::setupData( const core::ObjectWrapperConstPtr & data )
{
    DicomImageConstPtr img = data->get();
    auto width  = img->getWidth();
    auto height = img->getHeight();
    DicomImagePtr image = utils::const_pointer_cast<DicomImage>(img);

    // kod z forum : http://forum.dcmtk.org/viewtopic.php?t=120
    if ((image != NULL) && (image->getStatus() == EIS_Normal))
    {
        /* get image extension */
        const int width = (int)(image->getWidth());
        const int height = (int)(image->getHeight());
        char header[32];
        /* create PGM header */
        sprintf(header, "P5\n%i %i\n255\n", width, height);
        const int offset = strlen(header);
        const unsigned int length = width * height + offset;
        /* create output buffer for DicomImage class */
        Uint8 *buffer = new Uint8[length];
        if (buffer != NULL)
        {
            /* copy PGM header to buffer */
            OFBitmanipTemplate<Uint8>::copyMem((const Uint8 *)header, buffer, offset);
            if (image->getOutputData((void *)(buffer + offset), length, 8))
            {
                pixmap.loadFromData((const unsigned char *)buffer, length, "PGM", Qt::AvoidDither);
                // HACK - inaczej wizualizator sie za bardzo rozpycha (trzeba dodac scrollery, jesli wizualizator przekracza jakiœ rozmiar)
                pixmap = pixmap.scaledToHeight(570);
            }
            /* delete temporary pixel buffer */
            delete[] buffer;
        }
    }

//Uint8 *pixelData = (Uint8 *)(pimg->getOutputData(32 /* bits per sample */));
    //if (pixelData != nullptr)
    //{
    //    QImage i(pixelData, width, height, QImage::Format::Format_RGB32);
    //    pixmap = QPixmap::fromImage(i.scaledToWidth(800));
    //}
    
}

void DicomSerie::setTime( double time )
{
    
}

double DicomSerie::getLength() const
{
    return 1.0;
}
