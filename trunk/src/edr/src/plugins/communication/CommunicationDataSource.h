/********************************************************************
    created:  2011/10/28
    created:  28:10:2011   13:11
    filename: CommunicationDataSource.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___COMMUNICATIONDATASOURCE_H__
#define HEADER_GUARD___COMMUNICATIONDATASOURCE_H__

#include <plugins/dataExplorer/IDataExplorerSource.h>
#include <plugins/communication/CommunicationManager.h>
#include <plugins/subject/ISubjectService.h>
#include <plugins/subject/ISubject.h>
#include <plugins/subject/ISession.h>
#include <plugins/subject/IMotion.h>
#include <QtCore/QObject>
#include <QtGui/QTreeWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QListWidget>
#include <QtGui/QRadioButton>
#include <QtGui/QButtonGroup>
#include <QtGui/QLabel>
#include "Patient.h"
#include "PatientCardWidget.h"

class CommunicationDataSource;

enum DataLocality { UnknownLocality = 0x00, Local = 0x10, Remote = 0x01, PartiallyLocal = 0x11};
enum DataUsage { UnknownUsage = 0x00, Loaded = 0x10, Unloaded = 0x01, PartiallyLoaded = 0x11};

class ItemView;

class ItemBase : public QObject, public QTreeWidgetItem
{
    Q_OBJECT;
public:

    typedef std::vector<ItemBase*> Items;
    typedef Items::size_type size_type;

public:

    using QTreeWidgetItem::parent;

    ItemBase(ItemView * view);

    static QPixmap mergePixmaps(const QPixmap & pmL, const QPixmap & pmR);

    void refreshStatusContent();

    virtual void refreshContent() = 0;
    virtual void getHeaders(QStringList & headers) const = 0;
    virtual void fillContextMenu(QMenu & menu) = 0;

    virtual void refreshLocality(bool goUp = true) = 0;
    virtual void refreshUsage(bool goUp = true) = 0;
    virtual void refreshStatus(bool goUp = true) = 0;

    void addItemBase(ItemBase * item);

    ItemBase * getItemBaseChild(size_type idx);

    size_type itemBaseChildrenCount() const;

    DataLocality getDataLocality() const;

    void setDataLocality(DataLocality locality);

    DataUsage getDataUsage() const;

    void setDataUsage(DataUsage usage);

    const QIcon & getStatusIcon();

    const QIcon & getLocalityStatusIcon();
    const QIcon & getUsageStatusIcon();

public slots:

    void refresh();

private:


protected:
    DataLocality locality;
    DataUsage usage;
    Items items;
    QIcon localityStatusIcon;
    QIcon usageStatusIcon;

    QPixmap localityStatusPixmap;
    QPixmap usageStatusPixmap;

    QIcon statusIcon;
    ItemView * view;
};

class PatientItem : public ItemBase
{
public:
    PatientItem(ItemView * view, int patientID);

    int getPatientID() const;

    virtual void refreshContent();
    virtual void getHeaders(QStringList & headers) const;
    virtual void fillContextMenu(QMenu & menu);

    virtual void refreshLocality(bool goUp = true);
    virtual void refreshUsage(bool goUp = true);
    virtual void refreshStatus(bool goUp = true);

private:
    int patientID;     
};

class SubjectItem : public ItemBase
{
public:
    SubjectItem(ItemView * view, int subjectID);

    int getSubjectID() const;

    virtual void refreshContent();
    virtual void getHeaders(QStringList & headers) const;
    virtual void fillContextMenu(QMenu & menu);

    virtual void refreshLocality(bool goUp = true);
    virtual void refreshUsage(bool goUp = true);
    virtual void refreshStatus(bool goUp = true);

private:
    int subjectID;       
};

class DisorderItem : public ItemBase
{
public:
    DisorderItem(ItemView * view, int disorderID);

    int getDisorderID() const;

    virtual void refreshContent();
    virtual void getHeaders(QStringList & headers) const;
    virtual void fillContextMenu(QMenu & menu);

    virtual void refreshLocality(bool goUp = true);
    virtual void refreshUsage(bool goUp = true);
    virtual void refreshStatus(bool goUp = true);

private:
    int disorderID;
};

class SessionItem : public ItemBase
{
public:
    SessionItem(ItemView * view, int sessionID);

    int getSessionID() const;

    virtual void refreshContent();
    virtual void getHeaders(QStringList & headers) const;
    virtual void fillContextMenu(QMenu & menu);

    virtual void refreshLocality(bool goUp = true);
    virtual void refreshUsage(bool goUp = true);
    virtual void refreshStatus(bool goUp = true);

private:
    int sessionID;
};

class MotionItem : public ItemBase
{
public:
    MotionItem(ItemView * view, int motionID);

    int getMotionID() const;

    virtual void refreshContent();
    virtual void getHeaders(QStringList & headers) const;
    virtual void fillContextMenu(QMenu & menu);

    virtual void refreshLocality(bool goUp = true);
    virtual void refreshUsage(bool goUp = true);
    virtual void refreshStatus(bool goUp = true);

private:
    int motionID;
};

class FileItem : public ItemBase
{
public:
    FileItem(ItemView * view, int fileID);

    int getFileID() const;

    virtual void refreshContent();
    virtual void getHeaders(QStringList & headers) const;
    virtual void fillContextMenu(QMenu & menu);

    virtual void refreshLocality(bool goUp = true);
    virtual void refreshUsage(bool goUp = true);
    virtual void refreshStatus(bool goUp = true);

private:
    int fileID;
};

class SessionGroupItem : public ItemBase
{
public:
    SessionGroupItem(ItemView * view, const std::set<int> & sessionIDs);

    const std::set<int> & getSessionIDs() const;

    virtual void refreshContent();
    virtual void getHeaders(QStringList & headers) const;
    virtual void fillContextMenu(QMenu & menu);

    virtual void refreshLocality(bool goUp = true);
    virtual void refreshUsage(bool goUp = true);
    virtual void refreshStatus(bool goUp = true);

private:
    std::set<int> sessionIDs;
};

class ItemView : public QWidget
{
    Q_OBJECT;

    friend class ItemBase;
    friend class DisorderItem;
    friend class PatientItem;
    friend class SubjectItem;
    friend class SessionGroupItem;
    friend class SessionItem;
    friend class MotionItem;
    friend class FileItem;

public:
    ItemView(CommunicationDataSource * source);
    virtual ~ItemView();

    void refresh();

protected:

    virtual void doUIRefresh() = 0;

    virtual void refreshStatusIcons(ItemBase* item);

    //HEADERS
    virtual void getHeaders(const ItemBase * itemBase, QStringList & headers) const = 0;
    virtual void getHeaders(const DisorderItem * disorderItem, QStringList & headers) const = 0;
    virtual void getHeaders(const PatientItem * patientItem, QStringList & headers) const = 0;
    virtual void getHeaders(const SubjectItem * subjectItem, QStringList & headers) const = 0;
    virtual void getHeaders(const SessionGroupItem * sessionGroupItem, QStringList & headers) const = 0;
    virtual void getHeaders(const SessionItem * sessionItem, QStringList & headers) const = 0;
    virtual void getHeaders(const MotionItem * motionItem, QStringList & headers) const = 0;
    virtual void getHeaders(const FileItem * fileItem, QStringList & headers) const = 0;

    //CONTEXT MENU
    virtual void fillContextMenu(const ItemBase * itemBase, QMenu & menu);
    virtual void fillContextMenu(const DisorderItem * disorderItem, QMenu & menu);
    virtual void fillContextMenu(const PatientItem * patientItem, QMenu & menu);
    virtual void fillContextMenu(const SubjectItem * subjectItem, QMenu & menu);
    virtual void fillContextMenu(const SessionGroupItem * sessionGroupItem, QMenu & menu);
    virtual void fillContextMenu(const SessionItem * sessionItem, QMenu & menu);
    virtual void fillContextMenu(const MotionItem * motionItem, QMenu & menu);
    virtual void fillContextMenu(const FileItem * fileItem, QMenu & menu);

    //REFRESH/SET CONTENT
    virtual void refreshContent(ItemBase * itemBase) = 0;
    virtual void refreshContent(DisorderItem * disorderItem) = 0;
    virtual void refreshContent(PatientItem * patientItem) = 0;
    virtual void refreshContent(SubjectItem * subjectItem) = 0;
    virtual void refreshContent(SessionGroupItem * sessionGroupItem) = 0;
    virtual void refreshContent(SessionItem * sessionItem) = 0;
    virtual void refreshContent(MotionItem * motionItem) = 0;
    virtual void refreshContent(FileItem * fileItem) = 0;

    //REFRESH LOCALITY
    virtual void refreshLocality(ItemBase * itemBase, bool goUp = true);
    virtual void refreshLocality(DisorderItem * disorderItem, bool goUp = true);
    virtual void refreshLocality(PatientItem * patientItem, bool goUp = true);
    virtual void refreshLocality(SubjectItem * subjectItem, bool goUp = true);
    virtual void refreshLocality(SessionGroupItem * sessionGroupItem, bool goUp = true);
    virtual void refreshLocality(SessionItem * sessionItem, bool goUp = true);
    virtual void refreshLocality(MotionItem * motionItem, bool goUp = true);
    virtual void refreshLocality(FileItem * fileItem, bool goUp = true);

    //REFRESH USAGE
    virtual void refreshUsage(ItemBase * itemBase, bool goUp = true);
    virtual void refreshUsage(DisorderItem * disorderItem, bool goUp = true);
    virtual void refreshUsage(PatientItem * patientItem, bool goUp = true);
    virtual void refreshUsage(SubjectItem * subjectItem, bool goUp = true);
    virtual void refreshUsage(SessionGroupItem * sessionGroupItem, bool goUp = true);
    virtual void refreshUsage(SessionItem * sessionItem, bool goUp = true);
    virtual void refreshUsage(MotionItem * motionItem, bool goUp = true);
    virtual void refreshUsage(FileItem * fileItem, bool goUp = true);

    //REFRESH STATUS
    virtual void refreshStatus(ItemBase * itemBase, bool goUp = true);
    virtual void refreshStatus(DisorderItem * disorderItem, bool goUp = true);
    virtual void refreshStatus(PatientItem * patientItem, bool goUp = true);
    virtual void refreshStatus(SubjectItem * subjectItem, bool goUp = true);
    virtual void refreshStatus(SessionGroupItem * sessionGroupItem, bool goUp = true);
    virtual void refreshStatus(SessionItem * sessionItem, bool goUp = true);
    virtual void refreshStatus(MotionItem * motionItem, bool goUp = true);
    virtual void refreshStatus(FileItem * fileItem, bool goUp = true);

    static std::pair<DataLocality, DataUsage> getItemChildrenCommonStatus(ItemBase * item)
    {
        int locality = UnknownLocality;
        int usage = UnknownUsage;
        for(int i = 0; i < item->itemBaseChildrenCount(); i++){
            item->getItemBaseChild(i)->refreshStatus(false);
            locality |= item->getItemBaseChild(i)->getDataLocality();
            usage |= item->getItemBaseChild(i)->getDataUsage();

            if(locality == PartiallyLocal && usage == PartiallyLoaded){
                break;
            }
        }

        return std::pair<DataLocality, DataUsage>((DataLocality)locality, (DataUsage)usage);
    }

    static DataUsage getItemChildrenCommonUsage(ItemBase * item)
    {
        int usage = UnknownUsage;
        for(int i = 0; i < item->itemBaseChildrenCount(); i++){
            item->getItemBaseChild(i)->refreshUsage(false);
            usage |= item->getItemBaseChild(i)->getDataUsage();
        }

        return (DataUsage)usage;
    }

    static DataLocality getItemChildrenCommonLocality(ItemBase * item)
    {
        int locality = UnknownLocality;
        for(int i = 0; i < item->itemBaseChildrenCount(); i++){
            item->getItemBaseChild(i)->refreshLocality(false);
            locality |= item->getItemBaseChild(i)->getDataLocality();
        }

        return (DataLocality)locality;
    }

    static void refreshLocalityUp(QTreeWidgetItem * item)
    {
        ItemBase * itemBase = dynamic_cast<ItemBase*>(item);
        if(itemBase != nullptr){
            updateLocality(itemBase, getItemChildrenCommonLocality(itemBase), true);
        }
    }

    static void updateLocality(ItemBase * itemBase, DataLocality locality, bool goUp = true)
    {
        if(locality != itemBase->getDataLocality()){
            itemBase->setDataLocality(locality);
            if(goUp == true){
                refreshLocalityUp(itemBase->parent());
            }
        }
    }

    static void refreshUsageUp(QTreeWidgetItem * item)
    {
        ItemBase * itemBase = dynamic_cast<ItemBase*>(item);
        if(itemBase != nullptr){
            updateUsage(itemBase, getItemChildrenCommonUsage(itemBase), true);
        }
    }

    static void updateUsage(ItemBase * itemBase, DataUsage usage, bool goUp = true)
    {
        if(usage != itemBase->getDataUsage()){
            itemBase->setDataUsage(usage);
            if(goUp == true){
                refreshUsageUp(itemBase->parent());
            }
        }
    }

    static void refreshStatusUp(QTreeWidgetItem * item)
    {
        ItemBase * itemBase = dynamic_cast<ItemBase*>(item);
        if(itemBase != nullptr){
            updateStatus(itemBase, getItemChildrenCommonStatus(itemBase), true);
        }
    }

    static void updateStatus(ItemBase * itemBase, const std::pair<int, int> & status, bool goUp = true)
    {
        if(status.first != itemBase->getDataLocality() && status.second != itemBase->getDataUsage()){
            itemBase->setDataLocality((DataLocality)status.first);
            itemBase->setDataUsage((DataUsage)status.second);
            if(goUp == true){
                refreshStatusUp(itemBase->parent());
            }
        }else{
            updateLocality(itemBase, (DataLocality)status.first, goUp);
            updateUsage(itemBase, (DataUsage)status.second, goUp);
        }
    }

    virtual void refreshDisorderItem(int id) = 0;
    virtual void refreshPatientItem(int id) = 0;
    virtual void refreshSubjectItem(int id) = 0;
    virtual void refreshSessionGroupItem(int id) = 0;
    virtual void refreshSessionItem(int id) = 0;
    virtual void refreshMotionItem(int id) = 0;
    virtual void refreshFileItem(int id) = 0;

protected slots:

    void downloadDisorder();
    void downloadPatient();
    void downloadSubject();
    void downloadSessionGroup();
    void downloadSession();
    void downloadMotion();
    void downloadFile();


    void loadDisorder();
    void loadPatient();
    void loadSubject();
    void loadSessionGroup();
    void loadSession();
    void loadMotion();
    void loadFile();

    void onItemClicked(QTreeWidgetItem* item, int column);
    void onCustomContextMenuRequested ( const QPoint & pos );

protected:
    CommunicationDataSource * source;

    MotionShallowCopyPtr motionShallowCopy;
    const communication::MotionMetaData::MetaData & motionMetadata;
    MedicalShallowCopyPtr medicalShallowCopy;
    const communication::MedicalMetaData::MetaData & medicalMetadata;
};

class MotionView : public ItemView
{
    Q_OBJECT;

public:
    MotionView(CommunicationDataSource * source);
    virtual ~MotionView();

    //ItemView
protected:

    //HEADERS
    virtual void getHeaders(const ItemBase * itemBase, QStringList & headers) const;
    virtual void getHeaders(const DisorderItem * disorderItem, QStringList & headers) const;
    virtual void getHeaders(const PatientItem * patientItem, QStringList & headers) const;
    virtual void getHeaders(const SubjectItem * subjectItem, QStringList & headers) const;
    virtual void getHeaders(const SessionGroupItem * sessionGroupItem, QStringList & headers) const;
    virtual void getHeaders(const SessionItem * sessionItem, QStringList & headers) const;
    virtual void getHeaders(const MotionItem * motionItem, QStringList & headers) const;
    virtual void getHeaders(const FileItem * fileItem, QStringList & headers) const;

    //REFRESH/SET CONTENT
    virtual void refreshContent(ItemBase * itemBase);
    virtual void refreshContent(DisorderItem * disorderItem);
    virtual void refreshContent(PatientItem * patientItem);
    virtual void refreshContent(SubjectItem * subjectItem);
    virtual void refreshContent(SessionGroupItem * sessionGroupItem);
    virtual void refreshContent(SessionItem * sessionItem);
    virtual void refreshContent(MotionItem * motionItem);
    virtual void refreshContent(FileItem * fileItem);

protected:

    virtual void doUIRefresh();

    virtual void refreshDisorderItem(int id);
    virtual void refreshPatientItem(int id);
    virtual void refreshSubjectItem(int id);
    virtual void refreshSessionGroupItem(int id);
    virtual void refreshSessionItem(int id);
    virtual void refreshMotionItem(int id);
    virtual void refreshFileItem(int id);

    void fillItemContent(SubjectItem * subjectItem, const communication::MotionShallowCopy::Performer * performer);
    void fillItemContent(SessionItem * sessionItem, const communication::MotionShallowCopy::Session * session);
    void fillItemContent(MotionItem * motionItem, const communication::MotionShallowCopy::Trial * motion);
    void fillItemContent(FileItem * fileItem, const communication::MotionShallowCopy::File * file);

protected:
    QTreeWidget * subjectsTree;

    std::map<int, SubjectItem*> subjectsUIMapping;
    std::map<int, SessionItem*> sessionsUIMapping;
    std::map<int, MotionItem*> motionsUIMapping;
    std::map<int, FileItem*> filesUIMapping;
};

class MedicalView : public ItemView
{
    Q_OBJECT;

public:
    MedicalView(CommunicationDataSource * source);
    virtual ~MedicalView();

protected:

    //HEADERS
    virtual void getHeaders(const ItemBase * itemBase, QStringList & headers) const;
    virtual void getHeaders(const DisorderItem * disorderItem, QStringList & headers) const;
    virtual void getHeaders(const PatientItem * patientItem, QStringList & headers) const;
    virtual void getHeaders(const SubjectItem * subjectItem, QStringList & headers) const;
    virtual void getHeaders(const SessionGroupItem * sessionGroupItem, QStringList & headers) const;
    virtual void getHeaders(const SessionItem * sessionItem, QStringList & headers) const;
    virtual void getHeaders(const MotionItem * motionItem, QStringList & headers) const;
    virtual void getHeaders(const FileItem * fileItem, QStringList & headers) const;

    //REFRESH/SET CONTENT
    virtual void refreshContent(ItemBase * itemBase);
    virtual void refreshContent(DisorderItem * disorderItem);
    virtual void refreshContent(PatientItem * patientItem);
    virtual void refreshContent(SubjectItem * subjectItem);
    virtual void refreshContent(SessionGroupItem * sessionGroupItem);
    virtual void refreshContent(SessionItem * sessionItem);
    virtual void refreshContent(MotionItem * motionItem);
    virtual void refreshContent(FileItem * fileItem);

    virtual void doUIRefresh();

    virtual void refreshDisorderItem(int id);
    virtual void refreshPatientItem(int id);
    virtual void refreshSubjectItem(int id);
    virtual void refreshSessionGroupItem(int id);
    virtual void refreshSessionItem(int id);
    virtual void refreshMotionItem(int id);
    virtual void refreshFileItem(int id);

protected slots:

    void perspectiveChanged(QAbstractButton* button);
    void currentPatientChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);

protected:

    struct SessionGroupDescription
    {
        std::string name;
        std::string date;
    };

protected:

    void fillItemContent(DisorderItem * disorderItem, const communication::MedicalShallowCopy::Disorder * disorder);
    void fillItemContent(PatientItem * patientItem, const communication::MedicalShallowCopy::Patient * patient);
    void fillItemContent(SessionGroupItem * sessionItem, const SessionGroupDescription & description);
    void fillItemContent(SessionItem * sessionItem, const communication::MotionShallowCopy::Session * session);
    void fillItemContent(MotionItem * motionItem, const communication::MotionShallowCopy::Trial * motion);
    void fillItemContent(FileItem * fileItem, const communication::MotionShallowCopy::File * file);

    void createPatientRow(unsigned int row, const communication::MedicalShallowCopy::Patient * patient);
    PatientItem * createPatientItemTree(const communication::MedicalShallowCopy::Patient * patient);
    SessionGroupItem * createSessionGroupItem(const std::string & name, const communication::MotionShallowCopy::Sessions & sessions);
    SessionItem * createSessionItem(const QString & prefix, const communication::MotionShallowCopy::Session * session);

protected:

    QTreeWidget * disordersTree;
    QTreeWidget * patientsTree;

    PatientCardWidget * patientCardWidget;
    QListWidget * patientsList;
    QWidget * patientCardsWidget;
    QButtonGroup * perspectiveButtonsLogicGroup;
    QRadioButton * patientsPerspectiveRadio;
    QRadioButton * disorderPerspectiveRadio;

    std::map<int, DisorderItem*> disordersUIMapping;
    std::map<int, std::vector<PatientItem*>> patientsUIMapping;
    std::map<int, SessionItem*> sessionsUIMapping;
    std::map<int, MotionItem*> motionsUIMapping;
    std::map<int, FileItem*> filesUIMapping;

    std::map<SessionGroupItem*, SessionGroupDescription> sessionGroupsDescriprion;
    std::map<int, QString> sessionsPrefixes;
    
};



class CommunicationDataSource : public QObject, public IDataExplorerSource
{
    UNIQUE_ID("{451D9894-1019-4382-97EE-F18A511A49CB}","newCommunication Data Source");

    Q_OBJECT;

public:

    CommunicationDataSource(communication::CommunicationManager * communicationManager, PluginSubject::ISubjectService * subjectService);

    ~CommunicationDataSource();

    virtual QWidget * getOrCreateView();

    virtual QDialog * getOrCreateConfigurationDialog();

    virtual QWidget * getOrCreateFiltersWidget();

    virtual QAction * getOrCreateRefreshAction();

    virtual const std::string & getName() const;

    virtual void init(core::IMemoryDataManager * memoryDataManager, core::IFileDataManager * fileDataManager);

//-------------------------------------------------------------------------------------------------------------

    const MotionShallowCopyPtr & getMotionShallowCopy() const;
    const communication::MotionMetaData::MetaData & getMotionMetadata() const;

    const MedicalShallowCopyPtr & getMedicalShallowCopy() const;
    const communication::MedicalMetaData::MetaData & getMedicalMetadata() const;

    bool download(const communication::MedicalShallowCopy::Disorder * disorder);
    bool download(const communication::MedicalShallowCopy::Patient * patient);
    bool download(const communication::MotionShallowCopy::Performer * subject);
    bool download(const communication::MotionShallowCopy::Session * session);
    bool download(const std::set<const communication::MotionShallowCopy::Session *> & sessions);
    bool download(const communication::MotionShallowCopy::Trial * motion);
    bool download(const communication::MotionShallowCopy::File * file);

    void load(const communication::MedicalShallowCopy::Disorder * disorder);
    void load(const communication::MedicalShallowCopy::Patient * patient);
    void load(const communication::MotionShallowCopy::Performer * subject);
    void load(const communication::MotionShallowCopy::Session * session);
    void load(const std::set<const communication::MotionShallowCopy::Session *> & sessions);
    void load(const communication::MotionShallowCopy::Trial * motion);
    void load(const communication::MotionShallowCopy::File * file);
    void load(const communication::MotionShallowCopy::File * file, std::vector<core::ObjectWrapperPtr> & objects);

    DataLocality getLocality(const communication::MedicalShallowCopy::Disorder * disorder) const;
    DataLocality getLocality(const communication::MedicalShallowCopy::Patient * patient) const;
    DataLocality getLocality(const communication::MotionShallowCopy::Performer * subject) const;
    DataLocality getLocality(const communication::MotionShallowCopy::Session * session) const;
    DataLocality getLocality(const std::set<const communication::MotionShallowCopy::Session *> & sessions) const;
    DataLocality getLocality(const communication::MotionShallowCopy::Trial * motion) const;
    DataLocality getLocality(const communication::MotionShallowCopy::File * file) const;

    DataUsage getUsage(const communication::MedicalShallowCopy::Disorder * disorder) const;
    DataUsage getUsage(const communication::MedicalShallowCopy::Patient * patient) const;
    DataUsage getUsage(const communication::MotionShallowCopy::Performer * subject) const;
    DataUsage getUsage(const communication::MotionShallowCopy::Session * session) const;
    DataUsage getUsage(const std::set<const communication::MotionShallowCopy::Session *> & sessions) const;
    DataUsage getUsage(const communication::MotionShallowCopy::Trial * motion) const;
    DataUsage getUsage(const communication::MotionShallowCopy::File * file) const;
    DataUsage getUsage(const std::vector<const communication::MotionShallowCopy::File*> & files) const;

    std::pair<DataLocality, DataUsage> getStatus(const communication::MedicalShallowCopy::Disorder * disorder) const;
    std::pair<DataLocality, DataUsage> getStatus(const communication::MedicalShallowCopy::Patient * patient) const;
    std::pair<DataLocality, DataUsage> getStatus(const communication::MotionShallowCopy::Performer * subject) const;
    std::pair<DataLocality, DataUsage> getStatus(const communication::MotionShallowCopy::Session * session) const;
    std::pair<DataLocality, DataUsage> getStatus(const std::set<const communication::MotionShallowCopy::Session *> & sessions) const;
    std::pair<DataLocality, DataUsage> getStatus(const communication::MotionShallowCopy::Trial * motion) const;
    std::pair<DataLocality, DataUsage> getStatus(const communication::MotionShallowCopy::File * file) const;

    std::pair<DataLocality, DataUsage> getStatus(const std::vector<const communication::MotionShallowCopy::File *> files) const;

    core::shared_ptr<QPixmap> getPatientPhoto(const communication::MedicalShallowCopy::Patient * patient);

private:

    QWidget * createMedicalView();
    QWidget * createMotionView();

private slots:

    void refresh();


private:

    void getFiles(const communication::MedicalShallowCopy::Disorder * disorder, std::vector<const communication::MotionShallowCopy::File *> & files) const;
    void getFiles(const communication::MedicalShallowCopy::Patient * patient, std::vector<const communication::MotionShallowCopy::File *> & files) const;
    void getFiles(const communication::MotionShallowCopy::Performer * subject, std::vector<const communication::MotionShallowCopy::File *> & files) const;
    void getFiles(const communication::MotionShallowCopy::Session * session, std::vector<const communication::MotionShallowCopy::File *> & files) const;
    void getFiles(const std::set<const communication::MotionShallowCopy::Session *> & sessions, std::vector<const communication::MotionShallowCopy::File *> & files) const;
    void getFiles(const communication::MotionShallowCopy::Trial * motion, std::vector<const communication::MotionShallowCopy::File *> & files) const;

    void filterMissingFiles(std::vector<const communication::MotionShallowCopy::File *> & files) const;

    void filterUnloadedFiles(std::vector<const communication::MotionShallowCopy::File *> & files) const;

    bool downloadFiles(const std::vector<const communication::MotionShallowCopy::File *> & files);

    PatientPtr createPatient(const communication::MedicalShallowCopy::Patient * patient, PluginSubject::SubjectPtr & subject = PluginSubject::SubjectPtr() ) 
    {
        PatientPtr ret;

        auto patientIT = patientsMapping.find(patient);

        if(patientIT != patientsMapping.end()){
            ret = patientIT->second->get();
        }else{

            if(subject == nullptr){

                auto sub = motionShallowCopy->performers.find(patient->motionPerformerID);
                auto subWrapperIT = subjectsMapping.find(sub->second);

                if(subWrapperIT != subjectsMapping.end()){
                    subject = subWrapperIT->second->get();
                }
            }

            if(subject != nullptr){

                std::vector<Disorder> disorders;

                for(auto it = patient->disorders.begin(); it != patient->disorders.end(); it++){
                    Disorder dis;
                    dis.name = it->second.disorder->name;
                    dis.focus = it->second.focus;
                    dis.comments = it->second.comments;
                    dis.diagnosisDate = it->second.diagnosisDate;
                    disorders.push_back(dis);
                }

                ret.reset(new Patient(subject->getID(), patient->name, patient->surname, patient->birthDate, patient->gender, getPatientPhoto(patient), disorders));
                auto patientWrapper = core::IMemoryDataManager::addData(memoryDataManager, ret);
                patientsMapping[patient] = patientWrapper;
            }
        }

        return ret;
    }

    PluginSubject::SubjectPtr createSubject(const communication::MotionShallowCopy::Performer * subject) 
    {
        PluginSubject::SubjectPtr ret;
        if(subjectService != nullptr){
            ret = subjectService->createSubject();
            auto subjectWrapper = core::IMemoryDataManager::addData(memoryDataManager, ret);
            subjectsMapping[subject] = subjectWrapper;

            if(medicalShallowCopy != nullptr){
                for(auto it = medicalShallowCopy->patients.begin(); it != medicalShallowCopy->patients.end(); it++){
                    if(it->second->motionPerformerID == subject->performerID){
                        //stworz pacjenta
                        createPatient(it->second, ret);
                    }
                }
            }
        }

        return ret;
    }

    PluginSubject::SessionPtr createSession(const communication::MotionShallowCopy::Session * session, PluginSubject::SubjectConstPtr subject = PluginSubject::SubjectConstPtr()) 
    {
        PluginSubject::SessionPtr ret;
        if(subjectService != nullptr && sessionsMapping.find(session) == sessionsMapping.end()){
            if(subject == nullptr){
                auto subjectIT = subjectsMapping.find(session->performerConf->performer);
                if(subjectIT == subjectsMapping.end()){
                    subject = createSubject(session->performerConf->performer);
                }else{
                    subject = subjectIT->second->get();
                }
            }
            
            communication::Date date;
            unpackDate(session->sessionDate, date);

            core::Files files;

            fileDataManager->getManagedFiles(files);

            //pobierz OW sesji tutaj
            std::vector<core::ObjectWrapperPtr> sessionWrappers;

            for(auto it = session->files.begin(); it != session->files.end(); it++){
                auto filePath = getFilePath(it->second);
                
                if(files.find(filePath) == files.end() && core::Filesystem::pathExists(filePath) == true){
                    try{
                        fileDataManager->addFile(filePath);
                    }catch(...){

                    }
                }

                try{
                    fileDataManager->getObjectsForFile(filePath, sessionWrappers);
                }catch(...){

                }
            }

            std::vector<core::ObjectWrapperConstPtr> wrappers(sessionWrappers.begin(), sessionWrappers.end());

            ret = subjectService->createSession(subject, date.getYear(), date.getMonth(), date.getDay(), std::vector<core::ObjectWrapperConstPtr>(sessionWrappers.begin(), sessionWrappers.end()));
            auto sessionWrapper = core::IMemoryDataManager::addData(memoryDataManager, ret);
            sessionsMapping[session] = sessionWrapper;
        }

        return ret;
    }

    PluginSubject::MotionPtr createMotion(const communication::MotionShallowCopy::Trial * motion, const std::vector<core::ObjectWrapperConstPtr> & objects, PluginSubject::SessionConstPtr session = PluginSubject::SessionConstPtr())
    {
        PluginSubject::MotionPtr ret;

        if(subjectService != nullptr && motionsMapping.find(motion) == motionsMapping.end()){
            if(session == nullptr){
                auto sessionIT = sessionsMapping.find(motion->session);
                if(sessionIT == sessionsMapping.end()){
                    session = createSession(motion->session);
                }else{
                    session = sessionIT->second->get();
                }
            }

            ret = subjectService->createMotion(session, objects);
            auto motionWrapper = core::IMemoryDataManager::addData(memoryDataManager, ret);
            motionsMapping[motion] = motionWrapper;
        }

        return ret;
    }

    PluginSubject::MotionPtr createMotion(const communication::MotionShallowCopy::Trial * motion, PluginSubject::SessionConstPtr session = PluginSubject::SessionConstPtr())
    {
        PluginSubject::MotionPtr ret;

        if(subjectService != nullptr && motionsMapping.find(motion) == motionsMapping.end()){
            if(session == nullptr){
                auto sessionIT = sessionsMapping.find(motion->session);
                if(sessionIT == sessionsMapping.end()){
                    session = createSession(motion->session);
                }else{
                    session = sessionIT->second->get();
                }
            }

            core::Files files;

            fileDataManager->getManagedFiles(files);

            //pobierz OW sesji tutaj
            std::vector<core::ObjectWrapperPtr> motionWrappers;

            for(auto it = motion->files.begin(); it != motion->files.end(); it++){
                auto filePath = getFilePath(it->second);
                if(files.find(filePath) != files.end()){
                    try{
                        fileDataManager->getObjectsForFile(filePath, motionWrappers);
                    }catch(...){

                    }
                }
            }

            ret = subjectService->createMotion(session, std::vector<core::ObjectWrapperConstPtr>(motionWrappers.begin(), motionWrappers.end()));
            auto motionWrapper = core::IMemoryDataManager::addData(memoryDataManager, ret);
            motionsMapping[motion] = motionWrapper;
        }

        return ret;
    }

    void onPatientPhotosDownloadCompleteRefresh(const communication::CommunicationManager::RequestPtr & request);
    void onDataDownloadComplete(const communication::CommunicationManager::RequestPtr & request);
    void onDataDownloadCompleteRefresh(const communication::CommunicationManager::RequestPtr & request);
    
    void ensureSessionDirectories(const std::set<const communication::MotionShallowCopy::Session*> & sessions, std::set<core::Filesystem::Path> & createdPaths) const;

    void retrivePatientPhotos();

    /**
    Parsowanie plików xml p³ytkiej kopii bazy danych ruchu.
    */
    void readMotionDbSchemas(const std::string& shallowCopyPath, const std::string& metaDataPath);

    /**
    Parsowanie plików xml p³ytkiej kopii bazy danych ruchu.
    */
    void readMedicalDbSchemas(const std::string& shallowCopyPath, const std::string& metaDataPath);

    core::Filesystem::Path getSessionPath(const communication::MotionShallowCopy::Session * session) const;
    core::Filesystem::Path getFilePath(const communication::MotionShallowCopy::File * file) const;
    core::Filesystem::Path getPhotoPath(int photoID) const;

    static void unpackDate(const std::string & text, communication::Date & date);

private:
    communication::CommunicationManager * communicationManager;
    core::IMemoryDataManager * memoryDataManager;
    PluginSubject::ISubjectService * subjectService;

    core::IFileDataManager * fileDataManager;
    QAction * refreshAction;
    QWidget * widget;
    ItemView * currentView;
    QTreeWidget * patientsWidget;
    QTreeWidget * disordersWidget;

    MotionShallowCopyPtr motionShallowCopy;
    communication::MotionMetaData::MetaData motionMetadata;

    MedicalShallowCopyPtr medicalShallowCopy;
    communication::MedicalMetaData::MetaData medicalMetadata;

    //Œcie¿ki lokalne danych

    core::Filesystem::Path localDataPath;

    core::Filesystem::Path localSchemasPath;
    core::Filesystem::Path localMotionSchemasPath;
    core::Filesystem::Path localMedicalSchemasPath;
    core::Filesystem::Path localPatientPhotosPath;
    core::Filesystem::Path localMotionShallowCopyPath;
    core::Filesystem::Path localMotionMetadataPath;
    core::Filesystem::Path localMedicalShallowCopyPath;
    core::Filesystem::Path localMedicalMetadataPath;

    //parsery XML dla kopii schematów baz danych
    MotionShallowCopyParserPtr motionShallowCopyParser; 
    MotionMetadataParserPtr motionMetadataParser; 
    MedicalShallowCopyParserPtr medicalShallowCopyParser; 
    MedicalMetadataParserPtr medicalMetadataParser;

    std::map<unsigned int, core::shared_ptr<QPixmap> > patientPhotos;

    std::map<const communication::MedicalShallowCopy::Patient*, core::ObjectWrapperPtr> patientsMapping;
    std::map<const communication::MotionShallowCopy::Performer*, core::ObjectWrapperPtr> subjectsMapping;
    std::map<const communication::MotionShallowCopy::Session*, core::ObjectWrapperPtr> sessionsMapping;
    std::map<const communication::MotionShallowCopy::Trial*, core::ObjectWrapperPtr> motionsMapping;

    std::map<const communication::MedicalShallowCopy::Patient*, ItemBase *> patientsUIMapping;
    std::map<const communication::MotionShallowCopy::Performer*, ItemBase *> subjectsUIMapping;
    std::map<const communication::MotionShallowCopy::Session*, ItemBase *> sessionsUIMapping;
    std::map<const communication::MotionShallowCopy::Trial*, ItemBase *> motionsUIMapping;
    std::map<const communication::MotionShallowCopy::File*, ItemBase *> filesUIMapping;

    core::shared_ptr<QPixmap> noPhoto;
};

#endif HEADER_GUARD___COMMUNICATIONDATASOURCE_H__
