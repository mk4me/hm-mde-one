#include "DicomImporterPCH.h"
#include "DicomImporter.h"
#include <QtGui/QPixmap>
//
//#include "DicomVisualizer.h"
#include <QtGui/QPixmap>
//#include <QtGui/QLabel>
//#include <QtGui/QHBoxLayout>
//#include <QtGui/QSlider>
//#include <QtGui/QPushButton>
//#include <QtGui/QScrollArea>
#include <dcmtk/ofstd/ofbmanip.h>
#include <dcmtk/dcmimage/diregist.h>
#include "plugins/dicomImporter/Dicom.h"
//
using namespace dicomImporter;
//
QPixmap dicomImporter::convertToPixmap( DicomImagePtr image )
{
    QPixmap result;
    // kod z forum : http://forum.dcmtk.org/viewtopic.php?t=120
    // rozszerzony o obs³ugê kolorów
    if ((image != NULL) && (image->getStatus() == EIS_Normal))
    {
        /* get image extension */
        const int width = (int)(image->getWidth());
        const int height = (int)(image->getHeight());
        const int depth = (int)(image->getDepth());
        char header[32];
        /* create PGM header */
        sprintf(header, "P%i\n%i %i\n255\n", image->isMonochrome() ? 5 : 6, width, height);
        const int offset = strlen(header);
        const unsigned int length = image->getOutputDataSize(8) + offset;//width * height + offset;
        /* create output buffer for DicomImage class */
        Uint8 *buffer = new Uint8[length];
        if (buffer != NULL)
        {
            /* copy PGM header to buffer */
            OFBitmanipTemplate<Uint8>::copyMem((const Uint8 *)header, buffer, offset);
            if (image->getOutputData((void *)(buffer + offset), length))
            {
                result.loadFromData((const unsigned char *)buffer, length, image->isMonochrome() ? "PGM" : "PMM", Qt::AvoidDither);
            }
            /* delete temporary pixel buffer */
            delete[] buffer;
        }
    }

    return result;
}
//
//
//DicomVisualizer::DicomVisualizer()
//{
//    mainWidget = new QWidget();
//    mainWidget->setLayout(new QVBoxLayout());
//
//    scrollArea = new QScrollArea();
//    label = new QLabel();
//    scrollArea->setBackgroundRole(QPalette::Dark);
//    scrollArea->setWidgetResizable(true);
//    scrollArea->setWidget(label);
//
//    QWidget* cont = new QWidget();
//    cont->setLayout(new QHBoxLayout());
//    nextButton = new QPushButton(">");
//    prevButton = new QPushButton("<");
//    bar = new QSlider(Qt::Horizontal);
//    cont->layout()->addWidget(bar);
//    cont->layout()->addWidget(prevButton);
//    cont->layout()->addWidget(nextButton);
//
//    mainWidget->layout()->addWidget(scrollArea);
//    mainWidget->layout()->addWidget(cont);
//}
//
//DicomVisualizer::~DicomVisualizer()
//{
//
//}
//
//plugin::IVisualizer* DicomVisualizer::create() const
//{
//    return new DicomVisualizer();
//}
//
//plugin::IVisualizer::ISerie * DicomVisualizer::createSerie( const utils::TypeInfo & requestedType, const core::ObjectWrapperConstPtr& data )
//{
//    auto serie = new DicomSerie();
//    serie->setData(requestedType, data);
//    QPixmap pixmap = serie->getPixmap();
//    label->setPixmap(pixmap);
//    return serie; 
//}
//
//plugin::IVisualizer::ISerie * DicomVisualizer::createSerie( const plugin::IVisualizer::ISerie * serie )
//{
//    return nullptr;
//}
//
//void DicomVisualizer::removeSerie( plugin::IVisualizer::ISerie *serie )
//{
//
//}
//
//void DicomVisualizer::setActiveSerie( plugin::IVisualizer::ISerie * serie )
//{
//
//}
//
//const plugin::IVisualizer::ISerie * DicomVisualizer::getActiveSerie() const
//{
//    return nullptr;
//}
//
//QWidget* DicomVisualizer::createWidget()
//{
//    return mainWidget;
//}
//
//QIcon* DicomVisualizer::createIcon()
//{
//    return new QIcon();
//}
//
//int DicomVisualizer::getMaxDataSeries() const
//{
//    return -1;
//}
//
//void DicomVisualizer::getSupportedTypes( core::TypeInfoList & supportedTypes ) const
//{
//    supportedTypes.push_back(typeid(DcmDataset));
//    supportedTypes.push_back(typeid(DicomImage));
//}
//
//void DicomVisualizer::update( double deltaTime )
//{
//
//}
//
//QPixmap DicomVisualizer::takeScreenshot() const
//{
//    return QPixmap();
//}
//
//
//void DicomSerie::setupData( const core::ObjectWrapperConstPtr & data )
//{
//    DicomImageConstPtr img = data->get();
//    auto width  = img->getWidth();
//    auto height = img->getHeight();
//    DicomImagePtr image = utils::const_pointer_cast<DicomImage>(img);
//
//    pixmap = convertToPixmap(image);
//}
//
//void DicomSerie::setTime( double time )
//{
//    
//}
//
//double DicomSerie::getLength() const
//{
//    return 1.0;
//}
//
