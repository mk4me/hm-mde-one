#include "hmmPCH.h"
#include <QtGui/QToolBar>
#include "VisualizerWidget.h"
#include "HmmContexts.h"
#include "HmmMainWindow.h"


HMMVisualizerUsageContext::HMMVisualizerUsageContext(FlexiTabWidget * flexiTabWidget) : flexiTabWidget(flexiTabWidget), visualizerGroupID(-1)
{

}

void HMMVisualizerUsageContext::activateContext(QWidget * contextWidget)
{
    //nie wpsieramy kontekstu bez widgeta i nie ma sensu nic z kontekstem robic skoro jest juz zaladowany
    if(contextWidget == nullptr || visualizerGroupID != -1){
        return;
    }

    auto it = visualizersData.find(contextWidget);

    if(it == visualizersData.end()){
        return;
    }

    if(it->second.empty() == false){
        auto visWidget = qobject_cast<VisualizerWidget*>(contextWidget);
        auto vis = visWidget->getCurrentVisualizer();
        //tworzymy grupe dla wizualizatora
        visualizerGroupID = flexiTabWidget->addGroup(QString::fromUtf8("Visualizer - ") + QString::fromUtf8(vis->getName().c_str()), VisualizerManager::getInstance()->getIcon(visWidget->getCurrentVisualizer()->getID()));

        for(auto sectionIT = it->second.begin(); sectionIT != it->second.end(); sectionIT++){
            auto sectionID = flexiTabWidget->addSection(visualizerGroupID, sectionIT->second, sectionIT->first);
            sectionIT->second->setVisible(true);
            visualizerSectionsIDs.insert(sectionID);
        }

        flexiTabWidget->setCurrentGroup(visualizerGroupID);
    }
}

void HMMVisualizerUsageContext::deactivateContext(QWidget * nextContextWidget, bool refresh)
{
    if(nextContextWidget == getCurrentContextWidget()){
        return;
    }

    if(visualizerGroupID != -1){
        flexiTabWidget->removeGroup(visualizerGroupID);
        visualizerGroupID = -1;
        visualizerSectionsIDs.swap(std::set<FlexiTabWidget::GUIID>());
    }
}

void HMMVisualizerUsageContext::onRegisterContextWidget(QWidget * contextWidget)
{
    VisualizerWidget * visWidget = qobject_cast<VisualizerWidget*>(contextWidget);
    // przygotowanie do wypelnienia grupy
    VisualizerWidget::VisualizerTitleBarElements titleBarElements;

    visWidget->getVisualizerTitleBarElements(titleBarElements);

    if(titleBarElements.empty() == false){
        auto vis = visWidget->getCurrentVisualizer();

        //podziel elementy na 4 grupy - akcje, menusy, widget i inne nieobslugiwane elementy
        //przy okazji wyznaczamy ilosc elementow oraz ich sumaryczna szerokosc i najwieksza wysokosc do pozniejszego layoutowania
        //elementy sa indeksowane tak jak podeslal nam je klient, ale ich kolejnosc moze zostac zmieniona zeby lepiej je rozlozyc
        std::map<unsigned int, QAction*> actions;
        std::map<unsigned int, QMenu*> menus;
        //std::map<unsigned int, QWidget*> widgets;
        std::map<unsigned int, QObject*> others;

        // budujemy widgety ktore beda potem trafialy do toolbarow
        std::map<unsigned int, QWidget*> toolbarElements;

        int maxHeight = 0;
        int totalWidth = 0;
        int totalElements = titleBarElements.size();

        for(unsigned int i = 0; i < totalElements; i++){
            if(QAction * action = qobject_cast<QAction*>(titleBarElements[i].first)){
                actions[i] = action;
            }else if(QMenu * menu = qobject_cast<QMenu*>(titleBarElements[i].first)){
                menus[i] = menu;
            }else if(QWidget * widget = qobject_cast<QWidget*>(titleBarElements[i].first)){
                //widgets[i] = widget;
                toolbarElements[i] = widget;

                if(QComboBox * cbox = qobject_cast<QComboBox*>(widget)){
                    cbox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
                    cbox->setMinimumContentsLength(min(cbox->currentText().size(), 10));
                }

                //dodajemy od razu do elementow toolbara - indeksy zostaja zachowane dla pozniejszego rozmieszczania wg kolejnosci
                auto s = widget->sizeHint();
                int width = min(s.width(), 250);
                if(s.width() > 250){
                    widget->setMaximumWidth(250);
                }
                totalWidth += width;
                maxHeight = max(maxHeight, s.height());
            }else{
                others[i] = titleBarElements[i].first;
            }
        }

        totalElements -= others.size();

        if(totalElements == 0){
            return;
        }

        for(auto it = actions.begin(); it != actions.end(); it++){
            QToolButton * actionButton = new QToolButton();
            if(it->second->toolTip().isEmpty() == true){
                actionButton->setToolTip(it->second->text());
            }

            //actionButton->setIconSize(QSize(20,20));
            actionButton->setFixedSize(20,20);

            actionButton->setDefaultAction(it->second);
            //actionButton->setMaximumSize(QSize(20,20));
            actionButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            toolbarElements[it->first] = actionButton;

            auto s = actionButton->sizeHint();
            totalWidth += s.width();
            maxHeight = max(maxHeight, s.height());
        }

        for(auto it = menus.begin(); it != menus.end(); it++){
            QToolButton * menuButton = new QToolButton();

            if(it->second->toolTip().isEmpty() == true){
                menuButton->setToolTip(it->second->title());
            }

            menuButton->setMenu(it->second);

            //if(it->second->icon().isNull() == false){
            //menuButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextBesideIcon);
            //}

            menuButton->setText(it->second->title());
            menuButton->setPopupMode(QToolButton::InstantPopup);

            menuButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            toolbarElements[it->first] = menuButton;

            auto s = menuButton->sizeHint();
            totalWidth += s.width();
            maxHeight = max(maxHeight, s.height());
        }

        //rozkladamy to w niezaleznych QToolBarach. Maksymalnie 2 rzedy.
        //TODO
        //dodac maksymalna szerokosc + przerzucanie elementow do ukrytego panelu jesli za duzo ich jest

        int halfWidth = totalWidth;

        int halfElements = totalElements / 2;

        QVBoxLayout * layout = new QVBoxLayout();
        QToolBar * topToolbar = new QToolBar();
        topToolbar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        topToolbar->setIconSize(QSize(20,20));
        topToolbar->setStyleSheet(QString::fromUtf8("QToolBar { spacing: 2px; }"));

        layout->addWidget(topToolbar);

        QToolBar * bottomToolbar = nullptr;

        //budujemy 2 wiersze jesli conajmniej 5 elementow
        if(halfElements > 4){
            bottomToolbar = new QToolBar();
            bottomToolbar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            bottomToolbar->setIconSize(QSize(20,20));
            layout->addWidget(bottomToolbar);
            halfWidth /= 2;
        }

        layout->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding));

        int currentWidth = 0;
        auto it = toolbarElements.begin();

        while(it != toolbarElements.end()){
            auto s = it->second->sizeHint();
            int width = min(s.width(), 250);
            if(currentWidth + width > halfWidth){
                it++;
            }else{
                topToolbar->addWidget(it->second);
                currentWidth += width;
                it = toolbarElements.erase(it);
            }
        }

        if(bottomToolbar != nullptr){
            it = toolbarElements.begin();

            while(it != toolbarElements.end()){
                bottomToolbar->addWidget(it->second);
                it++;
            }
        }

        //wypelniamy grupe
        QWidget * widget = new QWidget();
        widget->setLayout(layout);
        layout->setContentsMargins(0,0,0,0);
        layout->setMargin(0);
        layout->setSpacing(1);

        visualizersData[contextWidget][QString::fromUtf8("Default operations")] = widget;
    }
}

void HMMVisualizerUsageContext::onUnregisterContextWidget(QWidget * contextWidget)
{
    auto it = visualizersData.find(contextWidget);

    if(it == visualizersData.end()){
        return;
    }

    for(auto sectionIT = it->second.begin(); sectionIT != it->second.end(); sectionIT++){
        try{

            delete sectionIT->second;

        }catch(...)
        {

        }
    }

    visualizersData.erase(it);
}


HMMTreeItemUsageContext::HMMTreeItemUsageContext( FlexiTabWidget * flexiTabWidget, HmmMainWindow* hmm ) :
    flexiTabWidget(flexiTabWidget),
    groupID(-1), 
    hmm(hmm),
    flexiSection(new QWidget(flexiTabWidget))
{

}

void HMMTreeItemUsageContext::activateContext( QWidget * contextWidget )
{
    //nie wpsieramy kontekstu bez widgeta i nie ma sensu nic z kontekstem robic skoro jest juz zaladowany
    if(contextWidget == nullptr || groupID != -1){
        return;
    }

    QTreeWidget* tree = qobject_cast<QTreeWidget*>(contextWidget);/*
    TreeItemHelper* item = dynamic_cast<TreeItemHelper*>(tree->currentItem());
    if (item) {*/
    groupID = flexiTabWidget->addGroup(QString::fromUtf8("Tree"));

    //recreateFlexiSectionWidget(flexiSection, item);
    flexiTabWidget->addSection(groupID, flexiSection, "Testowy");
    flexiSection->setVisible(true);
    flexiTabWidget->setCurrentGroup(groupID);
    //}*/
}

void HMMTreeItemUsageContext::deactivateContext( QWidget * nextContextWidget, bool refresh )
{
    if(nextContextWidget == getCurrentContextWidget()){
        return;
    }

    if(groupID != -1){
        flexiTabWidget->removeGroup(groupID);
        groupID = -1;
    }
}

void HMMTreeItemUsageContext::onRegisterContextWidget( QWidget * contextWidget )
{
    QTreeWidget* tree = qobject_cast<QTreeWidget*>(contextWidget);
    connect(tree, SIGNAL(currentItemChanged (QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(itemChanged (QTreeWidgetItem*, QTreeWidgetItem*)));
}

void HMMTreeItemUsageContext::onUnregisterContextWidget( QWidget * contextWidget )
{

}


void HMMTreeItemUsageContext::recreateFlexiSectionWidget(QWidget* flexiSection, TreeItemHelper* helper)
{
    if (flexiSection->layout()) {
        delete flexiSection->layout();
    }

    const QObjectList& children = flexiSection->children();
    for (int i = children.size() - 1; i >= 0; --i) {
        delete children[i];
    }
    
    if (helper) {
        QVBoxLayout* l = new QVBoxLayout(flexiSection);
        QLabel* label = new QLabel(helper->text(0));
        l->addWidget(label);
        QMenu* menu = hmm->getContextMenu(nullptr, helper);
        QWidget* horizontal = new QWidget(flexiSection);
        l->addWidget(horizontal);
        QHBoxLayout* hl = new QHBoxLayout();
        horizontal->setLayout(hl);

        QList<QAction*> actions = menu->actions();
        for (auto it = actions.begin(); it != actions.end(); it++) {
            QMenu* m = (*it)->menu();
            if (m) { 
                //connect(m, SIGNAL(triggered(QAction*)), this, SLOT(refresh(QAction*)));
                QToolButton* menuButton = new QToolButton();
                menuButton->setText((*it)->text());
                menuButton->setPopupMode(QToolButton::InstantPopup);
                
                menuButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
                
                menuButton->setMenu(m);
                hl->addWidget(menuButton);
            } else {
                QToolButton* actionButton = new QToolButton(horizontal);
                actionButton->setIcon(QIcon(QString::fromUtf8(":/resources/icons/viewa.png")));
                //actionButton->addAction(*it);
                connect(actionButton, SIGNAL(clicked()), *it, SLOT(trigger()));
                //connect(actionButton, SIGNAL(clicked()), this, SLOT(refresh()));
                hl->addWidget(actionButton);
            }
            
        }

       
        /*QToolButton * menuButton = new QToolButton();
        menuButton->setMenu(menu);*/

        //menuButton->setText("Item operations");
        //menuButton->setPopupMode(QToolButton::InstantPopup);
        //
        //menuButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        //hl->addWidget(menuButton);
        //l->addWidget(new QLabel("Label"));
        //l->addWidget(new QPushButton("Button"));
        flexiSection->setLayout(l);
    }
}

void HMMTreeItemUsageContext::itemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous )
{
    recreateFlexiSectionWidget(flexiSection, dynamic_cast<TreeItemHelper*>(current));
}

void HMMTreeItemUsageContext::refresh( QAction* dummy /*= nullptr*/ )
{
    //QTreeWidget* tree = qobject_cast<QTreeWidget*>(getCurrentContextWidget());
    //recreateFlexiSectionWidget(flexiSection, dynamic_cast<TreeItemHelper*>(tree->currentItem()));
}
