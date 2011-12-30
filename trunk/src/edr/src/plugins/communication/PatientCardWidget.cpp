#include "CommunicationPCH.h"

#include "PatientCardWidget.h"
#include <QtGui/QVBoxLayout>
#include <QtGui/QFormLayout>

CardSessionItem::CardSessionItem(unsigned int sessionId) : QTreeWidgetItem(), sessionID(sessionId)
{

}

unsigned int CardSessionItem::getSessionID() const
{
    return sessionID;
}

PatientCardWidget::PatientCardWidget()
    : patient(nullptr), performer(nullptr), antropometricGeneralDataModel(new QStandardItemModel()),
    antropometricLeftRightDataModel(new QStandardItemModel())
{
    init();
    refreshCard();
}

PatientCardWidget::PatientCardWidget(const communication::MedicalShallowCopy::Patient * patient,
    const communication::MotionShallowCopy::Performer * performer,
    const PhotoConstPtr & photo, const IFilterConstPtr & filter)
    : patient(patient), performer(performer), filter(filter), photo(photo), antropometricGeneralDataModel(new QStandardItemModel()),
    antropometricLeftRightDataModel(new QStandardItemModel())
{
    init();
    refreshCard();
}

void PatientCardWidget::init()
{
    setupUi(this);

    multiToolBoxWidget = new QMultiToolBox();
    //multiToolBoxWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    this->frame->layout()->addWidget(multiToolBoxWidget);
    this->frame->layout()->addItem(new QSpacerItem(0,0,QSizePolicy::Preferred, QSizePolicy::Expanding));

    //dodajemy poszczegolne elementy:

    //dane personalne
    //g³ówny widget i layout
    personalDataWidget = new QWidget();
    //personalDataWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto mainLayout = new QHBoxLayout();
    mainLayout->setMargin(0);
    personalDataWidget->setLayout(mainLayout);

    //miejsce na zdjêcie
    patientPhotoWidget = new QLabel();
    patientPhotoWidget->setText("Patient photo");
    //patientPhotoWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    mainLayout->addWidget(patientPhotoWidget);

    //formatka z danymi pacjenta
    QWidget * innerContent = new QWidget();
    //innerContent->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    auto innerLayout = new QFormLayout();
    innerLayout->setMargin(0);

    innerContent->setLayout(innerLayout);

    mainLayout->addWidget(innerContent);
    
    //elementy edytowalne formatki
    patientEdit = new QLineEdit();
    //patientEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    patientAgeEdit = new QSpinBox();
    //patientAgeEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    patientGenderEdit = new QComboBox();
    //patientGenderEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    patientGenderEdit->addItem(tr("Men"));
    patientGenderEdit->addItem(tr("Women"));

    patientBirthDateEdit = new QLabel();
    //patientBirthDateEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    //patientBirthDateEdit->setDisplayFormat(QString("dd.MM.yyyy"));

    //wypelnianie formatki
    innerLayout->addRow(tr("Patient"), patientEdit);
    innerLayout->addRow(tr("Age"), patientAgeEdit);
    innerLayout->addRow(tr("Gender"), patientGenderEdit);
    innerLayout->addRow(tr("Birth date"), patientBirthDateEdit);

    //dane o chorobach

    disordersTable = new QTableWidget();
    //disordersTable->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    
    //sesje
    sessionsWidget = new QTreeWidget();
    //sessionsWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    //adnotacje
    adnotationsWidget = new QTextEdit();
    //adnotationsWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    //dodajemy wszystko do multitoolboxa
    multiToolBoxWidget->addItem(personalDataWidget, tr("Patient data"), true);
    multiToolBoxWidget->addItem(disordersTable, tr("Disorders"));
    multiToolBoxWidget->addItem(sessionsWidget, tr("Sessions"));
    multiToolBoxWidget->addItem(adnotationsWidget, tr("Notes"));
}

PatientCardWidget::~PatientCardWidget()
{

}

void PatientCardWidget::setPatient(const communication::MedicalShallowCopy::Patient * patient, const communication::MotionShallowCopy::Performer * performer,
    const PhotoConstPtr & photo)
{
    this->photo = photo;
    if(patient != nullptr && performer != nullptr){
        this->patient = patient;
        this->performer = performer;
    }else{
        throw std::runtime_error("Uninitialized patient or performer pointers in patient card setPatient method");
    }
}

const communication::MedicalShallowCopy::Patient * PatientCardWidget::getPatient() const
{
    return patient;
}

const communication::MotionShallowCopy::Performer * PatientCardWidget::getPerformer() const
{
    return performer;
}

const PatientCardWidget::PhotoConstPtr & PatientCardWidget::getPhoto() const
{
    return photo;
}

const PatientCardWidget::IFilterConstPtr & PatientCardWidget::getFilter() const
{
    return filter;
}

void PatientCardWidget::resetCard()
{
    patient = nullptr;
    performer = nullptr;
    filter.reset();
}

void PatientCardWidget::refreshCard()
{
    centerTableCellsText(generalDataTable);
    centerTableCellsText(leftRightDataTable);


    generalDataTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    leftRightDataTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

    if(patient == nullptr || performer == nullptr){
        //proste inicjowanie pol karty wartosciami domyslnymi
        
        patientEdit->clear();
        patientEdit->setEnabled(false);

        patientBirthDateEdit->clear();
        patientBirthDateEdit->setEnabled(false);

        patientGenderEdit->setCurrentIndex(-1);
        patientGenderEdit->setEnabled(false);
        
        patientPhotoWidget->setPixmap(QPixmap());

        disordersTable->clearContents();
        sessionsWidget->clear();
        
        detailsFrame->hide();        
    }else{
        //wlasciwe inicjowanie karty dostarczonymi wartosciami
        patientEdit->setText(QString::fromUtf8((patient->surname + ", " + patient->name).c_str()));

        patientBirthDateEdit->setText(QString(patient->birthDate.c_str()));

        patientGenderEdit->setCurrentIndex(patient->gender == 'M' ? 0 : 1);
        
        //ustawiamy fotke
        if(photo != nullptr){
            patientPhotoWidget->clear();
            patientPhotoWidget->setPixmap(*photo);
            personalDataWidget->setMinimumSize(photo->size());
        }else{
            patientPhotoWidget->setText("No photo avaiable");
        }
        
        refreshDisorders();
        
        refreshSessions();

        detailsFrame->show();
    }
}

void PatientCardWidget::refreshDisorders()
{
    int sortColumns = disordersTable->horizontalHeader()->sortIndicatorSection();
    Qt::SortOrder order = disordersTable->horizontalHeader()->sortIndicatorOrder();
    //wylanczam tymczasowo sortowanie
    disordersTable->setSortingEnabled(false);

    //czyscimy
    disordersTable->clearContents();

    //ustalam ilosc wpisow
    disordersTable->setRowCount(patient->disorders.size());

    //wypelniamy chorobami
    int i = 0;
    for(auto it = patient->disorders.begin(); it != patient->disorders.end(); it++){
        //nazwa
        QTableWidgetItem *item = new QTableWidgetItem(QString::fromUtf8(it->second.disorder->name.c_str()));
        disordersTable->setItem(i,0, item);
        
        //data diagnozy
        item = new QTableWidgetItem(QString::fromUtf8(it->second.diagnosisDate.c_str()));
        disordersTable->setItem(i,1, item);
        
        //typ diagnozy
        item = new QTableWidgetItem(QString::fromUtf8(it->second.focus.c_str()));
        disordersTable->setItem(i,2, item);

        //komentarz
        item = new QTableWidgetItem(QString::fromUtf8(it->second.comments.c_str()));
        disordersTable->setItem(i,3, item);
        i++;
    }

    disordersTable->horizontalHeader()->setSortIndicator(sortColumns, order);

    //wlanczam spowrotem sortowanie
    disordersTable->setSortingEnabled(true);
    disordersTable->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
}

void PatientCardWidget::refreshSessions()
{
    //czyscimy
    sessionsWidget->clear();

    //wypelniamy sesjami

    //grupujemy wykresy na przed/po badaniu + upperBody, lowerBody
    std::map<std::string, std::map<int, const communication::MotionShallowCopy::Session *>> groupedData;

    if(filter != nullptr){

        for(auto it = performer->performerConfs.begin(); it != performer->performerConfs.end(); it++){
            auto session = it->second->session;

            if(filter->check(session) == true){
                groupedData[session->sessionDate][session->sessionID] = session;
            }else{
                groupedData[session->sessionDate];
            }
        }
    }else{
        for(auto it = performer->performerConfs.begin(); it != performer->performerConfs.end(); it++){
            auto session = it->second->session;
            groupedData[session->sessionDate][session->sessionID] = session;
        }
    }

    if(groupedData.begin()->second.empty() == false){

        QTreeWidgetItem * item = createBranch("Przed", groupedData.begin()->second);
        sessionsWidget->addTopLevelItem(item);
    }

    if(groupedData.begin()->first != groupedData.rbegin()->first && groupedData.rbegin()->second.empty() == false){
        QTreeWidgetItem * item = createBranch("Po", groupedData.rbegin()->second);
        sessionsWidget->addTopLevelItem(item);
    }

    if(sessionsWidget->topLevelItemCount() > 0){
        sessionsWidget->setCurrentItem(sessionsWidget->topLevelItem(0)->child(0));
    }

    sessionsWidget->expandAll();
    for(int i = 0; i < sessionsWidget->columnCount(); i++){
        sessionsWidget->resizeColumnToContents(i);
    }
}

void PatientCardWidget::currentSessionChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
    CardSessionItem * item = dynamic_cast<CardSessionItem*>(current);

    if(item == nullptr){
        resetAntropometricData();
    }else{

        for(auto it = performer->performerConfs.begin(); it != performer->performerConfs.end(); it++){
            if(it->second->session->sessionID == item->getSessionID()){
                fillAntropometricData(it->second->attrs);
                break;
            }
        }        
    }
}

void PatientCardWidget::centerTableCellsText(QTableWidget * table)
{
    for(int i = 0; i < table->columnCount(); i++){
        for(int j = 0; j < table->rowCount(); j++){
            auto item = table->item(j, i);
            if(item != nullptr){
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
}

void PatientCardWidget::resetAntropometricData()
{
    fillAntropometricData();
}

void PatientCardWidget::fillAntropometricData(const communication::MotionShallowCopy::Attrs & attributes)
{
    setAttribute(generalDataTable, 0, 2, "BodyMass", attributes);
    setAttribute(generalDataTable, 1, 2, "Height", attributes);
    setAttribute(generalDataTable, 2, 2, "InterAsisDistance", attributes);

    setAttribute(leftRightDataTable, 0, 2, "LeftLegLength", attributes);
    setAttribute(leftRightDataTable, 0, 3, "RightLegLenght", attributes);
    setAttribute(leftRightDataTable, 1, 2, "LeftKneeWidth", attributes);
    setAttribute(leftRightDataTable, 1, 3, "RightKneeWidth", attributes);
    setAttribute(leftRightDataTable, 2, 2, "LeftAnkleWidth", attributes);
    setAttribute(leftRightDataTable, 2, 3, "RightAnkleWidth", attributes);
    setAttribute(leftRightDataTable, 3, 2, "LeftCircuitThigh", attributes);
    setAttribute(leftRightDataTable, 3, 3, "RightCircuitThight", attributes);
    setAttribute(leftRightDataTable, 4, 2, "LeftCircuitShank", attributes);
    setAttribute(leftRightDataTable, 4, 3, "RightCircuitShank", attributes);
    setAttribute(leftRightDataTable, 5, 2, "LeftShoulderOffset", attributes);
    setAttribute(leftRightDataTable, 5, 3, "RightShoulderOffset", attributes);
    setAttribute(leftRightDataTable, 6, 2, "LeftElbowWidth", attributes);
    setAttribute(leftRightDataTable, 6, 3, "RightElbowWidth", attributes);
    setAttribute(leftRightDataTable, 7, 2, "LeftWristWidth", attributes);
    setAttribute(leftRightDataTable, 7, 3, "RightWristWidth", attributes);
    setAttribute(leftRightDataTable, 8, 2, "LeftWristThickness", attributes);
    setAttribute(leftRightDataTable, 8, 3, "RightWristThickness", attributes);
    setAttribute(leftRightDataTable, 9, 2, "LeftHandWidth", attributes);
    setAttribute(leftRightDataTable, 9, 3, "RightHandWidth", attributes);
    setAttribute(leftRightDataTable, 10, 2, "LeftHandThickness", attributes);
    setAttribute(leftRightDataTable, 10, 3, "RightHandThickness", attributes);
}

void PatientCardWidget::setAttribute(QTableWidget * table, int row, int column, const std::string & attribute, const communication::MotionShallowCopy::Attrs & attributes)
{
    QString value("-");
    auto it = attributes.find(attribute);
    if(it != attributes.end()){
        value = QString::fromUtf8(it->second.c_str());
    }

    auto item = table->item(row, column);

    if(item == nullptr){
        item = new QTableWidgetItem(value);
        table->setItem(row, column, item);
        item->setTextAlignment(Qt::AlignCenter);
    }else{
        item->setText(value);
    }
}

QTreeWidgetItem * PatientCardWidget::createBranch(const std::string & name, const std::map<int, const communication::MotionShallowCopy::Session *> & sessions)
{
    QTreeWidgetItem * ret = new QTreeWidgetItem();

    auto date = sessions.begin()->second->sessionDate;

    ret->setText(0, QString::fromUtf8(name.c_str()));
    ret->setText(1, date.c_str());

    //gorna partia
    if(sessions.begin()->first % 2 == 0){
        QTreeWidgetItem * item = new CardSessionItem(sessions.begin()->second->sessionID);
        item->setText(0, (std::string("Upper body - ") + sessions.begin()->second->sessionName).c_str());
        item->setText(1, QString::number(sessions.begin()->second->labID));
        item->setText(2, QString::number(sessions.begin()->second->userID));
        item->setText(3, QString::fromUtf8(sessions.begin()->second->motionKind.c_str()));
        item->setText(4, QString::fromUtf8(sessions.begin()->second->tags.c_str()));

        std::string attr;

        for(auto it = sessions.begin()->second->attrs.begin(); it != sessions.begin()->second->attrs.end(); it++){
            attr += it->first + "=" + it->second + ";";
        }

        item->setText(5, QString::fromUtf8(attr.c_str()));

        ret->addChild(item);
    }

    std::string sname("Lower body");

    //gorna partia
    if(sessions.begin()->first % 2 == 1){
        sname = "Upper body";
    }

    QTreeWidgetItem * item = new CardSessionItem(sessions.begin()->second->sessionID);
    item->setText(0, (sname + " - " + sessions.begin()->second->sessionName).c_str());
    item->setText(1, QString::number(sessions.begin()->second->labID));
    item->setText(2, QString::number(sessions.begin()->second->userID));
    item->setText(3, QString::fromUtf8(sessions.begin()->second->motionKind.c_str()));
    item->setText(4, QString::fromUtf8(sessions.begin()->second->tags.c_str()));

    std::string attr;

    for(auto it = sessions.begin()->second->attrs.begin(); it != sessions.begin()->second->attrs.end(); it++){
        attr += it->first + "=" + it->second + ";";
    }

    item->setText(5, QString::fromUtf8(attr.c_str()));

    ret->addChild(item);

    if(sessions.rbegin()->first % 2 == 1){
        return ret;
    }

    sname = "Lower body";

    item = new CardSessionItem(sessions.rbegin()->second->sessionID);
    item->setText(0, (sname + " - " + sessions.rbegin()->second->sessionName).c_str());
    item->setText(1, QString::number(sessions.rbegin()->second->labID));
    item->setText(2, QString::number(sessions.rbegin()->second->userID));
    item->setText(3, QString::fromUtf8(sessions.rbegin()->second->motionKind.c_str()));
    item->setText(4, QString::fromUtf8(sessions.rbegin()->second->tags.c_str()));

    attr.swap(std::string());

    for(auto it = sessions.rbegin()->second->attrs.begin(); it != sessions.rbegin()->second->attrs.end(); it++){
        attr += it->first + "=" + it->second + ";";
    }

    item->setText(5, QString::fromUtf8(attr.c_str()));

    ret->addChild(item);

    return ret;
}

void PatientCardWidget::setActiveSession(unsigned int id)
{

}

void PatientCardWidget::clearActiveSession()
{

}

void PatientCardWidget::setFilter(const IFilterConstPtr & filter)
{
    this->filter = filter;
}

void PatientCardWidget::resetFilter()
{
    this->filter.reset();
}