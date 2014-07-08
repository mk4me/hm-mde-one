#include "CoreUiPCH.h"
#include <coreUI/ContextAutoPlacer.h>
#include <QtWidgets/QWidgetAction>
#include <QtWidgets/QToolBar>
#include <coreui/ICoreActionSection.h>
#include <coreui/CoreTitleBar.h>
#include <coreui/CoreFlexiToolBarSection.h>
#include <coreui/CoreVisualizerWidget.h>

using namespace coreUI;

QWidget* ContextAutoPlacer::create(const QList<QAction*>& actions, const QList<QString>& sectionsToSkip)
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

        ////pomijam grupy które są tutaj niepotrzebne
        //if(skipSettings && it->first == QObject::tr("Settings")){
        //    continue;
        //}

        //if(skipUtils && it->first == QObject::tr("Utils")){
        //    continue;
        //}

        if (sectionsToSkip.contains(it->first)) {
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

