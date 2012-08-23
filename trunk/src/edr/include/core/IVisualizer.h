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

        //! Tylko tutaj powinno nast�powa� tworzenie widgetu. Metoda wywo�ywana tylko jeden raz.
        //! To wizualizator musi niszczy� widget w destruktorze. Gdy widget jest r�wnocze�nie
        //! widokiem OSG to wystarczy przypisa� go do osg::ref_ptr.
        //! \param action Akcje kt�re nale�y doda� do paska tytu�owego. Ka�dym elementem mo�e by�
        //! albo QAction, QMenu z zagnied�onymi akcjami, albo ca�y QWidget. Rodzice tych element�w nie s� zmieniani!
        //! \return Widget b�d�cy korzeniem wizualizacji. Musi mie� slot setActiveSource(int).
        virtual QWidget* createWidget(IActionsGroupManager * manager) = 0;

        //! Tworzy ikon� dla zadanego wizualizatora. Mo�e zwraca� nullptr, chocia� to niewskazane.
        //! W odgr�nieniu od createWidget ikona przejmowana jest na w�asno��.
        virtual QIcon* createIcon() = 0;

        virtual QPixmap print() const = 0;

        //! Aktualizacja wy�wietlania. NIE aktualizacja stanu wy�wietlanych danych.
        virtual void update(double deltaTime) = 0;

        //----------------- Obs�uga serii danych ---------------------
        //! Wizualizator musi zapewnic ze da si� stworzy� MAX serii za pomoca metody createSerie. Ilos cserii jest dynamicznie zarz�dzana z zewn�trz poprzez create i remove serie.
        //! SerieBase to klasa delegata, kt�ra implementuje specyficzne dla danego wizualizatora operacje ustawiania nazwy serii i jej danych. Kazdy wizualizator mo�e inaczej ustawiac te informacje i prezentowa� je.

        //! \return Maksymalna ilo�� serii danych jaka mo�e obs�u�yc wizualizator, warto�� 0 i mniej oznacza maksumalny zakres dla INT, inna warto�� stanowi gorna granice
        virtual int getMaxDataSeries() const = 0;

        //! \return Seria danych kt�ra mo�na ustawiac - nazwa i dane, nie zarz�dza ta seria danych - czasem jej zycia, my zwalniamy jej zasoby!!
        virtual SerieBase* createSerie(const ObjectWrapperConstPtr & data, const std::string & name = std::string()) = 0;

        virtual SerieBase* createSerie(const SerieBase* serie) = 0;

        //! \param serie Seria danych do usuni�cia, nie powinien usuwa� tej serii! Zarz�dzamy ni� my!!
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