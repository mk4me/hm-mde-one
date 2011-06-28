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
     *	U¿ytkowa klasa, u¿ywana tam, gdzie mêcz¹ca jest zmiana wartoœci jakiejœ zmiennej
     *  a nastêpnie rêczne jej przywracanie; jest to uci¹¿liwe zw³aszcza gdy funkcja ma
     *  wiele punktów wyjœcia.
     */
    template <class T>
    class Push
    {
    private:
        //! Poprzednia wartoœæ.
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
        //! Ukrycie konstruktora kopiuj¹cego.
        Push(const Push& push);
        /**
         *	Prze³adowania operatorów new-delete. Prywatne, aby niemo¿liwe by³o
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