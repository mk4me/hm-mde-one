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

//! Dialog Qt służący do zarządzania trajektoriami markerów
class TrajectoriesDialog : public QDialog, private Ui::TrajectoriesDialog
{
	Q_OBJECT;
public:
	//! Kontruktor widgeta
	//! \param parent parent dialogu
	TrajectoriesDialog(QWidget* parent);

public:
	//! Dodanie do dialogu drawera, który będzie modyfikowany
	//! \param drawer obiekt drawera osg
	//! \param rootName wyświetlana nazwa
	void setDrawer(OsgSchemeDrawerPtr drawer, const QString& rootName);

private slots:
	//! zmieniono kolor
	void colorClicked();
	//! zmieniono widoczność pojedynczego węzła
	//! \param visible czy widoczny
	void visibilityChanged(bool visible);
    //! zmieniono widoczność roota (działa na całość)
    //! \param visible czy widoczny
    void rootVisibilityChanged(bool visible);
	//! zmieniono szerokość trajektorii
	//! \param width nowa szerokość
	void widthChanged(double width);
	//! zmieniony został poczaktowy czas, dla którego trajektoria jest widoczna
	//! \param time ustawiwany czas
	void startTimeChanged(double time);
    //! zmieniony został końcowy czas, dla którego trajektoria jest widoczna
    //! \param time ustawiwany czas
	void endTimeChanged(double time);
	//! zmienił się aktywny element drzewa
	//! \param current aktualnie aktywny element
	//! \param previous poprzednio aktywny element
	void treeItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous );
	//! zmienił się slider reprezentujący początkowy czas, dla którego trajektoria jest widoczna
	//! \param value nowa wartość początkowa slidra
    void startSliderChanged(int value);
    //! zmienił się slider reprezentujący końcowy czas, dla którego trajektoria jest widoczna
	//! \param value nowa wartość końcowa slidera
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
	//! Metoda przechodzi przez drzewo i zwraca element, który zawiera w którejś kolumnie podany obiekt
	//! \param object szukany obiekt
    //! \return element drzwa lub wyjątek jeśli takiego nie znaleziono
	QTreeWidgetItem* getItemWhichContains(QObject* object) const;
	//! Pomocnicza metoda, zmienia kolor tła przycisku za pomocą styli Qt
	//! \param button przycisk ze zmienianym kolorem
	//! \param color nowy kolor
	void setButtonColor(QPushButton* button, const QColor& color);
	//! blokuje sygnały kontrolek modyfikujących trajektorie
	//! \param val czy zablokować / odblokować
	void blockAllSignals(bool val);

private:
    //! drawer z aktualnie wybranymi trajektoriami
	TrajectoryDrawerPtr currentTrajectories;
    //! mapa element drzewa -> drawer z trajektoriami
    std::map<QTreeWidgetItem*, TrajectoryDrawerPtr> item2Trajectories;
    //! mapa element drzewa -> drawer będący parentem dla drawera z trajektoriami
    std::map<QTreeWidgetItem*, OsgSchemeDrawerPtr> item2Root;
};


#endif
