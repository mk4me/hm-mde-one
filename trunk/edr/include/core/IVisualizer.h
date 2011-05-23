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
        //! Maksymalna liczba Ÿróde³.
        //static const int maxNumSources = 16;

        //! Pusty polimorficzny destruktor.
        virtual ~IVisualizer() 
        {

        }

        ////! Nowa instancja tego typu.
        //virtual IVisualizer* create() const = 0;

        ////! \return Nazwa wizualizatora.
        //virtual const std::string& getName() const = 0;

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

        //! Wype³nia listê wspieranych typów dla danego slotu. Pusta lista oznacza, ¿e ten slot i kolejne nie s¹ obs³ugiwane.
        //! \param inputNo Numer slotu.
        //! \param name Wynikowa Nazwa slotu.
        //! \param types Wynikowa lista wspieranych typów.
        //virtual void getInputInfo(int inputNo, std::string& name, ObjectWrapper::Types& types) = 0;
                
        //! Aktualizacja wyœwietlania. NIE aktualizacja stanu wyœwietlanych danych.
        virtual void update(double deltaTime) = 0;

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

} // namespace core


#endif  // HEADER_GUARD_CORE__IVISUALIZER_H__