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

class QObject;
class QWidget;
class QIcon;

namespace core 
{
    class IVisualizer : public IIdentifiable
    {
    public:
        //! Maksymalna liczba ürÛde≥.
        static const int maxNumSources = 16;

        //! Pusty polimorficzny destruktor.
        virtual ~IVisualizer() {}

        //! Nowa instancja tego typu.
        virtual IVisualizer* create() const = 0;

        //! \return Nazwa wizualizatora.
        virtual const std::string& getName() const = 0;

        //! Tylko tutaj powinno nastÍpowaÊ tworzenie widgetu. Metoda wywo≥ywana tylko jeden raz.
        //! To wizualizator musi niszczyÊ widget w destruktorze. Gdy widget jest rÛwnoczeúnie
        //! widokiem OSG to wystarczy przypisaÊ go do osg::ref_ptr.
        //! \param action Akcje ktÛre naleøy dodaÊ do paska tytu≥owego. Kaødym elementem moøe byÊ
        //! albo QAction, QMenu z zagniedøonymi akcjami, albo ca≥y QWidget. Rodzice tych elementÛw nie sπ zmieniani!
        //! \return Widget bÍdπcy korzeniem wizualizacji. Musi mieÊ slot setActiveSource(int).
        virtual QWidget* createWidget(std::vector<QObject*>& actions) = 0;

        //! Tworzy ikonÍ dla zadanego wizualizatora. Moøe zwracaÊ nullptr, chociaø to niewskazane.
        //! W odgrÛønieniu od createWidget ikona przejmowana jest na w≥asnoúÊ.
        virtual QIcon* createIcon() = 0;

        //! Wype≥nia listÍ wspieranych typÛw dla danego slotu. Pusta lista oznacza, øe ten slot i kolejne nie sπ obs≥ugiwane.
        //! \param source Numer slotu.
        //! \param name Wynikowa Nazwa slotu.
        //! \param types Wynikowa lista wspieranych typÛw.
        virtual void getSlotInfo(int source, std::string& name, ObjectWrapper::Types& types) = 0;

        //! Metoda wywo≥ywana po przypisaniu danych wejúciowych wizualizatorowi. Powinna uwzglÍdniÊ czyszczenie widgetu
        //! po poprzednim setUp'ie.
        //! \param source èrÛd≥o danych.
        virtual void setUp(IObjectSource* source) = 0;

        //! Aktualizacja wyúwietlania. NIE aktualizacja stanu wyúwietlanych danych.
        virtual void update(double deltaTime) = 0;
    };

    typedef shared_ptr<IVisualizer> IVisualizerPtr;
    typedef shared_ptr<const IVisualizer> IVisualizerConstPtr;
    typedef weak_ptr<IVisualizer> IVisualizerWeakPtr;
    typedef weak_ptr<const IVisualizer> IVisualizerConstWeakPtr;

} // namespace core


#endif  // HEADER_GUARD_CORE__IVISUALIZER_H__