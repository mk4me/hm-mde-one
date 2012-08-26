#include "CommunicationPCH.h"

#include "PatientCardWidget.h"
#include <QtGui/QVBoxLayout>
#include <QtGui/QFormLayout>
#include <utils/Debug.h>
#include <boost/any.hpp>
#include <qthtmlgenlib/DocumentGenerator.h>

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
    multiToolBoxWidget->setObjectName(QString::fromUtf8("closableDataGroups"));
    multiToolBoxWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    this->frame->layout()->addWidget(multiToolBoxWidget);
    this->frame->layout()->addItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding));

    multiToolBoxWidget->setContentsMargins(0,0,0,0);
    multiToolBoxWidget->layout()->setContentsMargins(0,0,0,0);
    //this->frame->layout()->addItem(new QSpacerItem(0,0,QSizePolicy::Preferred, QSizePolicy::Expanding));

    //dodajemy poszczegolne elementy:

    //dane o chorobach
    disordersTable = new QTableWidget();
    disordersTable->setMaximumHeight(300);
    
    //sesje
    sessionsWidget = new QTreeWidget();
    sessionsWidget->setMaximumHeight(300);
    sessionsWidget->header()->setResizeMode(QHeaderView::Fixed);
    sessionsWidget->header()->setVisible(false);

    connect(sessionsWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(currentSessionChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
    //adnotacje
    adnotationsWidget = new QTextEdit();

    //dodajemy wszystko do multitoolboxa
    multiToolBoxWidget->addItem(disordersTable, tr("Disorders"));
    multiToolBoxWidget->addItem(sessionsWidget, tr("Sessions"));
    multiToolBoxWidget->addItem(adnotationsWidget, tr("Notes"));
    multiToolBoxWidget->expandAllItems();

    QStringList headers;

    headers << tr("Name");
    headers << tr("Diagnosis Date");
    headers << tr("Focus");
    disordersTable->setColumnCount(3);
    disordersTable->setHorizontalHeaderLabels(headers);
    
    generalDataTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    leftRightDataTable->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

    generalDataTable->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    leftRightDataTable->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);

    generalDataTable->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
    leftRightDataTable->horizontalHeader()->setResizeMode(QHeaderView::Fixed);

    generalDataTable->verticalHeader()->setResizeMode(QHeaderView::Fixed);
    leftRightDataTable->verticalHeader()->setResizeMode(QHeaderView::Fixed);

    generalDataTable->hide();
    leftRightDataTable->hide();

    /*textEdit->setHtml(QString::fromUtf8("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">" \
        "<html><head><meta name=\"qrichtext\" content_=\"1\" /><style type=\"text/css\">" \
"#tableX {" \
"border: 1px solid #e3e3e3; " \
    "background-color: #f2f2f2;" \
"width: 100%;" \
    "border-radius: 6px;" \
    "-webkit-border-radius: 6px;" \
    "-moz-border-radius: 6px;}" \
"#tableX td, #table th {" \
"padding: 5px;" \
"color: #333;}" \
"#tableX thead {" \
    "font-family: \"Lucida Sans Unicode\", \"Lucida Grande\", sans-serif;" \
"padding: .2em 0 .2em .5em;" \
    "text-align: left;" \
"color: #4B4B4B;" \
    "background-color: #C8C8C8;" \
    "background-image: -webkit-gradient(linear, left top, left bottom, from(#f2f2f2), to(#e3e3e3), color-stop(.6,#B3B3B3));" \
    "background-image: -moz-linear-gradient(top, #D6D6D6, #B0B0B0, #B3B3B3 90%);" \
    "border-bottom: solid 1px #999;}"\
"#tableX th {" \
    "font-family: 'Helvetica Neue', Helvetica, Arial, sans-serif;" \
    "font-size: 17px;" \
    "line-height: 20px;" \
    "font-style: normal;" \
    "font-weight: normal;" \
    "text-align: left;" \
    "text-shadow: white 1px 1px 1px;}") + QString::fromUtf8("#table td {" \
    "line-height: 20px;" \
    "font-family: 'Helvetica Neue', Helvetica, Arial, sans-serif;" \
    "font-size: 14px;" \
    "border-bottom: 1px solid #fff;" \
    "border-top: 1px solid #fff;}" \
"#tableX td:hover {" \
"background-color: #fff;}" \
    "</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\"><table id=\"tableX\">" \
    "<tbody><tr><td style=\"font-family: 'Lucida Sans Unicode', 'Lucida Grande', sans-serif; padding: .2em 0 .2em .5em; text-align: left; color: #4B4B4B; background-color: #C8C8C8; background-image: -webkit-gradient(linear, left top, left bottom, from(#f2f2f2), to(#e3e3e3), color-stop(.6,#B3B3B3)); background-image: -moz-linear-gradient(top, #D6D6D6, #B0B0B0, #B3B3B3 90%); border-bottom: solid 1px #999;\">First Name</td><td>Last Name</td><td>Email Address</td><td>Website</td></tr>" \
        "<tr><td>John</td><td>Smith</td><td>johnsmith@example.com</td><td>http://www.example.com</td></tr>" \
        "<tr><td>Peter</td><td>James</td><td>peterjames@example.com</td><td>http://www.example.com</td></tr>" \
        "<tr><td>Ronald</td><td>Weeley</td><td>ronweeley@example.com</td><td>http://www.example.com</td></tr>" \
        "</tbody></table></body></html>"));*/

    textEdit->setReadOnly(true);
    textEdit->setMinimumWidth(400);

    initAntropometric();

    /*int vHeaderColumnMaxWidth = 0;
    for(int i = 0; i < leftRightDataTable->rowCount(); i++){
        vHeaderColumnMaxWidth = max(vHeaderColumnMaxWidth, leftRightDataTable->verticalHeaderItem(i)->sizeHint().width());
    }

    for(int i = 0; i < generalDataTable->rowCount(); i++){
        vHeaderColumnMaxWidth = max(vHeaderColumnMaxWidth, generalDataTable->verticalHeaderItem(i)->sizeHint().width());
    }

    for(int i = 0; i < leftRightDataTable->rowCount(); i++){
        leftRightDataTable->verticalHeaderItem(i)->setSizeHint(QSize(vHeaderColumnMaxWidth, leftRightDataTable->verticalHeaderItem(i)->sizeHint().height()));
    }

    for(int i = 0; i < generalDataTable->rowCount(); i++){
        generalDataTable->verticalHeaderItem(i)->setSizeHint(QSize(vHeaderColumnMaxWidth, generalDataTable->verticalHeaderItem(i)->sizeHint().height()));
    }*/

    resetAntropometricData();
    centerTableCellsText(generalDataTable);
    centerTableCellsText(leftRightDataTable);
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
    if(patient == nullptr || performer == nullptr){
        //proste inicjowanie pol karty wartosciami domyslnymi
        
        patientEdit->clear();

        birthdateEdit->clear();

        genderEdit->clear();
        
        photoContainer->setPixmap(QPixmap());

        disordersTable->clearContents();
        sessionsWidget->clear();
    }else{
        //wlasciwe inicjowanie karty dostarczonymi wartosciami
        patientEdit->setText(QString::fromUtf8((patient->surname + ", " + patient->name).c_str()));
        birthdateEdit->setText(QString(patient->birthDate.c_str()));
        
        genderEdit->setText(patient->gender == 'M' ? tr("Men") : tr("Women"));
        
        //ustawiamy fotke
        if(photo != nullptr){
            photoContainer->clear();
            photoContainer->setPixmap(photo->scaled(photoContainer->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }else{
            photoContainer->setText("No photo avaiable");
        }
        
        refreshDisorders();
        refreshSessions();
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

        QTreeWidgetItem * item = createBranch(tr("Before").toStdString(), groupedData.begin()->second);
        sessionsWidget->addTopLevelItem(item);
    }

    if(groupedData.begin()->first != groupedData.rbegin()->first && groupedData.rbegin()->second.empty() == false){
        QTreeWidgetItem * item = createBranch(tr("After").toStdString(), groupedData.rbegin()->second);
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
    setAttribute(generalDataTable, antropometricGeneralTable.antropometricContent, 0, 1, "BodyMass", attributes);
    setAttribute(generalDataTable, antropometricGeneralTable.antropometricContent, 1, 1, "Height", attributes);
    setAttribute(generalDataTable, antropometricGeneralTable.antropometricContent, 2, 1, "InterAsisDistance", attributes);

    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 0, 1, "LeftLegLength", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 0, 2, "RightLegLenght", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 1, 1, "LeftKneeWidth", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 1, 2, "RightKneeWidth", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 2, 1, "LeftAnkleWidth", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 2, 2, "RightAnkleWidth", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 3, 1, "LeftCircuitThigh", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 3, 2, "RightCircuitThight", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 4, 1, "LeftCircuitShank", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 4, 2, "RightCircuitShank", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 5, 1, "LeftShoulderOffset", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 5, 2, "RightShoulderOffset", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 6, 1, "LeftElbowWidth", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 6, 2, "RightElbowWidth", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 7, 1, "LeftWristWidth", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 7, 2, "RightWristWidth", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 8, 1, "LeftWristThickness", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 8, 2, "RightWristThickness", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 9, 1, "LeftHandWidth", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 9, 2, "RightHandWidth", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 10, 1, "LeftHandThickness", attributes);
    setAttribute(leftRightDataTable, antropometricLeftRightTable.antropometricContent, 10, 2, "RightHandThickness", attributes);

    leftRightDataTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
    generalDataTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

    generalDataTable->horizontalHeader()->resizeSection(0, leftRightDataTable->horizontalHeader()->sectionSize(0));
    generalDataTable->horizontalHeader()->resizeSection(1, leftRightDataTable->horizontalHeader()->sectionSize(1) + leftRightDataTable->horizontalHeader()->sectionSize(2));

    QString html;

    DocumentGenerator::openDocument(html);
    DocumentGenerator::openBody(html);

    QString genTable;

    HtmlDataTableGenerator::generateHtmlTable(genTable, antropometricGeneralTable.antropometricContent, antropometricGeneralTable.antropometricHeadersStructure,
        antropometricGeneralTable.antropometricCellsAttributes, antropometricGeneralTable.antropometricStyles);

    QString lrTable;

    HtmlDataTableGenerator::generateHtmlTable(lrTable, antropometricLeftRightTable.antropometricContent, antropometricLeftRightTable.antropometricHeadersStructure,
        antropometricLeftRightTable.antropometricCellsAttributes, antropometricLeftRightTable.antropometricStyles);

    html += genTable;

    html += "<p></p>";

    html += lrTable;

    DocumentGenerator::closeBody(html);
    DocumentGenerator::closeDocument(html);

    textEdit->setHtml(html);
}

void PatientCardWidget::setAttribute(QTableWidget * table, HtmlDataTableContent & htmlTable,int row, int column, const std::string & attribute, const communication::MotionShallowCopy::Attrs & attributes)
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

    htmlTable.setCell(row+1, column+1, value);
}

QTreeWidgetItem * PatientCardWidget::createBranch(const std::string & name, const std::map<int, const communication::MotionShallowCopy::Session *> & sessions)
{
    QTreeWidgetItem * ret = new QTreeWidgetItem();

    auto date = sessions.begin()->second->sessionDate;

    //ret->setText(0, QString::fromUtf8(name.c_str()));
    ret->setText(0, tr(name.c_str()));
    ret->setText(1, date.c_str());

    //gorna partia
    if(sessions.begin()->first % 2 == 0){
        QTreeWidgetItem * item = new CardSessionItem(sessions.begin()->second->sessionID);
        item->setText(0, tr("Upper body") + " - " + sessions.begin()->second->sessionName.c_str());
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

    QString sname(tr("Lower body"));

    //gorna partia
    if(sessions.begin()->first % 2 == 1){
        sname = tr("Upper body");
    }

    QTreeWidgetItem * item = new CardSessionItem(sessions.begin()->second->sessionID);
    item->setText(0, sname + " - " + QString(sessions.begin()->second->sessionName.c_str()));
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

    sname = tr("Lower body");

    item = new CardSessionItem(sessions.rbegin()->second->sessionID);
    item->setText(0, sname + " - " + QString(sessions.rbegin()->second->sessionName.c_str()));
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

void PatientCardWidget::setFilter(const IFilterConstPtr & filter)
{
    this->filter = filter;
}

void PatientCardWidget::resetFilter()
{
    this->filter.reset();
}

void PatientCardWidget::initAntropometric()
{
    initAntropometricContent();
    initAntropometricAtributes();
    initAntropometricHeadersStructure();
    initAntropometricStyles();
}

void PatientCardWidget::initAntropometricHeadersStructure()
{
    antropometricGeneralTable.antropometricHeadersStructure.headerRows = 1;
    antropometricGeneralTable.antropometricHeadersStructure.headerColumns = 1;

    antropometricLeftRightTable.antropometricHeadersStructure.headerRows = 1;
    antropometricLeftRightTable.antropometricHeadersStructure.headerColumns = 1;
}

void PatientCardWidget::initAntropometricContent()
{
    //wymiary
    antropometricGeneralTable.antropometricContent.setDimensions(4, 3);
    antropometricLeftRightTable.antropometricContent.setDimensions(12, 4);

    //zawartość tabeli z danymi ogólnymi
    //! wiersz nagłówkowy - pierwsza komórka pusta - łącznik z nagłówkiem pionowym
    antropometricGeneralTable.antropometricContent.setCell(0, 1, tr("Unit"));
    antropometricGeneralTable.antropometricContent.setCell(0, 2, tr("Value"));
    //! kolumna nagłówkowa - pierwsza komórka pusta - łącznik z nagłówkiem poziomym
    antropometricGeneralTable.antropometricContent.setCell(1, 0, tr("Body Mass"));
    antropometricGeneralTable.antropometricContent.setCell(2, 0, tr("Height"));
    antropometricGeneralTable.antropometricContent.setCell(3, 0, tr("ASIS-ASIS"));
    //! pierwsza kolumna z jednostkami
    antropometricGeneralTable.antropometricContent.setCell(1, 1, "[" + tr("kg") + "]");
    antropometricGeneralTable.antropometricContent.setCell(2, 1, "[" + tr("mm") + "]");
    antropometricGeneralTable.antropometricContent.setCell(3, 1, "[" + tr("mm") + "]");

    //zawartość tabeli z danymi szczegółowymi dla lewej i prawej storny
    //! wiersz nagłówkowy - pierwsza komórka pusta - łącznik z nagłówkiem pionowym
    antropometricLeftRightTable.antropometricContent.setCell(0, 1, tr("Unit"));
    antropometricLeftRightTable.antropometricContent.setCell(0, 2, tr("Left Value"));
    antropometricLeftRightTable.antropometricContent.setCell(0, 3, tr("Right Value"));
    //! kolumna nagłówkowa - pierwsza komórka pusta - łącznik z nagłówkiem poziomym
    antropometricLeftRightTable.antropometricContent.setCell(1, 0, tr("Leg Length"));
    antropometricLeftRightTable.antropometricContent.setCell(2, 0, tr("Knee Width"));
    antropometricLeftRightTable.antropometricContent.setCell(3, 0, tr("Anckle Width"));
    antropometricLeftRightTable.antropometricContent.setCell(4, 0, tr("Circuit Thight"));
    antropometricLeftRightTable.antropometricContent.setCell(5, 0, tr("Circuit Shank"));
    antropometricLeftRightTable.antropometricContent.setCell(6, 0, tr("Shoulder Offset"));
    antropometricLeftRightTable.antropometricContent.setCell(7, 0, tr("Elbow Width"));
    antropometricLeftRightTable.antropometricContent.setCell(8, 0, tr("Wrist Width"));
    antropometricLeftRightTable.antropometricContent.setCell(9, 0, tr("Wrist Thickness"));
    antropometricLeftRightTable.antropometricContent.setCell(10, 0, tr("Hand Width"));
    antropometricLeftRightTable.antropometricContent.setCell(11, 0, tr("Hand Thickness"));

    //! pierwsza kolumna z jednostkami
    antropometricLeftRightTable.antropometricContent.setCell(1, 1, "[" + tr("mm") + "]");
    antropometricLeftRightTable.antropometricContent.setCell(2, 1, "[" + tr("mm") + "]");
    antropometricLeftRightTable.antropometricContent.setCell(3, 1, "[" + tr("mm") + "]");
    antropometricLeftRightTable.antropometricContent.setCell(4, 1, "[" + tr("mm") + "]");
    antropometricLeftRightTable.antropometricContent.setCell(5, 1, "[" + tr("mm") + "]");
    antropometricLeftRightTable.antropometricContent.setCell(6, 1, "[" + tr("mm") + "]");
    antropometricLeftRightTable.antropometricContent.setCell(7, 1, "[" + tr("mm") + "]");
    antropometricLeftRightTable.antropometricContent.setCell(8, 1, "[" + tr("mm") + "]");
    antropometricLeftRightTable.antropometricContent.setCell(9, 1, "[" + tr("mm") + "]");
    antropometricLeftRightTable.antropometricContent.setCell(10, 1, "[" + tr("mm") + "]");
    antropometricLeftRightTable.antropometricContent.setCell(11, 1, "[" + tr("mm") + "]");
}

void PatientCardWidget::initAntropometricStyles()
{
    antropometricGeneralTable.antropometricStyles.tableStyle_ = "width: 300px; border-style: solid; border-color: rgb(0, 0, 0); border-radius: 4px;";
    antropometricGeneralTable.antropometricStyles.headerRowStyle_[StyleStatus::Single] = "color: white; background: rgb(41, 41, 41);";
    
    //wiersz nagłówkowy
    antropometricGeneralTable.antropometricStyles.headersStyles[StyleStatus::Single][StyleStatus::Middle] = "color: white; border-style: none; border-color: white;";
    // kolumna nagłówkowa - musze dodać kolor, nie da się kolumnom przypisywać właściwości
    antropometricGeneralTable.antropometricStyles.headersStyles[StyleStatus::Middle][StyleStatus::Single] = "color: white; border-style: none; border-color: white; background: rgb(41, 41, 41);";
    // ostatni element kolumny nagłówkowej - tylko kolor
    antropometricGeneralTable.antropometricStyles.headersStyles[StyleStatus::Last][StyleStatus::Single] = "color: white; background: rgb(41, 41, 41);";

    //style contentu - tutaj chcemy uzyskać efekt kratownicy bez ramki
    antropometricGeneralTable.antropometricStyles.contentStyles[StyleStatus::First][StyleStatus::First] = "border-style: none; border-color: rgb(41, 41, 41);";
    antropometricGeneralTable.antropometricStyles.contentStyles[StyleStatus::Last][StyleStatus::First] = "border-style: none; border-color: rgb(41, 41, 41);";
    antropometricGeneralTable.antropometricStyles.contentStyles[StyleStatus::Last][StyleStatus::Last] = "border-style: none; border-color: rgb(41, 41, 41);";

    // kopiujemy większość ustawień
    antropometricLeftRightTable.antropometricStyles = antropometricGeneralTable.antropometricStyles;
    //poprawiamy style contentu - więcej możliwości
    antropometricLeftRightTable.antropometricStyles.contentStyles[StyleStatus::First][StyleStatus::First] = "border-style: none; border-color: rgb(41, 41, 41);";
    antropometricLeftRightTable.antropometricStyles.contentStyles[StyleStatus::First][StyleStatus::Middle] = "border-style: none; border-color: rgb(41, 41, 41);";
    antropometricLeftRightTable.antropometricStyles.contentStyles[StyleStatus::First][StyleStatus::Last] = QString();

    antropometricLeftRightTable.antropometricStyles.contentStyles[StyleStatus::Middle][StyleStatus::First] = "border-style: none; border-color: rgb(41, 41, 41);";
    antropometricLeftRightTable.antropometricStyles.contentStyles[StyleStatus::Middle][StyleStatus::Middle] = "border-style: none; border-color: rgb(41, 41, 41);";
    antropometricLeftRightTable.antropometricStyles.contentStyles[StyleStatus::Middle][StyleStatus::Last] = "border-style: none; border-color: rgb(41, 41, 41);";

    antropometricLeftRightTable.antropometricStyles.contentStyles[StyleStatus::Last][StyleStatus::First] = "border-style: none; border-color: rgb(41, 41, 41);";
    antropometricLeftRightTable.antropometricStyles.contentStyles[StyleStatus::Last][StyleStatus::Middle] = "border-style: none; border-color: rgb(41, 41, 41);";
    antropometricLeftRightTable.antropometricStyles.contentStyles[StyleStatus::Last][StyleStatus::Last] = "border-style: none; border-color: rgb(41, 41, 41);";
}

void PatientCardWidget::initAntropometricAtributes()
{

}

void PatientCardWidget::resetContent()
{
    const static QString emptyCell("-");
    for(int i = antropometricLeftRightTable.antropometricHeadersStructure.headerRows; i < antropometricLeftRightTable.antropometricContent.rows(); ++i){
        for(int j = antropometricLeftRightTable.antropometricHeadersStructure.headerColumns; i < antropometricLeftRightTable.antropometricContent.columns(); ++j){
            antropometricLeftRightTable.antropometricContent.setCell(i,j, emptyCell);
        }
    }

    for(int i = antropometricGeneralTable.antropometricHeadersStructure.headerRows; i < antropometricGeneralTable.antropometricContent.rows(); ++i){
        for(int j = antropometricGeneralTable.antropometricHeadersStructure.headerColumns; i < antropometricGeneralTable.antropometricContent.columns(); ++j){
            antropometricGeneralTable.antropometricContent.setCell(i,j, emptyCell);
        }
    }
}
