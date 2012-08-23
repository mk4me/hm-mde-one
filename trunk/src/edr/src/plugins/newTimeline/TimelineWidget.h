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
    //! \param time Nowy czas dla timeline podany przez u¿ytkownika w sekundach
    //void setTime(double time);

signals:

    //! Informuje o zmianie d³ugoœci timeline
    void lengthChanged(double length);
    //! Informuje o zmianie aktualnego czasu timeline
    void timeChanged(double time);
    //! Informuje o zmianie Offsetu Timeline
    void offsetChanged(double offset);

private slots:
    void onItemClicked(QTreeWidgetItem * item, int column);
    void onItemDblClicked(QTreeWidgetItem * item, int column);

    //! \param pnt Pozycja zapytania o menu kotnekstowe dla drzewa kana³ów
    void showChannelsTreeContextMenu(const QPoint& pnt);
    //! \param value Nowa wartoœæ slidera kontroluj¹cego czas w wersji znormalizowanej
    void timeSliderChanged(int value);
    //! \param time Nowa wartoœæ czasu
    void timeSliderChanged(double time);
    //! \brief Usuwa zaznacznoe w drzewie kana³y
    void removeSelectedChannels();
    //! \param Nowy czas dla timeline
    void timeChanged(const QTime & time);
    //! \param scale Nowa skala dla timeline
    void scaleChanged(double scale);
    //! \param backward Czy mamy odtwarzac timeline do ty³u
    void playbackDirectionChanged(bool backward);
    //! \param pauseTimeline Czy wstrzymujemy timeline
    void pause(bool pauseTimeline);
    //! \brief Cofa czas do pocz¹tku
    void toBegin();
    //! \brief Ustawia czas na koñcu
    void toEnd();
    //! \brief Zatrzymuje timeline i wraca do pocz¹tku
    void stop();
    //! \param state Stan checkboxa zwi¹zanego z danym kana³em
    void channelsActivityChanged(int state);
    //! \brief W³aœciwa metoda update
    void refresh();

private:

    //! \brief Po dodaniu/usuniêciu kana³u odœwie¿a chierarchiê kana³ów w drzewie timeline
    void refreshChannelsHierarchy();
    //! \brief Odœwie¿a kana³, tagi, marker aktualnego czasu
    void refreshChannels();
    //! \brief Odœwie¿a ustawienia odtwarzacza timeline - skalê, kierunek, aktualny czas
    void refreshPlayerStatus();

    static void compareNodes(const std::set<QTreeWidgetItem*> & uiNodes, const std::set<timeline::Model::TChannelConstPtr> & modelNodes,
        std::set<QTreeWidgetItem*> & toDeleteUINodes, std::vector<timeline::Model::TChannelConstPtr> & missingModelNodes);

    void recursiveHierarchyRefresh(QTreeWidgetItem* uiNode);

    void recursiveRefreshChannels(QTreeWidgetItem* uiNode);

    static QWidget * createTreeItemWidget(QWidget * widget);

private:

    //! Wpis w drzewie bêd¹cy jego najwy¿szym poziomem - zawiera slider z osi¹ czasu
    QTreeWidgetItem * rootItem;
    //! Menu kontekstowe do usuwania zaznaczonych kana³ów
    QMenu * removeChannelsMenu;
    ////! Labelka opisuj¹ca kontrolkê zmiany skali
    //QLabel * scaleLabel;
    //! Kontrolka do zmiany skali timeline
    //QDoubleSpinBox * scaleSpinBox;
    ////! Labelka opisuj¹ca kontrolkê zmiany kierunku odtwarzania
    //QLabel * directionLabel;
    ////! Labelka opisuj¹ca kontrolkê zmiany czasu w formie tekstowej i spinnera
    //QLabel * timeLabel;
    //! Akcja do zmiany kierunku odtwarzania
    QAction * playbackDirectionAction;
    //! Akcja ustawiaj¹ca czas na pocz¹tek timeline
    QAction * timeToBeginAction;
    //! Akcja ustawiaj¹ca czas na koniec timeline
    QAction * timeToEndAction;
    //! Akcja zarz¹dzaj¹ca odtwarzaniem/wstrzymywaniem timeline
    QAction * playPauseAction;
    //! Akcja zatrzymuj¹ca timeline
    QAction * stopAction;
    //! Widget do precyzyjnego ustawiania czasu
    QDateTimeEdit * preciseTimeWidget;
    //! Kontener na akcje kontroluj¹ce timeline
    QWidget * timelineTabs;
    //! lewy kontener na kontrolki timeline - kierunek i precyzyjny czas
    QToolBar * leftTabButton;
    //! œrodkowy kontener na kontrolki timeline - do pocz¹tku, odtwarzaj/pauza, stop, do koñca
    QToolBar * middleTabButton;
    //! prawy kontener na kontrolki timeline - aktualnie pusty
    QToolBar * rightTabButton;


    //! Nowy slider z osi¹ czasu
    TimeSliderWidget * slider;

    std::map<timeline::Model::TChannelConstPtr, ChannelsTreeItem*> modelToUIChannels;

    TimelineService::UIChannelAction onItemClickAction;
    TimelineService::UIChannelAction onItemDblClickAction;
    TimelineService::UIChannelAction onItemRemoveAction;
};

#endif  //  HEADER_GUARD___TIMELINEWIDGET_H__