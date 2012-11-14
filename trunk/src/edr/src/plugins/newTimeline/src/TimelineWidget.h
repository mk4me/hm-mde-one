/********************************************************************
    created:  2011/08/02
    created:  2:8:2011   11:30
    filename: TimelineWidget.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___TIMELINEWIDGET_H__
#define HEADER_GUARD___TIMELINEWIDGET_H__

#include <QtGui/QWidget>
#include <QtGui/QMenu>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QSlider>
#include <QtGui/QComboBox>
#include <QtGui/QTimeEdit>
#include <QtGui/QTabBar>
#include <QtGui/QToolBar>
#include <QtGui/QTabWidget>
#include <QtGui/QLabel>
#include <QtGui/QAction>
#include <QtCore/QTime>
#include "ui_TimelineWidget.h"

#include <timelinelib/View.h>
#include <timelinelib/Model.h>

#include "TimelineService.h"
#include <plugins/newTimeline/TimeSliderWidget.h>
#include <plugins/newTimeline/TimelineControlsWidget.h>
#include "ChannelsTreeItem.h"


class TimelineWidget : public QWidget, public Ui::TimelineWidget, public timeline::View
{
    Q_OBJECT;

public:
    TimelineWidget(const timeline::ControllerPtr & controller, QWidget * parent = nullptr, Qt::WindowFlags f = 0);

    ~TimelineWidget();

    virtual void update(const State * state);

    void loadToolbarElements(core::IActionsGroupManager * actionsManager) const;

    void setChannelTooltip(const std::string & path, const std::string & tooltip);
    std::string getChannelTooltip(const std::string & path) const;

    //!
    void setOnChannelClick(const TimelineService::UIChannelAction & action);
    //!
    const TimelineService::UIChannelAction & getOnChannelClick() const;
    //!
    void setOnChannelDblClick(const TimelineService::UIChannelAction & action);
    //!
    const TimelineService::UIChannelAction & getOnChannelDblClick() const;

    void setOnChannelRemove(const TimelineService::UIChannelAction & action);
    //!
    const TimelineService::UIChannelAction & getOnChannelRemove() const;

public slots:
    //! \param time Nowy czas dla timeline podany przez użytkownika w sekundach
    //void setTime(double time);

signals:

    //! Informuje o zmianie długości timeline
    void lengthChanged(double length);
    //! Informuje o zmianie aktualnego czasu timeline
    void timeChanged(double time);
    //! Informuje o zmianie Offsetu Timeline
    void offsetChanged(double offset);

private slots:
    void onItemClicked(QTreeWidgetItem * item, int column);
    void onItemDblClicked(QTreeWidgetItem * item, int column);

    //! \param pnt Pozycja zapytania o menu kotnekstowe dla drzewa kanałów
    void showChannelsTreeContextMenu(const QPoint& pnt);
    //! \param value Nowa wartość slidera kontrolującego czas w wersji znormalizowanej
    void timeSliderChanged(int value);
    //! \param time Nowa wartość czasu
    void timeSliderChanged(double time);
    //! \brief Usuwa zaznacznoe w drzewie kanały
    void removeSelectedChannels();
    //! \param Nowy czas dla timeline
    void timeChanged(const QTime & time);
    //! \param scale Nowa skala dla timeline
    void scaleChanged(double scale);
    //! \param backward Czy mamy odtwarzac timeline do tyłu
    void playbackDirectionChanged(bool backward);
    //! \param pauseTimeline Czy wstrzymujemy timeline
    void pause(bool pauseTimeline);
    //! \brief Cofa czas do początku
    void toBegin();
    //! \brief Ustawia czas na końcu
    void toEnd();
    //! \brief Zatrzymuje timeline i wraca do początku
    void stop();
    //! \param state Stan checkboxa związanego z danym kanałem
    void channelsActivityChanged(int state);
    //! \brief Właściwa metoda update
    void refresh();

private:

    //! \brief Po dodaniu/usunięciu kanału odświeża chierarchię kanałów w drzewie timeline
    void refreshChannelsHierarchy();
    //! \brief Odświeża kanał, tagi, marker aktualnego czasu
    void refreshChannels();
    //! \brief Odświeża ustawienia odtwarzacza timeline - skalę, kierunek, aktualny czas
    void refreshPlayerStatus();

    static void compareNodes(const std::set<QTreeWidgetItem*> & uiNodes, const std::set<timeline::Model::TChannelConstPtr> & modelNodes,
        std::set<QTreeWidgetItem*> & toDeleteUINodes, std::vector<timeline::Model::TChannelConstPtr> & missingModelNodes);

    void recursiveHierarchyRefresh(QTreeWidgetItem* uiNode);

    void recursiveRefreshChannels(QTreeWidgetItem* uiNode);

    static QWidget * createTreeItemWidget(QWidget * widget);

private:

    //! Wpis w drzewie będący jego najwyższym poziomem - zawiera slider z osią czasu
    QTreeWidgetItem * rootItem;
    //! Menu kontekstowe do usuwania zaznaczonych kanałów
    QMenu * removeChannelsMenu;
    ////! Labelka opisująca kontrolkę zmiany skali
    //QLabel * scaleLabel;
    //! Kontrolka do zmiany skali timeline
    //QDoubleSpinBox * scaleSpinBox;
    ////! Labelka opisująca kontrolkę zmiany kierunku odtwarzania
    //QLabel * directionLabel;
    ////! Labelka opisująca kontrolkę zmiany czasu w formie tekstowej i spinnera
    //QLabel * timeLabel;
    //! Akcja do zmiany kierunku odtwarzania
    QAction * playbackDirectionAction;
    //! Akcja ustawiająca czas na początek timeline
    QAction * timeToBeginAction;
    //! Akcja ustawiająca czas na koniec timeline
    QAction * timeToEndAction;
    //! Akcja zarządzająca odtwarzaniem/wstrzymywaniem timeline
    QAction * playPauseAction;
    //! Akcja zatrzymująca timeline
    QAction * stopAction;
    //! Widget do precyzyjnego ustawiania czasu
    QDateTimeEdit * preciseTimeWidget;
    //! Kontener na akcje kontrolujące timeline
    QWidget * timelineTabs;
    //! lewy kontener na kontrolki timeline - kierunek i precyzyjny czas
    QToolBar * leftTabButton;
    //! środkowy kontener na kontrolki timeline - do początku, odtwarzaj/pauza, stop, do końca
    QToolBar * middleTabButton;
    //! prawy kontener na kontrolki timeline - aktualnie pusty
    QToolBar * rightTabButton;


    //! Nowy slider z osią czasu
    TimeSliderWidget * slider;

    std::map<timeline::Model::TChannelConstPtr, ChannelsTreeItem*> modelToUIChannels;

    TimelineService::UIChannelAction onItemClickAction;
    TimelineService::UIChannelAction onItemDblClickAction;
    TimelineService::UIChannelAction onItemRemoveAction;
};

#endif  //  HEADER_GUARD___TIMELINEWIDGET_H__
