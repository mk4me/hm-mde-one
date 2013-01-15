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
#include <core/IIdentifiable.h>
#include <core/IInputDescription.h>
#include <QtGui/QPixmap>

class QObject;
class QWidget;
class QIcon;

namespace plugin 
{
	class IActionsGroupManager;

    //! Interfejs wizualizatorów dla typów danych zarejestrowanych w aplikacji
    class IVisualizer :  public IIdentifiable, public IInputDescription
    {
    public:

        //! Klasa bazowa do tworzenia i zarządzania seriami danych
        class SerieBase
        {
        public:

            virtual ~SerieBase() {}

            //! \param name Nazwa serii danych do ustawienia
            virtual void setName(const std::string & name) = 0;

            //! \return Nazwa serii danych
            virtual const std::string & getName() const = 0;

            //! \param data Dane do ustawienia w serii danych. ObjecWrappery pozwalają nam uniknąć potrzeby generowania wielu metod dla różnych argumentów.
            //! Znacząco uprasza interfejs, w przeciwnym wypadku musielibyśmy skorzystać z template
            virtual void setData(const core::ObjectWrapperConstPtr & data) = 0;

            //! \return Dane serii
            virtual const core::ObjectWrapperConstPtr & getData() const = 0;
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

        //! Tylko tutaj powinno następować tworzenie widgetu. Metoda wywoływana tylko jeden raz.
        //! To wizualizator musi niszczyć widget w destruktorze. Gdy widget jest równocześnie
        //! widokiem OSG to wystarczy przypisać go do osg::ref_ptr.
        //! \param action Akcje które należy dodać do paska tytułowego. Każdym elementem może być
        //! albo QAction, QMenu z zagniedżonymi akcjami, albo cały QWidget. Rodzice tych elementów nie są zmieniani!
        //! \return Widget będący korzeniem wizualizacji. Musi mieć slot setActiveSource(int).
        virtual QWidget* createWidget(IActionsGroupManager * manager) = 0;

        //! Tworzy ikonę dla zadanego wizualizatora. Może zwracać nullptr, chociaż to niewskazane.
        //! W odgróżnieniu od createWidget ikona przejmowana jest na własność.
        virtual QIcon* createIcon() = 0;

        virtual QPixmap print() const = 0;

        //! Aktualizacja wyświetlania. NIE aktualizacja stanu wyświetlanych danych.
        virtual void update(double deltaTime) = 0;

        //----------------- Obsługa serii danych ---------------------
        //! Wizualizator musi zapewnic ze da się stworzyć MAX serii za pomoca metody createSerie. Ilos cserii jest dynamicznie zarządzana z zewnątrz poprzez create i remove serie.
        //! SerieBase to klasa delegata, która implementuje specyficzne dla danego wizualizatora operacje ustawiania nazwy serii i jej danych. Kazdy wizualizator może inaczej ustawiac te informacje i prezentować je.

        //! \return Maksymalna ilość serii danych jaka może obsłużyc wizualizator, wartość 0 i mniej oznacza maksumalny zakres dla INT, inna wartość stanowi górna granice
        virtual int getMaxDataSeries() const = 0;

        //! \return Seria danych która można ustawiac - nazwa i dane, nie zarządza ta seria danych - czasem jej zycia, my zwalniamy jej zasoby!!
        virtual SerieBase* createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name = std::string()) = 0;

        virtual SerieBase* createSerie(const SerieBase* serie) = 0;

        //! \param serie Seria danych do usunięcia, nie powinien usuwać tej serii! Zarządzamy nią my!!
        virtual void removeSerie(SerieBase* serie) = 0;
    };

    typedef core::shared_ptr<IVisualizer> IVisualizerPtr;
    typedef core::shared_ptr<const IVisualizer> IVisualizerConstPtr;
    typedef core::weak_ptr<IVisualizer> IVisualizerWeakPtr;
    typedef core::weak_ptr<const IVisualizer> IVisualizerConstWeakPtr;

    typedef core::shared_ptr<IVisualizer::SerieBase> VisualizerSeriePtr;
    typedef core::shared_ptr<const IVisualizer::SerieBase> VisualizerSerieConstPtr;

    typedef core::shared_ptr<IVisualizer::TimeSerieBase> VisualizerTimeSeriePtr;
    typedef core::shared_ptr<const IVisualizer::TimeSerieBase> VisualizerTimeSerieConstPtr;
    typedef core::weak_ptr<IVisualizer::TimeSerieBase> VisualizerTimeSerieWeakPtr;
    typedef core::weak_ptr<const IVisualizer::TimeSerieBase> VisualizerTimeSerieConstWeakPtr;

} // namespace core


#endif  // HEADER_GUARD_CORE__IVISUALIZER_H__
