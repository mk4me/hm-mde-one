/*******************************************************************
    created:  2011/03/14
    created:  14:3:2011   16:49
    filename: IVisualizer.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__IVISUALIZER_H__
#define HEADER_GUARD_CORE__IVISUALIZER_H__

#include <vector>
#include <list>
#include <core/IIdentifiable.h>
#include <core/IObjectSource.h>
#include <core/ObjectWrapper.h>
#include <core/IInputProcessItem.h>
#include <utils/DataChannel.h>

class QObject;
class QWidget;
class QIcon;

#ifdef DEBUG
namespace osg 
{
    class Node;
} // namespace osg
#endif

namespace core 
{
    //! Interfejs wizualizator�w dla typ�w danych zarejestrowanych w aplikacji
    class IVisualizer :  public IInputProcessItem
    {
    public:

        //! Klasa bazowa do tworzenia i zarz�dzania seriami danych
        class SerieBase
        {
        public:

            virtual ~SerieBase() {}

            //! \param name Nazwa serii danych do ustawienia
            virtual void setName(const std::string & name) = 0;

            //! \return Nazwa serii danych
            virtual const std::string & getName() const = 0;

            //! \param data Dane do ustawienia w serii danych. ObjecWrappery pozwalaj� nam unikn�� potrzeby generowania wielu metod dla r�nych argument�w.
            //! Znacz�co uprasza interfejs, w przeciwnym wypadku musieliby�my skorzysta� z template
            virtual void setData(const ObjectWrapperConstPtr & data) = 0;

            //! \return Dane serii
            virtual const ObjectWrapperConstPtr & getData() const = 0;
        };

        class TimeSerieBase : public SerieBase
        {
        public:

            virtual ~TimeSerieBase() {}

            virtual void setTime(float time) = 0;

            virtual float getLength() const = 0;

            virtual void setOffset(float offset)
            {

            }

            virtual void setScale(float scale)
            {

            }
        };

    public:

        //! Pusty polimorficzny destruktor.
        virtual ~IVisualizer() {}

        //! Tylko tutaj powinno nast�powa� tworzenie widgetu. Metoda wywo�ywana tylko jeden raz.
        //! To wizualizator musi niszczy� widget w destruktorze. Gdy widget jest r�wnocze�nie
        //! widokiem OSG to wystarczy przypisa� go do osg::ref_ptr.
        //! \param action Akcje kt�re nale�y doda� do paska tytu�owego. Ka�dym elementem mo�e by�
        //! albo QAction, QMenu z zagnied�onymi akcjami, albo ca�y QWidget. Rodzice tych element�w nie s� zmieniani!
        //! \return Widget b�d�cy korzeniem wizualizacji. Musi mie� slot setActiveSource(int).
        virtual QWidget* createWidget(std::vector<QObject*>& actions) = 0;

        //! Tworzy ikon� dla zadanego wizualizatora. Mo�e zwraca� nullptr, chocia� to niewskazane.
        //! W odgr�nieniu od createWidget ikona przejmowana jest na w�asno��.
        virtual QIcon* createIcon() = 0;

        //! Aktualizacja wy�wietlania. NIE aktualizacja stanu wy�wietlanych danych.
        virtual void update(double deltaTime) = 0;

        //----------------- Obs�uga serii danych ---------------------
        //! Wizualizator musi zapewnic ze da sie stworzyc MAX serii za pomoca metody createSerie. Ilos cserii jest dynamicznie zarzadzana z zewwnatrz poprzez create i remove serie.
        //! SerieBase to klasa delegata, ktora implementuje specyficzne dla danego wizualizatora operacje ustawiania nazwy serii i jej danych. Kazdy wizualizator moze inaczej ustawiac te informacje i prezentowac je.

        //! \return Maksymalna ilosc serii danych jaka moze obsluzyc wizualizator, wartosc 0 i mniej oznacza maksumalny zakres dla INT, inna wartosc stanowi gorna granice
        virtual int getMaxDataSeries() const = 0;

        //! \return Seria danych ktora mozna ustawiac - nazwa i dane, nie zarzadza ta seria danych - czasem jej zycia, my zwalniamy jej zasoby!!
        virtual SerieBase* createSerie(const ObjectWrapperConstPtr & data, const std::string & name = std::string()) = 0;

        virtual SerieBase* createSerie(const SerieBase* serie) = 0;

        //! \param serie Seria danych do usuni�cia, nie powinien usuwac tej serii! Zarzadzamy nia my!!
        virtual void removeSerie(SerieBase* serie) = 0;

#ifdef DEBUG
        //! \return Korze� lokalnej sceny osg.
        virtual osg::Node* debugGetLocalSceneRoot()
        {
            return nullptr;
        }
#endif
    };

    typedef shared_ptr<IVisualizer> IVisualizerPtr;
    typedef shared_ptr<const IVisualizer> IVisualizerConstPtr;
    typedef weak_ptr<IVisualizer> IVisualizerWeakPtr;
    typedef weak_ptr<const IVisualizer> IVisualizerConstWeakPtr;

    typedef shared_ptr<IVisualizer::SerieBase> VisualizerSeriePtr;
    typedef shared_ptr<const IVisualizer::SerieBase> VisualizerSerieConstPtr;

    typedef shared_ptr<IVisualizer::TimeSerieBase> VisualizerTimeSeriePtr;
    typedef shared_ptr<const IVisualizer::TimeSerieBase> VisualizerTimeSerieConstPtr;

} // namespace core


#endif  // HEADER_GUARD_CORE__IVISUALIZER_H__