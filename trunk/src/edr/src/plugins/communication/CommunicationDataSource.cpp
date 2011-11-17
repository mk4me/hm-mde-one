#include "CommunicationPCH.h"
#include "CommunicationDataSource.h"
#include <QtGui/QLayout>
#include <QtGui/QMenu>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QLabel>
#include <QtGui/QMessageBox>
#include <QtGui/QPainter>
#include <boost/tokenizer.hpp>
#include "DownloadDialogWidget.h"


Q_DECLARE_METATYPE(std::set<int>);

ItemBase::ItemBase(ItemView * view) : view(view), statusWidget(new QLabel()), pixmap(20,20)
{
    statusWidget->setMaximumSize(20, 20);
    statusWidget->setMinimumSize(20, 20);
    pixmap.fill(QColor("transparent"));
}

void ItemBase::refresh()
{
    refreshStatus();
}

QWidget * ItemBase::getStatusWidget()
{
    return statusWidget;
}

void ItemBase::addItemBase(ItemBase * item)
{
    items.push_back(item);
    addChild(item);
}

ItemBase * ItemBase::getItemBaseChild(size_type idx)
{
    return items[idx];
}

ItemBase::size_type ItemBase::itemBaseChildrenCount() const
{
    return items.size();
}

DataLocality ItemBase::getDataLocality() const
{
    return locality;
}

void ItemBase::setDataLocality(DataLocality locality)
{
    this->locality = locality;

    QColor color;

    switch(locality){
    case Local:
                color = Qt::GlobalColor::green;
                break;
    case Remote:
                color = Qt::GlobalColor::red;
                break;
    case PartiallyLocal:
                color = Qt::GlobalColor::yellow;
                break;
    }

    QPainter painter(&pixmap);
    painter.fillRect(QRectF(0,0,10,20), color);

    statusWidget->setPixmap(pixmap);
}

DataUsage ItemBase::getDataUsage() const
{
    return usage;
}

void ItemBase::setDataUsage(DataUsage usage)
{
    this->usage = usage;

    QColor color;

    switch(usage){
    case Loaded:
        color = Qt::GlobalColor::green;
        break;
    case Unloaded:
        color = Qt::GlobalColor::red;
        break;
    case PartiallyLoaded:
        color = Qt::GlobalColor::yellow;
        break;
    }

    QPainter painter(&pixmap);
    painter.fillRect(QRectF(10,0,10,20), color);

    statusWidget->setPixmap(pixmap);
}

PatientItem::PatientItem(ItemView * view, int patientID) : ItemBase(view), patientID(patientID)
{
    
}

int PatientItem::getPatientID() const
{
    return patientID;
}

void PatientItem::refreshContent()
{
    view->refreshContent(this);
}

void PatientItem::getHeaders(QStringList & headers) const
{
    view->getHeaders(this, headers);
}

void PatientItem::fillContextMenu(QMenu & menu)
{
    view->fillContextMenu(this, menu);
}

void PatientItem::refreshLocality(bool goUp)
{
    view->refreshLocality(this, goUp);
}

void PatientItem::refreshUsage(bool goUp)
{
    view->refreshUsage(this, goUp);
}

void PatientItem::refreshStatus(bool goUp)
{
    view->refreshStatus(this, goUp);
}

DisorderItem::DisorderItem(ItemView * view, int disorderID) : ItemBase(view), disorderID(disorderID)
{
    
}

int DisorderItem::getDisorderID() const
{
    return disorderID;
}

void DisorderItem::refreshContent()
{
    view->refreshContent(this);
}

void DisorderItem::getHeaders(QStringList & headers) const
{
    view->getHeaders(this, headers);
}

void DisorderItem::fillContextMenu(QMenu & menu)
{
    view->fillContextMenu(this, menu);
}

void DisorderItem::refreshLocality(bool goUp)
{
    view->refreshLocality(this, goUp);
}

void DisorderItem::refreshUsage(bool goUp)
{
    view->refreshUsage(this, goUp);
}

void DisorderItem::refreshStatus(bool goUp)
{
    view->refreshStatus(this, goUp);
}

SubjectItem::SubjectItem(ItemView * view, int subjectID) : ItemBase(view), subjectID(subjectID)
{

}

int SubjectItem::getSubjectID() const
{
    return subjectID;
}

void SubjectItem::refreshContent()
{
    view->refreshContent(this);
}

void SubjectItem::getHeaders(QStringList & headers) const
{
    view->getHeaders(this, headers);
}

void SubjectItem::fillContextMenu(QMenu & menu)
{
    view->fillContextMenu(this, menu);
}

void SubjectItem::refreshLocality(bool goUp)
{
    view->refreshLocality(this, goUp);
}

void SubjectItem::refreshUsage(bool goUp)
{
    view->refreshUsage(this, goUp);
}

void SubjectItem::refreshStatus(bool goUp)
{
    view->refreshStatus(this, goUp);
}

SessionItem::SessionItem(ItemView * view, int sessionID) : ItemBase(view), sessionID(sessionID)
{
    
}

int SessionItem::getSessionID() const
{
    return sessionID;
}

void SessionItem::refreshContent()
{
    view->refreshContent(this);
}

void SessionItem::getHeaders(QStringList & headers) const
{
    view->getHeaders(this, headers);
}

void SessionItem::fillContextMenu(QMenu & menu)
{
    view->fillContextMenu(this, menu);
}

void SessionItem::refreshLocality(bool goUp)
{
    view->refreshLocality(this, goUp);
}

void SessionItem::refreshUsage(bool goUp)
{
    view->refreshUsage(this, goUp);
}

void SessionItem::refreshStatus(bool goUp)
{
    view->refreshStatus(this, goUp);
}

MotionItem::MotionItem(ItemView * view, int motionID) : ItemBase(view), motionID(motionID)
{
    
}

int MotionItem::getMotionID() const
{
    return motionID;
}

void MotionItem::refreshContent()
{
    view->refreshContent(this);
}

void MotionItem::getHeaders(QStringList & headers) const
{
    view->getHeaders(this, headers);
}

void MotionItem::fillContextMenu(QMenu & menu)
{
    view->fillContextMenu(this, menu);
}

void MotionItem::refreshLocality(bool goUp)
{
    view->refreshLocality(this, goUp);
}

void MotionItem::refreshUsage(bool goUp)
{
    view->refreshUsage(this, goUp);
}

void MotionItem::refreshStatus(bool goUp)
{
    view->refreshStatus(this, goUp);
}

FileItem::FileItem(ItemView * view, int fileID) : ItemBase(view), fileID(fileID)
{
    
}

int FileItem::getFileID() const
{
    return fileID;
}

void FileItem::refreshContent()
{
    view->refreshContent(this);
}

void FileItem::getHeaders(QStringList & headers) const
{
    view->getHeaders(this, headers);
}

void FileItem::fillContextMenu(QMenu & menu)
{
    view->fillContextMenu(this, menu);
}

void FileItem::refreshLocality(bool goUp)
{
    view->refreshLocality(this, goUp);
}

void FileItem::refreshUsage(bool goUp)
{
    view->refreshUsage(this, goUp);
}

void FileItem::refreshStatus(bool goUp)
{
    view->refreshStatus(this, goUp);
}

SessionGroupItem::SessionGroupItem(ItemView * view, const std::set<int> & sessionIDs) : ItemBase(view), sessionIDs(sessionIDs)
{
    
}

const std::set<int> & SessionGroupItem::getSessionIDs() const
{
    return sessionIDs;
}

void SessionGroupItem::refreshContent()
{
    view->refreshContent(this);
}

void SessionGroupItem::getHeaders(QStringList & headers) const
{
    view->getHeaders(this, headers);
}

void SessionGroupItem::fillContextMenu(QMenu & menu)
{
    view->fillContextMenu(this, menu);
}

void SessionGroupItem::refreshLocality(bool goUp)
{
    view->refreshLocality(this, goUp);
}

void SessionGroupItem::refreshUsage(bool goUp)
{
    view->refreshUsage(this, goUp);
}

void SessionGroupItem::refreshStatus(bool goUp)
{
    view->refreshStatus(this, goUp);
}

ItemView::ItemView(CommunicationDataSource * source) : source(source), motionShallowCopy(source->getMotionShallowCopy()), motionMetadata(source->getMotionMetadata()),
    medicalShallowCopy(source->getMedicalShallowCopy()), medicalMetadata(source->getMedicalMetadata())
{

}

ItemView::~ItemView()
{

}

void ItemView::refresh()
{   
    motionShallowCopy = source->getMotionShallowCopy();
    //motionMetadata = source->getMotionMetadata();
    medicalShallowCopy = source->getMedicalShallowCopy();
    //medicalMetadata = source->getMedicalMetadata();
    doUIRefresh();   
}

//CONTEXT MENU
void ItemView::fillContextMenu(const ItemBase * itemBase, QMenu & menu)
{
    throw std::runtime_error("Unsupported item");
}

void ItemView::fillContextMenu(const DisorderItem * disorderItem, QMenu & menu)
{
    switch(disorderItem->getDataLocality()){
    case Remote:
        {
            QAction * downloadAction = menu.addAction(QString("Download"));
            downloadAction->setData(QVariant(disorderItem->getDisorderID()));
            connect(downloadAction, SIGNAL(triggered()), this, SLOT(downloadDisorder()));
        }

        break;
    case Local:

        break;
    case PartiallyLocal:
        {
            QAction * downloadAction = menu.addAction(QString("Download"));
            downloadAction->setData(QVariant(disorderItem->getDisorderID()));
            connect(downloadAction, SIGNAL(triggered()), this, SLOT(downloadDisorder()));
        }

        break;
    }

    switch(disorderItem->getDataUsage()){
    case Unloaded:

        if(disorderItem->getDataLocality() == Local){
            QAction * loadAction = menu.addAction(QString("Load"));
            loadAction->setData(QVariant(disorderItem->getDisorderID()));
            connect(loadAction, SIGNAL(triggered()), this, SLOT(loadDisorder()));
        }

        break;
    case Loaded:
        break;
    case PartiallyLoaded:

        if(disorderItem->getDataLocality() == Local){
            QAction * loadAction = menu.addAction(QString("Load"));
            loadAction->setData(QVariant(disorderItem->getDisorderID()));
            connect(loadAction, SIGNAL(triggered()), this, SLOT(loadDisorder()));
        }

        break;
    }
}

void ItemView::fillContextMenu(const PatientItem * patientItem, QMenu & menu)
{
    switch(patientItem->getDataLocality()){
    case Remote:
        {
            QAction * downloadAction = menu.addAction(QString("Download"));
            downloadAction->setData(QVariant(patientItem->getPatientID()));
            connect(downloadAction, SIGNAL(triggered()), this, SLOT(downloadPatient()));
        }

        break;
    case Local:

        break;
    case PartiallyLocal:
        {
            QAction * downloadAction = menu.addAction(QString("Download"));
            downloadAction->setData(QVariant(patientItem->getPatientID()));
            connect(downloadAction, SIGNAL(triggered()), this, SLOT(downloadPatient()));
        }

        break;
    }

    switch(patientItem->getDataUsage()){
    case Unloaded:

        if(patientItem->getDataLocality() == Local){
            QAction * loadAction = menu.addAction(QString("Load"));
            loadAction->setData(QVariant(patientItem->getPatientID()));
            connect(loadAction, SIGNAL(triggered()), this, SLOT(loadPatient()));
        }

        break;
    case Loaded:
        break;
    case PartiallyLoaded:

        if(patientItem->getDataLocality() == Local){
            QAction * loadAction = menu.addAction(QString("Load"));
            loadAction->setData(QVariant(patientItem->getPatientID()));
            connect(loadAction, SIGNAL(triggered()), this, SLOT(loadPatient()));
        }

        break;
    }
}

void ItemView::fillContextMenu(const SubjectItem * subjectItem, QMenu & menu)
{
    switch(subjectItem->getDataLocality()){
    case Remote:
        {
            QAction * downloadAction = menu.addAction(QString("Download"));
            downloadAction->setData(QVariant(subjectItem->getSubjectID()));
            connect(downloadAction, SIGNAL(triggered()), this, SLOT(downloadSubject()));
        }

        break;
    case Local:

        break;
    case PartiallyLocal:
        {
            QAction * downloadAction = menu.addAction(QString("Download"));
            downloadAction->setData(QVariant(subjectItem->getSubjectID()));
            connect(downloadAction, SIGNAL(triggered()), this, SLOT(downloadSubject()));
        }

        break;
    }

    switch(subjectItem->getDataUsage()){
    case Unloaded:

        if(subjectItem->getDataLocality() == Local){
            QAction * loadAction = menu.addAction(QString("Load"));
            loadAction->setData(QVariant(subjectItem->getSubjectID()));
            connect(loadAction, SIGNAL(triggered()), this, SLOT(loadSubject()));
        }

        break;
    case Loaded:
        break;
    case PartiallyLoaded:

        if(subjectItem->getDataLocality() == Local){
            QAction * loadAction = menu.addAction(QString("Load"));
            loadAction->setData(QVariant(subjectItem->getSubjectID()));
            connect(loadAction, SIGNAL(triggered()), this, SLOT(loadSubject()));
        }

        break;
    }
}

void ItemView::fillContextMenu(const SessionGroupItem * sessionGroupItem, QMenu & menu)
{
    /*switch(sessionGroupItem->getDataLocality()){
    case Remote:
        {
            QAction * downloadAction = menu.addAction(QString("Download"));
            downloadAction->setData(QVariant(sessionGroupItem->getS()));
            connect(downloadAction, SIGNAL(triggered()), this, SLOT(downloadSubject()));
        }

        break;
    case Local:

        break;
    case PartiallyLocal:
        {
            QAction * downloadAction = menu.addAction(QString("Download"));
            downloadAction->setData(QVariant(sessionGroupItem->getSubjectID()));
            connect(downloadAction, SIGNAL(triggered()), this, SLOT(downloadSubject()));
        }

        break;
    }

    switch(sessionGroupItem->getDataUsage()){
    case Unloaded:

        if(subjectItem->getDataLocality() == Local){
            QAction * loadAction = menu.addAction(QString("Load"));
            loadAction->setData(QVariant(sessionGroupItem->getSubjectID()));
            connect(loadAction, SIGNAL(triggered()), this, SLOT(loadSubject()));
        }

        break;
    case Loaded:
        break;
    case PartiallyLoaded:

        if(sessionGroupItem->getDataLocality() == Local){
            QAction * loadAction = menu.addAction(QString("Load"));
            loadAction->setData(QVariant(sessionGroupItem->getSubjectID()));
            connect(loadAction, SIGNAL(triggered()), this, SLOT(loadSubject()));
        }

        break;
    }*/

    //throw std::runtime_error("Not supported");
}

void ItemView::fillContextMenu(const SessionItem * sessionItem, QMenu & menu)
{
    switch(sessionItem->getDataLocality()){
    case Remote:
        {
            QAction * downloadAction = menu.addAction(QString("Download"));
            downloadAction->setData(QVariant(sessionItem->getSessionID()));
            connect(downloadAction, SIGNAL(triggered()), this, SLOT(downloadSession()));
        }

        break;
    case Local:

        break;
    case PartiallyLocal:
        {
            QAction * downloadAction = menu.addAction(QString("Download"));
            downloadAction->setData(QVariant(sessionItem->getSessionID()));
            connect(downloadAction, SIGNAL(triggered()), this, SLOT(downloadSession()));
        }

        break;
    }

    switch(sessionItem->getDataUsage()){
    case Unloaded:

        if(sessionItem->getDataLocality() == Local){
            QAction * loadAction = menu.addAction(QString("Load"));
            loadAction->setData(QVariant(sessionItem->getSessionID()));
            connect(loadAction, SIGNAL(triggered()), this, SLOT(loadSession()));
        }

        break;
    case Loaded:
        break;
    case PartiallyLoaded:

        if(sessionItem->getDataLocality() == Local){
            QAction * loadAction = menu.addAction(QString("Load"));
            loadAction->setData(QVariant(sessionItem->getSessionID()));
            connect(loadAction, SIGNAL(triggered()), this, SLOT(loadSession()));
        }

        break;
    }
}

void ItemView::fillContextMenu(const MotionItem * motionItem, QMenu & menu)
{
    switch(motionItem->getDataLocality()){
    case Remote:
        {
            QAction * downloadAction = menu.addAction(QString("Download"));
            downloadAction->setData(QVariant(motionItem->getMotionID()));
            connect(downloadAction, SIGNAL(triggered()), this, SLOT(downloadMotion()));
        }

        break;
    case Local:

        break;
    case PartiallyLocal:
        {
            QAction * downloadAction = menu.addAction(QString("Download"));
            downloadAction->setData(QVariant(motionItem->getMotionID()));
            connect(downloadAction, SIGNAL(triggered()), this, SLOT(downloadMotion()));
        }

        break;
    }

    switch(motionItem->getDataUsage()){
    case Unloaded:

        if(motionItem->getDataLocality() == Local){
            QAction * loadAction = menu.addAction(QString("Load"));
            loadAction->setData(QVariant(motionItem->getMotionID()));
            connect(loadAction, SIGNAL(triggered()), this, SLOT(loadMotion()));
        }

        break;
    case Loaded:
        break;
    case PartiallyLoaded:

        if(motionItem->getDataLocality() == Local){
            QAction * loadAction = menu.addAction(QString("Load"));
            loadAction->setData(QVariant(motionItem->getMotionID()));
            connect(loadAction, SIGNAL(triggered()), this, SLOT(loadMotion()));
        }

        break;
    }
}

void ItemView::fillContextMenu(const FileItem * fileItem, QMenu & menu)
{
    switch(fileItem->getDataLocality()){
    case Remote:
        {
            QAction * downloadAction = menu.addAction(QString("Download"));
            downloadAction->setData(QVariant(fileItem->getFileID()));
            connect(downloadAction, SIGNAL(triggered()), this, SLOT(downloadFile()));
        }

        break;
    case Local:

        break;
    case PartiallyLocal:
        {
            QAction * downloadAction = menu.addAction(QString("Download"));
            downloadAction->setData(QVariant(fileItem->getFileID()));
            connect(downloadAction, SIGNAL(triggered()), this, SLOT(downloadFile()));
        }

        break;
    }

    switch(fileItem->getDataUsage()){
    case Unloaded:

        if(fileItem->getDataLocality() == Local){
            QAction * loadAction = menu.addAction(QString("Load"));
            loadAction->setData(QVariant(fileItem->getFileID()));
            connect(loadAction, SIGNAL(triggered()), this, SLOT(loadFile()));
        }

        break;
    case Loaded:
        break;
    case PartiallyLoaded:

        if(fileItem->getDataLocality() == Local){
            QAction * loadAction = menu.addAction(QString("Load"));
            loadAction->setData(QVariant(fileItem->getFileID()));
            connect(loadAction, SIGNAL(triggered()), this, SLOT(loadFile()));
        }

        break;
    }
}

void ItemView::downloadDisorder()
{
    QAction * action = qobject_cast<QAction*>(sender());

    int id = action->data().toInt();

    bool status = source->download(medicalShallowCopy->disorders.find(id)->second);

    if(status == true){
        refreshDisorderItem(id);
    }
}

void ItemView::downloadPatient()
{
    QAction * action = qobject_cast<QAction*>(sender());

    int id = action->data().toInt();

    bool status = source->download(motionShallowCopy->performers.find(id)->second);

    if(status == true){
        refreshPatientItem(id);
    }
}

void ItemView::downloadSubject()
{
    QAction * action = qobject_cast<QAction*>(sender());

    int id = action->data().toInt();

    bool status = source->download(motionShallowCopy->performers.find(id)->second);

    if(status == true){
        refreshSubjectItem(id);
    }
}

void ItemView::downloadSessionGroup()
{
    /*QAction * action = qobject_cast<QAction*>(sender());

    int id = action->data().toInt();

    bool status = source->download(motionShallowCopy->sessions.find(id)->second);

    if(status == true){
        refreshSessionItem(id);
    }*/
}

void ItemView::downloadSession()
{
    QAction * action = qobject_cast<QAction*>(sender());

    int id = action->data().toInt();

    bool status = source->download(motionShallowCopy->sessions.find(id)->second);

    if(status == true){
        refreshSessionItem(id);
    }
}

void ItemView::downloadMotion()
{
    QAction * action = qobject_cast<QAction*>(sender());

    int id = action->data().toInt();

    bool status = source->download(motionShallowCopy->trials.find(id)->second);

    if(status == true){
        refreshMotionItem(id);
    }
}

void ItemView::downloadFile()
{
    QAction * action = qobject_cast<QAction*>(sender());

    int id = action->data().toInt();

    bool status = source->download(motionShallowCopy->files.find(id)->second);

    if(status == true){
        refreshFileItem(id);
    }
}

void ItemView::onItemClicked(QTreeWidgetItem* item, int column)
{
    ItemBase * itemBase = dynamic_cast<ItemBase*>(item);

    if(itemBase == nullptr){
        return;
    }

    QTreeWidget * tree = qobject_cast<QTreeWidget*>(sender());

    int columns = itemBase->columnCount();
    tree->setColumnCount(columns);

    QStringList headers;
    itemBase->getHeaders(headers);
    tree->setHeaderLabels(headers);

    for(int i = 0; i < columns; i++) {
        tree->resizeColumnToContents(i);
    }
}

void ItemView::onCustomContextMenuRequested ( const QPoint & pos )
{
    QMenu menu;

    QTreeWidget * tree = qobject_cast<QTreeWidget*>(sender());
    QTreeWidgetItem * item = tree->itemAt(pos);
    ItemBase * itemBase = dynamic_cast<ItemBase*>(item);
    itemBase->fillContextMenu(menu);  

    menu.addSeparator();

    auto refreshAction = menu.addAction(QString("Refresh"));
    connect(refreshAction, SIGNAL(triggered()), itemBase, SLOT(refresh()));

    menu.exec(tree->mapToGlobal(pos));
}

void ItemView::loadDisorder()
{
    QAction * action = qobject_cast<QAction*>(sender());
    int id = action->data().toInt();

    source->load(medicalShallowCopy->disorders.find(id)->second);

    refreshDisorderItem(id);
}

void ItemView::loadPatient()
{
    QAction * action = qobject_cast<QAction*>(sender());
    int id = action->data().toInt();

    source->load(medicalShallowCopy->patients.find(id)->second);

    refreshPatientItem(id);
}

void ItemView::loadSubject()
{
    QAction * action = qobject_cast<QAction*>(sender());
    int id = action->data().toInt();

    source->load(motionShallowCopy->performers.find(id)->second);

    refreshSubjectItem(id);
}

void ItemView::loadSession()
{
    QAction * action = qobject_cast<QAction*>(sender());
    int id = action->data().toInt();

    source->load(motionShallowCopy->sessions.find(id)->second);

    refreshSessionItem(id);
}

void ItemView::loadSessionGroup()
{
    /*QAction * action = qobject_cast<QAction*>(sender());

    int id = action->data().toInt();

    bool status = source->download(motionShallowCopy->sessions.find(id)->second);

    if(status == true){
        refreshSessionItem(id);
    }*/
}

void ItemView::loadMotion()
{
    QAction * action = qobject_cast<QAction*>(sender());
    int id = action->data().toInt();

    source->load(motionShallowCopy->trials.find(id)->second);

    refreshMotionItem(id);
}

void ItemView::loadFile()
{
    QAction * action = qobject_cast<QAction*>(sender());
    int id = action->data().toInt();

    source->load(motionShallowCopy->files.find(id)->second);

    refreshFileItem(id);
}

//REFRESH LOCALITY
void ItemView::refreshLocality(ItemBase * itemBase, bool goUp)
{
    throw std::runtime_error("Unsupported item");
}

void ItemView::refreshLocality(DisorderItem * disorderItem, bool goUp)
{
    throw std::runtime_error("Unsupported item");
}

void ItemView::refreshLocality(PatientItem * patientItem, bool goUp)
{
    throw std::runtime_error("Unsupported item");
}

void ItemView::refreshLocality(SubjectItem * subjectItem, bool goUp)
{
    int s = subjectItem->itemBaseChildrenCount();
    int locality;
    if(s == 0){
        locality = source->getLocality(motionShallowCopy->performers.find(subjectItem->getSubjectID())->second);
    }else{
        locality = getItemChildrenCommonLocality(subjectItem);
    }

    updateLocality(subjectItem, (DataLocality)locality, goUp);
}

void ItemView::refreshLocality(SessionGroupItem * sessionGroupItem, bool goUp)
{
    throw std::runtime_error("Unsupported item");
}

void ItemView::refreshLocality(SessionItem * sessionItem, bool goUp)
{
    int s = sessionItem->itemBaseChildrenCount();
    int locality;
    if(s == 0){
        locality = source->getLocality(motionShallowCopy->sessions.find(sessionItem->getSessionID())->second);
    }else{
        locality = getItemChildrenCommonLocality(sessionItem);
    }

    updateLocality(sessionItem, (DataLocality)locality, goUp);
}

void ItemView::refreshLocality(MotionItem * motionItem, bool goUp)
{
    int s = motionItem->itemBaseChildrenCount();
    int locality;
    if(s == 0){
        locality = source->getLocality(motionShallowCopy->trials.find(motionItem->getMotionID())->second);
    }else{
        locality = getItemChildrenCommonLocality(motionItem);
    }

    updateLocality(motionItem, (DataLocality)locality, goUp);
}

void ItemView::refreshLocality(FileItem * fileItem, bool goUp)
{
    updateLocality(fileItem, source->getLocality(motionShallowCopy->files.find(fileItem->getFileID())->second), goUp);
}

//REFRESH USAGE
void ItemView::refreshUsage(ItemBase * itemBase, bool goUp)
{
    throw std::runtime_error("Unsupported item");
}

void ItemView::refreshUsage(DisorderItem * disorderItem, bool goUp)
{
    int s = disorderItem->itemBaseChildrenCount();
    int usage;
    if(s == 0){
        usage = source->getUsage(medicalShallowCopy->disorders.find(disorderItem->getDisorderID())->second);
    }else{
        usage = getItemChildrenCommonUsage(disorderItem);
    }

    updateUsage(disorderItem, (DataUsage)usage, goUp);
}

void ItemView::refreshUsage(PatientItem * patientItem, bool goUp)
{
    int s = patientItem->itemBaseChildrenCount();
    int usage;
    if(s == 0){
        usage = source->getUsage(medicalShallowCopy->patients.find(patientItem->getPatientID())->second);
    }else{
        usage = getItemChildrenCommonUsage(patientItem);
    }

    updateUsage(patientItem, (DataUsage)usage, goUp);
}

void ItemView::refreshUsage(SubjectItem * subjectItem, bool goUp)
{
    int s = subjectItem->itemBaseChildrenCount();
    int usage;
    if(s == 0){
        usage = source->getUsage(motionShallowCopy->performers.find(subjectItem->getSubjectID())->second);
    }else{
        usage = getItemChildrenCommonUsage(subjectItem);
    }

    updateUsage(subjectItem, (DataUsage)usage, goUp);
}

void ItemView::refreshUsage(SessionGroupItem * sessionGroupItem, bool goUp)
{
    throw std::runtime_error("Unsupported item");
}

void ItemView::refreshUsage(SessionItem * sessionItem, bool goUp)
{
    int s = sessionItem->itemBaseChildrenCount();
    int usage;
    if(s == 0){
        usage = source->getUsage(motionShallowCopy->sessions.find(sessionItem->getSessionID())->second);
    }else{
        usage = getItemChildrenCommonUsage(sessionItem);

        if(usage != PartiallyLoaded){

            //weryfikuj pliki sesji!!
            auto session = motionShallowCopy->sessions.find(sessionItem->getSessionID())->second;
            std::vector<const communication::MotionShallowCopy::File*> files;
            for(auto it = session->files.begin(); it != session->files.end(); it++){
                files.push_back(it->second);
            }

            usage |= source->getUsage(files);
        }
    }

    updateUsage(sessionItem, (DataUsage)usage, goUp);
}

void ItemView::refreshUsage(MotionItem * motionItem, bool goUp)
{
    int s = motionItem->itemBaseChildrenCount();
    int usage;
    if(s == 0){
        usage = source->getUsage(motionShallowCopy->trials.find(motionItem->getMotionID())->second);
    }else{
        usage = getItemChildrenCommonUsage(motionItem);
    }

    updateUsage(motionItem, (DataUsage)usage, goUp);
}

void ItemView::refreshUsage(FileItem * fileItem, bool goUp)
{
    updateUsage(fileItem, source->getUsage(motionShallowCopy->files.find(fileItem->getFileID())->second), goUp);
}

//REFRESH STATUS
void ItemView::refreshStatus(ItemBase * itemBase, bool goUp)
{
    throw std::runtime_error("Unsupported item");
}

void ItemView::refreshStatus(DisorderItem * disorderItem, bool goUp)
{
    int s = disorderItem->itemBaseChildrenCount();
    std::pair<int, int> status(0,0);
    if(s == 0){
        status = source->getStatus(medicalShallowCopy->disorders.find(disorderItem->getDisorderID())->second);
    }else{
        status = getItemChildrenCommonStatus(disorderItem);
    }

    updateStatus(disorderItem, status, goUp);
}

void ItemView::refreshStatus(PatientItem * patientItem, bool goUp)
{
    int s = patientItem->itemBaseChildrenCount();
    std::pair<int, int> status(0,0);
    if(s == 0){
        status = source->getStatus(medicalShallowCopy->patients.find(patientItem->getPatientID())->second);
    }else{
        status = getItemChildrenCommonStatus(patientItem);
    }

    updateStatus(patientItem, status, goUp);
}

void ItemView::refreshStatus(SubjectItem * subjectItem, bool goUp)
{
    int s = subjectItem->itemBaseChildrenCount();
    std::pair<int, int> status(0,0);
    if(s == 0){
        status = source->getStatus(motionShallowCopy->performers.find(subjectItem->getSubjectID())->second);
    }else{
        status = getItemChildrenCommonStatus(subjectItem);
    }

    updateStatus(subjectItem, status, goUp);
}

void ItemView::refreshStatus(SessionGroupItem * sessionGroupItem, bool goUp)
{
    throw std::runtime_error("Unsupported item");
}

void ItemView::refreshStatus(SessionItem * sessionItem, bool goUp)
{
    int s = sessionItem->itemBaseChildrenCount();
    std::pair<int, int> status(0,0);
    if(s == 0){
        status = source->getStatus(motionShallowCopy->sessions.find(sessionItem->getSessionID())->second);
    }else{
        status = getItemChildrenCommonStatus(sessionItem);
    }

    updateStatus(sessionItem, status, goUp);
}

void ItemView::refreshStatus(MotionItem * motionItem, bool goUp)
{
    int s = motionItem->itemBaseChildrenCount();
    std::pair<int, int> status(0,0);
    if(s == 0){
        status = source->getStatus(motionShallowCopy->trials.find(motionItem->getMotionID())->second);
    }else{
        status = getItemChildrenCommonStatus(motionItem);
    }

    updateStatus(motionItem, status, goUp);
}

void ItemView::refreshStatus(FileItem * fileItem, bool goUp)
{
    int s = fileItem->itemBaseChildrenCount();
    std::pair<int, int> status(0,0);
    if(s == 0){
        status = source->getStatus(motionShallowCopy->files.find(fileItem->getFileID())->second);
    }else{
        status = getItemChildrenCommonStatus(fileItem);
    }

    updateStatus(fileItem, status, goUp);
}

void ItemView::setStatusWidgets(QTreeWidget * tree, ItemBase * item)
{
    tree->setItemWidget(item, 0, item->getStatusWidget());
    for(int i = 0; i < item->itemBaseChildrenCount(); i++){
        setStatusWidgets(tree, item->getItemBaseChild(i));
    }
}

MotionView::MotionView(CommunicationDataSource * source) : ItemView(source), subjectsTree(new QTreeWidget())
{
    QHBoxLayout * layout = new QHBoxLayout;

    subjectsTree->setContextMenuPolicy( Qt::CustomContextMenu );

    connect(subjectsTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));
    connect(subjectsTree, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenuRequested(const QPoint &)));

    this->setLayout(layout);

    layout->addWidget(subjectsTree);

    QStringList headers;
    getHeaders((SubjectItem*)nullptr, headers);
    subjectsTree->setHeaderLabels(headers);

    for(int i = 0; i < headers.size(); i++){
        subjectsTree->resizeColumnToContents(i);
    }
}

MotionView::~MotionView()
{

}

void MotionView::doUIRefresh()
{
    if(motionShallowCopy != nullptr){
        
        //budujemy drzewo
        for(auto performerIT = motionShallowCopy->performers.begin(); performerIT != motionShallowCopy->performers.end(); performerIT++){
            
            SubjectItem * subjectItem = nullptr;
            
            auto subjectUIIT = subjectsUIMapping.find(performerIT->first);

            if(subjectUIIT != subjectsUIMapping.end()){
                subjectItem = subjectUIIT->second;
            }else{
                subjectItem = new SubjectItem(this, performerIT->first);
                subjectsUIMapping[performerIT->first] = subjectItem;
                subjectsTree->addTopLevelItem(subjectItem);
            }            

            fillItemContent(subjectItem, performerIT->second);

            int subjectLocality = UnknownLocality;

            for(auto sessionIT = performerIT->second->performerConfs.begin(); sessionIT != performerIT->second->performerConfs.end(); sessionIT++){
                
                SessionItem * sessionItem = nullptr;
                auto sessionID = sessionIT->second->session->sessionID;

                auto sessionUIIT = sessionsUIMapping.find(sessionID);

                if(sessionUIIT != sessionsUIMapping.end()){
                    sessionItem = sessionUIIT->second;
                }else{
                    sessionItem = new SessionItem(this, sessionID);
                    sessionsUIMapping[sessionID] = sessionItem;
                    subjectItem->addItemBase(sessionItem);
                }    

                fillItemContent(sessionItem, sessionIT->second->session);

                int sessionLocality = UnknownLocality;                

                for(auto trialIT = sessionIT->second->session->trials.begin(); trialIT != sessionIT->second->session->trials.end(); trialIT++){
                    
                    MotionItem * motionItem = nullptr;

                    auto motionUIIT = motionsUIMapping.find(trialIT->first);

                    if(motionUIIT != motionsUIMapping.end()){
                        motionItem = motionUIIT->second;
                    }else{
                        motionItem = new MotionItem(this, trialIT->first);
                        motionsUIMapping[trialIT->first] = motionItem;
                        sessionItem->addItemBase(motionItem);
                    }

                    fillItemContent(motionItem, trialIT->second);

                    DataLocality motionLocality = source->getLocality(trialIT->second);

                    motionItem->setDataLocality(motionLocality);
                    motionItem->setDataUsage(Unloaded);

                    sessionLocality |= motionLocality;
                }

                sessionItem->setDataLocality((DataLocality)sessionLocality);
                sessionItem->setDataUsage(Unloaded);

                subjectLocality |= sessionLocality;
            }

            subjectItem->setDataLocality((DataLocality)subjectLocality);
            subjectItem->setDataUsage(Unloaded);

            setStatusWidgets(subjectsTree, subjectItem);
        }
    }else{
        subjectsTree->clear();
        subjectsUIMapping.clear();
        sessionsUIMapping.clear();
        motionsUIMapping.clear();
        filesUIMapping.clear();
    }

    QStringList headers;
    getHeaders((SubjectItem*)nullptr, headers);
    subjectsTree->setHeaderLabels(headers);

    for(int i = 0; i < headers.size(); i++){
        subjectsTree->resizeColumnToContents(i);
    }
}

void MotionView::refreshDisorderItem(int id)
{
    throw std::runtime_error("Not supported");
}

void MotionView::refreshPatientItem(int id)
{
    throw std::runtime_error("Not supported");
}

void MotionView::refreshSubjectItem(int id)
{
    subjectsUIMapping[id]->refreshStatus();
}

void MotionView::refreshSessionGroupItem(int id)
{
    throw std::runtime_error("Not supported");
}

void MotionView::refreshSessionItem(int id)
{
    sessionsUIMapping[id]->refreshStatus();
}

void MotionView::refreshMotionItem(int id)
{
    motionsUIMapping[id]->refreshStatus();
}

void MotionView::refreshFileItem(int id)
{
    filesUIMapping[id]->refreshStatus();
}

void MotionView::fillItemContent(SubjectItem * subjectItem, const communication::MotionShallowCopy::Performer * performer)
{
    subjectItem->setText(1, QString("Subject") + QString("%1").arg(performer->performerID, 4, 10, QChar('0')));
}

void MotionView::fillItemContent(SessionItem * sessionItem, const communication::MotionShallowCopy::Session * session)
{
    sessionItem->setText(1, QString(session->sessionName.c_str()));
    sessionItem->setText(2, QString(session->sessionDate.c_str()));
    sessionItem->setText(3, QString(session->motionKind.c_str()));
    sessionItem->setText(4, QString(session->sessionDescription.c_str()));

    QString attributes;

    for(auto it = session->attrs.begin(); it != session->attrs.end(); it++){
        attributes += QString(it->first.c_str()) + QString("=") + QString(it->second.c_str()) + QString(";");
    }

    sessionItem->setText(5, attributes);
    sessionItem->setText(6, QString(session->tags.c_str()));
}

void MotionView::fillItemContent(MotionItem * motionItem, const communication::MotionShallowCopy::Trial * motion)
{
    motionItem->setText(1, QString(motion->trialName.c_str()));
    motionItem->setText(2, QString(motion->trialDescription.c_str()));

    QString attributes;

    for(auto it = motion->attrs.begin(); it != motion->attrs.end(); it++){
        attributes += QString(it->first.c_str()) + QString("=") + QString(it->second.c_str()) + QString(";");
    }

    motionItem->setText(3, attributes);
}

void MotionView::fillItemContent(FileItem * fileItem, const communication::MotionShallowCopy::File * file)
{
    fileItem->setText(1, QString(file->fileName.c_str()));
    fileItem->setText(2, QString(file->fileDescription.c_str()));
}

//HEADERS
void MotionView::getHeaders(const ItemBase * itemBase, QStringList & headers) const
{
    throw std::runtime_error("Unsupported item");
}

void MotionView::getHeaders(const DisorderItem * disorderItem, QStringList & headers) const
{
    throw std::runtime_error("Unsupported item");
}

void MotionView::getHeaders(const PatientItem * patientItem, QStringList & headers) const
{
    throw std::runtime_error("Unsupported item");
}

void MotionView::getHeaders(const SubjectItem * subjectItem, QStringList & headers) const
{
    headers.push_back(QString("Status"));
    headers.push_back(QString("Subject"));
}

void MotionView::getHeaders(const SessionGroupItem * sessionGroupItem, QStringList & headers) const
{
    throw std::runtime_error("Unsupported item");
}

void MotionView::getHeaders(const SessionItem * sessionItem, QStringList & headers) const
{
    headers.push_back(QString("Status"));
    headers.push_back(QString("Session"));
    headers.push_back(QString("Date"));
    headers.push_back(QString("Motion kind"));
    headers.push_back(QString("Description"));
    headers.push_back(QString("Attributes"));
    headers.push_back(QString("Tags"));
}

void MotionView::getHeaders(const MotionItem * motionItem, QStringList & headers) const
{
    headers.push_back(QString("Status"));
    headers.push_back(QString("Motion"));
    headers.push_back(QString("Description"));
    headers.push_back(QString("Attributes"));
}

void MotionView::getHeaders(const FileItem * fileItem, QStringList & headers) const
{
    headers.push_back(QString("Status"));
    headers.push_back(QString("File"));
    headers.push_back(QString("Description"));
}

//REFRESH/SET CONTENT
void MotionView::refreshContent(ItemBase * itemBase)
{
    throw std::runtime_error("Unsupported item");
}

void MotionView::refreshContent(DisorderItem * disorderItem)
{
    throw std::runtime_error("Unsupported item");
}

void MotionView::refreshContent(PatientItem * patientItem)
{
    throw std::runtime_error("Unsupported item");
}

void MotionView::refreshContent(SubjectItem * subjectItem)
{
    fillItemContent(subjectItem, motionShallowCopy->performers.find(subjectItem->getSubjectID())->second);
}

void MotionView::refreshContent(SessionGroupItem * sessionGroupItem)
{
    throw std::runtime_error("Unsupported item");
}

void MotionView::refreshContent(SessionItem * sessionItem)
{
    fillItemContent(sessionItem, motionShallowCopy->sessions.find(sessionItem->getSessionID())->second);
}

void MotionView::refreshContent(MotionItem * motionItem)
{
    fillItemContent(motionItem, motionShallowCopy->trials.find(motionItem->getMotionID())->second);
}

void MotionView::refreshContent(FileItem * fileItem)
{
    fillItemContent(fileItem, motionShallowCopy->files.find(fileItem->getFileID())->second);
}

MedicalView::MedicalView(CommunicationDataSource * source) : ItemView(source), patientsTree(new QTreeWidget), disordersTree(new QTreeWidget)
{
    QHBoxLayout * layout = new QHBoxLayout;

    patientsTree->setContextMenuPolicy( Qt::CustomContextMenu );
    disordersTree->setContextMenuPolicy( Qt::CustomContextMenu );

    connect(patientsTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));
    connect(patientsTree, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenuRequested(const QPoint &)));

    connect(disordersTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onItemClicked(QTreeWidgetItem*, int)));
    connect(disordersTree, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenuRequested(const QPoint &)));

    this->setLayout(layout);

    layout->addWidget(patientsTree);
    layout->addWidget(disordersTree);
    currentView = patientsTree;
    disordersTree->hide();

    QStringList headers;
    getHeaders((PatientItem*)nullptr, headers);
    patientsTree->setHeaderLabels(headers);

    for(int i = 0; i < headers.size(); i++){
        patientsTree->resizeColumnToContents(i);
    }

    headers.clear();

    getHeaders((DisorderItem*)nullptr, headers);
    disordersTree->setHeaderLabels(headers);

    for(int i = 0; i < headers.size(); i++){
        disordersTree->resizeColumnToContents(i);
    }
}

MedicalView::~MedicalView()
{

}

//HEADERS
void MedicalView::getHeaders(const ItemBase * itemBase, QStringList & headers) const
{
    throw std::runtime_error("Not supported");
}

void MedicalView::getHeaders(const DisorderItem * disorderItem, QStringList & headers) const
{
    headers.push_back(QString("Status"));
    headers.push_back(QString("Disorder"));
}

void MedicalView::getHeaders(const PatientItem * patientItem, QStringList & headers) const
{
    headers.push_back(QString("Status"));
    headers.push_back(QString("Name"));
    headers.push_back(QString("Surname"));
    headers.push_back(QString("Gender"));
    headers.push_back(QString("Birth date"));
}

void MedicalView::getHeaders(const SubjectItem * subjectItem, QStringList & headers) const
{
    throw std::runtime_error("Not supported");
}

void MedicalView::getHeaders(const SessionGroupItem * sessionGroupItem, QStringList & headers) const
{
    headers.push_back(QString("Status"));
    headers.push_back(QString("Session Group"));
    headers.push_back(QString("Date"));
}

void MedicalView::getHeaders(const SessionItem * sessionItem, QStringList & headers) const
{
    headers.push_back(QString("Status"));
    headers.push_back(QString("Session"));
    headers.push_back(QString("Date"));
    headers.push_back(QString("Motion kind"));
    headers.push_back(QString("Description"));
    headers.push_back(QString("Attributes"));
    headers.push_back(QString("Tags"));
}

void MedicalView::getHeaders(const MotionItem * motionItem, QStringList & headers) const
{
    headers.push_back(QString("Status"));
    headers.push_back(QString("Motion"));
    headers.push_back(QString("Description"));
    headers.push_back(QString("Attributes"));
}

void MedicalView::getHeaders(const FileItem * fileItem, QStringList & headers) const
{
    headers.push_back(QString("Status"));
    headers.push_back(QString("File"));
    headers.push_back(QString("Description"));
}

//REFRESH/SET CONTENT
void MedicalView::refreshContent(ItemBase * itemBase)
{
    throw std::runtime_error("Not supported");
}

void MedicalView::refreshContent(DisorderItem * disorderItem)
{
    fillItemContent(disorderItem, medicalShallowCopy->disorders.find(disorderItem->getDisorderID())->second);
}

void MedicalView::refreshContent(PatientItem * patientItem)
{
    fillItemContent(patientItem, medicalShallowCopy->patients.find(patientItem->getPatientID())->second);
}

void MedicalView::refreshContent(SubjectItem * subjectItem)
{
    throw std::runtime_error("Not supported");
}

void MedicalView::refreshContent(SessionGroupItem * sessionGroupItem)
{
    fillItemContent(sessionGroupItem, sessionGroupsDescriprion.find(sessionGroupItem)->second);
}

void MedicalView::refreshContent(SessionItem * sessionItem)
{
    fillItemContent(sessionItem, motionShallowCopy->sessions.find(sessionItem->getSessionID())->second);
}

void MedicalView::refreshContent(MotionItem * motionItem)
{
    fillItemContent(motionItem, motionShallowCopy->trials.find(motionItem->getMotionID())->second);
}

void MedicalView::refreshContent(FileItem * fileItem)
{
    fillItemContent(fileItem, motionShallowCopy->files.find(fileItem->getFileID())->second);
}

PatientItem * MedicalView::createPatientItemTree(const communication::MedicalShallowCopy::Patient * patient)
{
    PatientItem * patientItem = nullptr;

    patientItem = new PatientItem(this, patient->patientID);
    patientsUIMapping[patient->patientID].insert(patientItem);
    patientsTree->addTopLevelItem(patientItem);

    fillItemContent(patientItem, patient);

    int patientLocality = UnknownLocality;

    auto performerIT = motionShallowCopy->performers.find(patient->motionPerformerID);

    //grupujemy sesje wg daty
    std::map<std::string, communication::MotionShallowCopy::Sessions > groupedByDateSessions;

    for(auto sessionIT = performerIT->second->performerConfs.begin(); sessionIT != performerIT->second->performerConfs.end(); sessionIT++){
        groupedByDateSessions[sessionIT->second->session->sessionDate][sessionIT->second->session->sessionID] = sessionIT->second->session;
    }

    if(groupedByDateSessions.size() == 2){    

        UTILS_ASSERT(groupedByDateSessions.size() == 2);

        //tworzymy grupy bada� - przed i po zabiegu
        auto & sessionGroups = groupedByDateSessions.begin()->second;

        UTILS_ASSERT(sessionGroups.size() == 2);

        SessionGroupItem * item = createSessionGroupItem("Before surgery", sessionGroups);

        patientLocality |= item->getDataLocality();

        patientItem->addItemBase(item);

        sessionGroups = groupedByDateSessions.rbegin()->second;

        UTILS_ASSERT(sessionGroups.size() == 2);

        item = createSessionGroupItem("After surgery", sessionGroups);

        patientLocality |= item->getDataLocality();

        patientItem->addItemBase(item);
    }
    

    patientItem->setDataLocality((DataLocality)patientLocality);
    patientItem->setDataUsage(Unloaded);

    setStatusWidgets(patientsTree, patientItem);

    return patientItem;
}

SessionGroupItem * MedicalView::createSessionGroupItem(const std::string & name, const communication::MotionShallowCopy::Sessions & sessions)
{
    SessionGroupItem * group = nullptr;
    
    std::set<int> sessionsSet;

    for(auto it = sessions.begin(); it != sessions.end(); it++){
        sessionsSet.insert(it->second->sessionID);
    }
    
    group = new SessionGroupItem(this, sessionsSet);

    SessionGroupDescription desc;
    desc.name = name;
    desc.date = sessions.begin()->second->sessionDate;

    fillItemContent(group, desc);

    sessionGroupsDescriprion[group] = desc;

    int groupLocality = UnknownLocality;

    SessionItem * session = createSessionItem("Upper body", sessions.begin()->second);

    groupLocality |= session->getDataLocality();

    group->addItemBase(session);

    session = createSessionItem("Lower body", sessions.rbegin()->second);

    groupLocality |= session->getDataLocality();

    group->addItemBase(session);

    group->setDataLocality((DataLocality)groupLocality);
    group->setDataUsage(Unloaded);

    return group;
}

SessionItem * MedicalView::createSessionItem(const std::string & prefix, const communication::MotionShallowCopy::Session * session)
{
    SessionItem * sessionItem = new SessionItem(this, session->sessionID);

    sessionsPrefixes[session->sessionID] = prefix;

    fillItemContent(sessionItem, session);

    int sessionLocality = UnknownLocality;

    for(auto it = session->trials.begin(); it != session->trials.end(); it++){
        MotionItem * motionItem = new MotionItem(this, it->first);

        sessionItem->addItemBase(motionItem);

        fillItemContent(motionItem, it->second);

        DataLocality locality = source->getLocality(it->second);

        motionItem->setDataLocality(locality);
        motionItem->setDataUsage(Unloaded);

        sessionLocality |= locality;
    }

    if(sessionLocality != PartiallyLocal){
        for(auto it = session->files.begin(); it != session->files.end(); it++){
            sessionLocality |= source->getLocality(it->second);
            if(sessionLocality == PartiallyLocal){
                break;
            }
        }
    }

    sessionItem->setDataLocality((DataLocality)sessionLocality);
    sessionItem->setDataUsage(Unloaded);

    return sessionItem;
}

void MedicalView::fillContextMenu(const DisorderItem * disorderItem, QMenu & menu)
{
    ItemView::fillContextMenu(disorderItem, menu);
    addChangeViewAction(menu);
}

void MedicalView::fillContextMenu(const PatientItem * patientItem, QMenu & menu)
{
    ItemView::fillContextMenu(patientItem, menu);
    addChangeViewAction(menu);
}

void MedicalView::fillContextMenu(const SubjectItem * subjectItem, QMenu & menu)
{
    ItemView::fillContextMenu(subjectItem, menu);
    addChangeViewAction(menu);
}

void MedicalView::fillContextMenu(const SessionGroupItem * sessionGroupItem, QMenu & menu)
{
    ItemView::fillContextMenu(sessionGroupItem, menu);
    addChangeViewAction(menu);
}

void MedicalView::fillContextMenu(const SessionItem * sessionItem, QMenu & menu)
{
    ItemView::fillContextMenu(sessionItem, menu);
    addChangeViewAction(menu);
}

void MedicalView::fillContextMenu(const MotionItem * motionItem, QMenu & menu)
{
    ItemView::fillContextMenu(motionItem, menu);
    addChangeViewAction(menu);
}

void MedicalView::fillContextMenu(const FileItem * fileItem, QMenu & menu)
{
    ItemView::fillContextMenu(fileItem, menu);
    addChangeViewAction(menu);
}

void MedicalView::showPatients()
{
    //this->layout()->removeWidget(currentView);
    currentView->hide();
    currentView = patientsTree;
    currentView->show();
    //this->layout()->addWidget(currentView);
}

void MedicalView::showDisorders()
{
    currentView->hide();
    currentView = disordersTree;
    currentView->show();
    /*this->layout()->removeWidget(currentView);
    currentView = disordersTree;
    this->layout()->addWidget(currentView);*/
}

void MedicalView::addChangeViewAction(QMenu & menu)
{
    menu.addSeparator();
    QMenu * view = menu.addMenu(QString("View"));
    QAction * patientsView = view->addAction(QString("Patients"));
    QAction * disordersView = view->addAction(QString("Disorders"));

    patientsView->setCheckable(true);
    disordersView->setCheckable(true);

    if(currentView == patientsTree){
        patientsView->setChecked(true);
        patientsView->setEnabled(false);
    }else{
        disordersView->setChecked(true);
        disordersView->setEnabled(false);
    }

    connect(patientsView, SIGNAL(triggered()), this, SLOT(showPatients()));
    connect(disordersView, SIGNAL(triggered()), this, SLOT(showDisorders()));
}

void MedicalView::doUIRefresh()
{
    if(motionShallowCopy != nullptr && medicalShallowCopy != nullptr){

        //budujemy drzewo pacjent�w
        for(auto patientIT = medicalShallowCopy->patients.begin(); patientIT != medicalShallowCopy->patients.end(); patientIT++){
            createPatientItemTree(patientIT->second);
        }

        //budujemy drzewo chorob
        for(auto disorderIT = medicalShallowCopy->disorders.begin(); disorderIT != medicalShallowCopy->disorders.end(); disorderIT++){
            DisorderItem * disorderItem = new DisorderItem(this, disorderIT->first);

            fillItemContent(disorderItem, disorderIT->second);

            disordersTree->addTopLevelItem(disorderItem);

            int disorderLocality = UnknownLocality;

            for(auto patientIT = medicalShallowCopy->patientsByDisorder[disorderIT->first].begin(); patientIT != medicalShallowCopy->patientsByDisorder[disorderIT->first].end(); patientIT++){
                PatientItem * patientItem = new PatientItem(this, (*patientIT)->patientID);

                disorderItem->addItemBase(patientItem);

                patientsUIMapping[(*patientIT)->patientID].insert(patientItem);

                fillItemContent(patientItem, *patientIT);

                patientItem->setDataLocality((*(patientsUIMapping[(*patientIT)->patientID].begin()))->getDataLocality());
                disorderLocality |= patientItem->getDataLocality();
                patientItem->setDataUsage(Unloaded);
            }

            disorderItem->setDataLocality((DataLocality)disorderLocality);
            disorderItem->setDataUsage(Unloaded);

            setStatusWidgets(disordersTree, disorderItem);
        }

    }else{
        patientsTree->clear();
        disordersTree->clear();
        patientsUIMapping.clear();
        sessionGroupsDescriprion.clear();
        sessionsPrefixes.clear();
        sessionsUIMapping.clear();
        motionsUIMapping.clear();
        filesUIMapping.clear();
    }

    QStringList headers;
    getHeaders((PatientItem*)nullptr, headers);
    patientsTree->setHeaderLabels(headers);

    for(int i = 0; i < headers.size(); i++){
        patientsTree->resizeColumnToContents(i);
    }

    headers.clear();

    getHeaders((DisorderItem*)nullptr, headers);
    disordersTree->setHeaderLabels(headers);

    for(int i = 0; i < headers.size(); i++){
        disordersTree->resizeColumnToContents(i);
    }
}

void MedicalView::refreshDisorderItem(int id)
{
    disordersUIMapping[id]->refreshStatus();
}

void MedicalView::refreshPatientItem(int id)
{
    auto it = patientsUIMapping[id].begin();
    (*it)->refreshStatus();
    DataLocality locality = (*it)->getDataLocality();
    DataUsage usage = (*it)->getDataUsage();
    
    it++;

    for( ; it != patientsUIMapping[id].end(); it++){
        (*it)->setDataLocality(locality);
        (*it)->setDataUsage(usage);
    }
}

void MedicalView::refreshSubjectItem(int id)
{
    throw std::runtime_error("Not supported");
}

void MedicalView::refreshSessionGroupItem(int id)
{
    throw std::runtime_error("Not supported");
}

void MedicalView::refreshSessionItem(int id)
{
    sessionsUIMapping[id]->refreshStatus();
}

void MedicalView::refreshMotionItem(int id)
{
    motionsUIMapping[id]->refreshStatus();
}

void MedicalView::refreshFileItem(int id)
{
    filesUIMapping[id]->refreshStatus();
}

void MedicalView::fillItemContent(DisorderItem * disorderItem, const communication::MedicalShallowCopy::Disorder * disorder)
{
    disorderItem->setText(1, QString(disorder->name.c_str()));
}

void MedicalView::fillItemContent(PatientItem * patientItem, const communication::MedicalShallowCopy::Patient * patient)
{
    patientItem->setText(1, QString(patient->name.c_str()));
    patientItem->setText(2, QString(patient->surname.c_str()));
    patientItem->setText(3, QString(QChar(patient->gender)));
    patientItem->setText(4, QString(patient->birthDate.c_str()));
}

void MedicalView::fillItemContent(SessionGroupItem * sessionItem, const SessionGroupDescription & description)
{
    sessionItem->setText(1, QString(description.name.c_str()));
    sessionItem->setText(2, QString(description.date.c_str()));
}

void MedicalView::fillItemContent(SessionItem * sessionItem, const communication::MotionShallowCopy::Session * session)
{
    sessionItem->setText(1, QString(sessionsPrefixes[session->sessionID].c_str()) + QString(" - ") + QString(session->sessionName.c_str()));
    sessionItem->setText(2, QString(session->sessionDate.c_str()));
    sessionItem->setText(3, QString(session->motionKind.c_str()));
    sessionItem->setText(4, QString(session->sessionDescription.c_str()));

    QString attributes;

    for(auto it = session->attrs.begin(); it != session->attrs.end(); it++){
        attributes += QString(it->first.c_str()) + QString("=") + QString(it->second.c_str()) + QString(";");
    }

    sessionItem->setText(5, attributes);
    sessionItem->setText(6, QString(session->tags.c_str()));
}

void MedicalView::fillItemContent(MotionItem * motionItem, const communication::MotionShallowCopy::Trial * motion)
{
    motionItem->setText(1, QString(motion->trialName.c_str()));
    motionItem->setText(2, QString(motion->trialDescription.c_str()));

    QString attributes;

    for(auto it = motion->attrs.begin(); it != motion->attrs.end(); it++){
        attributes += QString(it->first.c_str()) + QString("=") + QString(it->second.c_str()) + QString(";");
    }

    motionItem->setText(3, attributes);
}

void MedicalView::fillItemContent(FileItem * fileItem, const communication::MotionShallowCopy::File * file)
{
    fileItem->setText(1, QString(file->fileName.c_str()));
    fileItem->setText(2, QString(file->fileDescription.c_str()));
}

CommunicationDataSource::CommunicationDataSource(communication::CommunicationManager * communicationManager, ISubjectService * subjectService)
    : communicationManager(communicationManager), widget(nullptr), refreshAction(nullptr), subjectService(subjectService)
{
    UTILS_ASSERT((communicationManager != nullptr), "Niezainicjalizowany CommunicationManager");
    if(communicationManager == nullptr){
        throw std::runtime_error("Uninitialized communication manager");
    }
}

CommunicationDataSource::~CommunicationDataSource()
{

}

QWidget * CommunicationDataSource::getOrCreateView()
{
    if(widget == nullptr){
        //inicjalizujemy zak��dki naszego okienka
        widget = new QWidget;

        QHBoxLayout * layout = new QHBoxLayout;

        widget->setLayout(layout);

        //pacjenci
        //dodajemy jako domy�lny widget

        //ItemView * view = new MedicalView(this);
        ItemView * view = new MotionView(this);
        view->refresh();

        layout->addWidget(view);
    }

    return widget;
}

QDialog * CommunicationDataSource::getOrCreateConfigurationDialog()
{
    return nullptr;
}

QWidget * CommunicationDataSource::getOrCreateFiltersWidget()
{
    return nullptr;
}

QAction * CommunicationDataSource::getOrCreateRefreshAction()
{
    return refreshAction;
}

const std::string & CommunicationDataSource::getName() const
{
    static const std::string name("Communication Data Source");
    return name;
}

void CommunicationDataSource::init(core::IMemoryDataManager * memoryDataManager, core::IFileDataManager * fileDataManager)
{
    this->memoryDataManager = memoryDataManager;
    this->fileDataManager = fileDataManager;

    if(memoryDataManager == nullptr || fileDataManager == nullptr){
        throw std::runtime_error("Provided uninitialized managers");
    }

    motionShallowCopyParser.reset(new MotionShallowCopyParser());
    motionMetadataParser.reset(new MotionMetadataParser());
    medicalShallowCopyParser.reset(new MedicalShallowCopyParser());
    medicalMetadataParser.reset(new MedicalMetadataParser());

    patientsWidget = new QTreeWidget;
    patientsWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(patientsWidget, SIGNAL(customContextMenuRequested( const QPoint& )), this, SLOT(showContextMenu(const QPoint &)));

    disordersWidget = new QTreeWidget;
    disordersWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(disordersWidget, SIGNAL(customContextMenuRequested( const QPoint& )), this, SLOT(showContextMenu(const QPoint &)));

    connect(patientsWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(itemClicked(QTreeWidgetItem *, int)));
    connect(disordersWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(itemClicked(QTreeWidgetItem *, int)));

    localDataPath = core::getPathInterface()->getUserDataPath() / "data" / "sessions";

    if(core::Filesystem::pathExists(localDataPath) == false){
        core::Filesystem::createDirectory(localDataPath);
    }

    localSchemasPath = core::getPathInterface()->getApplicationDataPath() / "db" / "schema";
    localMotionSchemasPath = localSchemasPath / "motion";
    localMedicalSchemasPath = localSchemasPath / "medical";

    if(core::Filesystem::pathExists(localMotionSchemasPath) == false){
        core::Filesystem::createDirectory(localMotionSchemasPath);
    }

    if(core::Filesystem::pathExists(localMedicalSchemasPath) == false){
        core::Filesystem::createDirectory(localMedicalSchemasPath);
    }

    localMotionShallowCopyPath = localMotionSchemasPath / "shallowCopy.xml";
    localMotionMetadataPath = localMotionSchemasPath / "metadata.xml";
    localMedicalShallowCopyPath = localMedicalSchemasPath / "shallowCopy.xml";
    localMedicalMetadataPath = localMedicalSchemasPath / "metadata.xml";
    noPhoto.reset(new QPixmap());

    //tworzymy pakiet zlecen dla CommunicationManager
    bool exist = true;
    if(core::Filesystem::pathExists(localMotionShallowCopyPath) == false){
        exist = false;
    }
    
    if(core::Filesystem::pathExists(localMotionMetadataPath) == false){
        exist = false;
    }
    
    if(core::Filesystem::pathExists(localMedicalShallowCopyPath) == false){
        exist = false;
    }
    
    if(core::Filesystem::pathExists(localMedicalMetadataPath) == false){
        exist = false;
    }
    
    if(exist == false /*lub nasza wersja wymaga aktualizacji*/){
        //dorzucamy pakiet zlece� dla CM
        //czekamy a� si� sko�cz�
    }
    
    //parsujemy pliki i od�wie�amy schematy
    readMotionDbSchemas(localMotionShallowCopyPath.string(), localMotionMetadataPath.string());
    readMedicalDbSchemas(localMedicalShallowCopyPath.string(), localMedicalMetadataPath.string()); 
}

const MotionShallowCopyPtr & CommunicationDataSource::getMotionShallowCopy() const
{
    return motionShallowCopy;
}

const communication::MotionMetaData::MetaData & CommunicationDataSource::getMotionMetadata() const
{
    return motionMetadata;
}

const MedicalShallowCopyPtr & CommunicationDataSource::getMedicalShallowCopy() const
{
    return medicalShallowCopy;
}

const communication::MedicalMetaData::MetaData & CommunicationDataSource::getMedicalMetadata() const
{
    return medicalMetadata;
}

DataLocality CommunicationDataSource::getLocality(const communication::MotionShallowCopy::Performer * subject) const
{
    int locality = UnknownLocality;

    for(auto it = subject->performerConfs.begin(); it != subject->performerConfs.end(); it++){
        locality |= getLocality(it->second->session);
        if(locality == PartiallyLocal){
            break;
        }
    }

    return (DataLocality)locality;
}

DataLocality CommunicationDataSource::getLocality(const communication::MotionShallowCopy::Session * session) const
{
    int locality = UnknownLocality;

    for(auto it = session->files.begin(); it != session->files.end(); it++){
        locality |= getLocality(it->second);
        if(locality == PartiallyLocal){
            break;
        }
    }

    if(locality != PartiallyLocal){
        for(auto it = session->trials.begin(); it != session->trials.end(); it++){
            locality |= getLocality(it->second);
            if(locality == PartiallyLocal){
                break;
            }
        }
    }

    return (DataLocality)locality;
}

DataLocality CommunicationDataSource::getLocality(const communication::MotionShallowCopy::Trial * motion) const
{
    int locality = UnknownLocality;

    for(auto it = motion->files.begin(); it != motion->files.end(); it++){
        locality |= getLocality(it->second);
        if(locality == PartiallyLocal){
            break;
        }
    }

    return (DataLocality)locality;
}

DataLocality CommunicationDataSource::getLocality(const communication::MotionShallowCopy::File * file) const
{
    if(core::Filesystem::pathExists(getFilePath(file)) == true){
        return Local;
    }

    return Remote;
}

DataUsage CommunicationDataSource::getUsage(const communication::MedicalShallowCopy::Disorder * disorder) const
{
    int usage = UnknownUsage;

    for(auto it = medicalShallowCopy->patientsByDisorder[disorder->disorderID].begin(); it != medicalShallowCopy->patientsByDisorder[disorder->disorderID].end(); it++){
        usage |= getUsage(*it);
        if(usage == PartiallyLoaded){
            break;
        }
    }

    return (DataUsage)usage;
}

DataUsage CommunicationDataSource::getUsage(const communication::MedicalShallowCopy::Patient * patient) const
{
    return getUsage(motionShallowCopy->performers[patient->motionPerformerID]);
}

DataUsage CommunicationDataSource::getUsage(const communication::MotionShallowCopy::Performer * subject) const
{
    int usage = UnknownUsage;

    for(auto it = subject->performerConfs.begin(); it != subject->performerConfs.end(); it++){
        usage |= getUsage(it->second->session);
        if(usage == PartiallyLoaded){
            break;
        }
    }

    return (DataUsage)usage;
}

DataUsage CommunicationDataSource::getUsage(const communication::MotionShallowCopy::Session * session) const
{
    int usage = UnknownLocality;

    std::vector<const communication::MotionShallowCopy::File*> files;

    for(auto it = session->files.begin(); it != session->files.end(); it++){
        files.push_back(it->second);
    }

    usage |= getUsage(files);

    if(usage != PartiallyLoaded){

        for(auto it = session->trials.begin(); it != session->trials.end(); it++){
            usage |= getUsage(it->second);
            if(usage == PartiallyLoaded){
                break;
            }
        }
    }

    return (DataUsage)usage;
}

DataUsage CommunicationDataSource::getUsage(const communication::MotionShallowCopy::Trial * motion) const
{
    std::vector<const communication::MotionShallowCopy::File*> files;

    for(auto it = motion->files.begin(); it != motion->files.end(); it++){
        files.push_back(it->second);
    }

    //return (DataUsage)usage;
    return getUsage(files);
}

DataUsage CommunicationDataSource::getUsage(const communication::MotionShallowCopy::File * file) const
{
    std::vector<const communication::MotionShallowCopy::File*> files;

    files.push_back(file);

    return getUsage(files);
}

DataUsage CommunicationDataSource::getUsage(const std::vector<const communication::MotionShallowCopy::File*> & files) const
{
    int ret = UnknownUsage;

    core::Files managedFiles;

    fileDataManager->getManagedData(managedFiles);

    for(auto it = files.begin(); it != files.end(); it++){
        if(fileDataManager->isExtensionSupported(core::Filesystem::fileExtension((*it)->fileName)) == true){
            auto filePath = getFilePath(*it);

            if(managedFiles.find(filePath) != managedFiles.end()){
                ret |= Loaded;
            }else{
                ret |= Unloaded;
            }

            if(ret == PartiallyLoaded){
                break;
            }
        }
    }

    if(files.empty() == false && ret == UnknownUsage){
        ret = Loaded;
    }

    return (DataUsage)ret;
}

void CommunicationDataSource::getFiles(const communication::MedicalShallowCopy::Disorder * disorder, std::vector<const communication::MotionShallowCopy::File *> & files) const
{
    for(auto it = medicalShallowCopy->patientsByDisorder[disorder->disorderID].begin(); it != medicalShallowCopy->patientsByDisorder[disorder->disorderID].end(); it++){
        getFiles(*it, files);
    }
}

void CommunicationDataSource::getFiles(const communication::MedicalShallowCopy::Patient * patient, std::vector<const communication::MotionShallowCopy::File *> & files) const
{
    getFiles(motionShallowCopy->performers.find(patient->motionPerformerID)->second, files);
}

void CommunicationDataSource::getFiles(const communication::MotionShallowCopy::Performer * subject, std::vector<const communication::MotionShallowCopy::File *> & files) const
{
    for(auto it = subject->performerConfs.begin(); it != subject->performerConfs.end(); it++){
        getFiles(it->second->session, files);
    }
}

void CommunicationDataSource::getFiles(const communication::MotionShallowCopy::Session * session, std::vector<const communication::MotionShallowCopy::File *> & files) const
{
    for(auto it = session->trials.begin(); it != session->trials.end(); it++){
        getFiles(it->second, files);
    }

    for(auto it = session->files.begin(); it != session->files.end(); it++){
        files.push_back(it->second);
    }
}

void CommunicationDataSource::getFiles(const std::set<const communication::MotionShallowCopy::Session *> & sessions, std::vector<const communication::MotionShallowCopy::File *> & files) const
{
    for(auto it = sessions.begin(); it != sessions.end(); it++){
        getFiles(*it, files);
    }
}

void CommunicationDataSource::getFiles(const communication::MotionShallowCopy::Trial * motion, std::vector<const communication::MotionShallowCopy::File *> & files) const
{
    for(auto it = motion->files.begin(); it != motion->files.end(); it++){
        files.push_back(it->second);
    }
}

void CommunicationDataSource::filterMissingFiles(std::vector<const communication::MotionShallowCopy::File *> & files) const
{
    auto it = files.begin();
    while(it != files.end()){
        if(getLocality(*it) == Local){
            it = files.erase(it);
        }else{
            it++;
        }
    }
}

void CommunicationDataSource::filterUnloadedFiles(std::vector<const communication::MotionShallowCopy::File *> & files) const
{
    //TODO
    throw std::runtime_error("To implement");
}

bool CommunicationDataSource::download(const communication::MedicalShallowCopy::Disorder * disorder)
{
    std::vector<const communication::MotionShallowCopy::File *> files;
    getFiles(disorder, files);
    filterMissingFiles(files);
    return downloadFiles(files);
}

bool CommunicationDataSource::download(const communication::MedicalShallowCopy::Patient * patient)
{
    std::vector<const communication::MotionShallowCopy::File *> files;
    getFiles(patient, files);
    filterMissingFiles(files);
    return downloadFiles(files);
}

bool CommunicationDataSource::download(const communication::MotionShallowCopy::Performer * subject)
{
    std::vector<const communication::MotionShallowCopy::File *> files;
    getFiles(subject, files);
    filterMissingFiles(files);
    return downloadFiles(files);
}

bool CommunicationDataSource::download(const communication::MotionShallowCopy::Session * session)
{
    std::vector<const communication::MotionShallowCopy::File *> files;
    getFiles(session, files);
    filterMissingFiles(files);
    return downloadFiles(files);
}

bool CommunicationDataSource::download(const std::set<const communication::MotionShallowCopy::Session *> & sessions)
{
    std::vector<const communication::MotionShallowCopy::File *> files;
    getFiles(sessions, files);
    filterMissingFiles(files);
    return downloadFiles(files);
}

bool CommunicationDataSource::download(const communication::MotionShallowCopy::Trial * motion)
{
    std::vector<const communication::MotionShallowCopy::File *> files;
    getFiles(motion, files);
    for(auto it = motion->session->files.begin(); it != motion->session->files.end(); it++){
        files.push_back(it->second);
    }
    filterMissingFiles(files);
    return downloadFiles(files);
}

bool CommunicationDataSource::download(const communication::MotionShallowCopy::File * file)
{
    std::vector<const communication::MotionShallowCopy::File *> files;
    files.push_back(file);

    return downloadFiles(files);
}

void CommunicationDataSource::load(const communication::MedicalShallowCopy::Disorder * disorder)
{

}

void CommunicationDataSource::load(const communication::MedicalShallowCopy::Patient * patient)
{

}

void CommunicationDataSource::load(const communication::MotionShallowCopy::Performer * subject)
{
    for(auto it = subject->performerConfs.begin(); it != subject->performerConfs.end(); it++){
        load(it->second->session);
    }

    createSubject(subject);
}

void CommunicationDataSource::load(const communication::MotionShallowCopy::Session * session)
{
    for(auto it = session->trials.begin(); it != session->trials.end(); it++){
        load(it->second);
    }

    createSession(session);
}

void CommunicationDataSource::load(const std::set<const communication::MotionShallowCopy::Session *> & sessions)
{
    for(auto it = sessions.begin(); it != sessions.end(); it++){
        load(*it);
    }
}

void CommunicationDataSource::load(const communication::MotionShallowCopy::Trial * motion)
{
    for(auto it = motion->files.begin(); it != motion->files.end(); it++){
        load(it->second);
    }

    createMotion(motion);
}

void CommunicationDataSource::load(const communication::MotionShallowCopy::File * file)
{
    try{
        fileDataManager->addData(getFilePath(file));
    }catch(...){

    }
}

std::pair<DataLocality, DataUsage> CommunicationDataSource::getStatus(const communication::MedicalShallowCopy::Disorder * disorder) const
{
    std::vector<const communication::MotionShallowCopy::File *> files;

    getFiles(disorder, files);

    return getStatus(files);
}

std::pair<DataLocality, DataUsage> CommunicationDataSource::getStatus(const communication::MedicalShallowCopy::Patient * patient) const
{
    return getStatus(motionShallowCopy->performers.find(patient->motionPerformerID)->second);
}

std::pair<DataLocality, DataUsage> CommunicationDataSource::getStatus(const communication::MotionShallowCopy::Performer * subject) const
{
    std::vector<const communication::MotionShallowCopy::File *> files;

    getFiles(subject, files);

    return getStatus(files);
}

std::pair<DataLocality, DataUsage> CommunicationDataSource::getStatus(const communication::MotionShallowCopy::Session * session) const
{
    std::vector<const communication::MotionShallowCopy::File *> files;

    getFiles(session, files);

    return getStatus(files);
}

std::pair<DataLocality, DataUsage> CommunicationDataSource::getStatus(const std::set<const communication::MotionShallowCopy::Session *> & sessions) const
{
    std::vector<const communication::MotionShallowCopy::File *> files;

    getFiles(sessions, files);

    return getStatus(files);
}

std::pair<DataLocality, DataUsage> CommunicationDataSource::getStatus(const communication::MotionShallowCopy::Trial * motion) const
{
    std::vector<const communication::MotionShallowCopy::File *> files;

    getFiles(motion, files);

    return getStatus(files);
}

std::pair<DataLocality, DataUsage> CommunicationDataSource::getStatus(const communication::MotionShallowCopy::File * file) const
{
    return std::pair<DataLocality, DataUsage>(getLocality(file), getUsage(file));
}

std::pair<DataLocality, DataUsage> CommunicationDataSource::getStatus(const std::vector<const communication::MotionShallowCopy::File *> files) const
{
    std::pair<int, int> ret(UnknownLocality, UnknownUsage);

    core::Files managedFiles;
    fileDataManager->getManagedData(managedFiles);

    std::set<const communication::MotionShallowCopy::File *> uniqueFiles(files.begin(), files.end());

    for(auto it = uniqueFiles.begin(); it != uniqueFiles.end(); it++){
        ret.first |= getLocality(*it);

        ret.second |= managedFiles.find(getFilePath(*it)) == managedFiles.end() ? Unloaded : Loaded;

        if(ret.first == PartiallyLocal && ret.second == PartiallyLoaded){
            break;
        }
    }

    return std::pair<DataLocality, DataUsage>((DataLocality)ret.first, (DataUsage)ret.second); 
}

//void CommunicationDataSource::refreshSource()
//{
//    //tworzymy pakiet zlecen dla CommunicationManager
//    bool exist = true;
//    if(core::Filesystem::pathExists(localMotionShallowCopyPath) == false){
//        exist = false;
//    }
//
//    if(core::Filesystem::pathExists(localMotionMetadataPath) == false){
//        exist = false;
//    }
//
//    if(core::Filesystem::pathExists(localMedicalShallowCopyPath) == false){
//        exist = false;
//    }
//
//    if(core::Filesystem::pathExists(localMedicalMetadataPath) == false){
//        exist = false;
//    }
//
//    if(exist == false /*lub nasza wersja wymaga aktualizacji*/){
//        //dorzucamy pakiet zlece� dla CM
//        //czekamy a� si� sko�cz�
//    }
//
//    //parsujemy pliki i od�wie�amy schematy
//    readMotionDbSchemas(localMotionShallowCopyPath.string(), localMotionMetadataPath.string());
//    readMedicalDbSchemas(localMedicalShallowCopyPath.string(), localMedicalMetadataPath.string());   
//
//    //aktualizujemy zawarto�� widok�w
//    refreshPatientsView();
//    refreshDisordersView();
//}

void CommunicationDataSource::readMotionDbSchemas(const std::string& shallowCopyPath, const std::string& metaDataPath)
{
    motionShallowCopyParser->parseFile(shallowCopyPath);
    motionShallowCopy = motionShallowCopyParser->getShallowCopy();
    
    motionMetadataParser->parseFile(metaDataPath);
    motionMetadata = motionMetadataParser->getMetadata();
}

void CommunicationDataSource::readMedicalDbSchemas(const std::string& shallowCopyPath, const std::string& metaDataPath)
{
    medicalShallowCopyParser->parseFile(shallowCopyPath);
    medicalShallowCopy = medicalShallowCopyParser->getShallowCopy();

    medicalMetadataParser->parseFile(metaDataPath);
    medicalMetadata = medicalMetadataParser->getMetadata();
}

//void CommunicationDataSource::refreshPatientsView()
//{
//    PatientItem * patientItem = nullptr;
//    for(auto patientIT = medicalShallowCopy->patients.begin(); patientIT != medicalShallowCopy->patients.end(); patientIT++){
//        patientItem = createPatientItem(patientIT->second);
//
//        patientsWidget->addTopLevelItem(patientItem);
//
//        core::shared_ptr<QPixmap> photo(noPhoto);
//        auto photoPath = getPhotoPath(patientIT->second->patientID);
//        if(core::Filesystem::pathExists(photoPath) == true){
//            core::shared_ptr<QPixmap> tmpPhoto(new QPixmap(QString(photoPath.string().c_str())));
//            if(photo->isNull() == false){
//                photo = tmpPhoto;
//            }
//        }
//
//        patientPhotos.insert(std::map<communication::MedicalShallowCopy::Patient*, core::shared_ptr<QPixmap> >::value_type(patientIT->second,photo));
//
//        QLabel * label = new QLabel();
//        label->setMaximumSize(20, 20);
//        label->setMinimumSize(20, 20); 
//        label->setFrameShape(QFrame::Box);
//        label->setPixmap(*photo);
//
//        patientsWidget->setItemWidget(patientItem, 1, label);
//
//        bool foundLocal = false;
//        bool foundRemote = false;
//
//        auto performerIT = motionShallowCopy->performers.find(patientIT->second->motionPerformerID);
//        if(performerIT != motionShallowCopy->performers.end()){
//            //grupujemy sesje - goczalkowice i reszta
//
//            std::map<std::string, std::vector<communication::MotionShallowCopy::Session*>> sessionsByDate;
//            //Sessions otherSessions;
//
//            //iterujemy po wszystkich sesjach pacjenta
//            for(auto configurationIT = performerIT->second->performerConfs.begin(); configurationIT != performerIT->second->performerConfs.end(); configurationIT++){
//                //czy dane z goczalkowic?
//                //if(configurationIT->second->session->labID == 1){
//                //grupujemy po dacie sesje
//                sessionsByDate[configurationIT->second->session->sessionDate].push_back(configurationIT->second->session);
//                //}else{
//                //    otherSessions.insert(configurationIT->second->session);
//                //}
//            }
//
//            //czy sa dane z goczalkowic?
//            if(sessionsByDate.empty() == false){
//                //pierwszy wpis tworzy grupe przed zabiegiem
//                SessionGroupItem * sessionGroupItem = createSessionGroupItem("Badania przed zabiegiem", sessionsByDate.begin()->second);
//                patientItem->addItemBase(sessionGroupItem);
//
//                //drugi wpis tworzy grupe po zabiegu
//                sessionGroupItem = createSessionGroupItem("Badania po zabiegu", sessionsByDate.rbegin()->second);
//                patientItem->addItemBase(sessionGroupItem);
//
//                DataLocality patientLocality = Remote;
//
//                for(int i = 0; i < patientItem->size(); i++){
//                    DataLocality groupLocality = patientItem->getItemBaseChild(i)->getDataLocality();
//                    if(groupLocality == PartiallyLocal){
//                        patientLocality = PartiallyLocal;
//                        break;
//                    }else if(groupLocality == Local){
//                        foundLocal = true;
//                    }else{
//                        foundRemote = true;
//                    }
//
//                    if(foundLocal == true && foundRemote == true){
//                        patientLocality = PartiallyLocal;
//                        break;
//                    }
//                }
//
//                if(patientLocality == PartiallyLocal){
//                    patientItem->setDataLocality(PartiallyLocal);
//                }else if(foundLocal == true){
//                    patientItem->setDataLocality(Local);
//                }else{
//                    patientItem->setDataLocality(Remote);
//                }
//
//            }else{
//                patientItem->setDataLocality(Local);
//            }
//        }else{
//            patientItem->setDataLocality(Local);
//        }
//
//        setStatusWidgets(patientItem);
//    }
//
//    if(patientItem != nullptr){
//        int columns = patientItem->columnCount();
//        patientsWidget->setColumnCount(columns);
//        QStringList headers;
//        patientItem->getHeaders(headers);
//        patientsWidget->setHeaderLabels(headers);
//
//        for(int i = 0; i < columns; i++){
//            patientsWidget->resizeColumnToContents(i);
//        }
//    }
//}

//void CommunicationDataSource::refreshDisordersView()
//{
//
//}

void CommunicationDataSource::retrivePatientPhotos()
{
    if(medicalShallowCopy != nullptr && communicationManager != nullptr){
        for(auto it = medicalShallowCopy->patients.begin(); it != medicalShallowCopy->patients.end(); it++){
            communicationManager->requestPhoto(it->first, getPhotoPath(it->first).string());
        }
    }
}

core::Filesystem::Path CommunicationDataSource::getSessionPath(const communication::MotionShallowCopy::Session * session) const
{
    return localDataPath / session->sessionName;
}

core::Filesystem::Path CommunicationDataSource::getFilePath(const communication::MotionShallowCopy::File * file) const
{
    return getSessionPath(file->isSessionFile() ? file->session : file->trial->session) / file->fileName;
}

core::Filesystem::Path CommunicationDataSource::getPhotoPath(int photoID) const
{
    return localMedicalSchemasPath / boost::lexical_cast<std::string>(photoID);
}

//void CommunicationDataSource::itemClicked(QTreeWidgetItem * item, int column)
//{
//    ItemBase * itemBase = dynamic_cast<ItemBase*>(item);
//
//    if(itemBase == nullptr){
//        return;
//    }
//
//    QTreeWidget * tree = qobject_cast<QTreeWidget*>(sender());
//    int columns = itemBase->columnCount();
//    tree->setColumnCount(columns);
//
//    QStringList headers;
//    itemBase->getHeaders(headers);
//    tree->setHeaderLabels(headers);
//
//    for(int i = 0; i < columns; i++) {
//        tree->resizeColumnToContents(i);
//    }
//}
//
//void CommunicationDataSource::showContextMenu(const QPoint & pos)
//{
//    QTreeWidget * tree = qobject_cast<QTreeWidget*>(sender());
//
//    if(tree == nullptr){
//        return;
//    }
//
//    QMenu menu;
//
//    QMenu * view = menu.addMenu(tr("View"));
//    QAction * patientViewAction = view->addAction(tr("Patients"));
//    patientViewAction->setCheckable(true);
//    QAction * disordersViewAction = view->addAction(tr("Disorders"));
//    disordersViewAction->setCheckable(true);
//
//    if(tree == patientsWidget){
//        patientViewAction->setChecked(true);
//        patientViewAction->setEnabled(false);
//    }else{
//        disordersViewAction->setChecked(true);
//        disordersViewAction->setEnabled(false);
//    }
//
//    menu.addSeparator();
//
//    QTreeWidgetItem * item = tree->itemAt(pos);
//
//    if(item != nullptr){
//        ItemBase * itemBase = dynamic_cast<ItemBase*>(item);
//        itemBase->fillContextMenu(menu);
//    }
//
//    menu.exec(tree->mapToGlobal(pos));
//}

bool CommunicationDataSource::downloadFiles(const std::vector<const communication::MotionShallowCopy::File *> & files)
{
    bool ret = true;
    if(files.size() > 10){
        //warning - sporo plik�w, czy kontynuowa�
        std::stringstream info;
        info << "You are about to start the download of " << files.size() << " files. Are you sure there is enough space on disk and you can wait some longer time?";
        QMessageBox message(QMessageBox::Warning, QString("Attempt to download large amount of files"), QString(info.str().c_str()), QMessageBox::Ok | QMessageBox::Cancel);
        message.setDefaultButton(QMessageBox::Cancel);
        int ret = message.exec();

        if(ret == QMessageBox::Cancel){
            return false;
        }
    }

    //utorzone �cie�ki dla rollbacka je�li co� si� nie uda
    std::set<core::Filesystem::Path> createdDirectories;
    std::set<core::Filesystem::Path> downloadedFiles;

    std::map<int, std::string> toDownload;
    std::set<const communication::MotionShallowCopy::Session *> sessions;

    //przygotowujemy pliki do �ci�gania w formie transakcji kt�ra automatycznie si� rollbackuje
    for(auto it = files.begin(); it != files.end(); it++){
        toDownload[(*it)->fileID] = getFilePath(*it).string();
        sessions.insert((*it)->isSessionFile() == true ? (*it)->session : (*it)->trial->session);
    }

    std::map<int, std::string> downloaded;

    bool cleanRequired = false;
    std::string error;

    try{

        ensureSessionDirectories(sessions, createdDirectories);

        DownloadDialog dialog(communicationManager, toDownload);        
        bool result = dialog.doDownloading(downloaded);

        if(result == false){
            //rollback
            cleanRequired = true;
        }

    }catch(std::exception & e){
        error = e.what();
        cleanRequired = true;
    }catch(...){
        cleanRequired = true;
    }

    if(cleanRequired == true){
        ret = false;
        try{
            for(auto it = downloadedFiles.begin(); it != downloadedFiles.end(); it++){
                core::Filesystem::deleteFile(*it);
            }
        }catch(...){

        }

        try{
            for(auto it = createdDirectories.begin(); it != createdDirectories.end(); it++){
                core::Filesystem::deleteDirectory(*it);
            }
        }catch(...){

        }
    }

    return ret;
}

void CommunicationDataSource::ensureSessionDirectories(const std::set<const communication::MotionShallowCopy::Session*> & sessions, std::set<core::Filesystem::Path> & createdPaths) const
{
    std::set<core::Filesystem::Path> paths;

    try{

        for(auto it = sessions.begin(); it != sessions.end(); it++){
            auto path = getSessionPath(*it);
            if(core::Filesystem::pathExists(path) == false){
                core::Filesystem::createDirectory(path);
                paths.insert(path);
            }
        }

    }catch(...){
        //pr�bujemy sprz�ta�
        try{
            for(auto it = paths.begin(); it != paths.end(); it++){
                core::Filesystem::deleteDirectory(*it);
            }
        }catch(...){

        }
        //rzucamy dalej wyj�tkiem
        throw;
    }
}

void CommunicationDataSource::unpackDate(const std::string & text, communication::Date & date)
{
    boost::char_separator<char> sep("-");
    boost::tokenizer<boost::char_separator<char> > tokens(text, sep);

    auto it = tokens.begin();

    date.setYear(boost::lexical_cast<int>(*it++));
    date.setMonth(boost::lexical_cast<int>(*it++));
    date.setDay(boost::lexical_cast<int>(*it++));

    UTILS_ASSERT(it == tokens.end());
}