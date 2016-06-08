/********************************************************************
	created:  2014/09/17	17:45:49
	filename: DataViewConfigurationWidget.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_hmdbCommunication__DATAVIEWCONFIGURATIONWIDGET_H__
#define __HEADER_GUARD_hmdbCommunication__DATAVIEWCONFIGURATIONWIDGET_H__

#include <plugins/hmdbCommunication/Export.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QComboBox>
#include <utils/SmartPtr.h>
#include <plugins/hmdbCommunication/ShallowCopyFilter.h>
#include <plugins/hmdbCommunication/IDataSourcePerspective.h>
#include <plugins/hmdbCommunication/IDataSourceContent.h>

namespace Ui
{
	class DataViewConfigurationWidget;
}

class hmdbCommunication_EXPORT DataViewConfigurationWidget : public QWidget
{
	Q_OBJECT

public:

	//! \param parent Widget rodzica
	//! \param f Flagi okna
	DataViewConfigurationWidget(QWidget * parent = nullptr, Qt::WindowFlags f = 0);
	//! Destruktor wirtualny
	virtual ~DataViewConfigurationWidget();

	//! \return Aktualny filtr - mo�e by� nullptr
	hmdbCommunication::ShallowCopyFilter * currentFilter();
	//! \return Aktualna perspektywa danych - nie mo�e by� nullptr
	hmdbCommunication::IDataSourcePerspective * currentPerspective();
	//! \return Aktualny content danych - nie mo�e by� nullptr
	hmdbCommunication::IDataSourceContent * currentContent();

	//! \param filter Rejestrowany filtr
	void registerFilter(hmdbCommunication::ShallowCopyFilter * filter);
	//! Dodaje przerw� mi�dzy filtrami
	void addFilterSeparator();
	//! \param perspective Rejestrowana perspektywa
	void registerPerspective(hmdbCommunication::IDataSourcePerspective * perspective);
	//! Dodaje przerw� mi�dzy perspektywami
	void addPerspectiveSeparator();
	//! \param filter Rejestrowany content
	void registerContent(hmdbCommunication::IDataSourceContent * content);
	//! Dodaje przerw� mi�dzy contentami
	void addContentSeparator();
	//! Czy�ci filtry
	void clearFilters();
	//! Czy�ci perspektywy
	void clearPerspectives();
	//! Czy�ci contenty
	void clearContents();
	//! \return Ilo�� filtr�w
	const unsigned int filtersCount() const;
	//! \return Ilo�� perspektyw
	const unsigned int perspectivesCount() const;
	//! \return Ilo�� content�w
	const unsigned int contentsCount() const;

private slots:

	//! \param idx Aktualny indeks combo filtra
	void onFilterChange(int idx);
	//! \param idx Aktualny indeks combo perspektywy
	void onPerspectiveChange(int idx);
	//! \param idx Aktualny indeks combo content�w
	void onContentChange(int idx);

public slots:

	//! \param visible Czy wida� panel filtr�w
	void setFilterVisible(const bool visible);
	//! \param enable Czy panel filtr�w aktywny
	void setFilterEnabled(const bool enable);
	//! \param idx Indeks aktualnego filtra
	void setCurrentFilter(const int idx);
	//! \param visible Czy wida� panel perspektyw
	void setPerspectiveVisible(const bool visible);
	//! \param enable Czy panel perspektyw aktywny
	void setPerspectiveEnabled(const bool enable);
	//! \param idx Indeks aktualnej perspektywy
	void setCurrentPerspective(const int idx);
	//! \param visible Czy wida� panel content�w
	void setContentVisible(const bool visible);
	//! \param enable Czy panel content�w aktywny
	void setContentEnabled(const bool enable);
	//! \param idx Indeks aktualnego contentu
	void setCurrentContent(const int idx);

signals:
	//! Nast�pi�a zmiana filtra
	void filterChanged();
	//! Nast�pi�a zmiana perspektywy
	void perspectiveChanged();
	//! Nast�pi�a zmiana contentu
	void contentChanged();

private:

	template<class T>
	class ItemHelper
	{
	public:
		ItemHelper(QLabel * label, QComboBox * combo) : label(label),
			combo(combo), currentIDX(0), currentItem_(nullptr)
		{

		}

		~ItemHelper()
		{

		}

		void add(const QString & name)
		{
			++currentIDX;
			combo->addItem(name);
		}

		void add(T * item)
		{
			ItemDescription id = { combo->count(), item };
			mapping[currentIDX++] = id;
			combo->addItem(item->name());
		}

		void clear()
		{
			currentItem_ = nullptr;
			currentIDX = 0;
			std::map<unsigned int, ItemDescription>().swap(mapping);
			combo->clear();
		}

		void addSeparator()
		{
			++currentIDX;
			combo->insertSeparator(combo->count());
		}

		const bool setCurrentItem(const unsigned int idx, const bool updateUI = false)
		{			
			bool ret = false;
			auto it = mapping.find(idx);
			if (it != mapping.end()){
				currentItem_ = it->second.item;
				if (updateUI == true){
					combo->blockSignals(true);
					combo->setCurrentIndex(it->second.idx);
					combo->blockSignals(false);
				}

				ret = true;
			}else{
				currentItem_ = nullptr;
			}

			return ret;
		}

		T * currentItem()
		{
			return currentItem_;
		}

		const T * currentItem() const
		{
			return currentItem_;
		}

		const unsigned int count() const
		{
			return mapping.size();
		}

		void setVisible(const bool visible)
		{
			label->setVisible(visible);
			combo->setVisible(visible);
		}

		void setEnable(const bool enable)
		{
			combo->setEnabled(enable);
		}

	private:

		struct ItemDescription
		{
			unsigned int idx;
			T * item;
		};

	private:
		//! Aktualny item
		T * currentItem_;
		//! Mapowanie filtr�w do indeks�w
		std::map<unsigned int, ItemDescription> mapping;
		//! Aktualny indeks filtra w dropbox
		unsigned int currentIDX;
		//! Obs�ugiwane combo
		QComboBox * combo;
		//! Opis
		QLabel * label;
	};

private:
	//! Ui
	Ui::DataViewConfigurationWidget * ui;
	//! Obs�uga filtr�w
	utils::shared_ptr < ItemHelper<hmdbCommunication::ShallowCopyFilter>> filters;
	//! Obs�uga perspektyw
	utils::shared_ptr < ItemHelper<hmdbCommunication::IDataSourcePerspective>> perspectives;
	//! Obs�uga content�w
	utils::shared_ptr < ItemHelper<hmdbCommunication::IDataSourceContent>> contents;
};

#endif	// __HEADER_GUARD_hmdbCommunication__DATAVIEWCONFIGURATIONWIDGET_H__
