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
#include <core/PluginCommon.h>
#include <core/IInputProcessItem.h>

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
    class IVisualizer :  public IInputProcessItem
    {
    public:

        class SerieBase
        {
        public:

            virtual ~SerieBase() {}

            void setName(const std::string & name)
            {
                if(this->name != name){
                    this->name = name;
                    setSerieName(name);
                }
            }

            const std::string & getName() const
            {
                return name;
            }

            void setData(const ObjectWrapperConstPtr & data)
            {
                if(this->data != data){
                    this->data = data;
                    setSerieData(data);
                }
            }

            const ObjectWrapperConstPtr & getData() const
            {
                return data;
            }

        protected:

            virtual void setSerieName(const std::string & name) = 0;
            virtual void setSerieData(const ObjectWrapperConstPtr & data) = 0;

        private:
            std::string name;
            ObjectWrapperConstPtr data;
        };

    public:

        //! Pusty polimorficzny destruktor.
        virtual ~IVisualizer() 
        {

        }

        //! Tylko tutaj powinno nastêpowaæ tworzenie widgetu. Metoda wywo³ywana tylko jeden raz.
        //! To wizualizator musi niszczyæ widget w destruktorze. Gdy widget jest równoczeœnie
        //! widokiem OSG to wystarczy przypisaæ go do osg::ref_ptr.
        //! \param action Akcje które nale¿y dodaæ do paska tytu³owego. Ka¿dym elementem mo¿e byæ
        //! albo QAction, QMenu z zagnied¿onymi akcjami, albo ca³y QWidget. Rodzice tych elementów nie s¹ zmieniani!
        //! \return Widget bêd¹cy korzeniem wizualizacji. Musi mieæ slot setActiveSource(int).
        virtual QWidget* createWidget(std::vector<QObject*>& actions) = 0;

        //! Tworzy ikonê dla zadanego wizualizatora. Mo¿e zwracaæ nullptr, chocia¿ to niewskazane.
        //! W odgró¿nieniu od createWidget ikona przejmowana jest na w³asnoœæ.
        virtual QIcon* createIcon() = 0;

        //! Aktualizacja wyœwietlania. NIE aktualizacja stanu wyœwietlanych danych.
        virtual void update(double deltaTime) = 0;

        //----------------- Obs³uga serii danych ---------------------
        //! Wizualizator musi zapewnic ze da sie stworzyc MAX serii za pomoca metody createSerie. Ilos cserii jest dynamicznie zarzadzana z zewwnatrz poprzez create i remove serie.
        //! SerieBase to klasa delegata, ktora implementuje specyficzne dla danego wizualizatora operacje ustawiania nazwy serii i jej danych. Kazdy wizualizator moze inaczej ustawiac te informacje i prezentowac je.

        //! \return Maksymalna ilosc serii danych jaka moze obsluzyc wizualizator, wartosc 0 i mniej oznacza maksumalny zakres dla INT, inna wartosc stanowi gorna granice
        virtual int getMaxDataSeries() const = 0;

        //! \return Seria danych ktora mozna ustawiac - nazwa i dane, nie zarzadza ta seria danych - czasem jej zycia, my zwalniamy jej zasoby!!
        virtual SerieBase* createSerie(const ObjectWrapperConstPtr & data, const std::string & name = std::string()) = 0;

        //! \param serie Seria danych do usuniêcia, nie powinien usuwac tej serii! Zarzadzamy nia my!!
        virtual void removeSerie(SerieBase* serie) = 0;

#ifdef DEBUG
        //! \return Korzeñ lokalnej sceny osg.
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

} // namespace core


#endif  // HEADER_GUARD_CORE__IVISUALIZER_H__