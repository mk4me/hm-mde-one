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

    //! \param pnt Pozycja zapytania o menu kotnekstowe dla drzewa kana��w
    void showContextMenu(const QPoint& pnt);
    //! \param value Nowa warto�� slidera kontroluj�cego czas w wersji znormalizowanej
    void timeSliderChanged(int value);
    //! \brief Usuwa zaznacznoe w drzewie kana�y
    void removeSelectedChannels();
    //! \param Nowy czas dla timeline
    void timeChanged(const QTime & time);
    //! \param scale Nowa skala dla timeline
    void scaleChanged(double scale);
    //! \param idx Indeks aktualnego kierunku odtwarzania - 0 prz�d, 1 ty�
    void playbackDirectionChanged(int idx);
    //! \param pauseTimeline Czy wstrzymujemy timeline
    void pause(bool pauseTimeline);
    //! \brief Cofa czas do poczatku
    void toBegin();
    //! \brief Ustawia czas na ko�cu
    void toEnd();
    //! \brief Zatrzymuje timeline i wraca do poczatku
    void stop();
    //! \param state Stan checkboxa zwi�zanego z danym kana�em
    void channelsActivityChanged(int state);
    //! \brief W�a�ciwa metoda update
    void update();

private:

    //! \brief Po dodaniu/usuni�ciu kana�u od�wie�a chierarchi� kana��w w drzewie timeline
    void refreshChannelsHierarchy();
    //! \brief Od�wie�a marker czasu w kana�ach po wi�kszo�ci zmian
    void refreshTimeInChannels();
    //! \brief Od�wie�a o� czasu po zmianie offsetu lub skali (czasu trwania) roota (ca�ego timeline)
    void refreshTimeAxis();
    //! \brief Po zmianie offsetu lub skali od�wie�a struktur� czasow� kana��w
    void refreshChannelsTimeStructure();
    //! \brief Od�wie�a ustawienia odtwarzacza timeline - skal�, kierunek, aktualny czas
    void refreshPlayerStatus();

    static void compareNodes(const std::set<QTreeWidgetItem*> & uiNodes, const std::set<timeline::Model::TChannelConstPtr> & modelNodes,
        std::set<QTreeWidgetItem*> & toDeleteUINodes, std::vector<timeline::Model::TChannelConstPtr> & missingModelNodes);

    void recursiveHierarchyRefresh(QTreeWidgetItem* uiNode);

    void resursiveChannelsTimeStructureRefresh(QTreeWidgetItem* uiNode, double globalOffset, double globalLenght);

    void resursiveTimeInChannelsRefresh(QTreeWidgetItem* uiNode, double globalOffset, double globalLenght);

private:

    //! Menu kontekstowe do usuwania zaznaczonych kana��w
    QMenu * removeChannelsMenu;
    //! Labelka opisuj�ca kontrolk� zmiany skali
    QLabel * scaleLabel;
    //! Kontrolka do zmiany skali timeline
    QDoubleSpinBox * scaleSpinBox;
    //! Labelka opisuj�ca kontrolk� zmiany kierunku odtwarzania
    QLabel * directionLabel;
    //! Kontrolka do zmiany kierunku odtwarzania
    QComboBox * directionComboBox;
    //! Labelka opisuj�ca kontrolk� zmiany czasu w formie tekstowej i spinnera
    QLabel * timeLabel;
    //! Kontrolka do zmiany czasu w formie tekstowej i spinnera
    QTimeEdit * timeEditBox;
    //! Akcja ustawiaj�ca czas na pocz�tek timeline
    QAction * timeToBeginAction;
    //! Akcja ustawiaj�ca czas na koniec timeline
    QAction * timeToEndAction;
    //! Akcja zarz�dzaj�ca odtwarzaniem/wstrzymywaniem timeline
    QAction * playPauseAction;
    //! Akcja zatrzymuj�ca timeline
    QAction * stopAction;
};

#endif  //  HEADER_GUARD___TIMELINEWIDGET_H__