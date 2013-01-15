/********************************************************************
	created:	2011/10/14
	created:	14:10:2011   8:03
	filename: 	Vector3DFilterCommand.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_VECTOR3DFILTERCOMMAND__VECTOR3DFILTERCOMMAND_H__
#define HEADER_GUARD_VECTOR3DFILTERCOMMAND__VECTOR3DFILTERCOMMAND_H__

#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QCheckBox>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/type_traits.hpp>

#include <utils/DataChannel.h>
#include <utils/DataChannelCollection.h>
#include <core/SmartPtr.h>

#include <plugins/subject/ISubjectService.h>

#include "FilterCommand.h"
#include "TreeItemHelper.h"
#include "ConfigurationDialog.h"
#include "Measurements.h"

//! mapa [element konfiguracji (np. elektroda) -> wpis z odpowiadającą nazwą + ew. nazwą obrazka]
typedef std::map<QString, std::pair<QString,QString> > NamesDictionary;

//! klasa która wykrzystuje metody buildera drzewa danych, do stworzenia przefiltrowanej galezi
class BuilderFilterCommand : public IFilterCommand
{
public:
    typedef boost::function<QTreeWidgetItem* (const PluginSubject::MotionConstPtr&, const QString&, const QIcon&, const QIcon&)> BranchFunction;
public:
    //! Konstruktor
    //! \param function funkcja, która zostanie wykorzystana do stworzenia elementu drzewa
    //! \param rootIcon ikona korzenia galezi
    //! \param elementIcon ikona stworzonych elementów
    BuilderFilterCommand(BranchFunction function, const QIcon& rootIcon = QIcon(), const QIcon& elementIcon = QIcon());

public:
    //! tworzy gałąź drzewa z przefiltrowanymi danymi
    //! \param rootItemName nazwa korzenia
    //! \param sessions sesje do przefiltrowania
    virtual QTreeWidgetItem* createTreeBranch( const QString& rootItemName, const core::ObjectWrapperCollection& sessions );

protected:
    //! funkcja, która zostanie wykorzystana do stworzenia elementu drzewa
    BranchFunction branchFunction;
    //! ikona stworzonych elementów
    QIcon elementIcon;
    //! ikona korzenia galezi
    QIcon rootIcon;
};

//OBSOLETE
//! tworzy gałąź z reprezentacją stawów
class JointsCommand : public IFilterCommand
{
public:
    //! tworzy gałąź drzewa z przefiltrowanymi danymi
    //! \param rootItemName nazwa korzenia
    //! \param sessions sesje do przefiltrowania
    virtual QTreeWidgetItem* createTreeBranch( const QString& rootItemName, const core::ObjectWrapperCollection& sessions );
};

//! klasa, dzięki której możliwe jest korzystanie z sygnałow i slotów w klasie szablonowej.
//! obsługuje sloty w klasach Vector3DFilterCommand i Vector3DFilterCommand2
class __Helper : public QObject
{
    Q_OBJECT;
public:
    //! konstruktor, pobiera funkcję, która obsługuje zmianę widgeta
    //! \param function odpowiada sygnaturze checkBoxChanged
    __Helper(boost::function<void (const QString&, int)> function);

    public slots:
        //! slot, które przekaże informację o zmianie checkBoxa do ownera
        void checkBoxChanged ( int state );
        //! slot, które przekaże informację o zaznaczeniu elementu do ownera
        void onItemSelected(const QString& name, bool selected);
        //! slot, które przekaże informację o najechaniu elementu do ownera
        void onElementHovered(const QString& name, bool selected);

public:
    //! ustawia mapę z elementami konfiguratora
    //! \param val mapa z elementami konfiguratora oraz ich identyfikatorami
    void setNamesDictionary(const NamesDictionary& val) { namesDictionary = val; }
    //! \return mapa z elementami konfiguratora
    const NamesDictionary& getNamesDictionary() const { return namesDictionary; }

private:
    //! funkcja, która obsługuje zmianę widgeta
    boost::function<void (const QString&, int)> function;
    //! mapa z elementami konfiguratora
    NamesDictionary namesDictionary;
};

//! klasa która wykrzystuje metody buildera drzewa danych, do stworzenia przefiltrowanej gałęzi, dodatkowo dostarcza konfigurator
template <class Collection>
class BuilderConfiguredFilterCommand : public BuilderFilterCommand
{
public:
    //! wskaźnik na kolekcję z kanałami danych
    typedef typename core::ObjectWrapperT<Collection>::Ptr CollectionPtr;
    //! niemodyfikowalny wskaźnik na kolekcję z kanałami danych
    typedef typename core::ObjectWrapperT<Collection>::ConstPtr CollectionConstPtr;

public:
    //! konstruktor
    //! \param function funktor tworzący gałąź
    //! \param namesDictionary mapa z elementami konfiguratora
    //! \param frontXml informacje, o rozmieszczeniu elementów z przodu pacjenta
    //! \param backXml informacje, o rozmieszczeniu elementów z tyłu pacjenta
    //! \param rootIcon ikona gałęzi
    //! \param elementIcon ikona dla liści gałęzi
    BuilderConfiguredFilterCommand(BranchFunction function, const NamesDictionary& namesDictionary,
        const QString& frontXml, const QString& backXml, const QIcon& rootIcon = QIcon(), const QIcon& elementIcon = QIcon()) :
          BuilderFilterCommand(function, rootIcon, elementIcon),
          helper(boost::bind( &BuilderConfiguredFilterCommand::checkBoxChanged, this, _1, _2 )),
          frontXml(frontXml),
          backXml(backXml),
          configurationWidget(nullptr),
          parentWidget(nullptr)
      {
        helper.setNamesDictionary(namesDictionary);
      }

      //! metoda, tkóra jest związana z helperem, zmienił się stan elementu
      //! \param box nazwa zmienionego elementu
      //! \param state aktywny / nieaktywny
      void checkBoxChanged (const QString& box, int state )
      {
          std::string name = box.toStdString();
          activeElements[name] = (bool)state;
      }

private:
    //! tworzy mapę z elementami konfiguratora
    //! \param collection kolekcja, która będzie konfigurowana
    void createNameDictionary(const CollectionConstPtr & collection)
    {
        int count = collection->getNumChannels();
        for (int i = 0; i < count; ++i) {
            auto channel = collection->getChannel(i);
            std::string name = channel->getName();
            auto it = activeElements.find(name);
            if (it == activeElements.end()) {
                activeElements[name] = true;
            }
        }
    }

public:
    //! wywołane po zakończegu konfiguracji
    //! \param result rezultat, oznacza, czy zmiany mają wejść w życie
    virtual void configurationStop( ConfigurationResult result )
    {
        if (result == Cancel) {
            activeElements = tempNameDictionary;
        }
    }
    //! \return widget konfiguratora lub nullptr jesli nie został dostarczony
    virtual QWidget* getConfigurationWidget()
    {
        if (!configurationWidget) {
            parentWidget = new QWidget(nullptr);
            QLayout* h = new QHBoxLayout();
            h->setMargin(0);
            h->setContentsMargins(0, 0, 0, 0);
            configurationWidget = new ConfigurationWidget(nullptr);
            QWidget* left = new QWidget();
            QWidget* right = new QWidget();
            left->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            right->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            h->addWidget(left);
            h->addWidget(configurationWidget);
            h->addWidget(right);
            parentWidget->setLayout(h);
            configurationWidget->loadConfigurations(frontXml, backXml, helper.getNamesDictionary());
            QObject::connect(configurationWidget, SIGNAL(itemSelected(const QString&, bool)), &helper, SLOT(onItemSelected(const QString&, bool)));
            QObject::connect(configurationWidget, SIGNAL(elementHovered(const QString&, bool)), &helper, SLOT(onElementHovered(const QString&, bool)));
        }
        return parentWidget;
    }
    //! przywraca domyślne ustawienia konfiguratora
    virtual void reset()
    {
        for (auto it = activeElements.begin(); it != activeElements.end(); ++it) {
            it->second = true;
        }
        if (configurationWidget) {
            configurationStart();
            configurationWidget->repaint();
        }
    }
    //! konfigurator został wywołany
    virtual void configurationStart()
    {
        tempNameDictionary = activeElements;
        std::map<QString, bool> visibles;
        const NamesDictionary& names = helper.getNamesDictionary();
        for (auto elementIT = activeElements.begin(); elementIT != activeElements.end(); ++elementIT) {
            for (auto nameIT = names.cbegin(); nameIT != names.cend(); ++nameIT) {
                if (nameIT->second.first.toStdString() == elementIT->first) {
                    visibles[nameIT->first] = elementIT->second;
                }
            }
        }
        configurationWidget->setVisibles(visibles);
    }
    //! tworzy gałąź drzewa z przefiltrowanymi danymi
    //! \param rootItemName nazwa korzenia
    //! \param sessions sesje do przefiltrowania
    virtual QTreeWidgetItem* createTreeBranch(const QString& rootItemName, const core::ObjectWrapperCollection& sessions)
    {
        QTreeWidgetItem* root = BuilderFilterCommand::createTreeBranch(rootItemName, sessions);
        filterTree(root);
        return root;
    }

private:
    //! filtruje gałąź za pomocą ustawień konfiguratora
    //! \param item gałąź drzewa
    virtual void filterTree(QTreeWidgetItem* item)
    {
        if (item->childCount()) {
            for (int i = item->childCount() - 1; i >= 0; --i) {
                filterTree(item->child(i));
            }
        } else {
            std::string name = item->text(0).toStdString();
            auto entry = activeElements.find(name);
            if (entry != activeElements.end() && !entry->second) {
                delete item;
            }
        }

    }

protected:
    //! mapa aktywnych elemnetów konfiguratora
    std::map<std::string, bool> activeElements;
    //! pomocnicze, przy zatwierdzaniu lub odrzucaniu zmian konfiguratora
    std::map<std::string, bool> tempNameDictionary;
    //PluginSubject::DataFilterPtr simpleTypeFilter;
    //! pomocniczy obiekt, dzięki któremu można zgrać szablony z sygnałami qt
    __Helper helper;
    //! ścieżka do pliku z rozmieszczeniem elemnetów
    QString frontXml;
    //! ścieżka do pliku z rozmieszczeniem elemnetów
    QString backXml;
    //! konfigurator
    ConfigurationWidget* configurationWidget;
    //! rodzic
    QWidget* parentWidget;
};

//! klasa dostarcza drzewu przefiltrowanych danych emg
class EMGCommand : public BuilderConfiguredFilterCommand<EMGCollection>
{
public:
    //! konstruktor
    //! \param function funktor tworzący gałąź
    //! \param namesDictionary mapa z elementami konfiguratora
    //! \param frontXml informacje, o rozmieszczeniu elementów z przodu pacjenta
    //! \param backXml informacje, o rozmieszczeniu elementów z tyłu pacjenta
    //! \param rootIcon ikona gałęzi
    //! \param elementIcon ikona dla liści gałęzi
    EMGCommand(BranchFunction function, const NamesDictionary& namesDictionary,
        const QString& frontXml, const QString& backXml, const QIcon& rootIcon = QIcon(), const QIcon& elementIcon = QIcon());

public:
    //! tworzy gałąź drzewa z przefiltrowanymi danymi
    //! \param rootItemName nazwa korzenia
    //! \param sessions sesje do przefiltrowania
    virtual QTreeWidgetItem* createTreeBranch( const QString& rootItemName, const core::ObjectWrapperCollection& sessions );
    //! pobiera i zwraca metadane (konfigurację pomiarową)
    //! \param session badana sesja
    //! \return konfiguracja pomiarowa dla podanej sesji
    MeasurementConfigConstPtr getSessionMeta( const core::ObjectWrapperConstPtr & session);
};

//! klasa dostarcza drzewu  przefiltrowanych danych z kanałów z wektorami
template <class Channel, class Collection, class ItemHelper, bool useTreeItemHelperForRoot = false>
class Vector3DFilterCommand : public IFilterCommand
{
public:
    //! wskaźnik na kolekcję z kanałami wektorów
    typedef typename core::ObjectWrapperT<Collection>::Ptr CollectionPtr;
    //! niemodyfikowalny wskaźnik na kolekcję z kanałami wektorów
    typedef typename core::ObjectWrapperT<Collection>::ConstPtr CollectionConstPtr;

    //! wskaźnik na kanał z wektorami
    typedef typename core::ObjectWrapperT<Channel>::Ptr ChannelPtr;
    //! niemodyfikowalny wskaźnik na kanał z wektorami
    typedef typename core::ObjectWrapperT<Channel>::ConstPtr ChannelConstPtr;

public:
      //! konstruktor, tworzy dowiązanie do helpera
      Vector3DFilterCommand() :
          simpleTypeFilter(new TypeFilter(typeid(Collection))),
          helper(boost::bind( &Vector3DFilterCommand::checkBoxChanged, this, _1, _2 ))
      {

      }
      //! tworzy gałąź drzewa z przefiltrowanymi danymi
      //! \param rootItemName nazwa korzenia
      //! \param sessions sesje do przefiltrowania
      virtual QTreeWidgetItem* createTreeBranch(const QString& rootItemName, const core::ObjectWrapperCollection& sessions)
      {
          QTreeWidgetItem* root = new QTreeWidgetItem();
          root->setText(0, rootItemName);
          for(auto it = sessions.begin(); it != sessions.end(); ++it) {
              auto session = *it;
              PluginSubject::SessionPtr filtered = simpleTypeFilter->filterData(session);
              PluginSubject::Motions motions;
              filtered->getMotions(motions);
              for(auto itMotion = motions.begin(); itMotion != motions.end(); ++itMotion) {
                  auto motion = *itMotion;
                  std::vector<core::ObjectWrapperConstPtr> objects;
                  motion->getWrappers(objects, typeid(Collection));
                  if (objects.size() == 1) {
                      QTreeWidgetItem* motionItem;
                      if (useTreeItemHelperForRoot) {
                          TreeWrappedItemHelper* wrapped = new TreeWrappedItemHelper(objects[0]);
                          wrapped->setMotion(motion);
                          motionItem = wrapped;
                      } else {
                          motionItem = new QTreeWidgetItem();
                      }
                      motionItem->setText(0, motion->getLocalName().c_str());
                      root->addChild(motionItem);

                      CollectionConstPtr collection = objects[0]->get();
                      int count = collection->getNumChannels();
                      for (int i = 0 ; i < count; ++i) {
                          auto channel = collection->getChannel(i);
                          auto entry = activeElements.find(channel->getName());
                          if (entry == activeElements.end() || (entry != activeElements.end() && entry->second)) {

                              core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<Channel>();

                              //wrapper->set(boost::dynamic_pointer_cast<Channel>(collection->getChannel(i)));
                              wrapper->set(core::const_pointer_cast<Channel>(channel));
                              static int number = 0;
                              std::string name = "serie_" + boost::lexical_cast<std::string>(number);
                              wrapper->setName(name);
                              wrapper->setSource(name);
                              ItemHelper* treeItem = new ItemHelper(wrapper);
                              treeItem->setMotion(motion);
                              treeItem->setText(0, channel->getName().c_str());
                              motionItem->addChild(treeItem);
                          }
                      }
                  }
              }
          }

          return root;
      }
      //! \return widget konfiguratora lub nullptr jesli nie został dostarczony
      virtual QDialog* getConfigurationDialog(QWidget* parent)
      {
          std::vector<PluginSubject::SessionConstPtr> sessions;
          core::queryDataPtr(core::getDataManagerReader(), sessions, false);
          for (auto itSession = sessions.begin(); itSession != sessions.end(); itSession++) {
              auto session = *itSession;
              PluginSubject::SessionPtr filtered = simpleTypeFilter->filterData(session);
              PluginSubject::Motions motions;
              filtered->getMotions(motions);
              for (auto itMotion = motions.begin(); itMotion != motions.end(); ++itMotion) {
                  auto motion = *itMotion;
                  //QTreeWidgetItem* motionItem = new QTreeWidgetItem();
                  std::vector<core::ObjectWrapperConstPtr> objects;
                  motion->getWrappers(objects, typeid(Collection));
                  for (auto it = objects.begin(); it != objects.end(); ++it) {
                      core::ObjectWrapperConstPtr wrapper = *it;
                      CollectionConstPtr collection = wrapper->get();
                      createNameDictionary(collection);
                  }
              }
          }

          QDialog* widget = new QDialog(parent);
          QVBoxLayout* vlayout = new QVBoxLayout(widget);
          widget->setLayout(vlayout);
          for (auto it = activeElements.begin(); it != activeElements.end(); ++it) {
              QWidget* entry = new QWidget(widget);
              QHBoxLayout* hlayout = new QHBoxLayout(entry);
              entry->setLayout(hlayout);
              QLabel* label = new QLabel(entry);
              label->setText(it->first.c_str());
              hlayout->addWidget(label);
              QCheckBox* checkBox = new QCheckBox(entry);
              QObject::connect(checkBox, SIGNAL(stateChanged ( int)), &helper, SLOT(checkBoxChanged(int)));
              checkBox->setChecked(it->second);
              hlayout->addWidget(checkBox);
              vlayout->addWidget(entry);
          }

          return widget;
      }
      //! metoda, tkóra jest związana z helperem, zmienił się stan elementu
      //! \param box nazwa zmienionego elementu
      //! \param state aktywny / nieaktywny
      void checkBoxChanged (const QString& box, int state )
      {
          std::string name = box.toStdString();
          activeElements[name] = (bool)state;
      }

public:
    //! konfigurator został wywołany
    virtual void configurationStart()
    {
        tempNameDictionary = activeElements;
    }
    //! wywołane po zakończegu konfiguracji
    //! \param result rezultat, oznacza, czy zmiany mają wejść w życie
    virtual void configurationStop( ConfigurationResult result )
    {
        if (result == Cancel) {
            activeElements = tempNameDictionary;
        }
    }

private:
    //! tworzy mapę z elementami konfiguratora
    //! \param collection kolekcja, która będzie konfigurowana
    void createNameDictionary(const CollectionConstPtr & collection)
    {
        int count = collection->getNumChannels();
        for (int i = 0; i < count; ++i) {
            ChannelConstPtr channel = collection->getChannel(i);
            std::string name = channel->getName();
            auto it = activeElements.find(name);
            if (it == activeElements.end()) {
                activeElements[name] = true;
            }
        }
    }

protected:
    //! mapa aktywnych elemnetów konfiguratora
    std::map<std::string, bool> activeElements;
    //! pomocnicze, przy zatwierdzaniu lub odrzucaniu zmian konfiguratora
    std::map<std::string, bool> tempNameDictionary;
    //! zapewnia filtrowanie względem dostarczonego typu
    PluginSubject::DataFilterPtr simpleTypeFilter;
    //! pomocniczy obiekt, dzięki któremu można zgrać szablony z sygnałami qt
    __Helper helper;
};

//! klasa dostarcza drzewu  przefiltrowanych danych z kanałów z wektorami, dodatkowo dostarcza graficzne konfiguratory
template <class Channel, class Collection, class ItemHelper, bool useTreeItemHelperForRoot = false>
class Vector3DFilterCommand2 : public Vector3DFilterCommand<Channel, Collection, ItemHelper, useTreeItemHelperForRoot>
{
public:
    //! klasa bazowa
    typedef Vector3DFilterCommand<Channel, Collection, ItemHelper, useTreeItemHelperForRoot> BaseClass;

    //! konstruktor
    //! \param namesDictionary mapa z elementami konfiguratora
    //! \param frontXml informacje, o rozmieszczeniu elementów z przodu pacjenta
    //! \param backXml informacje, o rozmieszczeniu elementów z tyłu pacjenta
    Vector3DFilterCommand2(const NamesDictionary& namesDictionary, const QString& frontXml, const QString& backXml) :
      BaseClass(),
      frontXml(frontXml),
      backXml(backXml),
      dialog(nullptr)
    {
        this->helper.setNamesDictionary(namesDictionary);
    }
    //! destruktor
    virtual ~Vector3DFilterCommand2()
    {
    }

public:
    //! \return widget konfiguratora lub nullptr jesli nie został dostarczony
    virtual QDialog* getConfigurationDialog( QWidget* parent)
    {
        if (!dialog) {
            dialog = new ConfigurationWidget(parent);
            dialog->loadConfigurations(frontXml, backXml, this->helper.getNamesDictionary());
            QObject::connect(dialog, SIGNAL(itemSelected(const QString&, bool)), &this->helper, SLOT(onItemSelected(const QString&, bool)));
            QObject::connect(dialog, SIGNAL(elementHovered(const QString&, bool)), &this->helper, SLOT(onElementHovered(const QString&, bool)));
        }
        return dialog;
    }
    //! konfigurator został wywołany
    virtual void configurationStart()
    {
        BaseClass::configurationStart();
        std::map<QString, bool> visibles;
        const NamesDictionary& names = this->helper.getNamesDictionary();
        for (auto elementIT = this->activeElements.begin(); elementIT != this->activeElements.end(); ++elementIT) {
            for (auto nameIT = names.cbegin(); nameIT != names.cend(); ++nameIT) {
                if (nameIT->second.first.toStdString() == elementIT->first) {
                    visibles[nameIT->first] = elementIT->second;
                }
            }
        }
        dialog->setVisibles(visibles);
    }

private:
    //! ścieżka do pliku z rozmieszczeniem elemnetów
    QString frontXml;
    //! ścieżka do pliku z rozmieszczeniem elemnetów
    QString backXml;
    //! konfigurator
    ConfigurationWidget* dialog;
};



#endif



