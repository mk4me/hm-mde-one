#include "CommunicationPCH.h"

#include "PatientCardWidget.h"

PatientCardWidget::PatientCardWidget() : patient(nullptr), performer(nullptr)
{
    setupUi(this);
    refreshCard();

    generalTable->setItem(0,0, new QTableWidgetItem("General"));

    

}

PatientCardWidget::PatientCardWidget(const communication::MedicalShallowCopy::Patient * patient, const communication::MotionShallowCopy::Performer * performer,
    const PhotoConstPtr & photo, const IFilterConstPtr & filter)
    : patient(patient), performer(performer), filter(filter), photo(photo)
{
    setupUi(this);
    refreshCard();
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

    generalTable->columnSpan(0,3);
    auto maxSize = generalTable->maximumSize();
    generalTable->setMaximumSize(0,0);
    generalTable->setMaximumSize(maxSize.width(), maxSize.height());
    generalTable->resizeRowsToContents();

    leftRightTable->columnSpan(0,4);
    leftRightTable->resizeRowsToContents();

    if(patient == nullptr || performer == nullptr){
        //proste inicjowanie pol karty wartosciami domyslnymi
        
        patientValue->clear();
        birthdateValue->clear();
        birthdateValue->clear();
        genderValue->clear();
        photoContainer->setPixmap(QPixmap());

        tableWidget->clearContents();
        treeWidget->clear();
        
        detailsFrame->hide();        
    }else{
        //wlasciwe inicjowanie karty dostarczonymi wartosciami
        patientValue->setText(QString::fromUtf8((patient->surname + ", " + patient->name).c_str()));
        birthdateValue->setText(QString(patient->birthDate.c_str()));
        genderValue->setText(QChar(patient->gender));
        
        //ustawiamy fotke
        if(photo != nullptr){
            photoContainer->clear();
            photoContainer->setPixmap(*photo);
        }else{
            photoContainer->setText("No photo avaiable");
        }
        
        refreshDisorders();
        
        refreshSessions();
        detailsFrame->show();
    }
}

void PatientCardWidget::refreshDisorders()
{
    int sortColumns = tableWidget->horizontalHeader()->sortIndicatorSection();
    Qt::SortOrder order = tableWidget->horizontalHeader()->sortIndicatorOrder();
    //wylanczam tymczasowo sortowanie
    tableWidget->setSortingEnabled(false);

    //czyscimy
    tableWidget->clearContents();

    //ustalam ilosc wpisow
    tableWidget->setRowCount(patient->disorders.size());

    //wypelniamy chorobami
    int i = 0;
    for(auto it = patient->disorders.begin(); it != patient->disorders.end(); it++){
        //nazwa
        QTableWidgetItem *item = new QTableWidgetItem(QString::fromUtf8(it->second.diosorder->name.c_str()));
        tableWidget->setItem(i,0, item);
        
        //data diagnozy
        item = new QTableWidgetItem(QString::fromUtf8(it->second.diagnosisDate.c_str()));
        tableWidget->setItem(i,1, item);
        
        //typ diagnozy
        item = new QTableWidgetItem(QString::fromUtf8(it->second.focus.c_str()));
        tableWidget->setItem(i,2, item);

        //komentarz
        item = new QTableWidgetItem(QString::fromUtf8(it->second.comments.c_str()));
        tableWidget->setItem(i,3, item);
        i++;
    }

    tableWidget->horizontalHeader()->setSortIndicator(sortColumns, order);

    //wlanczam spowrotem sortowanie
    tableWidget->setSortingEnabled(true);
    tableWidget->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
}

void PatientCardWidget::refreshSessions()
{
    //czyscimy
    treeWidget->clear();

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
        treeWidget->addTopLevelItem(item);
    }

    if(groupedData.rbegin()->second.empty() == false){
        QTreeWidgetItem * item = createBranch("Po", groupedData.rbegin()->second);
        treeWidget->addTopLevelItem(item);
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
        QTreeWidgetItem * item = new QTreeWidgetItem();
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

    QTreeWidgetItem * item = new QTreeWidgetItem();
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

    item = new QTreeWidgetItem();
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