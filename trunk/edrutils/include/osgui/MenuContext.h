#ifndef __HEADER_GUARD__MENUCONTEXT_H__
#define __HEADER_GUARD__MENUCONTEXT_H__

#include <osgui/Grid.h>
#include <map>
#include <vector>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/tokenizer.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace osgUI {
////////////////////////////////////////////////////////////////////////////////

class MenuContext :
	public osgUI::Grid
{
public:

	//! string - path
	//! bool - checked
	typedef boost::function<void(const std::string&, bool)> OnClickCallback;

	typedef boost::function<void(MenuContext*)> OnCloseCallback;

	//redefinicja, tu mamy œcie¿kê jak poprzednio oraz czy jesteœmy w (onEnter - true) czy poza (onLeave - false)
	typedef OnClickCallback OnHoverCallback;

public:
	MenuContext(void);
	~MenuContext(void);

	virtual void managed(osgWidget::WindowManager * wm);
	virtual void unmanaged(osgWidget::WindowManager * wm);

	void setPathSeparators(const std::string & separators);
	const std::string & getPathSeparators() const;

	bool addMenuItem(const std::string & path, bool checked, const OnClickCallback & onClickCallback, const OnHoverCallback & onHooverCallback = OnHoverCallback());

	bool removeMenuItem(const std::string & path);
	
	bool setMenuItemChecked(const std::string & path, bool checked);

	bool setMenuItemOnClickCallback(const std::string & path, const OnClickCallback & callback);

	bool setMenuItemOnHooverCallback(const std::string & path, const OnHoverCallback & callback);

	bool setMenuOnCloseCallback(const std::string & path, const OnCloseCallback & callback);

	const OnClickCallback & getMenuItemOnClickCallback(const std::string & path) const;

	const OnHoverCallback & getMenuItemOnHooverCallback(const std::string & path) const;

	const OnCloseCallback & getMenuOnCloseCallback(const std::string & path) const;

	bool getMenuItemChecked(const std::string & path) const;

	bool showMenu(const osgWidget::XYCoord & pos);
	bool showMenu(osgWidget::point_type x, osgWidget::point_type y);
	void hideMenu();

protected:

	class CloseMenuContextEvent : public osgGA::GUIEventHandler{

	public:
		CloseMenuContextEvent(MenuContext * menu);
		virtual ~CloseMenuContextEvent();

		virtual bool handle(const osgGA::GUIEventAdapter& gea,
			osgGA::GUIActionAdapter&      gaa,
			osg::Object*                  obj,
			osg::NodeVisitor*             nv
			);

	protected:
		MenuContext * contextMenu;
	};

	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

	typedef struct MENU_ITEM{
		osg::ref_ptr<osgWidget::Label> menuItem;
		osg::ref_ptr<osgWidget::Widget> checkedWidget;
		bool checked;
		OnClickCallback onClickCallback;
		OnHoverCallback onHooverCallback;
	}MENU_ITEM;

	typedef struct MENU_SUBMENU{
		osg::ref_ptr<osgWidget::Label> submenuItem;
		osg::ref_ptr<MenuContext> submenu;
		osg::ref_ptr<osgWidget::Widget> emptyWidget;
	}MENU_SUBMENU;

	typedef std::map<std::string, MENU_SUBMENU> SUBMENUS_MAP;
	typedef std::map<std::string, MENU_ITEM> ITEMS_MAP;

protected:

	MenuContext(MenuContext * parent);

	bool onItemPush(osgWidget::Event& ev);
	bool onItemEnter(osgWidget::Event& ev);
	bool onItemLeave(osgWidget::Event& ev);

	bool onSubmenuEnter(osgWidget::Event& ev);
	bool onSubmenuLeave(osgWidget::Event& ev);

	bool isSubmenuDirectionRight() const;

	MENU_ITEM & findMenuItem(const std::string & path);

	const MENU_ITEM & findMenuItem(const std::string & path) const;

	MENU_SUBMENU & findMenu(const std::string & path);

	const MENU_SUBMENU & findMenu(const std::string & path) const;

	std::vector<std::string> parseNextMenuItems(const std::string & path) const;

	void refreshMenuItemCheckedStyle(const MENU_ITEM & menuItem, bool hoovered = false);

private:

	template<class Collection>
	bool addMenuItem(const Collection & path, bool checked, const OnClickCallback & onClickCallback, const OnHoverCallback & onHooverCallback = OnHoverCallback());

	template<class Iter>
	bool addMenuItem(Iter begin, Iter end, bool checked, const OnClickCallback & onClickCallback, const OnHoverCallback & onHooverCallback = OnHoverCallback());

	template<class Collection>
	bool removeMenuItem(const Collection & path);

	template<class Iter>
	bool removeMenuItem(Iter begin, Iter end);

	template<class Collection>
	bool setMenuItemChecked(const Collection & path, bool checked);

	template<class Iter>
	bool setMenuItemChecked(Iter begin, Iter end, bool checked);

	template<class Collection>
	bool setMenuItemOnClickCallback(const Collection & path, const OnClickCallback & callback);

	template<class Iter>
	bool setMenuItemOnClickCallback(Iter begin, Iter end, const OnClickCallback & callback);

	template<class Collection>
	bool setMenuItemOnHooverCallback(const Collection & path, const OnHoverCallback & callback);

	template<class Iter>
	bool setMenuItemOnHooverCallback(Iter begin, Iter end, const OnHoverCallback & callback);

	template<class Collection>
	bool setMenuOnCloseCallback(const Collection & path, const OnCloseCallback & callback);

	template<class Iter>
	bool setMenuOnCloseCallback(Iter begin, Iter end, const OnCloseCallback & callback);

	template<class Collection>
	const OnClickCallback & getMenuItemOnClickCallback(const Collection & path) const;

	template<class Iter>
	const OnClickCallback & getMenuItemOnClickCallback(Iter begin, Iter end) const;

	template<class Collection>
	const OnHoverCallback & getMenuItemOnHooverCallback(const Collection & path) const;

	template<class Iter>
	const OnHoverCallback & getMenuItemOnHooverCallback(Iter begin, Iter end) const;

	template<class Collection>
	const OnCloseCallback & getMenuOnCloseCallback(const Collection & path) const;

	template<class Iter>
	const OnCloseCallback & getMenuOnCloseCallback(Iter begin, Iter end) const;

	template<class Collection>
	bool getMenuItemChecked(const Collection & path) const;

	template<class Iter>
	bool getMenuItemChecked(Iter begin, Iter end) const;

	template<class Collection>
	MENU_ITEM & findMenuItem(const Collection & path);

	template<class Iter>
	MENU_ITEM & findMenuItem(Iter begin, Iter end);

	template<class Collection>
	const MENU_ITEM & findMenuItem(const Collection & path) const;

	template<class Iter>
	const MENU_ITEM & findMenuItem(Iter begin, Iter end) const;

	template<class Collection>
	MENU_SUBMENU & findMenu(const Collection & path);

	template<class Iter>
	MENU_SUBMENU & findMenu(Iter begin, Iter end);

	template<class Collection>
	const MENU_SUBMENU & findMenu(const Collection & path) const;

	template<class Iter>
	const MENU_SUBMENU & findMenu(Iter begin, Iter end) const;

protected:

	osgUI::MenuContext* rootMenu;
	osgUI::MenuContext* parentMenu;
	osgUI::MenuContext* activeSubMenu;
	
	ITEMS_MAP items;
	SUBMENUS_MAP submenus;

	std::string separators;

	OnCloseCallback closeMenuCallback;

	osg::ref_ptr<CloseMenuContextEvent> closeMenuEventHandler;

	const MENU_ITEM constEmptyMenuItem;
	const MENU_SUBMENU constEmptyMenuSubmenu;
	MENU_ITEM emptyMenuItem;
	MENU_SUBMENU emptyMenuSubmenu;
};

}

#endif

