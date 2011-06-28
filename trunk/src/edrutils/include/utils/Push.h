/********************************************************************
    created:  2011/04/22
    created:  22:4:2011   10:05
    filename: Push.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_UTILS__PUSH_H__
#define HEADER_GUARD_UTILS__PUSH_H__

namespace utils 
{
    /**
     *	U�ytkowa klasa, u�ywana tam, gdzie m�cz�ca jest zmiana warto�ci jakiej� zmiennej
     *  a nast�pnie r�czne jej przywracanie; jest to uci��liwe zw�aszcza gdy funkcja ma
     *  wiele punkt�w wyj�cia.
     */
    template <class T>
    class Push
    {
    private:
        //! Poprzednia warto��.
        T oldValue;
        //! Zmienna.
        T& variable;
    public:
        //! \param variable
        //! \param newValue
        Push(T& variable, const T& newValue) : oldValue(variable), variable(variable)
        {
            variable = newValue;
        }
        //! \param variable
        //! \param newValue
        Push(T& variable, T&& newValue) : oldValue(variable), variable(variable)
        {
            variable = newValue;
        }
        //! 
        ~Push()
        {
            variable = oldValue;
        }
    private:
        //! Ukrycie konstruktora kopiuj�cego.
        Push(const Push& push);
        /**
         *	Prze�adowania operator�w new-delete. Prywatne, aby niemo�liwe by�o
         *  tworzenie instancji typu na stercie.
         */     
        void *operator new(size_t bytes);
        //! \see LogInitializer::operator new
        void *operator new[](size_t bytes);
        //! \see LogInitializer::operator new
        void operator delete(void* p);
        //! \see LogInitializer::operator new
        void operator delete[](void* p);
    };

} // namespace utils


#endif  // HEADER_GUARD_UTILS__PUSH_H__