/********************************************************************
	created:	2013/08/13
	created:	13:8:2013   9:06
	filename: 	DicomVisualizer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__DICOMVISUALIZER_H__
#define HEADER_GUARD_DICOM__DICOMVISUALIZER_H__

#include <QtCore/QObject>
#include <QtGui/QWidget>
#include <corelib/IVisualizer.h>
#include <plugins/dicom/Dicom.h>

class QLabel;

// TODO Przeniesc do corelib
class AbstractSerie : public plugin::IVisualizer::ISerie
{
public:
    virtual ~AbstractSerie() {}

public:
    virtual void setName( const std::string & name ) { this->name = name; }
    virtual const std::string getName() const { return name; }
    virtual const core::ObjectWrapperConstPtr & getData() const { return data; }
    virtual const core::TypeInfo & getRequestedDataType() const { return requestedType; }
    virtual void setData( const core::TypeInfo & requestedDataType, const core::ObjectWrapperConstPtr & data )
    {
        this->requestedType = requestedDataType;
        this->data = data;
        setupData(data);
    }

public:
    virtual void setupData( const core::ObjectWrapperConstPtr & data ) = 0;

protected:
    //! wrapper przekazany serii
    core::ObjectWrapperConstPtr data;
    utils::TypeInfo requestedType;
    //! nazwa serii
    std::string name;
};
DEFINE_SMART_POINTERS(AbstractSerie);

class DicomSerie : public AbstractSerie, public plugin::IVisualizer::ITimeAvareSerieFeatures
{
public:
    virtual ~DicomSerie() {}

    virtual void setupData( const core::ObjectWrapperConstPtr & data );
    virtual void update() {}

    virtual void setTime( double time );

    virtual double getLength() const;

public:
    const QPixmap& getPixmap() { return pixmap; }

private:
    QPixmap pixmap;
};
DEFINE_SMART_POINTERS(DicomSerie)

//! Wizualizator Dicom
class DicomVisualizer : public QObject, public plugin::IVisualizer
{
    Q_OBJECT;
    UNIQUE_ID("{1CFBD466-C104-41B2-9CB5-B92DCD443D43}");
	CLASS_DESCRIPTION("Dicom Visualizer", "Dicom Visualizer");

public:
    DicomVisualizer();
	virtual ~DicomVisualizer();

public://! \return pusty obiekt wizualizatora
      virtual IVisualizer* create() const;
      //! Tworzy obiekt typu NewChartSerie
      //! \param data ObjectWrapper z interfejsem do odczytu DataChannela
      //! \param name nazwa tworzonej serii danych
      //! \return utworzona seria
      plugin::IVisualizer::ISerie *createSerie(const utils::TypeInfo & requestedType, const core::ObjectWrapperConstPtr& data);
      // \return metoda nie jest obsługiwana, nullptr
      plugin::IVisualizer::ISerie *createSerie(const plugin::IVisualizer::ISerie * serie);
      //! Usuwa serie z wizualizatora
      //! \param serie seria do usunięcia, musi należeć do wizualizatora i musi być przez niego stworzona
      virtual void removeSerie(plugin::IVisualizer::ISerie *serie);

	  virtual void setActiveSerie(plugin::IVisualizer::ISerie * serie);
	  virtual const plugin::IVisualizer::ISerie * getActiveSerie() const;
      //! Tworzy główny widget wizualizatora
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
      //! Metoda wywoływana cyklicznie pozwala odświeżyć stan wizualizatora
      //! \param deltaTime czas od ostatniego wywołania metody
      virtual void update(double deltaTime);
      //! Zrzut wizualizatora do pixmapy
      virtual QPixmap takeScreenshot() const;
private:
    QLabel* label;
};
DEFINE_SMART_POINTERS(DicomVisualizer);

#endif
