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
        //! Maksymalna liczba �r�de�.
        //static const int maxNumSources = 16;

        //! Pusty polimorficzny destruktor.
        virtual ~IVisualizer() 
        {

        }

        ////! Nowa instancja tego typu.
        //virtual IVisualizer* create() const = 0;

        ////! \return Nazwa wizualizatora.
        //virtual const std::string& getName() const = 0;

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

        //! Wype�nia list� wspieranych typ�w dla danego slotu. Pusta lista oznacza, �e ten slot i kolejne nie s� obs�ugiwane.
        //! \param inputNo Numer slotu.
        //! \param name Wynikowa Nazwa slotu.
        //! \param types Wynikowa lista wspieranych typ�w.
        //virtual void getInputInfo(int inputNo, std::string& name, ObjectWrapper::Types& types) = 0;
                
        //! Aktualizacja wy�wietlania. NIE aktualizacja stanu wy�wietlanych danych.
        virtual void update(double deltaTime) = 0;

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

} // namespace core


#endif  // HEADER_GUARD_CORE__IVISUALIZER_H__