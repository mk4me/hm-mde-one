/*******************************************************************
	created:  2011/03/14
	created:  14:3:2011   16:49
	filename: IVisualizer.h
	author:	  Piotr Gwiazdowski

	purpose:
	*********************************************************************/
#ifndef HEADER_GUARD_CORE__IVISUALIZER_H__
#define HEADER_GUARD_CORE__IVISUALIZER_H__

#include <string>
#include <mutex>
#include <utils/SmartPtr.h>
#include <corelib/Variant.h>
#include <corelib/IIdentifiable.h>
#include <QtGui/QPixmap>

class QObject;
class QWidget;
class QIcon;

namespace plugin
{
	//! Interfejs wizualizatorów dla typów danych zarejestrowanych w aplikacji
	class IVisualizer : public ICoreElement
	{
	public:

		//! !!UWAGA!! Wszystykie operacje na seriach danych muszą być robione z poziomu UI, wyjątkiem jest update
		class ISerie
		{
		private:
			//! Metoda wywoływana na seri danych kiedy dane się zmienią - wymusza odrysowanie
			virtual void update() = 0;

		protected:
			ISerie() : update_(true) {}
		public:
			//! Destrutkor prywatny
			virtual ~ISerie() {}
			//! \param name Nazwa serii danych do ustawienia
			virtual void setName(const std::string & name) = 0;
			//! \return Nazwa serii danych
			virtual const std::string getName() const = 0;
			//! \return Dane serii
			virtual const core::VariantConstPtr & getData() const = 0;
			//! \return Typ, jak mają być traktowane dane - może chcę pokazać dane z niższego typu
			virtual const utils::TypeInfo & getRequestedDataType() const = 0;
			//! Próbujemy odświeżać dane jeśli jest to konieczne
			void tryUpdate()
			{
				bool needUpdate = false;
				{
					std::lock_guard<std::recursive_mutex> lock(synch_);
					needUpdate = update_;
					update_ = false;
				}

				if (needUpdate == true){
					update();					
				}
			}

			//! Metoda zaznacza potrzebę odświeżenia serii danych
			void requestUpdate()
			{
				std::lock_guard<std::recursive_mutex> lock(synch_);
				update_ = true;
			}

		private:
			std::recursive_mutex synch_;
			bool update_;
		};
		//! Seria o charakterze czasowym - pozwala manipulować czasem w serii danych
		//! setTime może być wołane poza watkiem UI - trzeba to uwzględniać przy update
		class ITimeAvareSerieFeatures
		{
		public:
			//! Destruktor wirtualny
			virtual ~ITimeAvareSerieFeatures() {}

			//! \param time Aktualny czas serii - najprawdopodobniej timeline będzie go ustawiał
			virtual void setTime(double time) = 0;
			//! \return Czas trwania serii
			virtual double getLength() const = 0;
			//! \return Początek czasu
			virtual double getBegin() const = 0;
			//! \return Koniec czasu
			virtual double getEnd() const = 0;
		};

		//! Dla serii o charakterze czasowym - czy można edytować skalę czasu i offset
		class ITimeEditableSerieFeatures
		{
		public:
			//! Destruktor wirtualny
			virtual ~ITimeEditableSerieFeatures() {}

			//! \param offset Przesunięcie serii względem aktualnego stanu (pierwsze względem 0)
			virtual void setOffset(double offset) = 0;
			//! \param scale Zmienia skalę serii danych
			virtual void setScale(double scale) = 0;
		};

	public:
		//! Destruktor wirtualny
		virtual ~IVisualizer() {}

		//! Przeładowana - tworzy nowy wizualizator
		virtual IVisualizer * create() const = 0;

		//! Tylko tutaj powinno następować tworzenie widgetu. Metoda wywoływana tylko jeden raz.
		//! To wizualizator musi niszczyć widget w destruktorze. Gdy widget jest równocześnie
		//! widokiem OSG to wystarczy przypisać go do osg::ref_ptr.
		//! \return Widget będący korzeniem wizualizacji
		virtual QWidget* createWidget() = 0;
		//! Tworzy ikonę dla zadanego wizualizatora. Może zwracać nullptr, chociaż to niewskazane.
		//! W odróżnieniu od createWidget ikona przejmowana jest na własność.
		virtual QIcon* createIcon() = 0;
		//! \return Screenshot z wizualizatora
		virtual QPixmap takeScreenshot() const = 0;
		//! Aktualizacja wyświetlania. NIE aktualizacja stanu wyświetlanych danych.
		//! \param deltaTime Zmiana czasu od ostatniego update
		virtual void update(double deltaTime) = 0;
		//! \param requestedType Typ danych który faktycznie ma być przedstawiony w serii
		//! \param data Dane które będą brane do wizualizaowania w serii
		//! \return Seria danych która można ustawiac - nazwa i dane, nie zarządza ta seria danych - czasem jej zycia, my zwalniamy jej zasoby!!
		virtual ISerie* createSerie(const utils::TypeInfo & requestedType, const core::VariantConstPtr & data) = 0;
		//! \param serie Seria bazowa której konfigurację powielamy
		//! \return Nowa seria danych na bazie zadanej serii - powiela odwołąnie do danych
		virtual ISerie* createSerie(const ISerie* serie, const utils::TypeInfo & requestedType, const core::VariantConstPtr & data) = 0;
		//! \param serie Seria bazowa której konfigurację powielamy
		//! \return Nowa seria danych na bazie zadanej serii - powiela odwołąnie do danych
		virtual ISerie* createSerie(const ISerie* serie) = 0;

		//! \param serie Seria danych do usunięcia, nie powinien usuwać tej serii! Zarządzamy nią my!!
		virtual void removeSerie(ISerie* serie) = 0;

		//! Ustawia daną serię aktywną
		virtual void setActiveSerie(ISerie * serie) = 0;
		//! \return Pobiera aktywną serię, nullptr gdy nie ma żadnej aktywnej
		virtual const ISerie * getActiveSerie() const = 0;
		//! \return Pobiera aktywną serię, nullptr gdy nie ma żadnej aktywnej
		virtual ISerie * getActiveSerie() = 0;

		//! \param supportedTypes [out] Lista typów wspieranych przez dany wizualizator, z których potrafi stworzyć serię danych
		virtual void getSupportedTypes(utils::TypeInfoList & supportedTypes) const = 0;
		//----------------- Obsługa serii danych ---------------------
		//! Wizualizator musi zapewnic ze da się stworzyć MAX serii za pomoca metody createSerie. Ilos serii jest dynamicznie zarządzana z zewnątrz poprzez create i remove serie.
		//! ISerie to klasa delegata, która implementuje specyficzne dla danego wizualizatora operacje ustawiania nazwy serii i jej danych. Kazdy wizualizator może inaczej ustawiac te informacje i prezentować je.
		//! \return Maksymalna ilość serii danych jaka może obsłużyc wizualizator, wartość 0 i mniej oznacza maksumalny zakres dla INT, inna wartość stanowi górna granice
		virtual int getMaxDataSeries() const = 0;
	};

	DEFINE_SMART_POINTERS(IVisualizer);
} // namespace plugin

#endif  // HEADER_GUARD_CORE__IVISUALIZER_H__
