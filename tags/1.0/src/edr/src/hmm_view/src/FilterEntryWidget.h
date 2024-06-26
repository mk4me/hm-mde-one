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
    //! \param hmm główne okno aplikacji, dostarcza drzewa
    //! \param bigLabelText tekst z dużą etykietą
    //! \param filterCommand akcja dokonująca właściwego filtrowania
    //! \param icon ikona reprezentująca filtr
    FilterEntryWidget(QWidget* parent, HmmMainWindow* hmm, const QString& bigLabelText, IFilterCommandPtr filterCommand, const QPixmap* icon = nullptr);
	//! destruktor
	virtual ~FilterEntryWidget();

public:
    //! \return akcja dokonująca właściwego filtrowania
    IFilterCommandPtr getFilterCommand() const { return filterCommand; }
    //! wyzwala akcję filtrującą i tworzy element drzewa
    //! \return gotowy element drzewa, z przefiltrowanymi danymi
    QTreeWidgetItem* createTreeEntry(const core::ConstObjectsList& sessions);
    //! \return konfigurator filtru lub nullptr jeśli takiego nie dostarczono
    QWidget* getConfigurator() const { return filterCommand->getConfigurationWidget(); }
    //! \return nazwa filtru
    QString getName() const { return pushButton->text(); }
    //! \return czy filtr jest aktywny?
    bool isChecked() const { return pushButton->isChecked(); }
    //! ustawia aktywność filtru
    //! \param val aktywuje / deaktuwuje filtr
    void setChecked(bool val) { pushButton->setChecked(val); }

Q_SIGNALS:
    //! sygnał informujący, że filtr został kliknięty
    //! \param filter kliknięty filtr
    void onFilterClicked(FilterEntryWidget* filter);

private Q_SLOTS:
    //! wywoływane po wciśnięciu przycisku filtra
    void onButton();

private:
    //! akcja dokonująca właściwego filtrowania
    IFilterCommandPtr filterCommand;
    //! główne okno aplikacji, dostarcza drzewa
    HmmMainWindow* hmm;
    //! obiekt z konfiguratorem dla filtra
    QWidget* configurator;
};


typedef boost::shared_ptr<FilterEntryWidget> FilterEntryWidgetPtr;
typedef boost::shared_ptr<const FilterEntryWidget> FilterEntryWidgetConstPtr;


#endif
