/********************************************************************
	created:	2011/09/06
	created:	6:9:2011   12:49
	filename: 	TrajectoriesDialog.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_TRAJECTORIES_DIALOG__TRAJECTORIESDIALOG_H__
#define HEADER_GUARD_TRAJECTORIES_DIALOG__TRAJECTORIESDIALOG_H__


#include <core/IVisualizer.h>
#include <osg/Geode>
#include "TrajectoriesDrawer.h"
#include "ui_TrajectoriesDialog.h"

//! Dialog Qt s³u¿¹cy do zarz¹dzania trajektoriami markerów
class TrajectoriesDialog : public QDialog, private Ui::TrajectoriesDialog
{
	Q_OBJECT;
public:
	//! Kontruktor widgeta
	//! \param parent parent dialogu
	TrajectoriesDialog(QWidget* parent);

public:
	//! Dodanie do dialogu drawera, który bêdzie modyfikowany
	//! \param drawer obiekt drawera osg
	//! \param rootName wyœwietlana nazwa
	void setDrawer(OsgSchemeDrawerPtr drawer, const QString& rootName);

private slots:
	//! zmieniono kolor
	void colorClicked();
	//! zmieniono widocznoœæ pojedynczego wêz³a
	//! \param visible czy widoczny
	void visibilityChanged(bool visible);
    //! zmieniono widocznoœæ roota (dzia³a na ca³oœæ)
    //! \param visible czy widoczny
    void rootVisibilityChanged(bool visible);
	//! zmieniono szerokoœæ trajektorii
	//! \param width nowa szerokoœæ
	void widthChanged(double width);
	//! zmieniony zosta³ poczaktowy czas, dla którego trajektoria jest widoczna
	//! \param time ustawiwany czas
	void startTimeChanged(double time);
    //! zmieniony zosta³ koñcowy czas, dla którego trajektoria jest widoczna
    //! \param time ustawiwany czas
	void endTimeChanged(double time);
	//! zmieni³ siê aktywny element drzewa
	//! \param current aktualnie aktywny element
	//! \param previous poprzednio aktywny element
	void treeItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous );
	//! zmieni³ siê slider reprezentuj¹cy pocz¹tkowy czas, dla którego trajektoria jest widoczna
	//! \param value nowa wartoœæ pocz¹tkowa slidra
    void startSliderChanged(int value);
    //! zmieni³ siê slider reprezentuj¹cy koñcowy czas, dla którego trajektoria jest widoczna
	//! \param value nowa wartoœæ koñcowa slidera
	void endSliderChanged(int value);

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
	//! Pomocnicza metoda, zmienia kolor t³a przycisku za pomoc¹ styli Qt
	//! \param button przycisk ze zmienianym kolorem
	//! \param color nowy kolor
	void setButtonColor(QPushButton* button, const QColor& color);
	//! blokuje sygnaly kontrolek modyfikuj¹cych trajektorie
	//! \param val czy zablokowaæ / odblokowaæ
	void blockAllSignals(bool val);

private:
    //! drawer z aktualnie wybranymi trajektoriami
	TrajectoryDrawerPtr currentTrajectories;
    //! mapa element drzewa -> drawer z trajektoriami
    std::map<QTreeWidgetItem*, TrajectoryDrawerPtr> item2Trajectories;
    //! mapa element drzewa -> drawer bêd¹cy parentem dla drawera z trajektoriami
    std::map<QTreeWidgetItem*, OsgSchemeDrawerPtr> item2Root;
};


#endif
