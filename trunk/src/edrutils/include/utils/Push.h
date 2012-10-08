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
     *	Użytkowa klasa, używana tam, gdzie męcząca jest zmiana wartości jakiejś zmiennej
     *  a następnie ręczne jej przywracanie; jest to uciążliwe zwłaszcza gdy funkcja ma
     *  wiele punktów wyjścia.
     */
    template <class T>
    class Push
    {
    private:
        //! Poprzednia wartość.
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
        //! Ukrycie konstruktora kopiującego.
        Push(const Push& push);
        /**
         *	Przeładowania operatorów new-delete. Prywatne, aby niemożliwe było
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
