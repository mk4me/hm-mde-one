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

//! Dialog Qt sluzacy do zarzadzania trajektoriami markerow
class TrajectoriesDialog : public QDialog, private Ui::TrajectoriesDialog
{
	Q_OBJECT;
public:
	//! Kontruktor widgeta
	//! \param parent parent dialogu
	TrajectoriesDialog(QWidget* parent);

public:
	//! Dodanie do dialogu drawera, ktory bedzie modyfikowany
	//! \param drawer obiekt drawera osg
	//! \param rootName wyswietlana nazwa
	void setDrawer(OsgSchemeDrawerPtr drawer, const QString& rootName);

private slots:
	//! zmieniono kolor
	void colorClicked();
	//! zmieniono vidocznosc pojedynczego wezla
	//! \param visible czy widoczny
	void visibilityChanged(bool visible);
    //! zmieniono widocznosc roota (dziala na calosc)
    //! \param visible czy widoczny
    void rootVisibilityChanged(bool visible);
	//! zmieniono szerokosc trajektorii
	//! \param width nowa szerokosc
	void widthChanged(double width);
	//! zmieniony zostal poczaktowy czas, dla ktorej trajektoria jest widoczna
	//! \param time ustawiwany czas
	void startTimeChanged(double time);
    //! zmieniony zostal koncowy czas, dla ktorej trajektoria jest widoczna
    //! \param time ustawiwany czas
	void endTimeChanged(double time);
	//! Zmienil sie aktywny element drzewa
	//! \param current 
	//! \param previous 
	void treeItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous );
	//! zmienil sie slider reprezentujacy poczatkowy czas, dla ktorej trajektoria jest widoczna
	//! \param value 
    void startSliderChanged(int value);
    //! zmienil sie slider reprezentujacy koncowy czas, dla ktorej trajektoria jest widoczna
	//! \param value 
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
    //! drawer z aktualnie wybranymi trajektoriami
	TrajectoryDrawerPtr currentTrajectories;
    //! mapa element drzewa -> drawer z trajektoriami
    std::map<QTreeWidgetItem*, TrajectoryDrawerPtr> item2Trajectories;
    //! mapa element drzewa -> drawer bedacy parentem dla drawera z trajektoriami
    std::map<QTreeWidgetItem*, OsgSchemeDrawerPtr> item2Root;
};


#endif
