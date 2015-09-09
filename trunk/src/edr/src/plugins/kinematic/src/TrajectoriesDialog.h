/********************************************************************
	created:	2011/09/06
	created:	6:9:2011   12:49
	filename: 	TrajectoriesDialog.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_TRAJECTORIES_DIALOG__TRAJECTORIESDIALOG_H__
#define HEADER_GUARD_TRAJECTORIES_DIALOG__TRAJECTORIESDIALOG_H__

#include <corelib/IVisualizer.h>

#include <QtWidgets/QWidget>

class QTreeWidget;
class QTreeWidgetItem;
class KinematicVisualizer;

template<class T>
class ITTreeElement : public T
{
public:
	virtual ~ITTreeElement() {}

	virtual const std::vector<ITTreeElement*> children() const = 0;
};


class SchemeParametersEditor : public QWidget
{
	Q_OBJECT

public:

	class ISchemeElement
	{
	public:
		virtual ~ISchemeElement() {}

		virtual const QString name() const = 0;
		virtual const QColor color() const = 0;
		virtual const bool visible() const = 0;
		virtual const float size() const = 0;

		virtual void setColor(const QColor & color) = 0;
		virtual void setVisible(const bool visible) = 0;
		virtual void setSize(const float size) = 0;		
	};

	typedef ITTreeElement<ISchemeElement> TreeSchemeElement;

public:

	explicit SchemeParametersEditor(KinematicVisualizer * visualizer, QWidget * parent = nullptr, Qt::WindowFlags f = 0);
	virtual ~SchemeParametersEditor();

	void initialize(TreeSchemeElement * root);	

signals:

	void currentSchemeElementChanged(TreeSchemeElement * previous, TreeSchemeElement * current);

private slots:

	void onCurrentItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous );
	void onItemChanged(QTreeWidgetItem * current, int column);

private:	
	QTreeWidget * treeWidget;
	KinematicVisualizer * visualizer;

};

/*
class RangeSchemeParametersEditor : public QWidget
{
	Q_OBJECT

public:

	class IRangeSchemeElement : public SchemeParametersEditor::ISchemeElement
	{
	public:

		typedef std::pair<unsigned int, unsigned int> Range;

	public:
		virtual ~IRangeSchemeElement() {}

		virtual const Range range() const = 0;

		virtual void setRange(const Range range) = 0;		
	};



public:

	explicit SchemeParametersEditor(QWidget * parent = nullptr, Qt::WindowFlags f = 0);
	virtual ~SchemeParametersEditor();

	void initialize(ISchemeElement * root);

	void applyChangesRecursively(const bool recursive);

signals:

	void currentSchemeElementChanged(ISchemeElement * previous, ISchemeElement * current);

	private slots:

		void onCurrentItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous );
		void onItemChanged(QTreeWidgetItem * current, int column);

private:
	bool recursiveUpdate;
	QTreeWidget * treeWidget;
};
*/

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
    TrajectoriesDialog(QWidget* parent, KinematicVisualizer * visualizer);

public:
    //! Dodanie do dialogu drawera, kt�ry b�dzie modyfikowany
    //! \param drawer obiekt drawera osg
    //! \param rootName wy�wietlana nazwa
    void setDrawer(TrajectoryDrawerManagerPtr drawer, const QString& rootName, const QStringList& names);

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
    //! blokuje sygna�y kontrolek modyfikuj�cych trajektorie
    //! \param val czy zablokowa� / odblokowa�
    void blockAllSignals(bool val);

private:
    //! drawer z aktualnie wybranymi trajektoriami
    //TrajectoryDrawerPtr currentTrajectories;
    //! mapa element drzewa -> drawer z trajektoriami
    std::map<QTreeWidgetItem*, int> item2Trajectories;
    //! mapa element drzewa -> drawer b�d�cy parentem dla drawera z trajektoriami
    std::map<QTreeWidgetItem*, TrajectoryDrawerManagerPtr> item2Root;

	KinematicVisualizer * visualizer;
};

#endif
