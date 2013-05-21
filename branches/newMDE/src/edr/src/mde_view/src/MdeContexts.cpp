#include "MdePCH.h"

#include <boost/tuple/tuple.hpp>
#include <QtGui/QToolBar>
#include <QtGui/QMenu>
#include <QtCore/QBuffer>
#include <corelib/PluginCommon.h>
#include "MdeContexts.h"
#include "AnalisisWidget.h"
#include <coreui/CoreVisualizerWidget.h>
#include <coreui/CoreTextEditWidget.h>
#include <coreui/ICoreActionSection.h>
#include <coreui/CoreFlexiToolBarSection.h>
#include <QtGui/QWidgetAction>

//MdeAnalysisContext::MdeAnalysisContext(TreeRefresher * refresher) :
//    refresher(refresher)
//{
//}
//
//void MdeAnalysisContext::activateContext(QWidget * contextWidget)
//{
//	refresher->setPreventRefresh(false);
//}
//
//void MdeAnalysisContext::deactivateContext(QWidget * nextContextWidget, bool refresh)
//{
//	refresher->setPreventRefresh(true);
//}

//
//MdeVisualizerUsageContext::MdeVisualizerUsageContext(QTabWidget * flexiTabWidget) : flexiTabWidget(flexiTabWidget), visualizerGroupID(-1)
//{
//
//}
//
//void MdeVisualizerUsageContext::activateContext(QWidget * contextWidget)
//{	
//    //nie wpsieramy kontekstu bez widgeta i nie ma sensu nic z kontekstem robic skoro jest już zaladowany
//    if(contextWidget == nullptr || visualizerGroupID != -1){
//        return;
//    }
//
//    //TODO zapisać styl widgeta, dokleić nasz styl i ustawić a nie nadpisywać
//
//    contextWidget->setStyleSheet(QString::fromUtf8(
//        "coreUI--CoreVisualizerWidget, coreUI--CoreVisualizerWidget:focus:hover {" \
//            "border: 2px solid orange;"\
//        "}"
//    ));
//
//    auto it = visualizersData.find(contextWidget);
//
//    if(it == visualizersData.end()){
//        return;
//    }
//
//	visualizerGroupID = flexiTabWidget->addTab(it->second.widget , it->second.icon, it->second.name);
//	it->second.widget->setVisible(true);
//	flexiTabWidget->setCurrentIndex(visualizerGroupID);
//}
//
//void MdeVisualizerUsageContext::deactivateContext(QWidget * nextContextWidget, bool refresh)
//{
//    if(nextContextWidget == getCurrentContextWidget()){
//        return;
//    }
//
//	auto w = getCurrentContextWidget();
//
//    if(w != nullptr && isCurrentContextWidgetDestroyed() == false){
//
//        //TODO przywrócić poprzedni styl widgeta
//        getCurrentContextWidget()->setStyleSheet(QString());
//    }
//	
//    if(visualizerGroupID != -1){
//		flexiTabWidget->widget(visualizerGroupID)->setVisible(false);
//        flexiTabWidget->removeTab(visualizerGroupID);
//        visualizerGroupID = -1;
//    }
//}
//
//void MdeVisualizerUsageContext::onRegisterContextWidget(QWidget * contextWidget)
//{
//    coreUI::CoreVisualizerWidget * visWidget = qobject_cast<coreUI::CoreVisualizerWidget*>(contextWidget);
//
//	auto vis = visWidget->getVisualizer();
//
//    auto actions = visWidget->actions();
//	actions.append(vis->getOrCreateWidget()->actions());
//
//    if(actions.empty() == true){
//        return;
//    }
//
//	//grupujemy akcje wg sekcji
//	//jak nie potrafie wyciagnac sekcji to wrzucam do common
//	std::map<QString, std::list<QAction*>> sections;
//
//	for(auto it = actions.begin(); it != actions.end(); ++it){
//		auto coreSectionAction = dynamic_cast<coreUI::ICoreActionSection*>(*it);
//		if(coreSectionAction != nullptr){
//			sections[coreSectionAction->section()].push_back(*it);
//		}else {
//			sections[QObject::tr("Common")].push_back(*it);
//		}
//	}
//
//	UsageContextData data;
//	data.widget = new QWidget;	
//
//	auto layout_ = new QHBoxLayout;
//	layout_->setSpacing(0);
//	layout_->setContentsMargins(0,0,0,0);
//	data.widget->setLayout(layout_);
//	data.widget->setVisible(false);
//
//	//teraz tworze sekcje
//	for(auto it = sections.begin(); it != sections.end(); ++it){
//
//		//pomijam grupy które są tutaj niepotrzebne
//		if(it->first == QObject::tr("Settings")){
//			continue;
//		}
//
//		coreUI::CoreFlexiToolBarSection * section = new coreUI::CoreFlexiToolBarSection(it->first);
//		
//		//mam sekcje więc zasilam ją w akcje
//		//zakladam ze mam 2 toolbary do ktorych wrzucam akcje wg lewy prawy
//		std::map<coreUI::CoreTitleBar::SideType, std::pair<std::map<int, QAction*>, std::map<int, QWidgetAction*>> > sideActions;
//
//		int idx = 0;
//
//		for(auto aIT = it->second.begin(); aIT != it->second.end(); ++aIT){
//			auto side = coreUI::CoreTitleBar::Left;
//			auto sideInterface = dynamic_cast<coreUI::CoreTitleBar::ICoreTitleBarAction*>(*aIT);
//			if(sideInterface != nullptr){
//				side = sideInterface->side();
//			}
//
//			auto widgetAction = dynamic_cast<QWidgetAction*>(*aIT);
//			if(widgetAction != nullptr){
//				sideActions[side].second[idx++] = widgetAction;
//			}else{
//				sideActions[side].first[idx++] = *aIT;
//			}
//		}
//
//		//mam akcje pogrupowane - balansujemy aby bylo mniej wiecej rowno po lewej i prawej stronie (gora i dól)
//
//		std::map<int, QAction*> leftActions;
//
//		auto it = sideActions.find(coreUI::CoreTitleBar::Left);
//		if(it != sideActions.end()){
//			leftActions = it->second.first;
//			leftActions.insert(it->second.second.begin(), it->second.second.end());
//		}
//
//		std::map<int, QAction*> rightActions;
//
//		it = sideActions.find(coreUI::CoreTitleBar::Right);
//		if(it != sideActions.end()){
//			rightActions = it->second.first;
//			rightActions.insert(it->second.second.begin(), it->second.second.end());
//		}
//
//		double ratio = std::max(leftActions.size(), rightActions.size()) / (double)(leftActions.size() + rightActions.size());
//
//		if(ratio > 0.65){
//			//TODO
//			//rabalansowanie akcji po lewej i prawej stronie
//		}
//
//		QWidget * sectionWidget = new QWidget;
//		auto vLayout = new QVBoxLayout;
//		vLayout->setContentsMargins(0,0,0,0);
//		vLayout->setSpacing(3);
//		if(leftActions.empty() == false){
//			QToolBar * upperBar = new QToolBar(section);
//			upperBar->setMovable(false);
//			upperBar->setFloatable(false);
//			for(auto it = leftActions.begin(); it != leftActions.end(); ++it){
//				upperBar->addAction(it->second);
//			}
//
//			vLayout->addWidget(upperBar);
//		}
//
//		if(rightActions.empty() == false){
//			QToolBar * lowerBar = new QToolBar(section);
//			lowerBar->setMovable(false);
//			lowerBar->setFloatable(false);
//			for(auto it = rightActions.begin(); it != rightActions.end(); ++it){
//				lowerBar->addAction(it->second);
//			}
//
//			vLayout->addWidget(lowerBar);
//		}
//
//		sectionWidget->setLayout(vLayout);
//
//		section->setInnerWidget(sectionWidget);
//
//		layout_->addWidget(section);
//	}
//
//	layout_->addStretch();
//	
//	data.name = QObject::tr("Visualizer") + QString(" - ") + QString::fromUtf8(vis->getName().c_str());
//	data.icon = vis->getIcon();
//
//    visualizersData[contextWidget] = data;
//}
//
//void MdeVisualizerUsageContext::onUnregisterContextWidget(QWidget * contextWidget)
//{
//    auto it = visualizersData.find(contextWidget);
//
//    if(it == visualizersData.end()){
//        return;
//    }
//
//	//TODO
//	//czy to jest konieczne?
//    delete it->second.widget;
//
//    visualizersData.erase(it);
//}



//RaportsThumbnailsContext::RaportsThumbnailsContext( QTabWidget * flexiTabWidget, HmmMainWindow* hmm ) :
//    flexiTabWidget(flexiTabWidget),
//	flexiSection(nullptr),
//    groupID(-1),
//    hmm(hmm),
//    projectName(nullptr),
//	projectTemplate(nullptr),
//	cssCombo(nullptr)
//{
//	
//}
//
//void RaportsThumbnailsContext::activateContext( QWidget * contextWidget )
//{
//       //nie wpsieramy kontekstu bez widgeta i nie ma sensu nic z kontekstem robic skoro jest już zaladowany
//    if(contextWidget == nullptr || groupID != -1){
//        return;
//    }
//	
//	groupID = flexiTabWidget->addTab(flexiSection, QObject::tr("Raports Tab"));
//	flexiSection->setVisible(true);
//	flexiTabWidget->setCurrentIndex(groupID);
//}
//
//void RaportsThumbnailsContext::deactivateContext( QWidget * nextContextWidget, bool refresh )
//{
//    if(nextContextWidget == getCurrentContextWidget()){
//        return;
//    }
//	
//	if(groupID != -1){
//		flexiTabWidget->removeTab(groupID);
//		flexiSection->setVisible(false);
//		groupID = -1;
//	}
//}
//
//void RaportsThumbnailsContext::onRegisterContextWidget( QWidget * contextWidget )
//{
//    flexiSection = new QWidget(flexiTabWidget);
//	flexiSection->setVisible(false);
//    QVBoxLayout* l = new QVBoxLayout();
//    l->setContentsMargins(0, 0, 0, 0);
//    l->setSpacing(0);
//    QToolButton* button = new QToolButton();
//    connect(button, SIGNAL(clicked()), this, SLOT(createRaport()));
//    button->setText(tr("Create raport"));
//
//    projectName = new QPlainTextEdit();
//    projectName->setMaximumSize(100, 20);
//    projectName->setPlainText(tr("Simple Raport"));
//    projectName->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//
//    QString dirPath = QString::fromStdString(plugin::getResourcePath("templates").string());
//    QStringList filters;
//    filters << "*.htm" << "*.html";
//    QDir templateDir(dirPath);
//    templateDir.setNameFilters(filters);
//
//    projectTemplate = new QComboBox();
//    projectTemplate->addItem(tr("Empty"));
//    projectTemplate->addItems(templateDir.entryList());
//    projectTemplate->setMaximumSize(100, 20);
//    projectTemplate->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//
//    QDir cssDir(dirPath, "*.css");
//    cssCombo = new QComboBox();
//    cssCombo->addItem(tr("Empty"));
//    cssCombo->addItems(cssDir.entryList());
//    cssCombo->setMaximumSize(100, 20);
//    cssCombo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//
//    QWidget* upper = new QWidget();
//    upper->setLayout(new QHBoxLayout());
//    QLabel* label = new QLabel("Raport name:");
//    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//    upper->layout()->addWidget(label);
//    upper->layout()->addWidget(projectName);
//    upper->layout()->setContentsMargins(1, 0, 1, 1);
//
//    QWidget* lower = new QWidget();
//    lower->setLayout(new QHBoxLayout());
//    label = new QLabel(tr("Raport template:"));
//    label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//    lower->layout()->addWidget(label);
//    lower->layout()->addWidget(projectTemplate);
//    lower->layout()->addWidget(cssCombo);
//    lower->layout()->setContentsMargins(1, 1, 1, 0);
//
//    l->addWidget(upper);
//    l->addWidget(lower);
//    l->addWidget(button);
//    flexiSection->setLayout(l);
//}
//
//void RaportsThumbnailsContext::onUnregisterContextWidget( QWidget * contextWidget )
//{
//
//}
//
//void RaportsThumbnailsContext::createRaport()
//{
//    // tymczasowo
//    const QWidget* thumbParent = hmm->getAnalisis()->raportsArea;
//
//    QObjectList children = thumbParent->children();
//    QDateTime time = QDateTime::currentDateTime();
//    QString images;
//    int counter = 0;
//    for (auto it = children.begin(); it != children.end(); ++it) {
//        QLabel* l = qobject_cast<QLabel*>(*it);
//        if (l) {
//            const QPixmap* pixmap = l->pixmap();
//            if (pixmap) {
//                QBuffer buffer;
//                pixmap->save(&buffer, "PNG");
//                int w = pixmap->width();
//                int h = pixmap->height();
//                int maxWidth = 800;
//                if (w > maxWidth) {
//                    h *= static_cast<double>(maxWidth) / w;
//                    w = maxWidth;
//                }
//                QString base64 = buffer.buffer().toBase64();
//                images += tr("Screenshot %1 <br> <IMG SRC=\"data:image/png;base64,%2\" ALIGN=BOTTOM WIDTH=%3 HEIGHT=%4 BORDER=0></P> <br>").arg(counter++).arg(base64).arg(w).arg(h);
//            }
//
//        }
//    }
//
//    QString templateDir = QString::fromStdString(plugin::getResourcePath("templates\\").string());//QString("C:\\programming\\HMEdr\\src\\edr\\resources\\deploy\\templates\\");
//    QString cssFile = templateDir + cssCombo->currentText();
//    QFile f(cssFile);
//    QString css;
//    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        css = f.readAll();
//        f.close();
//    }
//
//
//    QString p = templateDir + projectTemplate->currentText();
//    QFile file(p);
//    QString html;
//    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        QString raw = QString::fromUtf8(file.readAll());
//        html = raw.arg(projectName->toPlainText()).arg(time.toString()).arg(images).arg(css);
//    } else {
//        if (css.length()) {
//            css = QString(
//                "<HEAD>                    "
//                "<style type=\"text/css\"> "
//                "%1                        "
//                "</style>                  "
//                "</HEAD> ").arg(css);
//        }
//        html = QString(
//            "<HTML> "
//            "%1     "
//            "<BODY> "
//            "<P><FONT SIZE=8> %3</FONT></P>"
//            "%2     "
//            "</BODY>"
//            "</HTML>").arg(css).arg(images).arg(projectName->toPlainText());
//    }
//
//    hmm->createRaport(html);
//}





//MdeVVdfUsageContext::MdeVVdfUsageContext( QTabWidget * flexiTabWidget ) :
//    flexiTabWidget(flexiTabWidget), 
//    groupID(-1)
//{
//
//}
//
//void MdeVVdfUsageContext::activateContext( QWidget * contextWidget )
//{
//    //nie wpsieramy kontekstu bez widgeta i nie ma sensu nic z kontekstem robic skoro jest już zaladowany
//    if(contextWidget == nullptr || groupID != -1){
//        return;
//    }
//
//    groupID = flexiTabWidget->addTab(widget, QObject::tr("Vdf Tab"));
//    widget->setVisible(true);
//    flexiTabWidget->setCurrentIndex(groupID);
//
//}
//
//void MdeVVdfUsageContext::deactivateContext( QWidget * nextContextWidget, bool refresh )
//{
//    if(nextContextWidget == getCurrentContextWidget()){
//        return;
//    }
//
//    if(groupID != -1){
//        widget->setVisible(false);
//        flexiTabWidget->removeTab(groupID);
//        groupID = -1;
//    }
//
//}

//void MdeVVdfUsageContext::onRegisterContextWidget( QWidget * contextWidget )
//{
//    auto actions = contextWidget->actions();
//    
//    if(actions.empty() == true){
//        return;
//    }
//    
//    //grupujemy akcje wg sekcji
//    //jak nie potrafie wyciagnac sekcji to wrzucam do common
//    std::map<QString, std::list<QAction*>> sections;
//    
//    for(auto it = actions.begin(); it != actions.end(); ++it){
//        auto coreSectionAction = dynamic_cast<coreUI::ICoreActionSection*>(*it);
//        if(coreSectionAction != nullptr){
//            sections[coreSectionAction->section()].push_back(*it);
//        }else {
//            sections[QObject::tr("Common")].push_back(*it);
//        }
//    }
//    
//    widget = new coreUI::CoreFlexiToolBar();
//    auto layout_ = new QHBoxLayout();
//
//    layout_->setSpacing(0);
//    layout_->setContentsMargins(0,0,0,0);
//    
//    //teraz tworze sekcje
//    for(auto it = sections.begin(); it != sections.end(); ++it){
//            
//        coreUI::CoreFlexiToolBarSection * section = new coreUI::CoreFlexiToolBarSection(it->first);
//    
//        //mam sekcje więc zasilam ją w akcje
//        //zakladam ze mam 2 toolbary do ktorych wrzucam akcje wg lewy prawy
//        std::map<coreUI::CoreTitleBar::SideType, std::pair<std::map<int, QAction*>, std::map<int, QWidgetAction*>> > sideActions;
//    
//        int idx = 0;
//    
//        for(auto aIT = it->second.begin(); aIT != it->second.end(); ++aIT){
//            auto side = coreUI::CoreTitleBar::Left;
//            auto sideInterface = dynamic_cast<coreUI::CoreTitleBar::ICoreTitleBarAction*>(*aIT);
//            if(sideInterface != nullptr){
//                side = sideInterface->side();
//            }
//    
//            auto widgetAction = dynamic_cast<QWidgetAction*>(*aIT);
//            if(widgetAction != nullptr){
//                sideActions[side].second[idx++] = widgetAction;
//            }else{
//                sideActions[side].first[idx++] = *aIT;
//            }
//        }
//    
//        //mam akcje pogrupowane - balansujemy aby bylo mniej wiecej rowno po lewej i prawej stronie (gora i dól)
//    
//        std::map<int, QAction*> leftActions;
//    
//        auto it = sideActions.find(coreUI::CoreTitleBar::Left);
//        if(it != sideActions.end()){
//            leftActions = it->second.first;
//            leftActions.insert(it->second.second.begin(), it->second.second.end());
//        }
//    
//        std::map<int, QAction*> rightActions;
//    
//        it = sideActions.find(coreUI::CoreTitleBar::Right);
//        if(it != sideActions.end()){
//            rightActions = it->second.first;
//            rightActions.insert(it->second.second.begin(), it->second.second.end());
//        }
//    
//        double ratio = std::max(leftActions.size(), rightActions.size()) / (double)(leftActions.size() + rightActions.size());
//    
//        if(ratio > 0.65){
//            //TODO
//            //rabalansowanie akcji po lewej i prawej stronie
//        }
//    
//        QWidget * sectionWidget = new QWidget;
//        auto vLayout = new QVBoxLayout;
//        vLayout->setContentsMargins(0,0,0,0);
//        vLayout->setSpacing(3);
//        if(leftActions.empty() == false){
//            QToolBar * upperBar = new QToolBar(section);
//            upperBar->setMovable(false);
//            upperBar->setFloatable(false);
//            for(auto it = leftActions.begin(); it != leftActions.end(); ++it){
//                upperBar->addAction(it->second);
//            }
//    
//            vLayout->addWidget(upperBar);
//        }
//    
//        if(rightActions.empty() == false){
//            QToolBar * lowerBar = new QToolBar(section);
//            lowerBar->setMovable(false);
//            lowerBar->setFloatable(false);
//            for(auto it = rightActions.begin(); it != rightActions.end(); ++it){
//                lowerBar->addAction(it->second);
//            }
//    
//            vLayout->addWidget(lowerBar);
//        }
//    
//        sectionWidget->setLayout(vLayout);
//    
//        section->setInnerWidget(sectionWidget);
//    
//        layout_->addWidget(section);
//    }
//    
//    layout_->addStretch();
//
//    auto section = new coreUI::CoreFlexiToolBarSection();
//    QWidget* inner = new QWidget();
//    inner->setLayout(layout_);
//    section->setInnerWidget(inner);
//
//    widget->addSection(section);
//
//}
//
//void MdeVVdfUsageContext::onUnregisterContextWidget( QWidget * contextWidget )
//{
//
//}
