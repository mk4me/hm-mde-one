/*******************************************************************
    created:  2011/03/14
    created:  14:3:2011   16:49
    filename: IVisualizer.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IVISUALIZER_H__
#define HEADER_GUARD_CORE__IVISUALIZER_H__

#include <string>
#include <core/ObjectWrapper.h>
#include <core/IInputProcessItem.h>
#include <QtGui/QPixmap>

class QObject;
class QWidget;
class QIcon;

namespace core 
{
	class IActionsGroupManager;

    //! Interfejs wizualizatorów dla typów danych zarejestrowanych w aplikacji
    class IVisualizer :  public IInputProcessItem
    {
    public:

        //! Klasa bazowa do tworzenia i zarz¹dzania seriami danych
        class SerieBase
        {
        public:

            virtual ~SerieBase() {}

            //! \param name Nazwa serii danych do ustawienia
            virtual void setName(const std::string & name) = 0;

            //! \return Nazwa serii danych
            virtual const std::string & getName() const = 0;

            //! \param data Dane do ustawienia w serii danych. ObjecWrappery pozwalaj¹ nam unikn¹æ potrzeby generowania wielu metod dla ró¿nych argumentów.
            //! Znacz¹co uprasza interfejs, w przeciwnym wypadku musielibyœmy skorzystaæ z template
            virtual void setData(const ObjectWrapperConstPtr & data) = 0;

            //! \return Dane serii
            virtual const ObjectWrapperConstPtr & getData() const = 0;
        };

        class TimeSerieBase : public SerieBase
        {
        public:

            virtual ~TimeSerieBase() {}

            virtual void setTime(double time) = 0;

            virtual double getLength() const = 0;

            virtual void setOffset(double offset)
            {

            }

            virtual void setScale(double scale)
            {

            }
        };

    public:

        //! Pusty polimorficzny destruktor.
        virtual ~IVisualizer() {}

        //! Tylko tutaj powinno nastêpowaæ tworzenie widgetu. Metoda wywo³ywana tylko jeden raz.
        //! To wizualizator musi niszczyæ widget w destruktorze. Gdy widget jest równoczeœnie
        //! widokiem OSG to wystarczy przypisaæ go do osg::ref_ptr.
        //! \param action Akcje które nale¿y dodaæ do paska tytu³owego. Ka¿dym elementem mo¿e byæ
        //! albo QAction, QMenu z zagnied¿onymi akcjami, albo ca³y QWidget. Rodzice tych elementów nie s¹ zmieniani!
        //! \return Widget bêd¹cy korzeniem wizualizacji. Musi mieæ slot setActiveSource(int).
        virtual QWidget* createWidget(IActionsGroupManager * manager) = 0;

        //! Tworzy ikonê dla zadanego wizualizatora. Mo¿e zwracaæ nullptr, chocia¿ to niewskazane.
        //! W odgró¿nieniu od createWidget ikona przejmowana jest na w³asnoœæ.
        virtual QIcon* createIcon() = 0;

        virtual QPixmap print() const = 0;

        //! Aktualizacja wyœwietlania. NIE aktualizacja stanu wyœwietlanych danych.
        virtual void update(double deltaTime) = 0;

        //----------------- Obs³uga serii danych ---------------------
        //! Wizualizator musi zapewnic ze da siê stworzyæ MAX serii za pomoca metody createSerie. Ilos cserii jest dynamicznie zarz¹dzana z zewn¹trz poprzez create i remove serie.
        //! SerieBase to klasa delegata, która implementuje specyficzne dla danego wizualizatora operacje ustawiania nazwy serii i jej danych. Kazdy wizualizator mo¿e inaczej ustawiac te informacje i prezentowaæ je.

        //! \return Maksymalna iloœæ serii danych jaka mo¿e obs³u¿yc wizualizator, wartoœæ 0 i mniej oznacza maksumalny zakres dla INT, inna wartoœæ stanowi gorna granice
        virtual int getMaxDataSeries() const = 0;

        //! \return Seria danych która mo¿na ustawiac - nazwa i dane, nie zarz¹dza ta seria danych - czasem jej zycia, my zwalniamy jej zasoby!!
        virtual SerieBase* createSerie(const ObjectWrapperConstPtr & data, const std::string & name = std::string()) = 0;

        virtual SerieBase* createSerie(const SerieBase* serie) = 0;

        //! \param serie Seria danych do usuniêcia, nie powinien usuwaæ tej serii! Zarz¹dzamy ni¹ my!!
        virtual void removeSerie(SerieBase* serie) = 0;
    };

    typedef shared_ptr<IVisualizer> IVisualizerPtr;
    typedef shared_ptr<const IVisualizer> IVisualizerConstPtr;
    typedef weak_ptr<IVisualizer> IVisualizerWeakPtr;
    typedef weak_ptr<const IVisualizer> IVisualizerConstWeakPtr;

    typedef shared_ptr<IVisualizer::SerieBase> VisualizerSeriePtr;
    typedef shared_ptr<const IVisualizer::SerieBase> VisualizerSerieConstPtr;

    typedef shared_ptr<IVisualizer::TimeSerieBase> VisualizerTimeSeriePtr;
    typedef shared_ptr<const IVisualizer::TimeSerieBase> VisualizerTimeSerieConstPtr;
    typedef weak_ptr<IVisualizer::TimeSerieBase> VisualizerTimeSerieWeakPtr;
    typedef weak_ptr<const IVisualizer::TimeSerieBase> VisualizerTimeSerieConstWeakPtr;

} // namespace core


#endif  // HEADER_GUARD_CORE__IVISUALIZER_H__