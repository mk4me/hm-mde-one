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

//! Dialog Qt s�u��cy do zarz�dzania trajektoriami marker�w
class TrajectoriesDialog : public QDialog, private Ui::TrajectoriesDialog
{
	Q_OBJECT;
public:
	//! Kontruktor widgeta
	//! \param parent parent dialogu
	TrajectoriesDialog(QWidget* parent);

public:
	//! Dodanie do dialogu drawera, kt�ry b�dzie modyfikowany
	//! \param drawer obiekt drawera osg
	//! \param rootName wy�wietlana nazwa
	void setDrawer(OsgSchemeDrawerPtr drawer, const QString& rootName);

private slots:
	//! zmieniono kolor
	void colorClicked();
	//! zmieniono widoczno�� pojedynczego w�z�a
	//! \param visible czy widoczny
	void visibilityChanged(bool visible);
    //! zmieniono widoczno�� roota (dzia�a na ca�o��)
    //! \param visible czy widoczny
    void rootVisibilityChanged(bool visible);
	//! zmieniono szeroko�� trajektorii
	//! \param width nowa szeroko��
	void widthChanged(double width);
	//! zmieniony zosta� poczaktowy czas, dla kt�rego trajektoria jest widoczna
	//! \param time ustawiwany czas
	void startTimeChanged(double time);
    //! zmieniony zosta� ko�cowy czas, dla kt�rego trajektoria jest widoczna
    //! \param time ustawiwany czas
	void endTimeChanged(double time);
	//! zmieni� si� aktywny element drzewa
	//! \param current aktualnie aktywny element
	//! \param previous poprzednio aktywny element
	void treeItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous );
	//! zmieni� si� slider reprezentuj�cy pocz�tkowy czas, dla kt�rego trajektoria jest widoczna
	//! \param value nowa warto�� pocz�tkowa slidra
    void startSliderChanged(int value);
    //! zmieni� si� slider reprezentuj�cy ko�cowy czas, dla kt�rego trajektoria jest widoczna
	//! \param value nowa warto�� ko�cowa slidera
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
	//! Metoda przechodzi przez drzewo i zwraca element, kt�ry zawiera w kt�rej� kolumnie podany obiekt
	//! \param object szukany obiekt
    //! \return element drzwa lub wyj�tek je�li takiego nie znaleziono
	QTreeWidgetItem* getItemWhichContains(QObject* object) const;
	//! Pomocnicza metoda, zmienia kolor t�a przycisku za pomoc� styli Qt
	//! \param button przycisk ze zmienianym kolorem
	//! \param color nowy kolor
	void setButtonColor(QPushButton* button, const QColor& color);
	//! blokuje sygnaly kontrolek modyfikuj�cych trajektorie
	//! \param val czy zablokowa� / odblokowa�
	void blockAllSignals(bool val);

private:
    //! drawer z aktualnie wybranymi trajektoriami
	TrajectoryDrawerPtr currentTrajectories;
    //! mapa element drzewa -> drawer z trajektoriami
    std::map<QTreeWidgetItem*, TrajectoryDrawerPtr> item2Trajectories;
    //! mapa element drzewa -> drawer b�d�cy parentem dla drawera z trajektoriami
    std::map<QTreeWidgetItem*, OsgSchemeDrawerPtr> item2Root;
};


#endif
