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

#include <plugins/newTimeline/TimelineService.h>
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
    //! \param time Nowy czas dla timeline podany przez u�ytkownika w sekundach
    //void setTime(double time);

signals:

    //! Informuje o zmianie d�ugo�ci timeline
    void lengthChanged(double length);
    //! Informuje o zmianie aktualnego czasu timeline
    void timeChanged(double time);
    //! Informuje o zmianie Offsetu Timeline
    void offsetChanged(double offset);

private slots:
    void onItemClicked(QTreeWidgetItem * item, int column);
    void onItemDblClicked(QTreeWidgetItem * item, int column);

    //! \param pnt Pozycja zapytania o menu kotnekstowe dla drzewa kana��w
    void showChannelsTreeContextMenu(const QPoint& pnt);
    //! \param value Nowa warto�� slidera kontroluj�cego czas w wersji znormalizowanej
    void timeSliderChanged(int value);
    //! \param time Nowa warto�� czasu
    void timeSliderChanged(double time);
    //! \brief Usuwa zaznacznoe w drzewie kana�y
    void removeSelectedChannels();
    //! \param Nowy czas dla timeline
    void timeChanged(const QTime & time);
    //! \param scale Nowa skala dla timeline
    void scaleChanged(double scale);
    //! \param backward Czy mamy odtwarzac timeline do ty�u
    void playbackDirectionChanged(bool backward);
    //! \param pauseTimeline Czy wstrzymujemy timeline
    void pause(bool pauseTimeline);
    //! \brief Cofa czas do pocz�tku
    void toBegin();
    //! \brief Ustawia czas na ko�cu
    void toEnd();
    //! \brief Zatrzymuje timeline i wraca do pocz�tku
    void stop();
    //! \param state Stan checkboxa zwi�zanego z danym kana�em
    void channelsActivityChanged(int state);
    //! \brief W�a�ciwa metoda update
    void refresh();

private:

    //! \brief Po dodaniu/usuni�ciu kana�u od�wie�a chierarchi� kana��w w drzewie timeline
    void refreshChannelsHierarchy();
    //! \brief Od�wie�a kana�, tagi, marker aktualnego czasu
    void refreshChannels();
    //! \brief Od�wie�a ustawienia odtwarzacza timeline - skal�, kierunek, aktualny czas
    void refreshPlayerStatus();

    static void compareNodes(const std::set<QTreeWidgetItem*> & uiNodes, const std::set<timeline::Model::TChannelConstPtr> & modelNodes,
        std::set<QTreeWidgetItem*> & toDeleteUINodes, std::vector<timeline::Model::TChannelConstPtr> & missingModelNodes);

    void recursiveHierarchyRefresh(QTreeWidgetItem* uiNode);

    void recursiveRefreshChannels(QTreeWidgetItem* uiNode);

    static QWidget * createTreeItemWidget(QWidget * widget);

private:

    //! Wpis w drzewie b�d�cy jego najwy�szym poziomem - zawiera slider z osi� czasu
    QTreeWidgetItem * rootItem;
    //! Menu kontekstowe do usuwania zaznaczonych kana��w
    QMenu * removeChannelsMenu;
    ////! Labelka opisuj�ca kontrolk� zmiany skali
    //QLabel * scaleLabel;
    //! Kontrolka do zmiany skali timeline
    //QDoubleSpinBox * scaleSpinBox;
    ////! Labelka opisuj�ca kontrolk� zmiany kierunku odtwarzania
    //QLabel * directionLabel;
    ////! Labelka opisuj�ca kontrolk� zmiany czasu w formie tekstowej i spinnera
    //QLabel * timeLabel;
    //! Akcja do zmiany kierunku odtwarzania
    QAction * playbackDirectionAction;
    //! Akcja ustawiaj�ca czas na pocz�tek timeline
    QAction * timeToBeginAction;
    //! Akcja ustawiaj�ca czas na koniec timeline
    QAction * timeToEndAction;
    //! Akcja zarz�dzaj�ca odtwarzaniem/wstrzymywaniem timeline
    QAction * playPauseAction;
    //! Akcja zatrzymuj�ca timeline
    QAction * stopAction;
    //! Widget do precyzyjnego ustawiania czasu
    QDateTimeEdit * preciseTimeWidget;
    //! Kontener na akcje kontroluj�ce timeline
    QWidget * timelineTabs;
    //! lewy kontener na kontrolki timeline - kierunek i precyzyjny czas
    QToolBar * leftTabButton;
    //! �rodkowy kontener na kontrolki timeline - do pocz�tku, odtwarzaj/pauza, stop, do ko�ca
    QToolBar * middleTabButton;
    //! prawy kontener na kontrolki timeline - aktualnie pusty
    QToolBar * rightTabButton;


    //! Nowy slider z osi� czasu
    TimeSliderWidget * slider;

    std::map<timeline::Model::TChannelConstPtr, ChannelsTreeItem*> modelToUIChannels;

    TimelineService::UIChannelAction onItemClickAction;
    TimelineService::UIChannelAction onItemDblClickAction;
    TimelineService::UIChannelAction onItemRemoveAction;
};

#endif  //  HEADER_GUARD___TIMELINEWIDGET_H__