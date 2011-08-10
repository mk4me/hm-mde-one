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
#include <QtGui/QComboBox>
#include <QtGui/QTimeEdit>
#include <QtGui/QLabel>
#include <QtGui/QAction>
#include <QtCore/QTime>
#include "ui_TimelineWidget.h"

#include <timelinelib/View.h>
#include <timelinelib/Model.h>

class TimelineWidget : public QWidget, public Ui::TimelineWidget, public timeline::View
{
    Q_OBJECT;

public:
    TimelineWidget(const timeline::ControllerPtr & controller, QWidget * parent = nullptr, Qt::WindowFlags f = 0);

    ~TimelineWidget();

    virtual void update(const State * state);

    static QTime convertToQTime(double timeInSeconds);
    static double convertToTime(const QTime & time);

    void loadToolbarElements(std::vector<QObject*> & elements) const;

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

    //! \param pnt Pozycja zapytania o menu kotnekstowe dla drzewa kana³ów
    void showContextMenu(const QPoint& pnt);
    //! \param value Nowa wartoœæ slidera kontroluj¹cego czas w wersji znormalizowanej
    void timeSliderChanged(int value);
    //! \brief Usuwa zaznacznoe w drzewie kana³y
    void removeSelectedChannels();
    //! \param Nowy czas dla timeline
    void timeChanged(const QTime & time);
    //! \param scale Nowa skala dla timeline
    void scaleChanged(double scale);
    //! \param idx Indeks aktualnego kierunku odtwarzania - 0 przód, 1 ty³
    void playbackDirectionChanged(int idx);
    //! \param pauseTimeline Czy wstrzymujemy timeline
    void pause(bool pauseTimeline);
    //! \brief Cofa czas do poczatku
    void toBegin();
    //! \brief Ustawia czas na koñcu
    void toEnd();
    //! \brief Zatrzymuje timeline i wraca do poczatku
    void stop();
    //! \param state Stan checkboxa zwi¹zanego z danym kana³em
    void channelsActivityChanged(int state);
    //! \brief W³aœciwa metoda update
    void update();

private:

    //! \brief Po dodaniu/usuniêciu kana³u odœwie¿a chierarchiê kana³ów w drzewie timeline
    void refreshChannelsHierarchy();
    //! \brief Odœwie¿a marker czasu w kana³ach po wiêkszoœci zmian
    void refreshTimeInChannels();
    //! \brief Odœwie¿a oœ czasu po zmianie offsetu lub skali (czasu trwania) roota (ca³ego timeline)
    void refreshTimeAxis();
    //! \brief Po zmianie offsetu lub skali odœwie¿a strukturê czasow¹ kana³ów
    void refreshChannelsTimeStructure();
    //! \brief Odœwie¿a ustawienia odtwarzacza timeline - skalê, kierunek, aktualny czas
    void refreshPlayerStatus();

    static void compareNodes(const std::set<QTreeWidgetItem*> & uiNodes, const std::set<timeline::Model::TChannelConstPtr> & modelNodes,
        std::set<QTreeWidgetItem*> & toDeleteUINodes, std::vector<timeline::Model::TChannelConstPtr> & missingModelNodes);

    void recursiveHierarchyRefresh(QTreeWidgetItem* uiNode);

    void resursiveChannelsTimeStructureRefresh(QTreeWidgetItem* uiNode, double globalOffset, double globalLenght);

    void resursiveTimeInChannelsRefresh(QTreeWidgetItem* uiNode, double globalOffset, double globalLenght);

private:

    //! Menu kontekstowe do usuwania zaznaczonych kana³ów
    QMenu * removeChannelsMenu;
    //! Labelka opisuj¹ca kontrolkê zmiany skali
    QLabel * scaleLabel;
    //! Kontrolka do zmiany skali timeline
    QDoubleSpinBox * scaleSpinBox;
    //! Labelka opisuj¹ca kontrolkê zmiany kierunku odtwarzania
    QLabel * directionLabel;
    //! Kontrolka do zmiany kierunku odtwarzania
    QComboBox * directionComboBox;
    //! Labelka opisuj¹ca kontrolkê zmiany czasu w formie tekstowej i spinnera
    QLabel * timeLabel;
    //! Kontrolka do zmiany czasu w formie tekstowej i spinnera
    QTimeEdit * timeEditBox;
    //! Akcja ustawiaj¹ca czas na pocz¹tek timeline
    QAction * timeToBeginAction;
    //! Akcja ustawiaj¹ca czas na koniec timeline
    QAction * timeToEndAction;
    //! Akcja zarz¹dzaj¹ca odtwarzaniem/wstrzymywaniem timeline
    QAction * playPauseAction;
    //! Akcja zatrzymuj¹ca timeline
    QAction * stopAction;
};

#endif  //  HEADER_GUARD___TIMELINEWIDGET_H__