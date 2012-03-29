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
	//! Typ zawartoœci
	enum ContentType {
		DisorderContent,		// schorzenie
		PatientContent,			// pacjent
		SubjectContent,			// subject
		SessionContent,			// sesja
		MotionContent,			// motion
		FileContent,			// plik
		DisordersGroupContent,	// grupa schorzeñ
		PatientsGroupContent,	// grupa pacjentów
		SubjectsGroupContent,	// grupa subjectów
		SessionsGroupContent,	// grupa sesji
		MotionsGroupContent,	// grupa motionów
		FilesGroupContent,		// grupa plików
		CustomContent			// w³asna zawartoœæ
	};

	class IContent
	{
	public:

		virtual ~IContent() {}
		virtual ContentType contentType() const = 0;
	};
	
	//! Wzorzec elementu drzewa danych. U¿ywany do przechowywania informacji o pacjentach, subjectach, sesjach, motionach i plikach
	template<class T>
	class Item : public QTreeWidgetItem, public IContent
	{
	public:
		//! Typ przechowywanych danych
		typedef T value_type;

	public:

		//! Konstruktor
		//! \param value Wartoœæ przechowywanych danych
		Item(const value_type * value) : value_(value)
		{

		}

		//! Destruktor wirtualny
		~Item()
		{

		}

		//! \return Typ zawartoœci reprezentowanej przez ten element - w³asne RTTI
		virtual ContentType contentType() const
		{
			return CustomContent;
		}

		//! \return Wartoœæ przechowywanych danych
		const value_type* value() const
		{
			return value_;
		}

	private:
		//! Wartoœæ przechowywanych danych
		const value_type * value_;
	};


	//! Wzorzec klasy agreguj¹cej elementy danego typu
	//! Patrz opis wzorca Item
	template<class T>
	class ItemGroup : public QTreeWidgetItem, public IContent
	{
	public:
		//! Typ kolekcji agreguj¹cej elementy
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

		//! \return Typ zawartoœci reprezentowanej przez ten element - w³asne RTTI
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

	//! Definicje typów
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

	//! Interfejs podstawowej funkcjonalnoœci generatorów zawartoœci - zarówno perspektywa jak i generatory go implementuj¹
	class IDataSourceContentFunctionality
	{
	public:
		//! Wirtualny destruktor
		virtual ~IDataSourceContentFunctionality() {}
		//! \param item Element który chcemy wype³niæ w³aœciw¹ zawartoœci¹
		//! \param shallowCopy P³ytka kopia zwi¹zana z aktualnym kontekstem - mo¿na z niej pobraæ dodatkowe info
		//! \return Czy wype³nilismy element
		virtual bool fillContent(QTreeWidgetItem * item, const ShallowCopy & shallowCopy) const = 0;
		//! \param item Element dla którego chcemy pobraæ nag³ówek
		//! \param headers [out] Nag³owek dla zadanego elementu
		//! \return Czy wype³nilismy nag³ówek
		virtual bool headers(const QTreeWidgetItem * item, QStringList & headers) const = 0;
		//! \param item Element dla którego chcemy pobraæ pole w którym zamieœcimy jego status
		//! \reuturn Numer pola ze statusem
		virtual int statusField(const QTreeWidgetItem * item) const = 0;
		//! \param item Element dla którego chcemy pobraæ pole w którym zamieœcimy info o aktualnym pobieraniu
		//! \reuturn Numer pola z info o pobieraniu
		virtual int downloadField(const QTreeWidgetItem * item) const = 0;
		//! Czyœcimy dane contentu (np. bo zmieniamy perspektywê)
		virtual void release() = 0;
		//! \param c Znak symbolizuj¹cy p³eæ - z webservices::MedicalShallowCopy::Patient, m,M => Mê¿czyzna, f,F,w,W,k,K => Kobieta
		//! \return Przet³umaczona nazwa p³ci
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

	//! Interfejc w³aœciwy contentu - dodaje nazwê contentu do jego podstawowej funkcjonalnoœci
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
