#include "DicomPCH.h"
#include "AdnotationsDelegate.h"
#include <QtGui/QComboBox>
#include "Adnotations.h"


dicom::AdnotationsDelegate::AdnotationsDelegate( QObject *parent /*= 0*/ ) : 
    QItemDelegate(parent)
{

}

QWidget *dicom::AdnotationsDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &/* option */,
    const QModelIndex &/* index */) const
{
    QComboBox *editor = new QComboBox(parent);
    adnotations::AdnotationsTypePtr adn = adnotations::instance();
    for (auto it = adn->left.begin(); it != adn->left.end(); ++it) {
        editor->addItem(it->second);
    }

    return editor;
}

void dicom::AdnotationsDelegate::setEditorData(QWidget *editor,
    const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::DisplayRole).toString();

    QComboBox *box = static_cast<QComboBox*>(editor);
    int idx = box->findText(value);
    box->setCurrentIndex(idx);
}

void dicom::AdnotationsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
    const QModelIndex &index) const
{
    QComboBox *box = static_cast<QComboBox*>(editor);
    //box->interpretText();
    QString value = box->currentText();

    model->setData(index, value, Qt::EditRole);
}

void dicom::AdnotationsDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}



