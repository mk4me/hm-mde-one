#include "MdePCH.h"
#include "ContextAutoPlacer.h"
#include <QtGui/QWidgetAction>
#include <QtGui/QToolBar>
#include <coreui/ICoreActionSection.h>
#include <coreui/CoreTitleBar.h>
#include <coreui/CoreFlexiToolBarSection.h>
#include <coreui/CoreVisualizerWidget.h>

QWidget* ContextAutoPlacer::create(const QList<QAction*>& actions, bool skipSettings, bool skipUtils)
{
    if(actions.empty() == true){
        return nullptr;
    }

    //grupujemy akcje wg sekcji
    //jak nie potrafie wyciagnac sekcji to wrzucam do common
    std::map<QString, std::list<QAction*>> sections;

    for(auto it = actions.begin(); it != actions.end(); ++it){
        auto coreSectionAction = dynamic_cast<coreUI::ICoreActionSection*>(*it);
        if(coreSectionAction != nullptr){
            sections[coreSectionAction->section()].push_back(*it);
        }else {
            sections[QObject::tr("Common")].push_back(*it);
        }
    }

    QWidget* widget = new QWidget;	

    auto layout_ = new QHBoxLayout;
    layout_->setSpacing(0);
    layout_->setContentsMargins(0,0,0,0);
    widget->setLayout(layout_);
    //widget->setVisible(false);

    //teraz tworze sekcje
    for(auto it = sections.begin(); it != sections.end(); ++it){

        //pomijam grupy które są tutaj niepotrzebne
        if(it->first == QObject::tr("Settings")){
            continue;
        }

        if(it->first == QObject::tr("Utils")){
            continue;
        }

        coreUI::CoreFlexiToolBarSection * section = new coreUI::CoreFlexiToolBarSection(it->first);

        //mam sekcje więc zasilam ją w akcje
        //zakladam ze mam 2 toolbary do ktorych wrzucam akcje wg lewy prawy
        std::map<coreUI::CoreTitleBar::SideType, std::pair<std::map<int, QAction*>, std::map<int, QWidgetAction*>> > sideActions;

        int idx = 0;

        for(auto aIT = it->second.begin(); aIT != it->second.end(); ++aIT){
            auto side = coreUI::CoreTitleBar::Left;
            auto sideInterface = dynamic_cast<coreUI::CoreTitleBar::ICoreTitleBarAction*>(*aIT);
            if(sideInterface != nullptr){
                side = sideInterface->side();
            }

            auto widgetAction = dynamic_cast<QWidgetAction*>(*aIT);
            if(widgetAction != nullptr){
                sideActions[side].second[idx++] = widgetAction;
            }else{
                sideActions[side].first[idx++] = *aIT;
            }
        }

        //mam akcje pogrupowane - balansujemy aby bylo mniej wiecej rowno po lewej i prawej stronie (gora i dól)

        std::map<int, QAction*> leftActions;

        auto it = sideActions.find(coreUI::CoreTitleBar::Left);
        if(it != sideActions.end()){
            leftActions = it->second.first;
            leftActions.insert(it->second.second.begin(), it->second.second.end());
        }

        std::map<int, QAction*> rightActions;

        it = sideActions.find(coreUI::CoreTitleBar::Right);
        if(it != sideActions.end()){
            rightActions = it->second.first;
            rightActions.insert(it->second.second.begin(), it->second.second.end());
        }

        double ratio = std::max(leftActions.size(), rightActions.size()) / (double)(leftActions.size() + rightActions.size());

        if(ratio > 0.65){
            //TODO
            //rabalansowanie akcji po lewej i prawej stronie
        }

        QWidget * sectionWidget = new QWidget;
        auto vLayout = new QVBoxLayout;
        vLayout->setContentsMargins(0,0,0,0);
        vLayout->setSpacing(3);
        if(leftActions.empty() == false){
            QToolBar * upperBar = new QToolBar(section);
            upperBar->setMovable(false);
            upperBar->setFloatable(false);
            for(auto it = leftActions.begin(); it != leftActions.end(); ++it){
                upperBar->addAction(it->second);
            }

            vLayout->addWidget(upperBar);
        }

        if(rightActions.empty() == false){
            QToolBar * lowerBar = new QToolBar(section);
            lowerBar->setMovable(false);
            lowerBar->setFloatable(false);
            for(auto it = rightActions.begin(); it != rightActions.end(); ++it){
                lowerBar->addAction(it->second);
            }

            vLayout->addWidget(lowerBar);
        }

        sectionWidget->setLayout(vLayout);

        section->setInnerWidget(sectionWidget);

        layout_->addWidget(section);
    }

    layout_->addStretch();

    return widget;
}


SimpleContext::SimpleContext( QTabWidget * flexiTabWidget, const QString& name ) :
flexiTabWidget(flexiTabWidget),
    groupID(-1),
    widget(nullptr),
    name(name)
{

}


void SimpleContext::activateContext( QWidget * contextWidget )
{
    //nie wpsieramy kontekstu bez widgeta i nie ma sensu nic z kontekstem robic skoro jest już zaladowany
    if(contextWidget == nullptr || groupID != -1){
        return;
    }

    groupID = flexiTabWidget->addTab(widget, name);
    widget->setVisible(true);
    flexiTabWidget->setCurrentIndex(groupID);
}

void SimpleContext::deactivateContext( QWidget * nextContextWidget, bool refresh )
{
    if(nextContextWidget == getCurrentContextWidget()){
        return;
    }

    if(groupID != -1){
        widget->setVisible(false);
        flexiTabWidget->removeTab(groupID);
        groupID = -1;
    }
}

void SimpleContext::onRegisterContextWidget( QWidget * contextWidget )
{
    QList<QAction*> actions = contextWidget->actions();
    if (!actions.empty()) {
        QWidget* actionsWidget = ContextAutoPlacer::create(actions);
        if (!widget) {
            widget = new coreUI::CoreFlexiToolBar(flexiTabWidget);
        }

        auto section = new coreUI::CoreFlexiToolBarSection(QObject::tr("")); // hack - wywalilem nazwe Operations
        section->setInnerWidget(actionsWidget);
        widget->addSection(section);
        widget->setVisible(false);
    }
}

void SimpleContext::onUnregisterContextWidget( QWidget * contextWidget )
{
}


MedusaVisualizerSimpleContext::MedusaVisualizerSimpleContext( QTabWidget * flexiTabWidget, const QString& name ) :
    SimpleContext(flexiTabWidget, name)
{

}

void MedusaVisualizerSimpleContext::onRegisterContextWidget( QWidget * contextWidget )
{
    if (!dynamic_cast<coreUI::CoreVisualizerWidget*>(contextWidget)) {
        SimpleContext::onRegisterContextWidget(contextWidget);
    }
}
