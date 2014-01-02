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
#include "LayeredSerie.h"
#include "Adnotations.h"

class QLabel;
class QScrollArea;
class QPushButton;
class QSlider;

namespace dicom {

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
        // \return metoda nie jest obs³ugiwana, nullptr
        plugin::IVisualizer::ISerie *createSerie(const plugin::IVisualizer::ISerie * serie);
        //! Usuwa serie z wizualizatora
        //! \param serie seria do usuniêcia, musi nale¿eæ do wizualizatora i musi byæ przez niego stworzona
        virtual void removeSerie(plugin::IVisualizer::ISerie *serie);

        virtual void setActiveSerie(plugin::IVisualizer::ISerie * serie);
        virtual const plugin::IVisualizer::ISerie * getActiveSerie() const;
        virtual plugin::IVisualizer::ISerie * getActiveSerie();
        //! Tworzy g³ówny widget wizualizatora
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
        //! Metoda wywo³ywana cyklicznie pozwala odœwie¿yæ stan wizualizatora
        //! \param deltaTime czas od ostatniego wywo³ania metody
        virtual void update(double deltaTime);
        //! Zrzut wizualizatora do pixmapy
        virtual QPixmap takeScreenshot() const;

        int getNumSeries() const;
        int getCurrentSerieNo() const;
        

    public Q_SLOTS:
        void setPrevSerie();
        void setNextSerie();
        void trySetSerie(int val);
        void saveSerie();
        void uploadSerie();
        void editSerie(int idx);
        void removeLayer(int idx);
        void selectLayer( int idx );

    Q_SIGNALS:
        void serieChanged();
        void changeLabel(const QString& newLabel);

    private:
        bool correctIndex(int i) const;
        int selectedLayer() const;
        
    private:
        LayeredImageVisualizerView* mainWidget;
        std::vector<LayeredSerie*> series;
        int currentSerie;
    };
    DEFINE_SMART_POINTERS(LayeredImageVisualizer);

}
#endif
