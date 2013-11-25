/********************************************************************
	created:	2013/10/12
	created:	12:10:2013   23:31
	filename: 	LayeredImageVisualizer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__LAYEREDIMAGEVISUALIZER_H__
#define HEADER_GUARD_DICOM__LAYEREDIMAGEVISUALIZER_H__

#include <QtCore/QObject>
#include <QtGui/QWidget>
#include <corelib/IVisualizer.h>
#include <corelib/AbstractSerie.h>
#include <plugins/dicom/ILayeredImage.h>

class QLabel;
class QScrollArea;
class QPushButton;
class QSlider;

namespace dicom {

    class LayeredSerie : public plugin::AbstractSerie, public plugin::IVisualizer::ITimeAvareSerieFeatures
    {
    public:
        virtual ~LayeredSerie() {}

        virtual void setupData( const core::ObjectWrapperConstPtr & data );

        //QPixmap convertToPixmap( DicomImagePtr image );

        virtual void update() {}

        virtual void setTime( double time );

        virtual double getLength() const;

    public:
        const QPixmap& getPixmap() const { return pixmap; }

    private:
        QPixmap pixmap;
    };
    DEFINE_SMART_POINTERS(LayeredSerie)

    class LayeredImageVisualizerView;
    //! Wizualizator Dicom
    class LayeredImageVisualizer : public QObject, public plugin::IVisualizer
    {
        Q_OBJECT;
        UNIQUE_ID("{4CBA33A4-F0EA-4607-9CB1-C1816697D1A1}");
        CLASS_DESCRIPTION("Layered Image Visualizer", "Layered Image Visualizer");

    public:
        LayeredImageVisualizer();
        virtual ~LayeredImageVisualizer();

    public://! \return pusty obiekt wizualizatora
        virtual IVisualizer* create() const;
        //! Tworzy obiekt typu NewChartSerie
        //! \param data ObjectWrapper z interfejsem do odczytu DataChannela
        //! \param name nazwa tworzonej serii danych
        //! \return utworzona seria
        plugin::IVisualizer::ISerie *createSerie(const utils::TypeInfo & requestedType, const core::ObjectWrapperConstPtr& data);
        // \return metoda nie jest obs�ugiwana, nullptr
        plugin::IVisualizer::ISerie *createSerie(const plugin::IVisualizer::ISerie * serie);
        //! Usuwa serie z wizualizatora
        //! \param serie seria do usuni�cia, musi nale�e� do wizualizatora i musi by� przez niego stworzona
        virtual void removeSerie(plugin::IVisualizer::ISerie *serie);

        virtual void setActiveSerie(plugin::IVisualizer::ISerie * serie);
        virtual const plugin::IVisualizer::ISerie * getActiveSerie() const;
        //! Tworzy g��wny widget wizualizatora
        //! \param manager Manager z akcjami do flexi bara
        //! \return utworzony widget
        virtual QWidget* createWidget();
        //! \return ikona wizualizatora
        virtual QIcon* createIcon();
        //! \return makymalna liczba serii, -1 bo nie ma limitu
        virtual int getMaxDataSeries() const;
        //! Zwraca info o wspieranych typach (tylko ScalarChannelReaderInterface)
        //! \param info zwracana struktura z opisami
        virtual void getSupportedTypes(core::TypeInfoList & supportedTypes) const;
        //! Metoda wywo�ywana cyklicznie pozwala od�wie�y� stan wizualizatora
        //! \param deltaTime czas od ostatniego wywo�ania metody
        virtual void update(double deltaTime);
        //! Zrzut wizualizatora do pixmapy
        virtual QPixmap takeScreenshot() const;

        int getNumSeries() const;
        int getCurrentSerieNo() const;

    public slots:
        void setPrevSerie();
        void setNextSerie();
        void trySetSerie(int val);

    signals:
        void serieChanged();
    private:
        bool correctIndex(int i) const;

    private:
       //QLabel* label;
       //QScrollArea* scrollArea;
       //QSlider* bar;
       //QPushButton* nextButton;
       //QPushButton* prevButton;
        //QWidget* mainWidget;
        LayeredImageVisualizerView* mainWidget;
        std::vector<LayeredSerie*> series;
        int currentSerie;
    };
    DEFINE_SMART_POINTERS(LayeredImageVisualizer);

}
#endif