/********************************************************************
	created:	2011/09/06
	created:	6:9:2011   12:49
	filename: 	SchemeDialog.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_SCHEME_DIALOG__SchemeDialog_H__
#define HEADER_GUARD_SCHEME_DIALOG__SchemeDialog_H__


#include <core/IVisualizer.h>
#include <osg/Geode>
#include <boost/tuple/tuple.hpp>
#include "TrajectoriesDrawer.h"
#include "GlLineSchemeDrawer.h"
#include "GlPointSchemeDrawer.h"
#include "ui_TrajectoriesDialog.h"

//! Dialog Qt sluzacy do zarzadzania widocznoscia elementow na scenie
class SchemeDialog : public QDialog, private Ui::TrajectoriesDialog
{
	Q_OBJECT;
public:
    //! Kontruktor widgeta
    //! \param parent parent dialogu
	SchemeDialog(QWidget* parent);

public:
    //! Dodanie do dialogu drawera, ktory bedzie modyfikowany
    //! \param drawer obiekt drawera osg
    //! \param rootName wyswietlana nazwa
	void setDrawer(OsgSchemeDrawerPtr drawer, const QString& rootName);

private slots:
    //! zmieniono vidocznosc pojedynczego wezla
    //! \param visible czy widoczny
    void visibilityChanged(bool visible);
    //! zmieniono widocznosc roota (dziala na calosc)
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
    //! Metoda przechodzi przez drzewo i zwraca element, ktory zawiera w ktorejs kolumnie podany obiekt
    //! \param object szukany obiekt
    //! \return element drzwa lub wyjatek jesli takiego nie znaleziono
    QTreeWidgetItem* getItemWhichContains(QObject* object) const;
    //! Pomocnicza metoda, zmienia kolor tla przycisku
    //! \param button 
    //! \param color 
    void setButtonColor(QPushButton* button, const QColor& color);
    //! blokuje sygnaly kontrolek modyfikujacych trajektorie
    //! \param val 
	void blockAllSignals(bool val);

private:
    std::map<QTreeWidgetItem*, boost::tuple<GlPointSchemeDrawerPtr, GlLineSchemeDrawerPtr, int>> item2Drawer;
    std::map<QTreeWidgetItem*, OsgSchemeDrawerPtr> item2Root;
};


#endif
