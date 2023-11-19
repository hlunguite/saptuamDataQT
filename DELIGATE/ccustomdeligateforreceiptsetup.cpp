#include "ccustomdeligateforreceiptsetup.h"
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include "caccountmap.h"
CcustomDeligateForReceiptSetup::CcustomDeligateForReceiptSetup(QObject * parent):
                                                               QItemDelegate(parent)
{


}

CcustomDeligateForReceiptSetup::~CcustomDeligateForReceiptSetup()
{

}

void CcustomDeligateForReceiptSetup::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}


QWidget *CcustomDeligateForReceiptSetup::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
   // int col = index.column();

    /*if (col == RECEIPT_SETUP_ACCOUNT) {
        const QStringList& accountList = CaccountMap::Object()->getAccountListWithHqReceipt();
        QComboBox * cmb = new QComboBox(parent);
        cmb->addItems(accountList);
        return cmb;
    }*/
    QCheckBox *editor = new QCheckBox(parent);

    return editor;
}

void CcustomDeligateForReceiptSetup::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int value = index.model()->data(index, Qt::DisplayRole).toInt();
    QCheckBox *edit = qobject_cast<QCheckBox*>(editor);
    if (value) {
        edit->setChecked(true);
    } else {
        edit->setChecked(false);
    }
    /*int col = index.column();
    if (col == RECEIPT_SETUP_ACCOUNT) {
        QComboBox *edit = qobject_cast<QComboBox*>(editor);
        if(edit){
            edit->setCurrentText(index.model()->data(index,Qt::EditRole).toString());
        }
        return;
    }
    QLineEdit *edit = qobject_cast<QLineEdit*>(editor);
    if (edit) {
        QString str = index.model()->data(index, Qt::EditRole).toString();
        edit->setText(str);
        return;
    }*/
}

void CcustomDeligateForReceiptSetup::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QCheckBox *checkBox = qobject_cast<QCheckBox*>(editor);
    Qt::CheckState value;

    if(checkBox->isChecked()) {
        value = Qt::Checked;
    } else {
        value = Qt::Unchecked;
    }

    model->setData( index, value, Qt::DisplayRole);
/*
    int col = index.column();
    if (col == RECEIPT_SETUP_ACCOUNT) {
        QComboBox *edit = qobject_cast<QComboBox*>(editor);
        if(edit){
            model->setData(index,edit->currentText());
        }
        return;
    }
    QLineEdit *edit = qobject_cast<QLineEdit *>(editor);
    if (edit) {
        QString str = edit->text();
        model->setData(index, str);
        return;
    }*/
}
