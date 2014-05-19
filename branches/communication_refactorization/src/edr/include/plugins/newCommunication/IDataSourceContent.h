/********************************************************************
	created:  2012/03/23
	created:  23:3:2012   13:47
	filename: IDataSourceContent.h
	author:   Mateusz Janiak

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCECONTENT_H__
#define HEADER_GUARD___IDATASOURCECONTENT_H__

#include <plugins/newCommunication/DataSourceShallowCopy.h>
#include <QtGui/QTreeWidgetItem>

namespace communication
{
	//! Typ zawartości
	enum ContentType {
		DisorderContent,		// schorzenie
		PatientContent,			// pacjent
		SubjectContent,			// subject
		SessionContent,			// sesja
		MotionContent,			// motion
		FileContent,			// plik
		DisordersGroupContent,	// grupa schorzeń
		PatientsGroupContent,	// grupa pacjentów
		SubjectsGroupContent,	// grupa subjectów
		SessionsGroupContent,	// grupa sesji
		MotionsGroupContent,	// grupa motionów
		FilesGroupContent,		// grupa plików
		CustomContent			// własna zawartość
	};

	//! Interfejs bazowy dla elementów realizujących zawartość
	class IContent
	{
	public:
		//! Wirtualny destruktor
		virtual ~IContent() {}
		//! \return Typ contentu zawieranego przez dany element
		virtual ContentType contentType() const = 0;
	};

	//! Wzorzec elementu drzewa danych. Używany do przechowywania informacji o pacjentach, subjectach, sesjach, motionach i plikach
	template<class T>
	class Item : public QTreeWidgetItem, public IContent
	{
	public:
		//! Typ przechowywanych danych
		typedef T value_type;

	public:

		//! Konstruktor
		//! \param value Wartość przechowywanych danych
		Item(const value_type * value) : value_(value)
		{
		}

		//! Destruktor wirtualny
		~Item()
		{
		}

		//! \return Typ zawartości reprezentowanej przez ten element - własne RTTI
		virtual ContentType contentType() const
		{
			return CustomContent;
		}

		//! \return Wartość przechowywanych danych
		const value_type* value() const
		{
			return value_;
		}

	private:
		//! Wartość przechowywanych danych
		const value_type * value_;
	};

	//! Wzorzec klasy agregującej elementy danego typu
	//! Patrz opis wzorca Item
	template<class T>
	class ItemGroup : public QTreeWidgetItem, public IContent
	{
	public:
		//! Typ kolekcji agregującej elementy
		typedef std::set<T*> value_type;

	public:
		//! \param value Elementu które opisujemy
		ItemGroup(const value_type & value) : value_(value)
		{
		}

		//! Destruktor
		~ItemGroup()
		{
		}

		//! \return Typ zawartości reprezentowanej przez ten element - własne RTTI
		virtual ContentType contentType() const
		{
			return CustomContent;
		}

		const value_type & value() const
		{
			return value_;
		}

	private:
		value_type value_;
	};

	//! Specjalizacje
	template<>
	inline ContentType Item<hmdbServices::MedicalShallowCopy::Patient>::contentType() const
	{
		return PatientContent;
	}

	template<>
	inline ContentType Item<hmdbServices::MedicalShallowCopy::Disorder>::contentType() const
	{
		return DisorderContent;
	}

	template<>
	inline ContentType Item<hmdbServices::MotionShallowCopy::Performer>::contentType() const
	{
		return SubjectContent;
	}

	template<>
	inline ContentType Item<hmdbServices::MotionShallowCopy::Session>::contentType() const
	{
		return SessionContent;
	}

	template<>
	inline ContentType Item<hmdbServices::MotionShallowCopy::Trial>::contentType() const
	{
		return MotionContent;
	}

	template<>
	inline ContentType Item<hmdbServices::MotionShallowCopy::File>::contentType() const
	{
		return FileContent;
	}

	template<>
	inline ContentType ItemGroup<hmdbServices::MedicalShallowCopy::Patient>::contentType() const
	{
		return PatientsGroupContent;
	}

	template<>
	inline ContentType ItemGroup<hmdbServices::MedicalShallowCopy::Disorder>::contentType() const
	{
		return DisordersGroupContent;
	}

	template<>
	inline ContentType ItemGroup<hmdbServices::MotionShallowCopy::Performer>::contentType() const
	{
		return SubjectsGroupContent;
	}

	template<>
	inline ContentType ItemGroup<hmdbServices::MotionShallowCopy::Session>::contentType() const
	{
		return SessionsGroupContent;
	}

	template<>
	inline ContentType ItemGroup<hmdbServices::MotionShallowCopy::Trial>::contentType() const
	{
		return MotionsGroupContent;
	}

	template<>
	inline ContentType ItemGroup<hmdbServices::MotionShallowCopy::File>::contentType() const
	{
		return FilesGroupContent;
	}

	//! Definicje typów
	typedef Item<hmdbServices::MotionShallowCopy::Performer> SubjectItem;
	typedef Item<hmdbServices::MotionShallowCopy::Session> SessionItem;
	typedef Item<hmdbServices::MotionShallowCopy::Trial> MotionItem;
	typedef Item<hmdbServices::MotionShallowCopy::File> FileItem;

	typedef Item<hmdbServices::MedicalShallowCopy::Patient> PatientItem;
	typedef Item<hmdbServices::MedicalShallowCopy::Disorder> DisorderItem;

	typedef ItemGroup<hmdbServices::MotionShallowCopy::Performer> SubjectsGroupItem;
	typedef ItemGroup<hmdbServices::MotionShallowCopy::Session> SessionsGroupItem;
	typedef ItemGroup<hmdbServices::MotionShallowCopy::Trial> MotionsGroupItem;
	typedef ItemGroup<hmdbServices::MotionShallowCopy::File> FilesGroupItem;

	typedef ItemGroup<hmdbServices::MedicalShallowCopy::Patient> PatientsGroupItem;
	typedef ItemGroup<hmdbServices::MedicalShallowCopy::Disorder> DisordersGroupItem;

	//! Interfejs podstawowej funkcjonalności generatorów zawartości - zarówno perspektywa jak i generatory go implementują
	class IDataSourceContentFunctionality
	{
	public:
		//! Wirtualny destruktor
		virtual ~IDataSourceContentFunctionality() {}
		//! \param item Element który chcemy wypełnić właściwą zawartością
		//! \param shallowCopy Płytka kopia związana z aktualnym kontekstem - można z niej pobrać dodatkowe info
		//! \return Czy wypełnilismy element
		virtual bool fillContent(QTreeWidgetItem * item, const ShallowCopy & shallowCopy) const = 0;
		//! \param item Element dla którego chcemy pobrać nagłówek
		//! \param headers [out] Nagłowek dla zadanego elementu
		//! \return Czy wypełnilismy nagłówek
		virtual bool headers(const QTreeWidgetItem * item, QStringList & headers) const = 0;
		//! \param item Element dla którego chcemy pobrać pole w którym zamieścimy jego status
		//! \reuturn Numer pola ze statusem
		virtual int statusField(const QTreeWidgetItem * item) const = 0;
		//! \param item Element dla którego chcemy pobrać pole w którym zamieścimy info o aktualnym pobieraniu
		//! \reuturn Numer pola z info o pobieraniu
		virtual int downloadField(const QTreeWidgetItem * item) const = 0;
		//! Czyścimy dane contentu (np. bo zmieniamy perspektywę)
		virtual void release() = 0;
		//! \param c Znak symbolizujący płeć - z hmdbServices::MedicalShallowCopy::Patient, m,M => Mężczyzna, f,F,w,W,k,K => Kobieta
		//! \return Przetłumaczona nazwa płci
		static QString decodeGender(const hmdbServices::xmlWsdl::Gender::Type g)
		{
			QString gender;

			switch (g){
			case hmdbServices::xmlWsdl::Gender::Male:
				gender = QObject::tr("Man");
				break;
			case hmdbServices::xmlWsdl::Gender::Female:
				gender = QObject::tr("Woman");
				break;
			default:
				gender = QObject::tr("Not specified");
				break;
			}

			return gender;
		}
	};

	//! Interfejc właściwy contentu - dodaje nazwę contentu do jego podstawowej funkcjonalności
	class IDataSourceContent : public IDataSourceContentFunctionality
	{
	public:
		//! Wirtualny destruktor
		virtual ~IDataSourceContent() {}
		//! \return Nazwa contentu
		virtual const std::string name() const = 0;
	};

	typedef utils::shared_ptr<IDataSourceContent> DataSourceContentPtr;
}

#endif	//	HEADER_GUARD___IDATASOURCECONTENT_H__
