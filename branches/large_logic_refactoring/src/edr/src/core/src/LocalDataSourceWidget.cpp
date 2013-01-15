#include "CorePCH.h"
#include "LocalDataSourceWidget.h"
#include "LocalDataSource.h"
#include "ParserManager.h"
#include "FileDataManager.h"

#include <QtGui/QFileDialog>
#include <QtCore/QString>
#include <QtGui/QMessageBox>
#include <core/PluginCommon.h>
#include <map>

using namespace core;

LocalDataSourceWidget::LocalDataSourceWidget(LocalDataSource * dataSource, QWidget * parent)
    : QWidget(parent), localDataSource(dataSource)
{
    UTILS_ASSERT((localDataSource != nullptr), "Błędny wskaźnik do źródła danych");
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
        //resetuj poprzednie ustawienia widgweta i źródła
        trialNameLabel->clear();
        std::string().swap(localDataSource->trialPath);
        LocalDataSource::OutputDescription().swap(localDataSource->outputDescription);
        LocalDataSource::Data().swap(localDataSource->data);
        LocalDataSource::Data().swap(data);
        std::map<QCheckBox*, core::TypeInfo>().swap(checkBoxToTypeMapping);

        core::Filesystem::Path p(text.toStdString());
        if(core::Filesystem::pathExists(p) == true){
            if(core::Filesystem::isDirectory(p) == true){
                std::vector<core::Filesystem::Path> files;
                core::Filesystem::Iterator it(p);
                core::Filesystem::Iterator endIT;

                auto parserManager = getParserManager();

                for( ; it != endIT; ++it){
					if(core::Filesystem::isRegularFile((*it).path()) == true && parserManager->sourceIsAccepted((*it).path().string()) == true){
                        files.push_back((*it).path());
                    }
                }

                if(files.empty() == false){
                    //ladujemy pliki!!
                    std::set<core::ObjectWrapperPtr> initialisedData;
                    std::set<core::TypeInfo> dataTypes;
                    std::vector<core::ObjectWrapperPtr> objects;

					auto fdm = getFileDataManager();
					auto fdmTransaction = fdm->transaction();

                    for(auto it = files.begin(); it != files.end(); ++it){
						if(fdmTransaction->tryAddFile(*it) == true){
							fdm->getObjects(*it, objects);
						}
                    }

                    for(auto it = objects.begin(); it != objects.end(); ++it){
                        core::TypeInfo typeInfo = (*it)->getTypeInfo();
                        auto dataIT = data.find(typeInfo);
                        if(dataIT == data.end()){
                            dataIT = data.insert(std::make_pair(typeInfo, core::ObjectWrapperCollectionPtr(new core::ObjectWrapperCollection(typeInfo)))).first;
                        }

                        dataIT->second->push_back(*it);
                    }

                    if(data.empty() == false){
                        //teraz budujemy opis wyjść
                        tableWidget->setRowCount(data.size());
                        bool checked = true;
                        int i = 0;
                        for(auto it = data.begin(); it != data.end(); ++it){
                            QCheckBox* chkBox = new QCheckBox();

                            //połączenie pomiędzy checkbox a typem danych pogrupowanych
                            checkBoxToTypeMapping.insert(std::make_pair(chkBox, it->first));

                            if(checked == true){
                                chkBox->setChecked(true);
                                chkBox->setEnabled(false);

                                //inicjalizuje pierwsze dane
                                localDataSource->data.insert(std::make_pair(it->first, it->second));
                                localDataSource->outputDescription.push_back(core::IOutputDescription::OutputInfo(it->second->front()->getClassName(), it->first));
                            }else{
                                chkBox->setChecked(false);
                            }

                            //połączenie sygnału zmiany zaznaczenia aby poprawnie budować output info
                            connect(chkBox, SIGNAL(stateChanged(int)), this, SLOT(onCheckChange(int)));

                            tableWidget->setCellWidget(i, 0, chkBox);
                            tableWidget->setItem(i, 1, new QTableWidgetItem(it->second->front()->getClassName().c_str()));
                            checked = false;
                            ++i;
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
        for(auto it = checkBoxToTypeMapping.begin(); it != checkBoxToTypeMapping.end(); ++it){
            it->first->setEnabled(true);
        }

        //dodaj dane do źródła!!
        localDataSource->data[typeInfo] = data[typeInfo];
    }else{
        //usun typ źrodla z opisu wyjść
        std::set<QCheckBox*> checked;
        for(auto it = checkBoxToTypeMapping.begin(); it != checkBoxToTypeMapping.end(); ++it){
            if(it->first->isChecked() == true){
                checked.insert(it->first);
            }
        }

        if(checked.size() == 1){
            auto it = checked.begin();
            (*it)->setEnabled(false);
        }

        //usuń dane do źródła!!
        localDataSource->data.erase(typeInfo);
    }

    LocalDataSource::OutputDescription().swap(localDataSource->outputDescription);
    for(auto it = checkBoxToTypeMapping.begin(); it != checkBoxToTypeMapping.end(); ++it){
        if(it->first->isChecked() == true){
            localDataSource->outputDescription.push_back(core::IOutputDescription::OutputInfo(data.find(it->second)->second->front()->getClassName(), it->second));
        }
    }
}
