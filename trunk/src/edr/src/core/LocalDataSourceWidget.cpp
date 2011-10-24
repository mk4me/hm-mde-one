#include "CorePCH.h"
#include "LocalDataSourceWidget.h"
#include "LocalDataSource.h"
#include "DataManager.h"

#include <QtGui/QFileDialog>
#include <QtCore/QString>
#include <map>

LocalDataSourceWidget::LocalDataSourceWidget(LocalDataSource * dataSource, QWidget * parent)
    : QWidget(parent), localDataSource(dataSource)
{
    UTILS_ASSERT((localDataSource != nullptr), "B³êdny wskaŸnik do Ÿród³a danych");
    setupUi(this);
}

LocalDataSourceWidget::~LocalDataSourceWidget()
{

}

void LocalDataSourceWidget::onOpen()
{
    core::Filesystem::Path initPath = core::getUserDataPath() / "trial";
    QString path = QFileDialog::getExistingDirectory(this, 0, initPath.string().c_str());
    if (!path.isEmpty()) 
    {
        lineEdit->setText(path);
    }
}

void LocalDataSourceWidget::onEdit(const QString & text)
{
    std::string errorDesc;

    if(text.isEmpty() == false){
        //resetuj poprzednie ustawienia widgweta i Ÿród³a
        trialNameLabel->clear();
        localDataSource->trialPath.swap(std::string());
        localDataSource->outputDescription.swap(LocalDataSource::OutputDescription());
        localDataSource->data.swap(LocalDataSource::Data());
        data.swap(LocalDataSource::Data());
        checkBoxToTypeMapping.swap(std::map<QCheckBox*, core::TypeInfo>());

        core::Filesystem::Path p(text.toStdString());
        if(core::Filesystem::pathExists(p) == true){
            if(core::Filesystem::isDirectory(p) == true){
                std::vector<core::Filesystem::Path> files;
                core::Filesystem::Iterator it(p);
                core::Filesystem::Iterator endIT;

                //DataManager* dataManager = DataManager::getInstance();

                for( ; it != endIT; it++){
                    //if(core::Filesystem::isRegularFile((*it).path()) == true && dataManager->isExtensionSupported((*it).path().extension().string()) == true){
                        files.push_back((*it).path());
                    //}
                }

                if(files.empty() == false){
                    //ladujemy pliki!!
                    std::vector<core::ObjectWrapperPtr> possibleData;
                    std::set<core::ObjectWrapperPtr> initialisedData;
                    //dataManager->loadFiles(files, possibleData);

                    std::set<core::TypeInfo> dataTypes;

                    for(auto it = possibleData.begin(); it != possibleData.end(); it++){
                        dataTypes.insert((*it)->getTypeInfo());
                    }

                    for(auto it = dataTypes.begin(); it != dataTypes.end(); it++){
                        std::vector<core::ObjectWrapperPtr> objects;
                        //dataManager->getObjects(objects, *it, true);
                        for(auto iT = objects.begin(); iT != objects.end(); iT++){
                            if((*iT)->isNull() == false && std::find(possibleData.begin(), possibleData.end(), *iT) != possibleData.end()){
                                initialisedData.insert(*iT);
                            }
                        }
                    }

                    if(initialisedData.empty() == false){

                        //grupuj dane po typach - tym moge zasilic juz agregaty!!

                        for(auto it = initialisedData.begin(); it != initialisedData.end(); it++){
                            core::TypeInfo typeInfo((*it)->getTypeInfo());
                            if(data.find(typeInfo) == data.end()){
                                data.insert(std::make_pair(typeInfo, core::ObjectWrapperCollectionPtr(new core::ObjectWrapperCollection(typeInfo))));
                            }

                            data[typeInfo]->addObject(*it);
                        }

                        //teraz budujemy opis wyjœæ
                        tableWidget->setRowCount(data.size());
                        bool checked = true;
                        int i = 0;
                        for(auto it = data.begin(); it != data.end(); it++){
                            QCheckBox* chkBox = new QCheckBox();

                            //po³¹czenie pomiêdzy checkbox a typem danych pogrupowanych
                            checkBoxToTypeMapping.insert(std::make_pair(chkBox, it->first));

                            if(checked == true){
                                chkBox->setChecked(true);
                                chkBox->setEnabled(false);

                                //inicjalizuje pierwsze dane
                                localDataSource->data.insert(std::make_pair(it->first, it->second));
                                localDataSource->outputDescription.push_back(core::IOutputDescription::OutputInfo(it->second->getObject(0)->getClassName(), it->first));
                            }else{
                                chkBox->setChecked(false);
                            }

                            //po³¹czenie sygna³u zmiany zaznaczenia aby poprawnie budowaæ output info
                            connect(chkBox, SIGNAL(stateChanged(int)), this, SLOT(onCheckChange(int)));

                            tableWidget->setCellWidget(i, 0, chkBox);
                            tableWidget->setItem(i, 1, new QTableWidgetItem(it->second->getObject(0)->getClassName().c_str()));
                            checked = false;
                            i++;
                        }
                    }else{
                        errorDesc = "No data loaded from the given directory";
                    }
                }else{
                    errorDesc = "The given path does not contain any supported format";
                }
            }else{
                errorDesc = "The given path is not a directory";
            }
        }else{
            errorDesc = "The given path does not exist";
        }
    }else{
        errorDesc = "The given path is empty";
    }

    if(errorDesc.empty() == false){
        QMessageBox msgBox;
        msgBox.setText(errorDesc.c_str());
        msgBox.exec();
    }
}

void LocalDataSourceWidget::onCheckChange(int state)
{
    QCheckBox* chkBox = qobject_cast<QCheckBox*>(sender());
    if(chkBox == nullptr){
        return;
    }

    core::TypeInfo typeInfo(checkBoxToTypeMapping.find(chkBox)->second);
    if(chkBox->isChecked() == true){
        for(auto it = checkBoxToTypeMapping.begin(); it != checkBoxToTypeMapping.end(); it++){
            it->first->setEnabled(true);
        }

        //dodaj dane do Ÿród³a!!
        localDataSource->data[typeInfo] = data[typeInfo];
    }else{
        //usun typ Ÿrodla z opisu wyjœæ
        std::set<QCheckBox*> checked;
        for(auto it = checkBoxToTypeMapping.begin(); it != checkBoxToTypeMapping.end(); it++){
            if(it->first->isChecked() == true){
                checked.insert(it->first);
            }
        }

        if(checked.size() == 1){
            auto it = checked.begin();
            (*it)->setEnabled(false);
        }

        //usuñ dane do Ÿród³a!!
        localDataSource->data.erase(typeInfo);
    }

    localDataSource->outputDescription.swap(LocalDataSource::OutputDescription());
    for(auto it = checkBoxToTypeMapping.begin(); it != checkBoxToTypeMapping.end(); it++){
        if(it->first->isChecked() == true){
            localDataSource->outputDescription.push_back(core::IOutputDescription::OutputInfo(data.find(it->second)->second->getObject(0)->getClassName(), it->second));
        }
    }
}