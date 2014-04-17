#include "DicomPCH.h"
#include "AdnotationsDelegate.h"
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit.h>
#include "Adnotations.h"
#include "LayeredModelView.h"


dicom::AdnotationsDelegate::AdnotationsDelegate(const int column, QObject *parent /*= 0*/ ) : 
    QItemDelegate(parent), column(column), editionActive(false)
{

}

void dicom::AdnotationsDelegate::setEditionActive(const bool active)
{
	editionActive = active;
}

void dicom::AdnotationsDelegate::setImage(ILayeredImageConstPtr val)
{
	this->val = val;
}

QWidget *dicom::AdnotationsDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex & index ) const
{
	QWidget * ret = nullptr;
	if(val != nullptr && editionActive == true){

		LayeredModelView::treedata* td = static_cast<LayeredModelView::treedata*>(index.internalPointer());
		int idx = td->idx;
		if (idx != -1) {
			std::string tag = val->getTag(td->tag);
			if (val->getNumLayerItems(tag) > idx ) {// && idx >= 0) { //! TODO: wczeœniej sprawdzxamy czy rozne od -1 wiec chyba mniej juz nigdy nie ma
				auto il = val->getLayerItem(tag, idx);
				auto aidx = il->getAdnotationIdx();

				if(aidx > 7){

					if(column == 1){

						QComboBox *editor = new QComboBox(parent);

						switch(aidx) {

						case adnotations::bloodLevel:
							{
								auto adn = dicom::adnotations::instanceBloodLevels();
								for(auto it = adn->left.begin(); it != adn->left.end(); ++it){
									editor->addItem(it->second, it->first);
								}
							}
							break;

						case adnotations::inflammatoryLevel:
							{
								auto adn = dicom::adnotations::instanceInflammatoryLevels();
								for(auto it = adn->left.begin(); it != adn->left.end(); ++it){
									editor->addItem(it->second, it->first);
								}
							}
							break;

						case adnotations::fingerType:
							{
								auto adn = dicom::adnotations::instanceFingerTypes();
								for(auto it = adn->left.begin(); it != adn->left.end(); ++it){
									editor->addItem(it->second, it->first);
								}
							}
							break;

						case adnotations::jointType:
							{
								auto adn = dicom::adnotations::instanceJointTypes();
								for(auto it = adn->left.begin(); it != adn->left.end(); ++it){
									editor->addItem(it->second, it->first);
								}
							}
							break;
						}

						ret = editor;
					}
				}else if(column == 0){
					ret = new QLineEdit(parent);
				}
			}
		}
	}

    return ret;
}

void dicom::AdnotationsDelegate::setEditorData(QWidget *editor,
    const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::DisplayRole).toString();

	bool set = false;

	if(val != nullptr){

		LayeredModelView::treedata* td = static_cast<LayeredModelView::treedata*>(index.internalPointer());
		int idx = td->idx;
		if (idx != -1) {
			std::string tag = val->getTag(td->tag);
			if (val->getNumLayerItems(tag) > idx ) {// && idx >= 0) { //! TODO: wczeœniej sprawdzxamy czy rozne od -1 wiec chyba mniej juz nigdy nie ma
				auto il = val->getLayerItem(tag, idx);
				auto aidx = il->getAdnotationIdx();

				if(aidx > 7){

					QComboBox *box = static_cast<QComboBox*>(editor);
					int idx = box->findText(value);
					box->setCurrentIndex(idx);
					set = true;
				}
			}
		}	
	}
	
	if(set == false) {
		QLineEdit * edit = static_cast<QLineEdit*>(editor);
		edit->setText(value);
	}
}

void dicom::AdnotationsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
    const QModelIndex &index) const
{
	bool set = false;

	if(val != nullptr){
		LayeredModelView::treedata* td = static_cast<LayeredModelView::treedata*>(index.internalPointer());
		int idx = td->idx;
		if (idx != -1) {
			std::string tag = val->getTag(td->tag);
			if (val->getNumLayerItems(tag) > idx ) {// && idx >= 0) { //! TODO: wczeœniej sprawdzxamy czy rozne od -1 wiec chyba mniej juz nigdy nie ma
				auto il = val->getLayerItem(tag, idx);
				auto aidx = il->getAdnotationIdx();

				if(aidx > 7){
					QComboBox *box = static_cast<QComboBox*>(editor);		

					model->setData(index, box->itemData(box->currentIndex()), Qt::EditRole);

					set = true;
				}
			}
		}
	}
				
	if(set == false){
		QLineEdit * edit = static_cast<QLineEdit*>(editor);
		QString value = edit->text();
		model->setData(index, value, Qt::EditRole);
	}
}

void dicom::AdnotationsDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}



