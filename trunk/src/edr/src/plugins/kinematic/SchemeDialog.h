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

//! Dialog Qt s³u¿¹cy do zarz¹dzania widocznoœcia elementów na scenie
class SchemeDialog : public QDialog, private Ui::TrajectoriesDialog
{
	Q_OBJECT;
public:
    //! Kontruktor widgeta
    //! \param parent parent dialogu
	SchemeDialog(QWidget* parent);

public:
    //! Dodanie do dialogu drawera, który bêdzie modyfikowany
    //! \param drawer obiekt drawera osg
    //! \param rootName wyœwietlana nazwa
	void setDrawer(OsgSchemeDrawerPtr drawer, const QString& rootName);

private slots:
    //! zmieniono widocznoœæ pojedynczego wêz³a
    //! \param visible czy widoczny
    void visibilityChanged(bool visible);
    //! zmieniono widocznoœæ roota (dzia³a na ca³oœæ)
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
    //! Metoda przechodzi przez drzewo i zwraca element, który zawiera w którejœ kolumnie podany obiekt
    //! \param object szukany obiekt
    //! \return element drzwa lub wyj¹tek jeœli takiego nie znaleziono
    QTreeWidgetItem* getItemWhichContains(QObject* object) const;
    //! Pomocnicza metoda, zmienia kolor t³a przycisku za pomoc¹ stylów Qt
    //! \param button przycisk, któremu zmienione zostanie t³o
    //! \param color nowy kolor przycisku
    void setButtonColor(QPushButton* button, const QColor& color);
    //! blokuje sygna³y kontrolek modyfikuj¹cych trajektorie
    //! \param val czy zablokowaæ, czy odblokowaæ
	void blockAllSignals(bool val);

private:
    //! mapa (element drzewa - > krotka(punkty, linie, nr markera))
    std::map<QTreeWidgetItem*, boost::tuple<GlPointSchemeDrawerPtr, GlLineSchemeDrawerPtr, int>> item2Drawer;
    //! mapa (element drzewa -> g³owny drawer obiektu powi¹zanego z elementem drzewa)
    std::map<QTreeWidgetItem*, OsgSchemeDrawerPtr> item2Root;
};


#endif
