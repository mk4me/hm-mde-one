/********************************************************************
	created:	2011/10/05
	created:	5:10:2011   10:22
	filename: 	FilterEntryWidget.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_HMM__FILTERENTRYWIDGET_H__
#define HEADER_GUARD_HMM__FILTERENTRYWIDGET_H__

#include <boost/function.hpp>
#include <QtGui/QWidget>
#include <osg/Vec3>
#include "FilterCommand.h"
#include "ui_filterEntry.h"

class HmmMainWindow;

//! widget z widokiem dla filtru
class FilterEntryWidget : public QWidget, private Ui::FilterEntry
{
    Q_OBJECT;
public:
    //! konstruktor
    //! \param parent obiekt rodzica
    //! \param hmm g³ówne okno aplikacji, dostarcza drzewa
    //! \param bigLabelText tekst z du¿¹ etykiet¹
    //! \param filterCommand akcja dokonuj¹ca w³aœciwego filtrowania
    //! \param icon ikona reprezentuj¹ca filtr
    FilterEntryWidget(QWidget* parent, HmmMainWindow* hmm, const QString& bigLabelText, IFilterCommandPtr filterCommand, const QPixmap* icon = nullptr);
	//! destruktor
	virtual ~FilterEntryWidget();

public:
    //! \return akcja dokonuj¹ca w³aœciwego filtrowania
    IFilterCommandPtr getFilterCommand() const { return filterCommand; }
    //! wyzwala akcjê filtruj¹c¹ i tworzy element drzewa
    //! \return gotowy element drzewa, z przefiltrowanymi danymi
    QTreeWidgetItem* createTreeEntry(const std::vector<PluginSubject::SessionConstPtr>& sessions);
    //! \return konfigurator filtru lub nullptr jeœli takiego nie dostarczono
    QWidget* getConfigurator() const { return filterCommand->getConfigurationWidget(); }
    //! \return nazwa filtru
    QString getName() const { return pushButton->text(); }
    //! \return czy filtr jest aktywny?
    bool isChecked() const { return pushButton->isChecked(); }
    //! ustawia aktywnoœæ filtru
    //! \param val aktywuje / deaktuwuje filtr
    void setChecked(bool val) { pushButton->setChecked(val); }

signals:
    //! sygna³ informuj¹cy, ¿e filtr zosta³ klikniêty
    //! \param filter klikniêty filtr
    void onFilterClicked(FilterEntryWidget* filter);

private slots:
    //! wywo³ywane po wciœniêciu przycisku filtra
    void onButton();

private:
    //! akcja dokonuj¹ca w³aœciwego filtrowania
    IFilterCommandPtr filterCommand;
    //! g³ówne okno aplikacji, dostarcza drzewa
    HmmMainWindow* hmm;
    //! obiekt z konfiguratorem dla filtra
    QWidget* configurator;
};


typedef boost::shared_ptr<FilterEntryWidget> FilterEntryWidgetPtr;
typedef boost::shared_ptr<const FilterEntryWidget> FilterEntryWidgetConstPtr;


#endif
