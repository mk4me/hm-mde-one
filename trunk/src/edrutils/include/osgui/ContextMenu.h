#ifndef HEADER_GUARD__CONTEXTMENU_H__
#define HEADER_GUARD__CONTEXTMENU_H__

#include <osgui/Grid.h>
#include <osgWidget/Label>
#include <osgWidget/Widget>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/tokenizer.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////


/**
 *	Klasa do tworzenia i zarzadzania menu kontekstowym w OSG
 *  Idea jej dzia³ania jest i¿ w œrodku tworz¹ siê klasy tego samego typu, jednak nie widac
 *  ich na zewnatrz.
 */

class ContextMenu :	public osgui::Grid
{
public:
    //! Sygnatura callbacka wywo³ywana przy klikniêciu w opcjê menu.
    //! Pierwszy parametr to pe³na œcie¿ka do opcji, druga to flaga logiczna okreœlaj¹ca,
    //! jaki jest stan opcji (checked/unchecked)
	typedef boost::function<void(const std::string&, bool)> OnClickCallback;
    //! Sygnatura callbacka wywo³ywanego gdy menu zostaje zamkniête.
	typedef boost::function<void(ContextMenu*)> OnCloseCallback;
	//! Sygnatura callbacka wywo³ywana przy najechaniu na opcjê menu.
    //! Pierwszy parametr to pe³na œcie¿ka do opcji, druga to flaga logiczna okreœlaj¹ca,
    //! czy wjechano/zjechano z opcji.
	typedef OnClickCallback OnHoverCallback;

public:
	ContextMenu(void);
	~ContextMenu(void);

// osgWidget::Window
public:
	virtual void managed(osgWidget::WindowManager * wm);
    virtual void update();

public:
    bool isEmpty() const;

    static const std::string & getDefaultPathSeparator();

    //! \return
    const std::string& getPathSeparators() const;
    //! \param separators
    void setPathSeparators(const std::string& separators);

    //! string - sciezka do elementu menu
    //! bool - checked, czy element jest zaznaczony (jak checkbox)
    //! onClickCallback - akcja useraa na klikniecie elementu
    //! onHoverCallback - akcja useraa na najechanie elementu menu, domyslnie pusta
	bool addMenuItem(const std::string & path, bool checked, const OnClickCallback & onClickCallback,
        const OnHoverCallback & onHoverCallback = OnHoverCallback());

    //! string - sciezka do elementu menu, ktory chcemy usunac
    bool removeMenuItem(const std::string & path);
	
    //! string - sciezka do elementu menu, ktory chcemy modyfikowac
    //! bool - checked, stan zaznaczenia danego elementu
	bool setMenuItemChecked(const std::string & path, bool checked);

    //! string - sciezka do elementu menu, ktory chcemy modyfikowac
    //! OnClickCallback - nowa akcja usera na klikniêcie elementu
	bool setMenuItemOnClickCallback(const std::string & path, const OnClickCallback & callback);

    //! string - sciezka do elementu menu, ktory chcemy modyfikowac
    //! OnHoverCallback - nowa akcja usera na najechanie elementu
	bool setMenuItemOnHoverCallback(const std::string & path, const OnHoverCallback & callback);

    //! string - sciezka do submenu, ktory chcemy modyfikowac
    //! OnCloseCallback - nowa akcja usera po zamknieciu danego poziomu menu
	bool setMenuOnCloseCallback(const std::string & path, const OnCloseCallback & callback);

    //! string - sciezka do elementu menu
	const OnClickCallback & getMenuItemOnClickCallback(const std::string & path) const;

    //! string - sciezka do elementu menu
	const OnHoverCallback & getMenuItemOnHoverCallback(const std::string & path) const;

    //! string - sciezka do submenu
	const OnCloseCallback & getMenuOnCloseCallback(const std::string & path) const;

    //! string - sciezka do elementu menu
	bool getMenuItemChecked(const std::string & path) const;

    bool showMenu();

    //! osgWidget::XYCoord - nowa pozycja menu (lewego dolnego rogu)
	bool showMenu(const osgWidget::XYCoord & pos);

    //! osgWidget::point_type x, nowa wspolrzedna x menu (lewego dolnego rogu)
    //! osgWidget::point_type y, nowa wspolrzedna y menu (lewego dolnego rogu)
	bool showMenu(osgWidget::point_type x, osgWidget::point_type y);

    //! chowa menu
	void hideMenu();

    //! czysci menu i chowa je
    void clearMenu();

protected:

    //! definicja tokenizera opartego o boost
	typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;

    //! struktura opisujaca element menu
	typedef struct MenuItem{
    public:
        MenuItem() : checked(false){};

        //! element menu
		osg::ref_ptr<osgWidget::Label> menuItem;

        //! skojarzony z elementem menu obszar jego stanu - pseudo checkbox
		osg::ref_ptr<osgWidget::Widget> checkedWidget;

        //! informacja czy dany element jest zaznaczony czy nie
		bool checked;

        //! akcja zadana przez uzytkownika na klikniecie elementu
		OnClickCallback onClickCallback;

        //! akcja zadana przez uzytkownika na najechanie elementu
		OnHoverCallback onHoverCallback;

	}MenuItem;

    //! struktura opisujaca submenu
	typedef struct MenuSubmenu{

        //! reprezentacja submenu w menu
		osg::ref_ptr<osgWidget::Label> submenuItem;

        //! submenu
		osg::ref_ptr<ContextMenu> submenu;

        //! atrapa checkboxa, zaslepka wzgledem elementu menu
		osg::ref_ptr<osgWidget::Widget> emptyWidget;

	}MenuSubmenu;

    //! mapa nazwy submenu z obiektem opisujacym submenu
	typedef std::map<std::string, MenuSubmenu> Submenus;

    //! mapa nazwy elementu z obiektem opisujacym element menu
	typedef std::map<std::string, MenuItem> Items;

protected:

    //! ContextMenu - rodzic aktualnego menu, ukryty na potrzeby klasy
	ContextMenu(ContextMenu * parent);

	bool onItemPush(osgWidget::Event& ev);
	bool onItemEnter(osgWidget::Event& ev);
	bool onItemLeave(osgWidget::Event& ev);

	bool onSubmenuEnter(osgWidget::Event& ev);
	bool onSubmenuLeave(osgWidget::Event& ev);

    //! sprawdza po ktorej stronie wzgledem rodzica powinno pojawic sie submenu
	bool isSubmenuDirectionRight() const;

    //! string - sciezka do elementu menu
	MenuItem & findMenuItem(const std::string & path);

    //! string - sciezka do elementu menu
	const MenuItem & findMenuItem(const std::string & path) const;

    //! string - sciezka do submenu
	MenuSubmenu & findMenu(const std::string & path);

    //! string - sciezka do submenu
	const MenuSubmenu & findMenu(const std::string & path) const;

    //! string - sciezka do elementu menu
	std::vector<std::string> parseNextMenuItems(const std::string & path) const;

    //! MenuItem - opis elementu menu, ktory odswiezamy
    //! bool - informacja o tym czy myszka nad elementem
	void refreshMenuItemCheckedStyle(const MenuItem & menuItem, bool hovered = false);

private:


    //! Collection - kolekajca z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
    //! bool - checked, czy element jest zaznaczony (jak checkbox)
    //! onClickCallback - akcja useraa na klikniecie elementu
    //! onHoverCallback - akcja useraa na najechanie elementu menu, domyslnie pusta
	template<class Collection>
	bool addMenuItem(const Collection & path, bool checked, const OnClickCallback & onClickCallback,
        const OnHoverCallback & onHoverCallback = OnHoverCallback());

    //! Iter begin - poczatek kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
    //! Iter end - koniec kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
    //! bool - checked, czy element jest zaznaczony (jak checkbox)
    //! onClickCallback - akcja useraa na klikniecie elementu
    //! onHoverCallback - akcja useraa na najechanie elementu menu, domyslnie pusta
	template<class Iter>
	bool addMenuItem(Iter begin, Iter end, bool checked, const OnClickCallback & onClickCallback,
        const OnHoverCallback & onHoverCallback = OnHoverCallback());

    //! Collection - kolekajca z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu), ktory chemy usunac
	template<class Collection>
	bool removeMenuItem(const Collection & path);

    //! Iter begin - poczatek kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
    //! Iter end - koniec kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
	template<class Iter>
	bool removeMenuItem(Iter begin, Iter end);

    //! Collection - kolekajca z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu), ktory chemy usunac
    //! bool checked, nowy stan danego elementu
	template<class Collection>
	bool setMenuItemChecked(const Collection & path, bool checked);

    //! Iter begin - poczatek kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
    //! Iter end - koniec kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
    //! bool checked, nowy stan danego elementu
	template<class Iter>
	bool setMenuItemChecked(Iter begin, Iter end, bool checked);

    //! Collection - kolekajca z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu), ktory chemy usunac
    //! OnClickCallback nowa akcja uzytkownika po kliknieciu danego elementu
	template<class Collection>
	bool setMenuItemOnClickCallback(const Collection & path, const OnClickCallback & callback);

    //! Iter begin - poczatek kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
    //! Iter end - koniec kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
    //! OnClickCallback nowa akcja uzytkownika po kliknieciu danego elementu
	template<class Iter>
	bool setMenuItemOnClickCallback(Iter begin, Iter end, const OnClickCallback & callback);

    //! Collection - kolekajca z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu), ktory chemy usunac
    //! OnHoverCallback nowa akcja uzytkownika po najechaniu myszka danego elementu
	template<class Collection>
	bool setMenuItemOnHoverCallback(const Collection & path, const OnHoverCallback & callback);

    //! Iter begin - poczatek kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
    //! Iter end - koniec kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
    //! OnHoverCallback nowa akcja uzytkownika po najechaniu myszka danego elementu
	template<class Iter>
	bool setMenuItemOnHoverCallback(Iter begin, Iter end, const OnHoverCallback & callback);

    //! Collection - kolekajca z kolejnymi poziomami submenu
    //! OnCloseCallback nowa akcja uzytkownika na zamkniecie danego submenu
	template<class Collection>
	bool setMenuOnCloseCallback(const Collection & path, const OnCloseCallback & callback);

    //! Iter begin - poczatek kolekcji z kolejnymi submenu
    //! Iter end - koniec kolekcji z kolejnymi submenu
    //! OnCloseCallback nowa akcja uzytkownika na zamkniecie danego submenu
	template<class Iter>
	bool setMenuOnCloseCallback(Iter begin, Iter end, const OnCloseCallback & callback);

    //! Collection - kolekajca z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
	template<class Collection>
	const OnClickCallback & getMenuItemOnClickCallback(const Collection & path) const;

    //! Iter begin - poczatek kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
    //! Iter end - koniec kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
	template<class Iter>
	const OnClickCallback & getMenuItemOnClickCallback(Iter begin, Iter end) const;

    //! Collection - kolekajca z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
	template<class Collection>
	const OnHoverCallback & getMenuItemOnHoverCallback(const Collection & path) const;

    //! Iter begin - poczatek kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
    //! Iter end - koniec kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
	template<class Iter>
	const OnHoverCallback & getMenuItemOnHoverCallback(Iter begin, Iter end) const;

    //! Collection - kolekajca z kolejnymi poziomami submenu
	template<class Collection>
	const OnCloseCallback & getMenuOnCloseCallback(const Collection & path) const;

    //! Iter begin - poczatek kolekcji z kolejnymi submenu
    //! Iter end - koniec kolekcji z kolejnymi submenu
	template<class Iter>
	const OnCloseCallback & getMenuOnCloseCallback(Iter begin, Iter end) const;

    //! Collection - kolekajca z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
	template<class Collection>
	bool getMenuItemChecked(const Collection & path) const;

    //! Iter begin - poczatek kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
    //! Iter end - koniec kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
	template<class Iter>
	bool getMenuItemChecked(Iter begin, Iter end) const;

    //! Collection - kolekajca z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
	template<class Collection>
	MenuItem & findMenuItem(const Collection & path);

    //! Iter begin - poczatek kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
    //! Iter end - koniec kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
	template<class Iter>
	MenuItem & findMenuItem(Iter begin, Iter end);

    //! Collection - kolekajca z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
	template<class Collection>
	const MenuItem & findMenuItem(const Collection & path) const;

    //! Iter begin - poczatek kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
    //! Iter end - koniec kolekcji z kolejnymi elementami menu (ostatni to element, inne po drodze to submenu)
	template<class Iter>
	const MenuItem & findMenuItem(Iter begin, Iter end) const;

    //! Collection - kolekajca z kolejnymi poziomami submenu
	template<class Collection>
	MenuSubmenu & findMenu(const Collection & path);

    //! Iter begin - poczatek kolekcji z kolejnymi submenu
    //! Iter end - koniec kolekcji z kolejnymi submenu
	template<class Iter>
	MenuSubmenu & findMenu(Iter begin, Iter end);

    //! Collection - kolekajca z kolejnymi poziomami submenu
	template<class Collection>
	const MenuSubmenu & findMenu(const Collection & path) const;

    //! Iter begin - poczatek kolekcji z kolejnymi submenu
    //! Iter end - koniec kolekcji z kolejnymi submenu
	template<class Iter>
	const MenuSubmenu & findMenu(Iter begin, Iter end) const;

protected:

    //! osg::observer_ptr<osgui::ContextMenu> glowa hierarchi menu kontekstowego, menu najwyzszego poziomu
    //! przez nie user ma dostep do calego menu
	osg::observer_ptr<osgui::ContextMenu> rootMenu;
    
    //! osg::observer_ptr<osgui::ContextMenu> rodzic aktualnego menu, dla ktorego jest to menu submenu
	osg::observer_ptr<osgui::ContextMenu> parentMenu;

    //! osg::observer_ptr<osgui::ContextMenu> aktualnie aktywne submenu
	osg::observer_ptr<osgui::ContextMenu> activeSubMenu;
	
    //! elementy menu na danym poziomie
	Items items;

    //! submenu na dnym poziomie
	Submenus submenus;

    //! Znaki bêd¹ce separatorami.
    boost::shared_ptr<std::string> pathSeparator;

    //! OnCloseCallback akcja uzytkownika na zamkniecie danego menu
	OnCloseCallback closeMenuCallback;

	static const MenuItem constEmptyMenuItem;
	static const MenuSubmenu constEmptyMenuSubmenu;
	static MenuItem emptyMenuItem;
	static MenuSubmenu emptyMenuSubmenu;

    static const std::string defaultPathSeparator;
};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////

#endif  // HEADER_GUARD__CONTEXTMENU_H__

