#include "ccustomdeligateforremittance.h"
#include <QLineEdit>
#include <QComboBox>

CcustomDeligateForRemittance::CcustomDeligateForRemittance(QObject *parent)
    : QItemDelegate{parent}
{

}

QWidget *CcustomDeligateForRemittance::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    int col = index.column();
    int row = index.row();

    if (REMIT_DLG_TABLE_PIAKSAN == col) {
        QComboBox * cmb = new QComboBox(parent);
        cmb->addItems(m_accountList);


        return cmb;
    }
    QLineEdit *editor = new QLineEdit(parent);
    return editor;
}

void CcustomDeligateForRemittance::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int col = index.column();
    if (col == REMIT_DLG_TABLE_PIAKSAN) {
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
    }
}

void CcustomDeligateForRemittance::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    int col = index.column();
    if (col == REMIT_DLG_TABLE_PIAKSAN) {
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
    }
}

void CcustomDeligateForRemittance::setAccountList(const QStringList &newAccountList)
{
    m_accountList = newAccountList;
}
