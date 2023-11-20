#include "ccustomdeligatefortransaction.h"
#include <QComboBox>
#include <QVector>
#include <QLineEdit>
#include <QCompleter>
#include <QtAlgorithms>
#include <QStandardItemModel>
#include "ccontactmap.h"
#include "caccountmap.h"
#include <QDebug>
#include "ctransactionutils.h"

CcustomDeligateForTransaction::CcustomDeligateForTransaction(QObject * parent):
     QItemDelegate(parent)
{

}

CcustomDeligateForTransaction::~CcustomDeligateForTransaction()
{

}

void CcustomDeligateForTransaction::commitAndCloseEditor()
{
    QLineEdit *editor = qobject_cast<QLineEdit *>(sender());
    //qDebug()<<"Commit and close";
    emit commitData(editor);
    emit closeEditor(editor);
}

EtransactionType CcustomDeligateForTransaction::getTransTypeFromParticular(const QModelIndex &index) const
{
    //check for transaction to be added in bank account base on name
     QModelIndex idx = index.model()->index(index.row(), TRANS_FROM_TO_COL);
    if (idx.isValid()) {
         QString name = index.model()->data(idx  ).toString();

        //QString name = CcontactMap::Object()->getContanceName(id);
        if (name == gBankCashDepositName) {
            return BANK_CASH_DEPOSIT_TRANSACTION_TYPE;
        }
        if (name == gBankChargesName) {
            return BANK_CHARGES_TRANSACTION_TYPE;
        }
        if (name == gBankInterestName) {
            return BANK_INTEREST_TRANSACTION_TYPE;
        }
    }
    return INVALID_TRANSACTION_TYPE;
}



QWidget *CcustomDeligateForTransaction::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
    Q_UNUSED(option);
    int col = index.column();
    int row = index.row();
    if (col == TRANS_ACCOUNT_COL) {
        QComboBox * cmb = new QComboBox(parent);
        EtransactionType transType = getTransTypeFromParticular(index);
        if (transType != INVALID_TRANSACTION_TYPE) {
            QStringList accountList;
            accountList.push_back(gBankAccountName);
            cmb->addItems(accountList);
        } else {
            const QStringList& accountList = CaccountMap::Object()->getAccountList();
            //const QStringList& accountList = CaccountMap::Object()->getIncomeAccountList();
            //const QStringList& accountList = CaccountMap::Object()->getPaymentAccountList();


            cmb->addItems(accountList);
        }
        return cmb;
    }

    if (col == TRANS_MODE_COL) {
        QComboBox * cmb = new QComboBox(parent);
        QModelIndex idx = index.model()->index(row, TRANS_TYPE_COL);
        if (idx.isValid()) {

            QString transTypeStr = index.model()->data(idx  ).toString();
            //int type = CaccountMap::Object()->getAccountType(account);
            int transType = CtransactionUtils::Object()->getTransactionType(transTypeStr);
            QStringList list = CtransactionUtils::Object()->getTransectionModeForTransactionType((EtransactionType)transType);
            if (list.isEmpty() == false) {
                cmb->addItems(list);
                return cmb;
            }
        }
        //list<<"Cash"<<"Cheque"<<"Internet Transfer"<<"Bank Cash Deposit"<<"Bank Charges"<<"Bank Interest"<<"Contra";
        QStringList list = CtransactionUtils::Object()->getTransactionModeList();

        cmb->addItems(list);
        return cmb;
    }
    if (col == TRANS_TYPE_COL) {
        QModelIndex idx = index.model()->index(row, TRANS_ACCOUNT_COL);
        if (idx.isValid()) {
            QComboBox * cmb = new QComboBox(parent);
            EtransactionType transType = getTransTypeFromParticular(index);
            if (transType != INVALID_TRANSACTION_TYPE) {
                QStringList types;
                types.push_back(CtransactionUtils::Object()->getTransactionTypeStr(transType));
                cmb->addItems(types);
            } else {
                QString account = index.model()->data(idx  ).toString();
                int type = CaccountMap::Object()->getAccountType(account);
                QStringList types = CtransactionUtils::Object()->getTransactionTypesForAccountType(type);
                cmb->addItems(types);
            }
            return cmb;
        }

    }
    QLineEdit *editor = new QLineEdit(parent);
    if (col == TRANS_FROM_TO_COL) {
        //qDebug()<<"min list "<<CcontactMap::Object()->getNameList();
        QCompleter * autocompleter = new QCompleter(CcontactMap::Object()->getNameList(), NULL);
        autocompleter->setCaseSensitivity(Qt::CaseInsensitive);
        autocompleter->setWrapAround(true);
        autocompleter->setFilterMode(Qt::MatchContains);
        editor->setCompleter(autocompleter);
        //connect(editor, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()));
    }
    return editor;
}

void CcustomDeligateForTransaction::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int col = index.column();
    if (col == TRANS_ACCOUNT_COL || col == TRANS_MODE_COL || col == TRANS_TYPE_COL) {
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

void CcustomDeligateForTransaction::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    int col = index.column();
    if (col == TRANS_ACCOUNT_COL || col == TRANS_MODE_COL || col == TRANS_TYPE_COL) {
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
