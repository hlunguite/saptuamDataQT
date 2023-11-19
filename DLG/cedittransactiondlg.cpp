#include "cedittransactiondlg.h"
#include "ui_cedittransactiondlg.h"
#include "ccontactmap.h"
#include "caccountmap.h"
#include "ctransactionutils.h"
//#include "creceiptbooktable.h"
//#include "caccountreceiptbook.h"
#include "creceiptbookmap.h"
#include "caccounttable.h"
#include "cremittancetable.h"
#include <QMessageBox>
CeditTransactionDlg::CeditTransactionDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CeditTransactionDlg),
    m_idForEdit(0),
    m_markInvalid(false),
    m_import(false),
    m_currentTrans(nullptr)
{
    ui->setupUi(this);
    ui->m_invalidCheckbox->hide();
    ui->m_invalidReason->hide();
    m_fromToCompleter = new QCompleter(CcontactMap::Object()->getNameList(), NULL);
    m_fromToCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_fromToCompleter->setWrapAround(true);
    m_fromToCompleter->setFilterMode(Qt::MatchContains);
    ui->m_fromTo->setCompleter(m_fromToCompleter);

    const QStringList& accountList = CaccountMap::Object()->getAccountList();
    m_accountCompleter = new QCompleter(accountList, nullptr);
    m_accountCompleter->setCaseSensitivity(Qt::CaseSensitive);
    m_accountCompleter->setWrapAround(true);
    m_accountCompleter->setFilterMode(Qt::MatchContains);
    ui->m_account->setCompleter(m_accountCompleter);

    QStringList list = CtransactionUtils::Object()->getTransactionModeList();
    ui->m_modecombo->addItems(list);
    ui->m_amount->setEnabled(false);
    ui->m_label->setText("");

}

CeditTransactionDlg::~CeditTransactionDlg()
{
    delete ui;
    delete m_fromToCompleter;
    delete m_accountCompleter;
    if (m_currentTrans) {
        delete m_currentTrans;
    }
}

bool CeditTransactionDlg::checkAndSave()
{
    m_newTransID = 0;
    if (checkValue()) {
        StransactionData* transData = m_idForEdit > 0?  CtransactionTable::Object()->getTransaction(m_idForEdit) : nullptr;
        int slNo = transData ? transData->m_receiptSlNo.toInt() : 0;
        int bookNo = transData ? transData->m_reeiptBook.toInt() : 0;
        SreceiptBook receiptBook = CreceiptBookMap::Object()->getReceiptBookFromBookNo(bookNo);

        if (m_markInvalid && ui->m_invalidCheckbox->isChecked()) {
            invalidateRemittance();

            m_currentTrans->m_status = false;
            m_currentTrans->m_particular = ui->m_invalidReason->text().simplified() +
                                           ";" + m_currentTrans->m_particular;
            if (receiptBook.isValid() && receiptBook.isReceiptUsed(slNo)) {
                receiptBook.markReceiptCancelled(slNo);
                receiptBook.save();
            }
        } else {
            bool updateReceipt = false;

            m_currentTrans->m_fromId = CcontactMap::Object()->getContactID(ui->m_fromTo->text().trimmed().simplified());
            m_currentTrans->m_accountId = CaccountMap::Object()->getAccountID(ui->m_account->text().trimmed().simplified());
            m_currentTrans->m_amount = ui->m_amount->text().toDouble();
            m_currentTrans->m_mode = CtransactionUtils::Object()->getTransactionMode(ui->m_modecombo->currentText());
            m_currentTrans->m_ref = ui->m_reference->text().simplified();
            m_currentTrans->m_reeiptBook = ui->m_receiptBook->text().simplified();
            m_currentTrans->m_receiptSlNo = ui->m_receiptNo->text().simplified();
            m_currentTrans->m_particular = ui->m_particular->text().simplified();
            m_currentTrans->m_date = ui->m_dateEdit->date();
            m_currentTrans->m_type = CtransactionUtils::Object()->getTransactionType(ui->m_typeCombo->currentText());
            int book =  m_currentTrans->m_reeiptBook.toInt();
            if (m_idForEdit > 0 && (receiptBook.isValid() && receiptBook.isReceiptUsed(slNo))) {
                updateReceipt = true;
            }else  if (book > 0 && m_import) {
                updateReceipt = true;
                slNo = 0;
                bookNo = 0;
            }

            if (updateReceipt) {
                int sl = m_currentTrans->m_receiptSlNo.toInt();
                if ((book != bookNo) || (slNo != sl )) {
                    if (slNo > 0) {
                        receiptBook.markReceiptCancelled(slNo);
                        receiptBook.save();
                    }
                    SreceiptBook receiptBookNew = CreceiptBookMap::Object()->getReceiptBookFromBookNo(book);
                    if (receiptBookNew.isValid()) {
                        receiptBookNew.markReceiptUse(sl);
                        receiptBookNew.save();
                    }

                }
            }

        }
        m_newTransID = m_currentTrans->save();
        //qDebug()<<"saving trans "<<m_newTransID<<" type is "<<m_currentTrans->m_type;
        if (transData) {
            delete transData;
        }
        return true;
    }

    return false;
}
void CeditTransactionDlg::on_m_okButton_clicked()
{
    if(checkAndSave()) {
            emit accept();
    }


}

void CeditTransactionDlg::on_m_cancelButton_clicked()
{
    emit reject();
}

bool CeditTransactionDlg::checkValue()
{
    m_newTransID = 0;
    QString msg;
    if (m_markInvalid) {
        if (ui->m_invalidCheckbox->isChecked()) {
            if (ui->m_invalidReason->text().trimmed().isEmpty()) {
                msg = "Reson not specify";
            } else  {


                return true;
            }
        }
    } else {
        //check name
        if (CcontactMap::Object()->getContactID(ui->m_fromTo->text().trimmed().simplified()) == 0) {
            msg = ui->m_fromTo->text().trimmed().simplified() + " name not found" ;
        } else {
            // check account
            if (CaccountMap::Object()->getAccountID(ui->m_account->text().trimmed().simplified()) == 0) {
                msg = ui->m_account->text() + " account not found";
            } else {
                bool ok;
                ui->m_amount->text().toFloat(&ok);
                if (ok == false) {
                    msg = ui->m_amount->text() + " is not a number";
                } else {
                    QString mode = ui->m_modecombo->currentText();
                    if (CtransactionUtils::Object()->isModeRequireRef(mode) ) {
                        if (ui->m_reference->text().isEmpty()) {
                            msg = "Reference empty";
                        }
                    }

                    if (msg.isEmpty()) {
                        QString rBook = ui->m_receiptBook->text().trimmed();
                        QString rNo = ui->m_receiptNo->text().trimmed();
                        if (rBook.isEmpty() == false && rNo.isEmpty() == false) {
                            if (!(m_currentTrans->m_reeiptBook == rBook &&
                                  m_currentTrans->m_receiptSlNo == rNo)){
                                bool ok;
                                int receiptBook = rBook.toInt(&ok);
                                if (ok) {
                                    int receiptNo = rNo.toInt(&ok);
                                    if (ok) {
                                        if (receiptBook == 0 ) {
                                            return true;
                                        }
                                        //1. check if rbook and rno already use
                                        //2. check if rbook and rno valid
                                        bool isValid = CreceiptBookMap::Object()->isReceiptAvailable(receiptBook, receiptNo);
                                        if (isValid == false) {
                                            msg = "Receipt already use or not available";
                                        } else {
                                            // check if receipt and account match
                                            if (m_import == false) {
                                                int accountID =CaccountMap::Object()->getAccountID(ui->m_account->text().trimmed().simplified());
                                                if (CreceiptBookMap::Object()->getIsReceiptAssignToAccount(receiptBook, accountID) == false) {
                                                    msg = "Receipt is not assign to Account";
                                                }
                                            }
                                        }


                                    } else {
                                        msg = "Receipt should be number";
                                    }

                                } else {
                                    msg = "Receipt should be number";
                                }
                            }

                            if (msg.isEmpty()) {
                                return true;
                            }
                        } else {
                            return true;
                        }

                    }
                }
            }
        }
    }
    QMessageBox::warning(this, "SaptuamData",
                         msg);

    return false;

}


bool CeditTransactionDlg::markInvalid() const
{
    return m_markInvalid;
}

void CeditTransactionDlg::setMarkInvalid(bool markInvalid)
{
    bool enable = true;
    m_markInvalid = markInvalid;
    if (m_markInvalid) {
        enable = false;
        ui->m_invalidCheckbox->show();
        ui->m_invalidReason->show();

    } else {
        enable = true;

        ui->m_invalidCheckbox->hide();
        ui->m_invalidReason->hide();

    }
    ui->m_fromTo->setEnabled(enable);
    ui->m_amount->setEnabled(false);
    ui->m_account->setEnabled(enable);
    ui->m_receiptNo->setEnabled(enable);
    ui->m_receiptBook->setEnabled(enable);
    ui->m_particular->setEnabled(enable);
    ui->m_modecombo->setEnabled(enable);
    ui->m_reference->setEnabled(enable);
    ui->m_typeCombo->setEnabled(enable);
    ui->m_dateEdit->setEnabled(enable);
    QString str;

    if (markInvalid) {
        ui->m_label->setEnabled(true);
        if (ui->m_receiptBook->text().isEmpty() == false && ui->m_receiptNo->text().isEmpty() == false) {
            str = "*Receipt " + ui->m_receiptBook->text() + ":" + ui->m_receiptNo->text() + " will be mark cancelled. Re-add if require";
        }
    } else {
        ui->m_label->setEnabled(false);
    }
    ui->m_label->setText(str);
}

int CeditTransactionDlg::idForEdit() const
{
    return m_idForEdit;
}

void CeditTransactionDlg::setIdForEdit(int idForEdit)
{
    if (idForEdit < 1) {
        return;
    }
    m_idForEdit = idForEdit;
    m_currentTrans = CtransactionTable::Object()->getTransaction(m_idForEdit);
    populateForm();


}

void CeditTransactionDlg::setForImport(StransactionData *data)
{
    m_idForEdit = 0;
    m_import = false;
    if (data) {
        m_import = true;
        if (m_currentTrans) {
            delete m_currentTrans;
        }
        m_currentTrans = data;
        populateForm();
    }
}



void CeditTransactionDlg::keyPressEvent(QKeyEvent * e)
{
    if(m_markInvalid) {
        return;
    }
       //ctrl+N
    if(e->matches(QKeySequence::New)) {
       ui->m_amount->setEnabled(true);
     }
}

void CeditTransactionDlg::on_m_account_editingFinished()
{
    QString accountName = ui->m_account->text().trimmed();
    if (accountName.isEmpty() == false) {
        ui->m_typeCombo->clear();
        QStringList types = CtransactionUtils::Object()->getTransactionTypesForAccountType(CaccountMap::Object()->getAccountType(accountName));
        ui->m_typeCombo->addItems(types);
    }
}

int CeditTransactionDlg::newTransID() const
{
    return m_newTransID;
}


void CeditTransactionDlg::populateForm()
{
    if (m_currentTrans) {
        QString name = CcontactMap::Object()->getContanceName(m_currentTrans->m_fromId);
        ui->m_dateEdit->setDate(m_currentTrans->m_date);
        ui->m_fromTo->setText(name);
        ui->m_amount->setText(QString::number(m_currentTrans->m_amount, 'f', 2));
        ui->m_account->setText(CaccountMap::Object()->getAccountName(m_currentTrans->m_accountId));
        QString receiptBook = m_currentTrans->m_reeiptBook;
        if (receiptBook.compare("0") == 0) {
            receiptBook = "";
        }
        ui->m_receiptBook->setText(receiptBook);
        ui->m_receiptNo->setText(m_currentTrans->m_receiptSlNo);
        //qDebug()<<"current trans ["<<m_currentTrans->m_particular<<"]";
        ui->m_particular->setText(m_currentTrans->m_particular);
        QString mode = CtransactionUtils::Object()->getTransactionModeStr((EtransactionMode)m_currentTrans->m_mode);

        ui->m_modecombo->setCurrentText(mode);
        ui->m_reference->setText(m_currentTrans->m_ref);

        QStringList types = CtransactionUtils::Object()->getTransactionTypesForAccountType(CaccountMap::Object()->getAccountType(CaccountMap::Object()->getAccountName(m_currentTrans->m_accountId)));
        ui->m_typeCombo->addItems(types);
        QString type = CtransactionUtils::Object()->getTransactionTypeStr((EtransactionType)m_currentTrans->m_type);
        ui->m_typeCombo->setCurrentText(type);
        //qDebug()<<"\nEdit for "<<name<<" type "<<type<<" "<<m_currentTrans->m_type<<" amount "<<ui->m_amount->text();

    }
}

void CeditTransactionDlg::invalidateRemittance()
{
    int id = m_currentTrans->m_id;
    int accountID = m_currentTrans->m_accountId;
    SaccountData* account = CaccountTable::Object()->getAccountDataForId(accountID);
    int accountType = account->m_accountType;
    delete account;
    if ( accountType != REMITTANCE_ACCOUNT_TYPE) {
        return;
    }
    SremittanceData * remitData = CremittanceTable::Object()->getRemittanceDataFromTransID(id);
    int remitID = 0;
    if (remitData) {
        remitID = remitData->m_id;

    } else {
        return;
    }
    QString filter = "(" +CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1) AND ";
    QString query = "SELECT * FROM " + CtransactionTable::Object()->getTableName() + " WHERE ";
    filter +=  CtransactionTable::Object()->getColName(TRANSACTION_REMITTANCE_ID_IDX) + "=" + QString::number(remitID);
    query += filter;

    QVector<StransactionData*> alltrans =  CtransactionTable::Object()->getAllTransaction(query);
    for (int i = 0; i < alltrans.size(); ++i) {
        StransactionData* transData = alltrans.at(i);
        //qDebug()<<"Transaction: "<<transData->m_amount<<" "<<CaccountMap::Object()->getAccountName(transData->m_accountId)<<" "<<CcontactMap::Object()->getContanceName(transData->m_fromId);
        transData->m_remittanceID = 0;
        transData->save();
        delete transData;
    }


    QVector<SremittanceDetailData*>* vData = CremittanceDetailTable::Object()->getRemittanceDetailForTableID(remitID);
    if (vData) {
        for (auto detail: *vData) {
            detail->m_remitanceTableID = 0;
            //qDebug()<<"remit detail "<<detail->m_hqShare<<" "<<detail->m_localShare;
            detail->deleteObj();
            delete detail;
        }
        delete vData;
    }
    QVector<SremittanceReconcileData*>* vdata = CremittanceReconcileTable::Object()->getRemittanceReconcileForTableID(remitID);
    if (vdata) {
        for (auto detail: *vdata) {
            //qDebug()<<"reconcile "<<detail->m_isAccount;
            detail->deleteObj();
            delete detail;
        }

        delete vdata;
    }


    remitData->m_transactionID = 0;
    remitData->save();
    remitData->deleteObj();
    delete remitData;

}
