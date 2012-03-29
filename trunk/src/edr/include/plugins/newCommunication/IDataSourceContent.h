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
	//! Typ zawarto�ci
	enum ContentType {
		DisorderContent,		// schorzenie
		PatientContent,			// pacjent
		SubjectContent,			// subject
		SessionContent,			// sesja
		MotionContent,			// motion
		FileContent,			// plik
		DisordersGroupContent,	// grupa schorze�
		PatientsGroupContent,	// grupa pacjent�w
		SubjectsGroupContent,	// grupa subject�w
		SessionsGroupContent,	// grupa sesji
		MotionsGroupContent,	// grupa motion�w
		FilesGroupContent,		// grupa plik�w
		CustomContent			// w�asna zawarto��
	};

	class IContent
	{
	public:

		virtual ~IContent() {}
		virtual ContentType contentType() const = 0;
	};
	
	//! Wzorzec elementu drzewa danych. U�ywany do przechowywania informacji o pacjentach, subjectach, sesjach, motionach i plikach
	template<class T>
	class Item : public QTreeWidgetItem, public IContent
	{
	public:
		//! Typ przechowywanych danych
		typedef T value_type;

	public:

		//! Konstruktor
		//! \param value Warto�� przechowywanych danych
		Item(const value_type * value) : value_(value)
		{

		}

		//! Destruktor wirtualny
		~Item()
		{

		}

		//! \return Typ zawarto�ci reprezentowanej przez ten element - w�asne RTTI
		virtual ContentType contentType() const
		{
			return CustomContent;
		}

		//! \return Warto�� przechowywanych danych
		const value_type* value() const
		{
			return value_;
		}

	private:
		//! Warto�� przechowywanych danych
		const value_type * value_;
	};


	//! Wzorzec klasy agreguj�cej elementy danego typu
	//! Patrz opis wzorca Item
	template<class T>
	class ItemGroup : public QTreeWidgetItem, public IContent
	{
	public:
		//! Typ kolekcji agreguj�cej elementy
		typedef std::set<T*> value_type;

	public:
		//! \param value Elementu kt�re opisujemy
		ItemGroup(const value_type & value) : value_(value)
		{

		}

		//! Destruktor
		~ItemGroup()
		{

		}

		//! \return Typ zawarto�ci reprezentowanej przez ten element - w�asne RTTI
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
	ContentType Item<webservices::MedicalShallowCopy::Patient>::contentType() const
	{
		return PatientContent;
	}

	template<>
	ContentType Item<webservices::MedicalShallowCopy::Disorder>::contentType() const
	{
		return DisorderContent;
	}

	template<>
	ContentType Item<webservices::MotionShallowCopy::Performer>::contentType() const
	{
		return SubjectContent;
	}

	template<>
	ContentType Item<webservices::MotionShallowCopy::Session>::contentType() const
	{
		return SessionContent;
	}

	template<>
	ContentType Item<webservices::MotionShallowCopy::Trial>::contentType() const
	{
		return MotionContent;
	}

	template<>
	ContentType Item<webservices::MotionShallowCopy::File>::contentType() const
	{
		return FileContent;
	}

	template<>
	ContentType ItemGroup<webservices::MedicalShallowCopy::Patient>::contentType() const
	{
		return PatientsGroupContent;
	}

	template<>
	ContentType ItemGroup<webservices::MedicalShallowCopy::Disorder>::contentType() const
	{
		return DisordersGroupContent;
	}

	template<>
	ContentType ItemGroup<webservices::MotionShallowCopy::Performer>::contentType() const
	{
		return SubjectsGroupContent;
	}

	template<>
	ContentType ItemGroup<webservices::MotionShallowCopy::Session>::contentType() const
	{
		return SessionsGroupContent;
	}

	template<>
	ContentType ItemGroup<webservices::MotionShallowCopy::Trial>::contentType() const
	{
		return MotionsGroupContent;
	}

	template<>
	ContentType ItemGroup<webservices::MotionShallowCopy::File>::contentType() const
	{
		return FilesGroupContent;
	}

	//! Definicje typ�w
	typedef Item<webservices::MotionShallowCopy::Performer> SubjectItem;
	typedef Item<webservices::MotionShallowCopy::Session> SessionItem;
	typedef Item<webservices::MotionShallowCopy::Trial> MotionItem;
	typedef Item<webservices::MotionShallowCopy::File> FileItem;

	typedef Item<webservices::MedicalShallowCopy::Patient> PatientItem;
	typedef Item<webservices::MedicalShallowCopy::Disorder> DisorderItem;

	typedef ItemGroup<webservices::MotionShallowCopy::Performer> SubjectsGroupItem;
	typedef ItemGroup<webservices::MotionShallowCopy::Session> SessionsGroupItem;
	typedef ItemGroup<webservices::MotionShallowCopy::Trial> MotionsGroupItem;
	typedef ItemGroup<webservices::MotionShallowCopy::File> FilesGroupItem;

	typedef ItemGroup<webservices::MedicalShallowCopy::Patient> PatientsGroupItem;
	typedef ItemGroup<webservices::MedicalShallowCopy::Disorder> DisordersGroupItem;

	//! Interfejs podstawowej funkcjonalno�ci generator�w zawarto�ci - zar�wno perspektywa jak i generatory go implementuj�
	class IDataSourceContentFunctionality
	{
	public:
		//! Wirtualny destruktor
		virtual ~IDataSourceContentFunctionality() {}
		//! \param item Element kt�ry chcemy wype�ni� w�a�ciw� zawarto�ci�
		//! \param shallowCopy P�ytka kopia zwi�zana z aktualnym kontekstem - mo�na z niej pobra� dodatkowe info
		//! \return Czy wype�nilismy element
		virtual bool fillContent(QTreeWidgetItem * item, const ShallowCopy & shallowCopy) const = 0;
		//! \param item Element dla kt�rego chcemy pobra� nag��wek
		//! \param headers [out] Nag�owek dla zadanego elementu
		//! \return Czy wype�nilismy nag��wek
		virtual bool headers(const QTreeWidgetItem * item, QStringList & headers) const = 0;
		//! \param item Element dla kt�rego chcemy pobra� pole w kt�rym zamie�cimy jego status
		//! \reuturn Numer pola ze statusem
		virtual int statusField(const QTreeWidgetItem * item) const = 0;
		//! \param item Element dla kt�rego chcemy pobra� pole w kt�rym zamie�cimy info o aktualnym pobieraniu
		//! \reuturn Numer pola z info o pobieraniu
		virtual int downloadField(const QTreeWidgetItem * item) const = 0;
		//! Czy�cimy dane contentu (np. bo zmieniamy perspektyw�)
		virtual void release() = 0;
		//! \param c Znak symbolizuj�cy p�e� - z webservices::MedicalShallowCopy::Patient, m,M => M�czyzna, f,F,w,W,k,K => Kobieta
		//! \return Przet�umaczona nazwa p�ci
		static QString decodeGender(char c)
		{
			QChar g(c);
			g = g.toUpper();

			QString gender;

			switch(g.toAscii()){
			case 'M':
				gender = QObject::tr("Man");
				break;
			case 'K':
			case 'F':
			case 'W':
				gender = QObject::tr("Woman");
				break;
			default:
				gender = QObject::tr("Not specified");
				break;
			}

			return gender;
		}
	};

	//! Interfejc w�a�ciwy contentu - dodaje nazw� contentu do jego podstawowej funkcjonalno�ci
	class IDataSourceContent : public IDataSourceContentFunctionality
	{
	public:
		//! Wirtualny destruktor
		virtual ~IDataSourceContent() {}
		//! \return Nazwa contentu
		virtual const std::string name() const = 0;
	};

	typedef core::shared_ptr<IDataSourceContent> DataSourceContentPtr;
}

#endif	//	HEADER_GUARD___IDATASOURCECONTENT_H__
