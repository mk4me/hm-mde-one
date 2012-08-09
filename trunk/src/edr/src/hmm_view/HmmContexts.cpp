#include "hmmPCH.h"
#include <boost/tuple/tuple.hpp>
#include <QtGui/QToolBar>
#include <QtGui/QMenu>
#include <QtCore/QBuffer>
#include <core/PluginCommon.h>
#include "VisualizerManager.h"
#include "VisualizerWidget.h"
#include "HmmContexts.h"
#include "HmmMainWindow.h"
#include "textedit.h"
#include "AnalisisWidget.h"

HMMAnalysisContext::HMMAnalysisContext(TreeRefresher * refresher) :
    refresher(refresher)
{
}

void HMMAnalysisContext::activateContext(QWidget * contextWidget)
{
	refresher->setPreventRefresh(false);
}

void HMMAnalysisContext::deactivateContext(QWidget * nextContextWidget, bool refresh)
{
	refresher->setPreventRefresh(true);
}


HMMVisualizerUsageContext::HMMVisualizerUsageContext(FlexiTabWidget * flexiTabWidget) : flexiTabWidget(flexiTabWidget), visualizerGroupID(-1)
{

}

void HMMVisualizerUsageContext::activateContext(QWidget * contextWidget)
{
    //nie wpsieramy kontekstu bez widgeta i nie ma sensu nic z kontekstem robic skoro jest juz zaladowany
    if(contextWidget == nullptr || visualizerGroupID != -1){
        return;
    }

    //TODO zapisaæ styl widgeta, dokleiæ nasz styl i ustawiæ a nie nadpisywaæ

    contextWidget->setStyleSheet(QString::fromUtf8(
        "VisualizerWidget > .QWidget, VisualizerWidget > .QWidget:focus:hover {" \
            "border: 2px solid orange;"\
        "}"
    ));

    auto it = visualizersData.find(contextWidget);

    if(it == visualizersData.end()){
        return;
    }

    if(it->second.empty() == false){
        auto visWidget = qobject_cast<VisualizerWidget*>(contextWidget);
        auto vis = visWidget->getCurrentVisualizer();
        //tworzymy grupe dla wizualizatora
        visualizerGroupID = flexiTabWidget->addGroup(QObject::tr("Visualizer") + QString(" - ") + QString::fromUtf8(vis->getName().c_str()), VisualizerManager::getInstance()->getIcon(visWidget->getCurrentVisualizer()->getID()));

        for(auto sectionIT = it->second.begin(); sectionIT != it->second.end(); ++sectionIT){
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

	auto w = getCurrentContextWidget();

    if(w != nullptr && isCurrentContextWidgetDestroyed() == false){

        //TODO przywróciæ poprzedni styl widgeta
        getCurrentContextWidget()->setStyleSheet(QString());
    }

    if(visualizerGroupID != -1){
        flexiTabWidget->removeGroup(visualizerGroupID);
        visualizerGroupID = -1;
        std::set<FlexiTabWidget::GUIID>().swap(visualizerSectionsIDs);
    }
}

void HMMVisualizerUsageContext::onRegisterContextWidget(QWidget * contextWidget)
{
    VisualizerWidget * visWidget = qobject_cast<VisualizerWidget*>(contextWidget);

    auto actionsManager = visWidget->getVisualizerActionsManager();

    if(actionsManager.empty() == true){
        return;
    }
    auto vis = visWidget->getCurrentVisualizer();

    for(auto groupIT = actionsManager.begin(); groupIT != actionsManager.end(); ++groupIT){

        //podziel elementy na 4 grupy - akcje, menusy, widget i inne nieobslugiwane elementy
        //przy okazji wyznaczamy ilosc elementow oraz ich sumaryczna szerokosc i najwieksza wysokosc do pozniejszego layoutowania
        //elementy sa indeksowane tak jak podeslal nam je klient, ale ich kolejnosc moze zostac zmieniona zeby lepiej je rozlozyc
        std::map<int, QAction*> actions;
        std::map<int, QMenu*> menus;
        std::map<int, QObject*> others;
        std::map<int, QObject*> all;

        // budujemy widgety ktore beda potem trafialy do toolbarow
        std::map<int, QWidget*> toolbarElements;

        (*groupIT).getActions(actions);
        (*groupIT).getMenus(menus);
        (*groupIT).getObjects(others);
        (*groupIT).getWidgets(toolbarElements);
        (*groupIT).getAllObjects(all);


        int maxHeight = 0;
        int totalWidth = 0;
        int totalElements = all.size();

        for(auto it = toolbarElements.begin(); it != toolbarElements.end(); ++it){
            QWidget * widget = it->second;
            if(QComboBox * cbox = qobject_cast<QComboBox*>(widget)){
                cbox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
                cbox->setMinimumContentsLength(std::min(cbox->currentText().size(), 10));
            }

            //dodajemy od razu do elementow toolbara - indeksy zostaja zachowane dla pozniejszego rozmieszczania wg kolejnosci
            auto s = widget->sizeHint();
            int width = std::min(s.width(), 250);
            if(s.width() > 250){
                widget->setMaximumWidth(250);
            }
            totalWidth += width;
            maxHeight = std::max(maxHeight, s.height());
        }

        totalElements -= others.size();

        if(totalElements == 0){
            return;
        }

        for(auto it = actions.begin(); it != actions.end(); ++it){
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
            maxHeight = std::max(maxHeight, s.height());
        }

        for(auto it = menus.begin(); it != menus.end(); ++it){
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
            maxHeight = std::max(maxHeight, s.height());
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
            int width = std::min(s.width(), 250);
            if(currentWidth + width > halfWidth){
                ++it;
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
                ++it;
            }
        }

        //wypelniamy grupe
        QWidget * widget = new QWidget();
        widget->setLayout(layout);
        layout->setContentsMargins(0,0,0,0);
        layout->setMargin(0);
        layout->setSpacing(1);

        visualizersData[contextWidget][(*groupIT).name()] = widget;
    }
}

void HMMVisualizerUsageContext::onUnregisterContextWidget(QWidget * contextWidget)
{
    auto it = visualizersData.find(contextWidget);

    if(it == visualizersData.end()){
        return;
    }

    for(auto sectionIT = it->second.begin(); sectionIT != it->second.end(); ++sectionIT){
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
	flexiSection(new QWidget()),
    groupID(-1),
    hmm(hmm)
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
    groupID = flexiTabWidget->addGroup(QObject::tr("Tree"));

    recreateFlexiSectionWidget(flexiSection, dynamic_cast<HmmTreeItem*>(tree->currentItem()));
    flexiTabWidget->addSection(groupID, flexiSection, tr("Item Actions"));
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


void HMMTreeItemUsageContext::recreateFlexiSectionWidget(QWidget* flexiSection, HmmTreeItem* helper)
{
   if (!flexiSection->layout()) {
       flexiSection->setLayout(new QVBoxLayout());
   }

    const QObjectList& children = flexiSection->children();
    for (int i = children.size() - 1; i >= 0; --i) {
        QWidget* w = qobject_cast<QWidget*>(children[i]);
        if (w) {
            w->setVisible(false);
        }
    }

    if (helper) {
        QLayout* l = flexiSection->layout() ? flexiSection->layout() : new QVBoxLayout(flexiSection);
        l = flexiSection->layout();
        l->setContentsMargins(0, 0, 0, 0);
        l->setSpacing(3);
        QLabel* label = new QLabel(helper->text(0));
        l->addWidget(label);
        QMenu* menu = hmm->getContextMenu(nullptr, helper);
        QWidget* horizontal = new QWidget(flexiSection);
        l->addWidget(horizontal);
        QHBoxLayout* hl = new QHBoxLayout();
        hl->setContentsMargins(0, 0, 0, 0);
        horizontal->setLayout(hl);

        QList<QAction*> actions = menu->actions();
        for (auto it = actions.begin(); it != actions.end(); ++it) {
            QMenu* m = (*it)->menu();
            if (m) {
                QToolButton* menuButton = new QToolButton();
                menuButton->setText((*it)->text());
                menuButton->setPopupMode(QToolButton::InstantPopup);

                menuButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

                menuButton->setMenu(m);
                hl->addWidget(menuButton);
            } else if (!(*it)->isSeparator()) {
                QToolButton* actionButton = new QToolButton(horizontal);
                actionButton->setIcon(QIcon(QString::fromUtf8(":/resources/icons/viewa.png")));
                connect(actionButton, SIGNAL(clicked()), *it, SLOT(trigger()));
                hl->addWidget(actionButton);
            }

        }
        flexiSection->setLayout(l);
    }
}

void HMMTreeItemUsageContext::itemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous )
{
    recreateFlexiSectionWidget(flexiSection, dynamic_cast<HmmTreeItem*>(current));
}


void HMMTreeItemUsageContext::refresh()
{
   QTreeWidget* tree = qobject_cast<QTreeWidget*>(getCurrentContextWidget());
   if (tree) {
     recreateFlexiSectionWidget(flexiSection, dynamic_cast<HmmTreeItem*>(tree->currentItem()));
   }
}


RaportsThumbnailsContext::RaportsThumbnailsContext( FlexiTabWidget * flexiTabWidget, HmmMainWindow* hmm ) :
    flexiTabWidget(flexiTabWidget),
	flexiSection(new QWidget()),
    groupID(-1),
    hmm(hmm),
    projectName(nullptr),
	projectTemplate(nullptr),
	cssCombo(nullptr)
{

}

void RaportsThumbnailsContext::activateContext( QWidget * contextWidget )
{
       //nie wpsieramy kontekstu bez widgeta i nie ma sensu nic z kontekstem robic skoro jest juz zaladowany
    if(contextWidget == nullptr || groupID != -1){
        return;
    }

    groupID = flexiTabWidget->addGroup(QObject::tr("Raports Tab"));

    flexiTabWidget->addSection(groupID, flexiSection, tr("Actions"));
    flexiSection->setVisible(true);
    flexiTabWidget->setCurrentGroup(groupID);
}

void RaportsThumbnailsContext::deactivateContext( QWidget * nextContextWidget, bool refresh )
{
    if(nextContextWidget == getCurrentContextWidget()){
        return;
    }

    if(groupID != -1){
        flexiTabWidget->removeGroup(groupID);
        groupID = -1;
    }
}

void RaportsThumbnailsContext::onRegisterContextWidget( QWidget * contextWidget )
{
    flexiSection = new QWidget();
    QVBoxLayout* l = new QVBoxLayout();
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    QToolButton* button = new QToolButton();
    connect(button, SIGNAL(clicked()), this, SLOT(createRaport()));
    button->setText(tr("Create raport"));

    projectName = new QPlainTextEdit();
    projectName->setMaximumSize(100, 20);
    projectName->setPlainText(tr("Simple Raport"));
    projectName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QString dirPath = core::getResourceString("templates");
    QStringList filters;
    filters << "*.htm" << "*.html";
    QDir templateDir(dirPath);
    templateDir.setNameFilters(filters);

    projectTemplate = new QComboBox();
    projectTemplate->addItem(tr("Empty"));
    projectTemplate->addItems(templateDir.entryList());
    projectTemplate->setMaximumSize(100, 20);
    projectTemplate->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QDir cssDir(dirPath, "*.css");
    cssCombo = new QComboBox();
    cssCombo->addItem(tr("Empty"));
    cssCombo->addItems(cssDir.entryList());
    cssCombo->setMaximumSize(100, 20);
    cssCombo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    QWidget* upper = new QWidget();
    upper->setLayout(new QHBoxLayout());
    QLabel* label = new QLabel("Raport name:");
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    upper->layout()->addWidget(label);
    upper->layout()->addWidget(projectName);
    upper->layout()->setContentsMargins(1, 0, 1, 1);

    QWidget* lower = new QWidget();
    lower->setLayout(new QHBoxLayout());
    label = new QLabel(tr("Raport template:"));
    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    lower->layout()->addWidget(label);
    lower->layout()->addWidget(projectTemplate);
    lower->layout()->addWidget(cssCombo);
    lower->layout()->setContentsMargins(1, 1, 1, 0);

    l->addWidget(upper);
    l->addWidget(lower);
    l->addWidget(button);
    flexiSection->setLayout(l);
}

void RaportsThumbnailsContext::onUnregisterContextWidget( QWidget * contextWidget )
{

}

void RaportsThumbnailsContext::createRaport()
{
    // tymczasowo
    const QWidget* thumbParent = hmm->getAnalisis()->raportsArea;

    QObjectList children = thumbParent->children();
    QDateTime time = QDateTime::currentDateTime();
    QString images;
    int counter = 0;
    for (auto it = children.begin(); it != children.end(); ++it) {
        QLabel* l = qobject_cast<QLabel*>(*it);
        if (l) {
            const QPixmap* pixmap = l->pixmap();
            if (pixmap) {
                QBuffer buffer;
                pixmap->save(&buffer, "PNG");
                int w = pixmap->width();
                int h = pixmap->height();
                int maxWidth = 800;
                if (w > maxWidth) {
                    h *= static_cast<double>(maxWidth) / w;
                    w = maxWidth;
                }
                QString base64 = buffer.buffer().toBase64();
                images += tr("Screenshot %1 <br> <IMG SRC=\"data:image/png;base64,%2\" ALIGN=BOTTOM WIDTH=%3 HEIGHT=%4 BORDER=0></P> <br>").arg(counter++).arg(base64).arg(w).arg(h);
            }

        }
    }

    QString templateDir = core::getResourceString("templates\\");//QString("C:\\programming\\HMEdr\\src\\edr\\resources\\deploy\\templates\\");
    QString cssFile = templateDir + cssCombo->currentText();
    QFile f(cssFile);
    QString css;
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        css = f.readAll();
        f.close();
    }


    QString p = templateDir + projectTemplate->currentText();
    QFile file(p);
    QString html;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString raw = QString::fromUtf8(file.readAll());
        html = raw.arg(projectName->toPlainText()).arg(time.toString()).arg(images).arg(css);
    } else {
        if (css.length()) {
            css = QString(
                "<HEAD>                    "
                "<style type=\"text/css\"> "
                "%1                        "
                "</style>                  "
                "</HEAD> ").arg(css);
        }
        html = QString(
            "<HTML> "
            "%1     "
            "<BODY> "
            "<P><FONT SIZE=8> %3</FONT></P>"
            "%2     "
            "</BODY>"
            "</HTML>").arg(css).arg(images).arg(projectName->toPlainText());
    }

    hmm->createRaport(html);
}

RaportsTabContext::RaportsTabContext( FlexiTabWidget * flexiTabWidget, HmmMainWindow* hmm ) :
    flexiTabWidget(flexiTabWidget),
	editSection(nullptr),
    groupID(-1),
    hmm(hmm),
    textSection(nullptr),
    fileSection(nullptr)
{

}


void RaportsTabContext::activateContext( QWidget * contextWidget )
{
    //nie wpsieramy kontekstu bez widgeta i nie ma sensu nic z kontekstem robic skoro jest juz zaladowany
    if(contextWidget == nullptr || groupID != -1){
        return;
    }

    groupID = flexiTabWidget->addGroup(QObject::tr("Raports Tab"));

    fileSection->setVisible(true);
    editSection->setVisible(true);
    textSection->setVisible(true);

    flexiTabWidget->addSection(groupID, fileSection, tr("File"));
    flexiTabWidget->addSection(groupID, editSection, tr("Edit"));
    flexiTabWidget->addSection(groupID, textSection, tr("Text"));

    flexiTabWidget->setCurrentGroup(groupID);
}

void RaportsTabContext::deactivateContext( QWidget * nextContextWidget, bool refresh )
{
    if(nextContextWidget == getCurrentContextWidget()){
        return;
    }

    if(groupID != -1){
        flexiTabWidget->removeGroup(groupID);
        groupID = -1;
    }
}

void RaportsTabContext::onRegisterContextWidget( QWidget * contextWidget )
{
    TextEdit* textEdit = qobject_cast<TextEdit*>(contextWidget);

    auto twoLine = createTwoLineWidget();
    editSection = twoLine.get<0>();
    placeObjects(textEdit->getEditActions(), twoLine.get<1>(), twoLine.get<2>());

    twoLine = createTwoLineWidget();
    fileSection = twoLine.get<0>();
    placeObjects(textEdit->getFileActions(), twoLine.get<1>(), twoLine.get<2>());

    twoLine = createTwoLineWidget();
    textSection = twoLine.get<0>();
    placeObjects(textEdit->getTextActions(), twoLine.get<1>(), twoLine.get<2>(), true);
}

void RaportsTabContext::onUnregisterContextWidget( QWidget * contextWidget )
{

}

void RaportsTabContext::placeObjects( const QList<QObject*> &editList, QLayout* lowerLayout, QLayout* upperLayout, bool actionsOnTop)
{
    int count = editList.size();
    for (int i = 0; i < count; ++i) {
        QAction* a = qobject_cast<QAction*>(editList[i]);
        if (a) {
            QToolButton* b = new QToolButton();
            b->setDefaultAction(a);
            b->setToolTip(a->text().remove("&"));
            b->setFixedSize(20,20);
            b->setIcon(a->icon());
            b->setIconSize(QSize(20, 20));
            if (actionsOnTop || i < count / 2) {
                upperLayout->addWidget(b);
            } else {
                lowerLayout->addWidget(b);
            }
        } else {
            QWidget* w = qobject_cast<QWidget*>(editList[i]);
            if (actionsOnTop || i >= count / 2) {
                lowerLayout->addWidget(w);
            } else {
                upperLayout->addWidget(w);
            }
        }

    }
}

boost::tuple<QWidget*, QLayout*, QLayout*> RaportsTabContext::createTwoLineWidget()
{
    QWidget* flexiSection = new QWidget();
    QVBoxLayout* l = new QVBoxLayout();
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    QWidget* upper = new QWidget();
    QLayout* upperLayout = new QHBoxLayout();
    upper->setLayout(upperLayout);
    QWidget* lower = new QWidget();
    QLayout* lowerLayout = new QHBoxLayout();
    lower->setLayout(lowerLayout);

    upperLayout->setContentsMargins(3, 3, 3, 3);
    lowerLayout->setContentsMargins(3, 3, 3, 3);
    l->addWidget(upper);
    l->addWidget(lower);
    flexiSection->setLayout(l);

    return boost::make_tuple(flexiSection, lowerLayout, upperLayout);
}
