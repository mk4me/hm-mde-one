/********************************************************************
	created:  2014/05/05	16:28:08
	filename: ObjectWrapper.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_CORE__OBJECTWRAPPER_H__
#define __HEADER_GUARD_CORE__OBJECTWRAPPER_H__

#include <corelib/Export.h>
#include <utils/SmartPtr.h>
#include <utils/ObjectWrapper.h>
#include <threading/SynchronizationPolicies.h>
#include <list>
#include <set>
#include <vector>

namespace core
{

	class Variant;

	//! Klasa realizuj¹ca inicjalizacjê danych dla OW
	class IVariantInitializer
	{
	public:
		//! Destruktor wirtualny
		virtual ~IVariantInitializer() {}
		//! \param object Obiekt który inicjujemy
		virtual void initialize(Variant * object) = 0;
		//! \return Kopia inicjalizatora
		virtual IVariantInitializer * clone() const = 0;
		//! \param initializer Inicjalizator z ktorym siê porównujemy
		//! \return Czy inicjalizatory dadz¹ takie same dane (czêœc wspólna)
		virtual const bool isEqual(const IVariantInitializer & initializer) const { return false; }
	};

	typedef utils::shared_ptr<IVariantInitializer> VariantInitializerPtr;
	typedef utils::shared_ptr<const IVariantInitializer> VariantInitializerConstPtr;
	typedef utils::weak_ptr<IVariantInitializer> VariantInitializerWeakPtr;
	typedef utils::weak_ptr<const IVariantInitializer> VariantInitializerConstWeakPtr;

	DEFINE_SMART_POINTERS(Variant);
	typedef std::list<VariantPtr> VariantsList;
	typedef std::list<VariantConstPtr> ConstVariantsList;
	typedef std::set<VariantPtr> VariantsSet;
	typedef std::set<VariantConstPtr> ConstVariantsSet;
	typedef std::vector<VariantPtr> VariantsVector;
	typedef std::vector<VariantConstPtr> ConstVariantsVector;
	

	class CORELIB_EXPORT Variant
	{
	private:

		//! Typ przechowuj¹cy metadane OW
		typedef std::map<std::string, std::string> Metadata;
		//! Typ loklanej synchronizacji
		typedef utils::ScopedLock<utils::RecursiveSyncPolicy> ScopedLock;

	public:

		//! Proxy dostarczaj¹ce danych i synchronizuj¹ce je
		struct CORELIB_EXPORT data_t
		{
		private:
			//! Variant dla którego bêdziemy wyci¹gaæ dane
			const Variant* variant;

		private:
			//! Prywatny operator przypisania
			data_t & operator=(const data_t &);

		public:
			//! \param variant Variant z którego wyci¹gamy dane
			data_t(const Variant* variant);
			//! Destruktor niepolimorficzny!!
			~data_t();
			//! Operator wyci¹gaj¹cy dane w formie sta³ej
			const utils::ObjectWrapperConstPtr operator->() const;
		};

		friend struct data_t;

		//! Typ klonowania
		typedef utils::ObjectWrapper::CloneOp CloneOp;

	private:
		//! Inicjalizator danych
		VariantInitializerPtr initializer_;
		//! Czy obiekt by³ ju¿ inicjalizowany
		mutable bool initialized_;
		//! Metadane w formie klucz -> wartoœæ tekstowo
		utils::shared_ptr<Metadata> metadata_;
		//! Faktyczne dane
		utils::ObjectWrapperPtr wrapper_;
		//! Obiekt synchronizuj¹cy
		mutable utils::RecursiveSyncPolicy sync_;
		//! Czy jesteœmy w trakcie inicjalizacji
		mutable bool initializing_;

	private:

		//! \param ow Kopiowany obiekt
		//! \param co Typ klonowania
		Variant(const Variant & ow, const CloneOp co);

		//! \param wrapper Placeholder dla danych
		Variant(const utils::ObjectWrapperPtr & wrapper);

	public:

		//! \param wrapper Wrapowane dane
		//! \return Pe³ny wrapper z obs³uga metadanych i leniw¹ inicjalizacj¹
		static const VariantPtr create(const utils::ObjectWrapperPtr & wrapper);

		//! \tparam T Typ dla jakiego tworzymy wrapper
		//! \param dummy Nieuzywany
		//! \return Pe³ny wrapper z obs³uga metadanych i leniw¹ inicjalizacj¹
		template<typename T>
		static const VariantPtr create(const T * /*dummy*/ = nullptr)
		{
			return VariantPtr(new Variant(utils::ObjectWrapper::create<T>()));
		}

		template <class Ptr>
		void set(const Ptr& object)
		{
			ScopedLock lock(sync_);
			if (!wrapper_->trySet(object)) {
				throw std::bad_cast();
			}

			tryResetInitializer();
		}

		//! \param wrapper Obiekt z danymi
		void set(const utils::ObjectWrapperPtr & wrapper);

		//! Próba pobrania obiektu z wrappera.
		//! \tparam Ptr Wskaznik o ktory pytamy
		//! \param object Rezultat.
		//! \param exact Czy ma byæ tylko i wy³¹cznie ten typ czy te¿ mo¿e byæ rzutowanie w dó³?
		//! \return Sukces/pora¿ka.
		template <class Ptr>
		const bool tryGet(Ptr& object, bool exact = false)
		{
			initialize();
			return wrapper_->tryGet(object, exact);
		}

		//! Próba pobrania obiektu z wrappera.
		//! \tparam Ptr Wskaznik o ktory pytamy
		//! \param object Rezultat.
		//! \param exact Czy ma byæ tylko i wy³¹cznie ten typ czy te¿ mo¿e byæ rzutowanie w dó³?
		//! \return Sukces/pora¿ka.
		template <class Ptr>
		const bool tryGet(Ptr& object, bool exact = false) const
		{
			initialize();
			return utils::ObjectWrapperConstPtr(wrapper_)->tryGet(object, exact);
		}

		utils::ObjectWrapper::get_t get(const bool exact = false);

		const utils::ObjectWrapper::get_t get(const bool exact = false) const;

		void * getRawPtr();

		const void * getRawPtr() const;

		//! Destruktor niepolimorficzny!!
		~Variant();

		//! \return Pusty obiekt dla danych mojego typu
		const VariantPtr create() const;
		//! \param key Klucz metadanych który sprawdzamy czy istnieje
		//! \return Czy dany klucz istnieje
		const bool existMetadata(const std::string & key) const;
		//! \param key Klucz metadanych który sprawdzamy czy istnieje
		//! \param val [out] Wartoœæ dla zadanego klucza, w przypadku braku klucza zmienna nie zostanie nadpisana
		//! \return Czy udalo sie pobrac wartosc klucza
		const bool getMetadata(const std::string & key, std::string & val) const;
		//! \param key Klucz metadanych
		//! \param val Wartoœæ dla zadanego klucza
		void setMetadata(const std::string & key, const std::string & val);
		//! \param key Klucz ktorego wpis usuwamy
		void removeMetadata(const std::string & key);
		//! \param srcWrapper Zrodlowy OW z ktorego kopiujemy wpisy
		void copyMetadata(const Variant & srcWrapper);
		//! Metoda usuwa wszystkie metadane
		void clearMetadata();

		//! \param initializer Obiekt leniwie inicjuj¹cy wartoœæ OW
		void setInitializer(const VariantInitializerPtr & initializer);
		//! \return Obiekt leniwie inicjuj¹cy wartoœæ OW
		const VariantInitializerConstPtr initializer() const;
		//! \return Czy obiekt by³ inicjalizowany
		const bool initialized() const;
		//! \return Dostêp do danych do odczytu
		const data_t data() const;
		//! \return Dostêp do danych do odczytu po inicjalizacji
		const data_t initializedData() const;
		//! \param co Typ klonowania
		//! \return Kopia naszego obiektu
		const VariantPtr clone(const CloneOp co = utils::ObjectWrapper::DeepClone) const;
		//! \param dest [out] Obiekt docelowy
		//! \param co Typ klonowania
		void clone(Variant & dest, const CloneOp co = utils::ObjectWrapper::DeepClone) const;
		//! Zamienia zawartosc OW jesli to mo¿liwe
		//! \param ow Obiekt z którym zamienimy przechowywane wartoœci
		void swap(Variant & ow);
		//! \param obj Obiekt z którym siê porównujemy
		//! \return Czy obiekty s¹ takie same - trzymaj¹ te same dane
		const bool isEqual(const Variant & obj) const;

	private:
		//! Próbuje resetowaæ inicjalizator kiedy ustawia siê bezpoœrednio dane
		void tryResetInitializer();
		//! Zeruje dane
		void reset();
		//! Metoda próbuje inicjowac obiekt
		void initialize() const;
		//! Prywatny operator kopiowania - nie mo¿na kopiowaæ sync_
		Variant & operator=(const Variant &);
	};
}

////////////////////////////////////////////////////////////////////////////////
// Operatory porównania dla OW
////////////////////////////////////////////////////////////////////////////////

//! Operator poównania realizowany w oparciu o metodê isEqual
bool operator==(const core::Variant & a, const core::Variant & b);

//! Operator ró¿noœci realizowany w oparciu o operator porównania
bool operator!=(const core::Variant & a, const core::Variant & b);

#endif	// __HEADER_GUARD_CORE__OBJECTWRAPPER_H__