/********************************************************************
	created:	2011/10/18
	created:	18:10:2011   8:55
	filename: 	ConfigurationDialog.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__CONFIGURATIONDIALOG_H__
#define HEADER_GUARD_HMM__CONFIGURATIONDIALOG_H__

#include <QtGui/QDialog>
#include <QtCore/QFileInfo>
#include "ConfigurationPainter.h"
#include "ui_configurationDialog.h"
#include "ui_FilterConfiguratorWidget.h"

//! Widget z konfiguratorem graficznym
class ConfigurationWidget : public QWidget, public Ui::FilterConfiguratorWidget
{
    Q_OBJECT;
public:
    ConfigurationWidget(QWidget* parent);

public:
    //! ustawia opis konfiguratora
    //! \param text ustawiany tekst
    void setText(const QString& text);
    //! wczytuje pliki z rozmieszczeniem obszarow konfiguratora
    //! \param frontXml plik z obszarami dla przodu postaci
    //! \param backXml plik z obszarami dla tylu postaci
    //! \param names mapa [nazwa elementu - > nazwa do wyswietlenia]
    void loadConfigurations(const QString& frontXml, const QString& backXml, const  std::map<QString, std::pair<QString, QString>>& names);
    //! Ustawia widocznosc elementow w konfiguratorze
    //! \param visibles mapa [nazwa -> czy element ma byc widoczny]
    void setVisibles(const std::map<QString, bool>& visibles);

signals:
    //! kliknieto w element
    //! \param name nazwa elementu
    //! \param  
    void itemSelected(const QString& name, bool);
    //! Element zostal najechany myszka
    //! \param name nazwa elementu
    //! \param selected 
    void elementHovered(const QString& name, bool selected);

private slots:
    //! element zostal najechany przez myszke
    //! \param name nazwa elementu
    //! \param selected
    void onElementHovered(const QString& name, bool selected);
    //! kliknieto w element
    //! \param name nazwa elementu
    //! \param selected 
    void onItemSelected(const QString& name, bool selected);
    //! nacisnieto przycisk odwracajacy postac
    void onSwitchButton();

private:   
    //! wczytuje wszystkie pliki z listy i dodaje je do paintera (pozycja 0,0)
    //! \param painter tam trafia wczytywane obrazki
    //! \param files pliki do wczytania
    void loadPictures(ConfigurationPainter& painter, const QStringList &files );
    //! Wczytuje pojedynczy obrazek i dodaje go do paintera
    //! \param painter tam trafi wczytywany obrazek
    //! \param name nazwa pliku do wczytania
    //! \param x pozycja x
    //! \param y pozycja y
    //! \param alwaysVisible czy obrazek ma byc widoczny zawsze
    void loadPicture(ConfigurationPainter& painter, const QString& name, int x, int y, bool alwaysVisible = false);
    //! laduje plik xml z zapisanymi obszarami w postaci markerow lub obrazkow
    //! \param painter konfigurowany painter, tam trafia wczytane obszary
    //! \param filename sciezka do pliku
    void loadXml(ConfigurationPainter& painter, const QString& filename);
    //! Tworzy pojedynczy marker i dodaje go do paintera
    //! \param painter tam trafi tworzony marker
    //! \param name nazwa markera
    //! \param x pozycja x
    //! \param y pozycja y
    void createMarker(ConfigurationPainter &painter, const QString& name, int x, int y);
    //! pokazuje przod postaci
    void showFront();
    //! pokazuje tyl postaci
    void showBack();
    //! Ustawia obrazek z tlem
    //! \param painter modyfikowany painter
    //! \param name nazwa widoku
    //! \param pixmap obrazek tla
    void setBackground(ConfigurationPainter& painter, const QString& name, QPixmapConstPtr pixmap);

private:
    //! painter, ktory rysuje przod podstaci
    ConfigurationPainter painterFront;
    //! painter, ktory rysuje tyl podstaci
    ConfigurationPainter painterBack;
    //! aktualny painter
    ConfigurationPainter* currentPainter;
    //! layout ze scrollem (jesli widget nie miesci sie w wyznaczonym miejscu)
    QHBoxLayout scrollLayout;
    //! czy konfigurator aktualnie pokazuje przod postaci
    bool isFront;
};

#endif
