/********************************************************************
	created:	2011/09/06
	created:	6:9:2011   12:49
	filename: 	SchemeDialog.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SCHEME_DIALOG__SchemeDialog_H__
#define HEADER_GUARD_SCHEME_DIALOG__SchemeDialog_H__


#include <corelib/IVisualizer.h>
#include <osg/Geode>
#include <boost/tuple/tuple.hpp>
#include "TrajectoriesDrawer.h"
#include "GlLineSchemeDrawer.h"
#include "GlPointSchemeDrawer.h"
#include "ui_TrajectoriesDialog.h"

//! Dialog Qt służący do zarządzania widocznościa elementów na scenie
class SchemeDialog : public QDialog, private Ui::TrajectoriesDialog
{
	Q_OBJECT;
public:
    //! Kontruktor widgeta
    //! \param parent parent dialogu
	SchemeDialog(QWidget* parent);

public:
    //! Dodanie do dialogu drawera, który będzie modyfikowany
    //! \param drawer obiekt drawera osg
    //! \param rootName wyświetlana nazwa
	void setDrawer(OsgSchemeDrawerPtr drawer, const QString& rootName);

private slots:
    //! zmieniono widoczność pojedynczego węzła
    //! \param visible czy widoczny
    void visibilityChanged(bool visible);
    //! zmieniono widoczność roota (działa na całość)
    //! \param visible czy widoczny
    void rootVisibilityChanged(bool visible);
    //! zmieniono kolor
    void globalColorClicked();  

private:
    //! Zmiana formatu koloru 
    //! \param color kolor w formacie RGBA(0-1)
    //! \return kolor w formacie Qt
    QColor transformColor(const osg::Vec4& color) const;
    //! Zmiana formatu koloru
    //! \param color kolor w formacie Qt 
    //! \return kolor w formacie RGBA(0-1)
    osg::Vec4 transformColor(const QColor& color) const;
    //! Metoda przechodzi przez drzewo i zwraca element, który zawiera w którejś kolumnie podany obiekt
    //! \param object szukany obiekt
    //! \return element drzwa lub wyjątek jeśli takiego nie znaleziono
    QTreeWidgetItem* getItemWhichContains(QObject* object) const;
    //! Pomocnicza metoda, zmienia kolor tła przycisku za pomocą stylów Qt
    //! \param button przycisk, któremu zmienione zostanie tło
    //! \param color nowy kolor przycisku
    void setButtonColor(QPushButton* button, const QColor& color);
    //! blokuje sygnały kontrolek modyfikujących trajektorie
    //! \param val czy zablokować, czy odblokować
	void blockAllSignals(bool val);

private:
    //! mapa (element drzewa - > krotka(punkty, linie, nr markera))
    std::map<QTreeWidgetItem*, boost::tuple<GlPointSchemeDrawerPtr, GlLineSchemeDrawerPtr, int>> item2Drawer;
    //! mapa (element drzewa -> głowny drawer obiektu powiązanego z elementem drzewa)
    std::map<QTreeWidgetItem*, OsgSchemeDrawerPtr> item2Root;
};


#endif
