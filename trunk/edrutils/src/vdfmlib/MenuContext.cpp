#include "StdAfx.h"
#include <vdfmlib/MenuContext.h>
#include <vdfmlib/osgUI2DUtils.h>
#include <vdfmlib/AspectRatioKeeper.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI{
////////////////////////////////////////////////////////////////////////////////

MenuContext::MenuContext(void) : parentMenu(0), rootMenu(0), activeSubMenu(0),	separators("/") {
	rootMenu = this;
	closeMenuEventHandler = new CloseMenuContextEvent(this);
	hide();
	setStyle("osg.contextmenu.menu");
}

MenuContext::MenuContext(MenuContext * parent) : parentMenu(parent), rootMenu(0), activeSubMenu(0), separators("/"){
	if(parentMenu == 0){
		closeMenuEventHandler = new CloseMenuContextEvent(this);
		rootMenu = this;
	}else{
		rootMenu = parentMenu->rootMenu;
	}

	hide();
	setStyle("osg.contextmenu.menu");
}


MenuContext::~MenuContext(void)
{
}

void MenuContext::managed(osgWidget::WindowManager * wm){
	osgUI::Grid::managed(wm);
	
	if(wm != 0 && closeMenuEventHandler != 0){
		wm->getView()->addEventHandler(closeMenuEventHandler);
	}
}

void MenuContext::unmanaged(osgWidget::WindowManager * wm){
	osgUI::Grid::unmanaged(wm);

	if(wm != 0 && closeMenuEventHandler != 0){
		wm->getView()->removeEventHandler(closeMenuEventHandler);
	}
}

bool MenuContext::addMenuItem(const std::string & path, bool checked, const MenuContext::OnClickCallback & onClickCallback, const MenuContext::OnHoverCallback & onHooverCallback){
	return addMenuItem(parseNextMenuItems(path), checked, onClickCallback, onHooverCallback);
}

bool MenuContext::removeMenuItem(const std::string & path){
	return removeMenuItem(parseNextMenuItems(path));
}

bool MenuContext::setMenuItemChecked(const std::string & path, bool checked){
	return setMenuItemChecked(parseNextMenuItems(path), checked);
}

bool MenuContext::isSubmenuDirectionRight() const{
	if(parentMenu == 0 || getWindowManager() == 0 ||
		parentMenu->getAbsoluteOrigin().x() + parentMenu->getWidth() + getWidth() <= getWindowManager()->getWidth()){
		return true;
	}

	return false;
}

bool MenuContext::setMenuOnCloseCallback(const std::string & path, const MenuContext::OnCloseCallback & callback){
	if(path.empty() == true){
		this->closeMenuCallback = callback;
		return true;
	}

	return setMenuOnCloseCallback(parseNextMenuItems(path), callback);
}

bool MenuContext::showMenu(const osgWidget::XYCoord & pos){	
	setOrigin(pos);
	resize();
	if(this->isVisible() == true){
		return true;
	}

	if(rootMenu == this){
		show();
		return true;
	}

	if(parentMenu->isVisible() == true){
		if(parentMenu->activeSubMenu != 0){
			parentMenu->activeSubMenu->hideMenu();
		}
		parentMenu->activeSubMenu = this;

		if(parentMenu->getWindowManager() != 0){
			parentMenu->getWindowManager()->addChild(this);
		}
		setOrigin(pos);
		resize();
		show();
		return true;
	}

	return false;
}

bool MenuContext::showMenu(osgWidget::point_type x, osgWidget::point_type y){
	return showMenu(osgWidget::XYCoord(x,y));
}

void MenuContext::hideMenu(){
	//go down, and start hiding up to here
	if(isVisible() == true){
		MenuContext* last = this;
		while(last->activeSubMenu != 0){
			last = last->activeSubMenu;
		}

		while(last != this){
			if(last->closeMenuCallback.empty() == false){
				last->closeMenuCallback(last);
			}

			last->hide();
			last->activeSubMenu = 0;
			MenuContext* paret = last->parentMenu;
			if(paret->getWindowManager() != 0){
				paret->getWindowManager()->removeChild(last);
			}

			last = paret;
		}

		if(closeMenuCallback.empty() == false){
			closeMenuCallback(this);
		}

		hide();
		activeSubMenu = 0;

		if(parentMenu != 0){
			parentMenu->activeSubMenu = 0;
			if(parentMenu->getWindowManager() != 0){
				parentMenu->getWindowManager()->removeChild(this);
			}
		}
	}
}

std::vector<std::string> MenuContext::parseNextMenuItems(const std::string & path) const{
	std::vector<std::string> ret;

	tokenizer tokens(path, boost::char_separator<char>(separators.c_str()));
	for(tokenizer::iterator it = tokens.begin(); it != tokens.end(); it++){
		ret.push_back(*it);
	}

	return ret;
}

bool MenuContext::onItemPush(osgWidget::Event& ev){
	//call callback function

	if(getWindowManager() == 0 || getWindowManager()->isLeftMouseButtonDown() == false){
		return false;
	}

	MENU_ITEM * menuItem = static_cast<MENU_ITEM *>(ev.getData());

	menuItem->checked = ! menuItem->checked;
	if(menuItem->onClickCallback.empty() == false){
		menuItem->onClickCallback(menuItem->menuItem->getLabel(), menuItem->checked);
	}

	refreshMenuItemCheckedStyle(*menuItem);

	//hide context menu
	rootMenu->hideMenu();
	return false;
}

bool MenuContext::onItemEnter(osgWidget::Event& ev){
	
	MENU_ITEM * menuItem = static_cast<MENU_ITEM *>(ev.getData());

	menuItem->menuItem->setStyle("osg.contextmenu.item.hoovered");

	if(getWindowManager() != 0 && getWindowManager()->getStyleManager() != 0){
		getWindowManager()->getStyleManager()->applyStyles(menuItem->menuItem);
	}

	refreshMenuItemCheckedStyle(*menuItem, true);

	if(menuItem->onHooverCallback.empty() == false){
		menuItem->onHooverCallback(menuItem->menuItem->getLabel(), true);
	}

	return false;
}

bool MenuContext::onItemLeave(osgWidget::Event& ev){
	
	MENU_ITEM * menuItem = static_cast<MENU_ITEM *>(ev.getData());

	menuItem->menuItem->setStyle("osg.contextmenu.item.normal");

	if(getWindowManager() != 0 && getWindowManager()->getStyleManager() != 0){
		getWindowManager()->getStyleManager()->applyStyles(menuItem->menuItem);
	}

	refreshMenuItemCheckedStyle(*menuItem);

	if(menuItem->onHooverCallback.empty() == false){
		menuItem->onHooverCallback(menuItem->menuItem->getLabel(), false);
	}

	return false;
}

bool MenuContext::onSubmenuEnter(osgWidget::Event& ev){

	MENU_SUBMENU * submenuItem = static_cast<MENU_SUBMENU *>(ev.getData());

	submenuItem->submenuItem->setStyle("osg.contextmenu.submenuitem.hoovered");
	submenuItem->emptyWidget->setStyle("osg.contextmenu.submenuitem.hoovered");
	if(getWindowManager() != 0 && getWindowManager()->getStyleManager() != 0){
		getWindowManager()->getStyleManager()->applyStyles(submenuItem->submenuItem);
		getWindowManager()->getStyleManager()->applyStyles(submenuItem->emptyWidget);
	}

	if(submenuItem->submenu == activeSubMenu){
		return false;
	}

	MenuContext* submenu = dynamic_cast<MenuContext*>(ev.getWindow());

	osgWidget::XYCoord pos = submenu->getAbsoluteOrigin();
	pos.y() += submenuItem->submenuItem->getOrigin().y() + submenuItem->submenuItem->getHeight() - submenuItem->submenu->getHeight();

	if(submenuItem->submenu->isSubmenuDirectionRight() == true){
		pos.x() += submenu->getWidth();
	}else{
		pos.x() -= submenuItem->submenu->getWidth();
	}

	submenuItem->submenu->showMenu(pos);
	return false;
}

bool MenuContext::onSubmenuLeave(osgWidget::Event& ev){

	MENU_SUBMENU * submenuItem = static_cast<MENU_SUBMENU *>(ev.getData());

	submenuItem->submenuItem->setStyle("osg.contextmenu.submenuitem.normal");
	submenuItem->emptyWidget->setStyle("osg.contextmenu.submenuitem.normal");
	if(getWindowManager() != 0 && getWindowManager()->getStyleManager() != 0){
		getWindowManager()->getStyleManager()->applyStyles(submenuItem->submenuItem);
		getWindowManager()->getStyleManager()->applyStyles(submenuItem->emptyWidget);
	}
	
	return false;
}

void MenuContext::refreshMenuItemCheckedStyle(const MENU_ITEM & menuItem, bool hoovered){
	if(menuItem.checkedWidget != 0){
		if(menuItem.checked == true){
			if(hoovered == true){
				menuItem.checkedWidget->setStyle("osg.contextmenu.checked.hoovered");
			}else{
				menuItem.checkedWidget->setStyle("osg.contextmenu.checked.normal");
			}
		}else{
			if(hoovered == true){
				menuItem.checkedWidget->setStyle("osg.contextmenu.unchecked.hoovered");
			}else{
				menuItem.checkedWidget->setStyle("osg.contextmenu.unchecked.normal");
			}
		}

		if(getWindowManager() != 0 && getWindowManager()->getStyleManager() != 0){
			getWindowManager()->getStyleManager()->applyStyles(menuItem.checkedWidget);
		}
	}
}

MenuContext::CloseMenuContextEvent::CloseMenuContextEvent(MenuContext * menu) : contextMenu(menu){

}

MenuContext::CloseMenuContextEvent::~CloseMenuContextEvent(){

}

bool MenuContext::CloseMenuContextEvent::handle(const osgGA::GUIEventAdapter& gea,
	osgGA::GUIActionAdapter&      gaa,
	osg::Object*                  obj,
	osg::NodeVisitor*             nv
	){

	if(contextMenu != 0 && contextMenu->isVisible() == true && gea.getEventType() == osgGA::GUIEventAdapter::PUSH &&
		gea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON){

		contextMenu->hideMenu();
	}

	return false;
}

const MenuContext::MENU_SUBMENU & MenuContext::findMenu(const std::string & path) const{
	return findMenu(parseNextMenuItems(path));
}

const MenuContext::MENU_ITEM & MenuContext::findMenuItem(const std::string & path) const{
	return findMenuItem(parseNextMenuItems(path));
}

MenuContext::MENU_SUBMENU & MenuContext::findMenu(const std::string & path){
	return findMenu(parseNextMenuItems(path));
}

MenuContext::MENU_ITEM & MenuContext::findMenuItem(const std::string & path){
	return findMenuItem(parseNextMenuItems(path));
}

template<class Collection>
bool MenuContext::addMenuItem(const Collection & path, bool checked, const OnClickCallback & onClickCallback, const OnHoverCallback & onHooverCallback){
	return addMenuItem(path.begin(), path.end(), checked, onClickCallback, onHooverCallback);
}

template<class Iter>
bool MenuContext::addMenuItem(Iter begin, Iter end, bool checked, const OnClickCallback & onClickCallback, const OnHoverCallback & onHooverCallback){
	if(begin == end){
		return false;
	}

	Iter prev = begin;
	MenuContext* currentMenu = this;
	while(++begin != end){
		SUBMENUS_MAP::iterator it = currentMenu->submenus.find(*begin);
		if(it == currentMenu->submenus.end()){
			//add new submenu
			MENU_SUBMENU submenuItem;
			//create new submenu
			submenuItem.submenu = new MenuContext(currentMenu);

			//create widget
			submenuItem.submenuItem = new osgWidget::Label("", *begin);
			//format widget
			submenuItem.submenuItem->setSize(0,0);
			submenuItem.submenuItem->setFontSize(10);
			submenuItem.submenuItem->setCanFill(true);
			submenuItem.submenuItem->setStyle("osg.contextmenu.submenuitem.normal");

			submenuItem.emptyWidget = new osgWidget::Widget();
			submenuItem.emptyWidget->setCanFill(true);
			submenuItem.emptyWidget->setStyle("osg.contextmenu.submenuitem.normal");
			submenuItem.emptyWidget->setSize(submenuItem.submenuItem->getHeight(),submenuItem.submenuItem->getHeight());

			//add widget
			it = currentMenu->submenus.insert(SUBMENUS_MAP::value_type(*begin, submenuItem)).first;

			//configure widget events - onEnter, onLeave
			osgWidget::Callback * mc = new osgWidget::Callback(&MenuContext::onSubmenuEnter, currentMenu, osgWidget::EVENT_MOUSE_ENTER, &(it->second));
			osgWidget::Callback * md = new osgWidget::Callback(&MenuContext::onSubmenuLeave, currentMenu, osgWidget::EVENT_MOUSE_LEAVE, &(it->second));

			submenuItem.submenuItem->setEventMask(osgWidget::EVENT_MASK_MOUSE_MOVE);
			submenuItem.submenuItem->addCallback(mc);
			submenuItem.submenuItem->addCallback(md);

			submenuItem.emptyWidget->setEventMask(osgWidget::EVENT_MASK_MOUSE_MOVE);
			submenuItem.emptyWidget->addCallback(mc);
			submenuItem.emptyWidget->addCallback(md);

			unsigned int row = currentMenu->getNumRows();

			currentMenu->setNumRows(row + 1);

			if(row == 0){
				currentMenu->setNumColumns(2);
				currentMenu->setColumnWeight(0,0);
				currentMenu->setColumnWeight(1,1);
			}

			currentMenu->addWidget(submenuItem.submenuItem, row, 1);
			//osgUI::AspectRatioKeeper * ar = new osgUI::AspectRatioKeeper(submenuItem.emptyWidget,1.0);
			//ar->setCanFill(true);
			//currentMenu->addWidget(ar, row, 0);
			//ar->setSize(submenuItem.emptyWidget->getSize());
			currentMenu->addWidget(submenuItem.emptyWidget, row, 0);

			//currentMenu->resize();
		}

		currentMenu = it->second.submenu;
		prev = begin;
	}

	//try to add menu item
	if(currentMenu->items.find(*prev) == currentMenu->items.end()){
		//create widget

		MENU_ITEM item;

		item.menuItem = new osgWidget::Label("", *prev);

		//format widget
		item.menuItem->setSize(0,0);
		item.menuItem->setFontSize(10);
		item.menuItem->setCanFill(true);
		item.menuItem->setStyle("osg.contextmenu.item.normal");
		//configure widget events - onPush

		item.checked = checked;

		item.onClickCallback = onClickCallback;
		item.onHooverCallback = onHooverCallback;

		//checked operation
		item.checkedWidget = new osgWidget::Widget();
		item.checkedWidget->setSize(item.menuItem->getHeight(),item.menuItem->getHeight());
		item.checkedWidget->setCanFill(true);

		//add widget
		ITEMS_MAP::iterator it = currentMenu->items.insert(ITEMS_MAP::value_type(*prev, item)).first;

		osgWidget::Callback * cc = new osgWidget::Callback(&MenuContext::onItemPush, currentMenu, osgWidget::EVENT_MOUSE_PUSH, &(it->second));
		osgWidget::Callback * ec = new osgWidget::Callback(&MenuContext::onItemEnter, currentMenu, osgWidget::EVENT_MOUSE_ENTER, &(it->second));
		osgWidget::Callback * lc = new osgWidget::Callback(&MenuContext::onItemLeave, currentMenu, osgWidget::EVENT_MOUSE_LEAVE, &(it->second));

		item.menuItem->setEventMask(osgWidget::EVENT_MASK_MOUSE_MOVE | osgWidget::EVENT_MASK_MOUSE_CLICK);
		item.menuItem->addCallback(cc);
		item.menuItem->addCallback(ec);
		item.menuItem->addCallback(lc);

		item.checkedWidget->setEventMask(osgWidget::EVENT_MASK_MOUSE_MOVE | osgWidget::EVENT_MASK_MOUSE_CLICK);
		item.checkedWidget->addCallback(cc);
		item.checkedWidget->addCallback(ec);
		item.checkedWidget->addCallback(lc);

		refreshMenuItemCheckedStyle(item);
		
		unsigned int row = currentMenu->getNumRows();

		currentMenu->setNumRows(row +1);

		if(row == 0){
			currentMenu->setNumColumns(2);
			currentMenu->setColumnWeight(0,0);
			currentMenu->setColumnWeight(1,1);
		}

		currentMenu->addWidget(item.menuItem, row, 1);
		currentMenu->addWidget(item.checkedWidget, row, 0);
		return true;
	}

	return false;
}

template<class Collection>
bool MenuContext::removeMenuItem(const Collection & path){
	return removeMenuItem(path.begin(), path.end());
}

template<class Iter>
bool MenuContext::removeMenuItem(Iter begin, Iter end){
	MENU_ITEM menuItem = findMenuItem(begin, end);

	if(menuItem.menuItem == 0){
		return false;
	}

	//TODO
	//implement
	return false;
}

template<class Collection>
bool MenuContext::setMenuItemChecked(const Collection & path, bool checked){
	return setMenuItemChecked(path.begin(), path.end(), checked);
}

template<class Iter>
bool MenuContext::setMenuItemChecked(Iter begin, Iter end, bool checked){
	MENU_ITEM & menuItem = findMenuItem(begin, end);

	if(menuItem.menuItem == 0){
		return false;
	}

	if(menuItem.checked != checked){
		menuItem.checked = checked;
		
		refreshMenuItemCheckedStyle(menuItem);
		return true;
	}

	return false;
}

template<class Collection>
bool MenuContext::setMenuItemOnClickCallback(const Collection & path, const OnClickCallback & callback){
	return setMenuItemOnClickCallback(path.begin(), path.end(), callback);
}

template<class Iter>
bool MenuContext::setMenuItemOnClickCallback(Iter begin, Iter end, const OnClickCallback & callback){
	MENU_ITEM & menuItem = findMenuItem(begin, end);

	if(menuItem.menuItem == 0 || menuItem.onClickCallback == callback){
		return false;
	}

	menuItem.onClickCallback = callback;

	return true;
}

template<class Collection>
bool MenuContext::setMenuItemOnHooverCallback(const Collection & path, const OnHoverCallback & callback){
	return setMenuItemOnHooverCallback(path.begin(), path.end(), callback);
}

template<class Iter>
bool MenuContext::setMenuItemOnHooverCallback(Iter begin, Iter end, const OnHoverCallback & callback){
	MENU_ITEM & menuItem = findMenuItem(begin, end);

	if(menuItem.menuItem == 0 || menuItem.onHooverCallback == callback){
		return false;
	}

	menuItem.onHooverCallback = callback;

	return true;
}

template<class Collection>
bool MenuContext::setMenuOnCloseCallback(const Collection & path, const OnCloseCallback & callback){
	return setMenuOnCloseCallback(path.begin(), path.end(), callback);
}

template<class Iter>
bool MenuContext::setMenuOnCloseCallback(Iter begin, Iter end, const OnCloseCallback & callback){
	if(begin == end){
		this->closeMenuCallback = callback;
		return true;
	}

	MENU_SUBMENU & submenu = findMenu(begin, end);

	if(submenu.submenu == 0){
		return false;
	}

	submenu.submenu->closeMenuCallback = callback;

	return true;
}

template<class Collection>
const MenuContext::OnClickCallback & MenuContext::getMenuItemOnClickCallback(const Collection & path) const{
	return getMenuItemOnClickCallback(path.begin(), path.end(), callback);
}

template<class Iter>
const MenuContext::OnClickCallback & MenuContext::getMenuItemOnClickCallback(Iter begin, Iter end) const{
	return findMenuItem(begin, end).onClickCallback;
}

template<class Collection>
const MenuContext::OnHoverCallback & MenuContext::getMenuItemOnHooverCallback(const Collection & path) const{
	return getMenuItemOnHooverCallback(path.begin(), path.end(), callback);
}

template<class Iter>
const MenuContext::OnHoverCallback & MenuContext::getMenuItemOnHooverCallback(Iter begin, Iter end) const{
	return findMenuItem(begin, end).onHooverCallback;
}

template<class Collection>
const MenuContext::OnCloseCallback & MenuContext::getMenuOnCloseCallback(const Collection & path) const{
	return getMenuOnCloseCallback(path.begin(), path.end(), callback);
}

template<class Iter>
const MenuContext::OnCloseCallback & MenuContext::getMenuOnCloseCallback(Iter begin, Iter end) const{
	return findMenu(begin, end).second->m_fCloseMenuCallback;
}

template<class Collection>
bool MenuContext::getMenuItemChecked(const Collection & path) const{
	return getMenuItemChecked(path.begin(), path.end(), callback);
}

template<class Iter>
bool MenuContext::getMenuItemChecked(Iter begin, Iter end) const{
	return findMenuItem(begin, end).checked;
}

template<class Collection>
MenuContext::MENU_ITEM & MenuContext::findMenuItem(const Collection & path){
	return findMenuItem(path.begin(), path.end());
}

template<class Iter>
MenuContext::MENU_ITEM & MenuContext::findMenuItem(Iter begin, Iter end){		
	if(std::distance(begin, end) == 1){
		//check only this level
		ITEMS_MAP::iterator it = items.find(*begin);
		if(it != items.end()){
			return it->second;
		}
	}else{
		Iter tmpEnd = end;
		tmpEnd--;
		MENU_SUBMENU submenu = findMenu(begin, tmpEnd);

		if(submenu.submenu != 0){
			ITEMS_MAP::iterator it = submenu.submenu->items.find(*tmpEnd);
			if(it != submenu.submenu->items.end()){
				return it->second;
			}
		}
	}

	emptyMenuItem = constEmptyMenuItem;
	return emptyMenuItem;
}

template<class Collection>
const MenuContext::MENU_ITEM & MenuContext::findMenuItem(const Collection & path) const{
	return findMenuItem(path.begin(), path.end());
}

template<class Iter>
const MenuContext::MENU_ITEM & MenuContext::findMenuItem(Iter begin, Iter end) const{

	if(std::distance(begin, end) == 1){
		//check only this level
		ITEMS_MAP::const_iterator it = items.find(*begin);
		if(it != items.end()){
			return it->second;
		}
	}else{
		Iter tmpEnd = end;
		tmpEnd--;
		MENU_SUBMENU submenu = findMenu(begin, tmpEnd);

		if(submenu.submenu != 0){
			ITEMS_MAP::const_iterator it = submenu.submenu->items.find(*tmpEnd);
			if(it != submenu.submenu->items.end()){
				return it->second;
			}
		}
	}

	return constEmptyMenuItem;
}

template<class Collection>
MenuContext::MENU_SUBMENU & MenuContext::findMenu(const Collection & path){
	return findMenu(path.begin(), path.end());
}

template<class Iter>
MenuContext::MENU_SUBMENU & MenuContext::findMenu(Iter begin, Iter end){
	emptyMenuSubmenu = constEmptyMenuSubmenu;

	if(begin == end){
		return emptyMenuSubmenu;
	}

	SUBMENUS_MAP::iterator it;
	MenuContext* currentMenu = this;
	while(++begin != end){
		it = currentMenu->submenus.find(*begin);
		if(it != currentMenu->submenus.end()){
			currentMenu = it->second.submenu;
		}else{
			return emptyMenuSubmenu;
		}
	}

	return it->second;
}

template<class Collection>
const MenuContext::MENU_SUBMENU & MenuContext::findMenu(const Collection & path) const{
	return findMenu(path.begin(), path.end());
}

template<class Iter>
const MenuContext::MENU_SUBMENU & MenuContext::findMenu(Iter begin, Iter end) const{
	if(begin == end){
		return constEmptyMenuSubmenu;
	}

	SUBMENUS_MAP::const_iterator it;
	const MenuContext* currentMenu = this;
	while(++begin != end){
		it = currentMenu->submenus.find(*begin);
		if(it != currentMenu->submenus.end()){
			currentMenu = it->second.submenu;
		}else{
			return constEmptyMenuSubmenu;
		}
	}

	return it->second;
}

}