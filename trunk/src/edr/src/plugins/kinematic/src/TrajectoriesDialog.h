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

#include <QtGui/QWidget>

class QTreeWidget;
class QTreeWidgetItem;


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

	explicit SchemeParametersEditor(QWidget * parent = nullptr, Qt::WindowFlags f = 0);
	virtual ~SchemeParametersEditor();

	void initialize(TreeSchemeElement * root);	

signals:

	void currentSchemeElementChanged(TreeSchemeElement * previous, TreeSchemeElement * current);

private slots:

	void onCurrentItemChanged( QTreeWidgetItem * current, QTreeWidgetItem * previous );
	void onItemChanged(QTreeWidgetItem * current, int column);

private:	
	QTreeWidget * treeWidget;

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

#endif
