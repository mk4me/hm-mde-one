#ifndef HEADER_GUARD__KEYBOARDMAPPER_H__
#define HEADER_GUARD__KEYBOARDMAPPER_H__

#include <set>
#include <boost/range.hpp>
#include <osgWidget/ViewerEventHandlers>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

class KeyboardMapper
{
    friend class KeyboardMapperHandler;

public:
    //! Zestaw klawiszy.
	typedef std::set<osgGA::GUIEventAdapter::KeySymbol> Keys;
    typedef Keys::const_iterator const_iterator;
    typedef const_iterator iterator;

private:
    //! Zestaw klawiszy.
    Keys keysPressed;

public:
    //! Konstruktor zeruj�cy
	KeyboardMapper();

protected:
    //! Chroniony wirtualny destruktor - niszczenie tylko polimorficzne!
    virtual ~KeyboardMapper();

public:

    //! \return Liczba wci�ni�tych klawiszy.
    int getNumPressedKeys() const
    {
        return keysPressed.size();
    }

    //! \return Czy zdany klaiwsz jest naci�ni�ty?
	bool isKeyPressed(osgGA::GUIEventAdapter::KeySymbol key) const
    {        
        return keysPressed.find(key) != keysPressed.end();
    }

    //! \return Czy kt�ry� z zadanych klawiszy jest naci�ni�ty?
    template <class Range>
    bool areAnyKeysPressed(const Range& range) const
    {
        return areAnyKeysPressed(boost::begin(range), boost::end(range));
    }

    //! \return Czy kt�ry� z zadanych klawiszy jest naci�ni�ty?
    template <class FwdIter>
    bool areAnyKeysPressed(FwdIter first, FwdIter last) const
    {
        while ( first != last && !isKeyPressed(*first) ) {
            ++first;
        }
        return first != last;
    }

    //! \param range Zakres.
    //! \param sorted Czy zakres jest posortowany?
    //! \return Czy wszystkie zadane klawisze s� naci�ni�te?
    template <class Range>
    bool areKeysPressed(const Range& range, bool sorted = true) const
    {
        return areKeysPressed(boost::begin(range), boost::end(range), sorted);
    }

    //! \param first Pocz�tek zakresu.
    //! \param last Koniec zakresu.
    //! \param sorted Czy zakres jest posortowany?
    //! \return Czy wszystkie zadane klawisze s� naci�ni�te?
    template <class FwdIter>
    bool areKeysPressed(FwdIter first, FwdIter last, bool sorted = true) const
    {
        // TODO: rozwa�y�, czy wariant z includes w og�le jest przydatny
        if (sorted) {
            return std::includes(begin(), end(), first, last);
        } else {
            while ( first != last && isKeyPressed(*first) ) {
                ++first;
            }
            return first == last;
        }
    }

    //! \return Wszystkie naci�ni�te klawisze.
	const Keys & getPressedKeys() const;

    //! \return Pocz�tek wyliczenia klawiszy.
    const_iterator begin() const
    {
        return keysPressed.begin();
    }
    //! \return Koniec wyliczenia klawiszy.
    const_iterator end() const
    {
        return keysPressed.end();
    }

protected:
	void keyPressed(osgGA::GUIEventAdapter::KeySymbol key);
	void keyReleased(osgGA::GUIEventAdapter::KeySymbol key);
};

////////////////////////////////////////////////////////////////////////////////
} // namespace osgui
////////////////////////////////////////////////////////////////////////////////

#endif
