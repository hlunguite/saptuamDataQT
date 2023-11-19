#include "cnewtransactiondlg.h"
#include "ui_cnewtransactiondlg.h"
#include "ccontactmap.h"
#include "caccountmap.h"
#include "csdutils.h"
#include <QDebug>
#include <QMessageBox>
#include <QMenu>
#include "caccounttable.h"
#include "ctransactiontable.h"
#include "ctransactionutils.h"
#include "cimportbanktransactiontable.h"
#include "ccontacttable.h"
#include <QMessageBox>

CnewTransactionDlg::CnewTransactionDlg(bool bankImport, int importID, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CnewTransactionDlg),
    m_bankImport(bankImport),
    m_importID(importID),
    m_currentBankTransDetailIdx(-1),
    m_disableCheckRef(false),
    m_incomeTotal(0),
    m_paymentTotal(0),
    m_calcTotalCall(false)
{
    ui->setupUi(this);
    setMaximumHeight(CsdUtils::maxHeight());
    setMaximumWidth(CsdUtils::maxWidth());
    ui->m_date->setCalendarPopup(true);
    QDate date = QDate::currentDate();
    ui->m_date->setDate(date);
    ui->m_transactionTable->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->m_transactionTable->setColumnCount(TRANS_END_COL);

    m_deleteRow = new QAction("Delete Row", this);
    m_deleteAllRow = new QAction("Delete All Row", this);
    connect(m_deleteRow, SIGNAL(triggered()), this, SLOT(deleteRow()));
    connect(m_deleteAllRow, SIGNAL(triggered()), this, SLOT(deleteAllRow()));
    m_rowNumber = -1;
    m_deligateForTransaction = new CcustomDeligateForTransaction();
    ui->m_transactionTable->setItemDelegate(m_deligateForTransaction);

    ui->m_summaryTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    populateBankImport(importID);
    populateBankTransDetail();
    resize();

}

CnewTransactionDlg::~CnewTransactionDlg()
{
    delete ui;
    delete m_deligateForTransaction;
    delete m_deleteAllRow;
    delete m_deleteRow;
}

void CnewTransactionDlg::deleteRow()
{
    if (m_rowNumber > -1) {
        ui->m_transactionTable->removeRow(m_rowNumber);
        int norow = ui->m_transactionTable->rowCount();
        ui->m_transactionTable->insertRow(norow);
        calcTotal();
        m_rowNumber = -1;

    }
}

void CnewTransactionDlg::deleteAllRow()
{
    QMessageBox msg;
     msg.setText("Do you want to delete all rows?");
     msg.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
     msg.setDefaultButton(QMessageBox::Cancel);
     int ret = msg.exec();
     if(ret != QMessageBox::Ok) {
         return;
     }
     ui->m_transactionTable->clearContents();
     ui->m_transactionTable->setRowCount(0);
     calcTotal();

}

void CnewTransactionDlg::resize()
{
    int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();
    int x = ui->m_transactionTable->geometry().x();
    int y = ui->m_transactionTable->geometry().y();
    int buttonheight = ui->m_okButton->geometry().height();
    int buttonwidth = ui->m_okButton->geometry().width();

    int width = windowwidth - x -x;
    int height = windowheight*.5;
    ui->m_transactionTable->setGeometry(x,y,width,height);
    ui->m_transactionTable->setMinimumWidth(.30*(float)windowwidth);
    ui->m_transactionTable->setMinimumHeight(.15*(float)windowwidth);
    ui->m_transactionTable->setMaximumHeight(.8*windowheight);


    ui->m_transactionTable->horizontalHeader()->resizeSection(TRANS_FROM_TO_COL,width*.25);
    ui->m_transactionTable->horizontalHeader()->resizeSection(TRANS_ACCOUNT_COL,width*.20);
    ui->m_transactionTable->horizontalHeader()->resizeSection(TRANS_TYPE_COL,width*.10);
    ui->m_transactionTable->horizontalHeader()->resizeSection(TRANS_AMOUNT_COL,width*.10);
    ui->m_transactionTable->horizontalHeader()->resizeSection(TRANS_MODE_COL,width*.1);
    ui->m_transactionTable->horizontalHeader()->resizeSection(TRANS_REF_COL,width*.1);
    ui->m_transactionTable->horizontalHeader()->resizeSection(TRANS_PARTICULAR_REMARK_COL,width*.15);
    y = y + height;
    int height1 =0;
    //int y1 = 0;
    //int x1 = 0;
    if (m_bankImport == false) {
        width = windowwidth *.40;
        ui->m_importLine->hide();
        ui->m_bankImportLbl->hide();
        ui->m_bankImportTable->hide();
        ui->m_previousTable->hide();
        ui->m_summaryLbl->setGeometry(x,y,width,buttonheight);
        y +=buttonheight;
        height1 = windowheight -  (y + buttonheight);
        ui->m_summaryTable->setGeometry(x,y,width,height1);
        ui->m_summaryTable->setMinimumWidth(.10*(float)windowwidth);
        ui->m_summaryTable->horizontalHeader()->resizeSection(0,width*0.45);
        ui->m_summaryTable->horizontalHeader()->resizeSection(1,width*0.15);
        ui->m_summaryTable->horizontalHeader()->resizeSection(2,width*0.20);
        ui->m_summaryTable->horizontalHeader()->resizeSection(2,width*0.20);

    } else {
        int datewidth = ui->m_date->geometry().width();
        ui->m_importLine->setGeometry(ui->m_date->geometry().x() + datewidth,
                                      ui->m_date->geometry().y(),
                                      width - datewidth,
                                      ui->m_importLine->geometry().height());
        width = windowwidth *.20;
        ui->m_summaryLbl->hide();
        ui->m_summaryTable->hide();

        ui->m_importLine->setDisabled(true);

        ui->m_bankImportLbl->setGeometry(x,y,width,buttonheight);
        y +=buttonheight;
        height1 = windowheight -  (y + buttonheight);
        ui->m_bankImportTable->setGeometry(x,y,width,height1);
        ui->m_bankImportTable->horizontalHeader()->resizeSection(0, width);
        ui->m_okButton->setText("Add");

    }

    x = x + width + 3;
    ui->m_bankTotalTable->setGeometry(x,y,width,height1);
    ui->m_bankTotalTable->setMinimumWidth(.10*(float)windowwidth);
    ui->m_bankTotalTable->horizontalHeader()->resizeSection(0,width*.5);
    ui->m_bankTotalTable->horizontalHeader()->resizeSection(1,width*.5);
    ui->m_verifyBankLbl->setGeometry(x,(y-buttonheight),width,buttonheight);

    if (m_bankImport == true) {
        x += width + 3;
        width = ui->m_transactionTable->width() - 2*width - ui->m_transactionTable->x() -(2*buttonwidth);
        ui->m_previousTable->setGeometry(x, y, width, height1);
        ui->m_previousTable->horizontalHeader()->resizeSection(0, width);
        //ui->m_okButton->setGeometry(x + 3, y, buttonwidth, buttonheight);
        y += height1/2;

    } else {
        y += height1 - buttonheight;
        ui->m_rejectBtn->hide();;
        ui->m_skipBtn->hide();
    }
    x = ui->m_transactionTable->x() + ui->m_transactionTable->width() -(3*buttonwidth);

    x += buttonwidth;

    ui->m_cancelButton->setGeometry(x,y,buttonwidth,buttonheight);
    x += buttonwidth;
    ui->m_okButton->setGeometry(x,y,buttonwidth,buttonheight);
    if (m_bankImport) {
        x -= buttonwidth;
        y += buttonheight;
        ui->m_skipBtn->setGeometry(x,y,buttonwidth,buttonheight);
        x += buttonwidth;
        ui->m_rejectBtn->setGeometry(x,y,buttonwidth,buttonheight);


    }

}

QString CnewTransactionDlg::getErrorString(EtransErrorType error)
{
    QString errorstr;
    switch(error) {
    case TRANS_EMPTY_NAME_ERROR: errorstr = "Empty From/To"; break;
    case TRANS_NAME_NOT_FOUND_ERROR: errorstr = "From/To not found in contact"; break;
    case TRANS_AMOUNT_EMPTY_ERROR: errorstr = "Amount Empty"; break;
    case TRANS_AMOUNT_NOT_NUMBER_ERROR: errorstr = "Amount is Not a Number"; break;
    case TRANS_ACCOUNT_NOT_FOUND_ERROR: errorstr = "Account not found"; break;
    case TRANS_ACCOUNT_EMPTY_ERROR: errorstr = "Account empty"; break;
    case TRANS_REF_EMPTY_ERROR: errorstr = "Refernce empty"; break;
    case TRANS_TYPE_EMPTY_ERROR: errorstr = "Type empty"; break;
    case TRANS_MODE_EMPTY_ERROR: errorstr = "Mode empty"; break;
    case TRANS_NULL_CELL_ERROR: errorstr = "Empty cell"; break;
    case TRANS_AMOUNT_NOT_MATCH_ERROR: errorstr = "Total Amount not match with bank"; break;
    case TRANS_EMPTY_BANK_IMPORT_ERROR: errorstr = "No bank import data"; break;
    case TRANS_MODE_AND_ACCOUNT_TYPE_MISMATCH: errorstr = "Account type and mode mismatch"; break;
    case TRANS_INCOME_ACCOUNT_FOR_PAYMENT: errorstr = "Income acccount/type for bank payment"; break;
    case TRANS_PAYMENT_ACCOUNT_FOR_INCOME: errorstr = "Payment account/type for bank income"; break;
    case TRANS_BANK_AND_ENTRY_MISMATCH: errorstr = "Account/Type mismatch"; break;
    case TRANS_TYPE_MISMATCH: errorstr = "Type mismatch"; break;

    default: break;
    }
    return errorstr;
}

EtransErrorType CnewTransactionDlg::calcTotal()
{
    if (m_calcTotalCall) {
        return TRANS_NO_ERROR;
    }
    m_calcTotalCall = true;
    m_allRowData.clear();
    m_accountSummaryMap.clear();
    m_bankTotalMap.clear();
    m_incomeTotal = 0;
    m_paymentTotal = 0;
    int rowcount = ui->m_transactionTable->rowCount();
    EtransErrorType status = TRANS_NO_ERROR;
    double totalAmount = 0;
    for(int i = 0; i < rowcount; ++i){
        QVector<QVariant> rowData(TRANS_END_COL);
        EtransErrorType error = checkForValidityOfRow(i, rowData);
        if (error == TRANS_IGNORE_ROW) {
            continue;
        } else if (error != TRANS_NO_ERROR) {
            status = error;
            continue;
        }
        m_allRowData.push_back(rowData);
        totalAmount += rowData.at(TRANS_AMOUNT_COL).toDouble();
    }

    if (status != TRANS_NO_ERROR) {
        m_calcTotalCall = false;
        return status;
    }
    status = calcTotalNetTrans();
    if (status == TRANS_NO_ERROR && m_bankImport) {
        if (m_currentBankTransDetailIdx > -1 && m_bankTransDetail.size() > m_currentBankTransDetailIdx) {
            const SbanktransDetail& bankTransDetail = m_bankTransDetail.at(m_currentBankTransDetailIdx);
            double amount = bankTransDetail.m_amount;
            if (amount != totalAmount) {

                status = TRANS_AMOUNT_NOT_MATCH_ERROR;
            } else {
                int type = bankTransDetail.m_type;
                if (type == BANK_CASH_DEPOSIT || type == BANK_CHARGES || type == BANK_INTEREST){

                } else {
                    if (bankTransDetail.m_isIncome) {
                        if (m_incomeTotal != amount) {

                            status = TRANS_AMOUNT_NOT_MATCH_ERROR;
                        }
                    } else if (m_paymentTotal != amount) {

                        status = TRANS_AMOUNT_NOT_MATCH_ERROR;
                    }
                }
                if (status == TRANS_NO_ERROR) {

                    if (bankTransDetail.m_isIncome && m_paymentTotal > 0) {
                        status = TRANS_PAYMENT_ACCOUNT_FOR_INCOME;
                    } else if (bankTransDetail.m_isIncome == false && m_incomeTotal > 0) {
                        status = TRANS_INCOME_ACCOUNT_FOR_PAYMENT;
                    }
                }
            }
        } else {
            status = TRANS_EMPTY_BANK_IMPORT_ERROR;
        }
    }
    m_calcTotalCall = false;
    return status;
}

EtransErrorType CnewTransactionDlg::calcTotalNetTrans()
{
    EtransErrorType status = TRANS_NO_ERROR;

    int totalcount = 0;
    m_incomeTotal = 0;
    m_paymentTotal = 0;
    //int rowcount = ui->m_summaryTable->rowCount();
    ui->m_summaryTable->clearContents();
    ui->m_summaryTable->setRowCount(0);
    QMapIterator<QString, QPair<int, double> > iterator1(m_accountSummaryMap);
    while (iterator1.hasNext()) {
        iterator1.next();
        QString accountName = iterator1.key();
        int count = iterator1.value().first;
        double amount = iterator1.value().second;
        int accountType = CaccountMap::Object()->getAccountType(accountName);
        if (accountType > REMITTANCE_ACCOUNT_TYPE) {
            if (accountName == gBankChargeTransactionType ||
                accountName == gLoanDisburseTransactionType) {
                accountType = PAYMENT_ACCOUNT_TYPE;
            } else if (accountName == gBankInterestTransactionType ||
                       accountName == gBankCashDepositTransactionType ||
                       accountName == gLoanRecoverTransactionType) {
                accountType = INCOME_ACCOUNT_TYPE;
            }  else {
                continue;
            }
        }

        QString income;
        QString payment;
        if (INCOME_ACCOUNT_TYPE == accountType || REQUEST_ACCOUNT_TYPE == accountType) {
            income = QLocale(QLocale::English).toString(amount, 'f', 2);
            m_incomeTotal += amount;

        } else if (accountType == PAYMENT_ACCOUNT_TYPE || accountType == REMITTANCE_ACCOUNT_TYPE) {
            payment = QLocale(QLocale::English).toString(amount, 'f', 2);
            m_paymentTotal += amount;
        } else {


                continue;

        }
        totalcount += count;
        int row = ui->m_summaryTable->rowCount();
        ui->m_summaryTable->insertRow(row);
        QTableWidgetItem * ac = new QTableWidgetItem(tr("%1").arg(accountName));
        QTableWidgetItem * cnt = new QTableWidgetItem(tr("%1").arg(count));
        QTableWidgetItem * amtIncome = new QTableWidgetItem(tr("%1").arg(income));
        QTableWidgetItem * amtPayment = new QTableWidgetItem(tr("%1").arg(payment));

        ui->m_summaryTable->setItem(row, 0, ac);
        ui->m_summaryTable->setItem(row, 1, cnt);
        ui->m_summaryTable->setItem(row, 2, amtIncome);
        ui->m_summaryTable->setItem(row, 3, amtPayment);
    }
    if (totalcount > 0) {
        int row = ui->m_summaryTable->rowCount();
        ui->m_summaryTable->insertRow(row);
        QString income = "₹ " + QLocale(QLocale::English,QLocale::India).toString(m_incomeTotal, 'f', 2) ;
        QString payment = "₹ " + QLocale(QLocale::English,QLocale::India).toString(m_paymentTotal, 'f', 2) ;

        QTableWidgetItem * ac = new QTableWidgetItem(tr("%1").arg("Total"));
        QTableWidgetItem * cnt = new QTableWidgetItem(tr("%1").arg(totalcount));
        QTableWidgetItem * amtIncome = new QTableWidgetItem(tr("%1").arg(income));
        QTableWidgetItem * amtPayment = new QTableWidgetItem(tr("%1").arg(payment));

        ui->m_summaryTable->setItem(row, 0, ac);
        ui->m_summaryTable->setItem(row, 1, cnt);
        ui->m_summaryTable->setItem(row, 2, amtIncome);
        ui->m_summaryTable->setItem(row, 3, amtPayment);
    }

    ui->m_bankTotalTable->clearContents();
    ui->m_bankTotalTable->setRowCount(0);
    QMapIterator<QString,double > iterator2(m_bankTotalMap);
    while(iterator2.hasNext()){
        iterator2.next();
        QString refname = iterator2.key();
        double amount = iterator2.value();
        QString number = "₹ " + QLocale(QLocale::English,QLocale::India).toString(amount, 'f', 2) ;

        int row = ui->m_bankTotalTable->rowCount();
        ui->m_bankTotalTable->insertRow(row);

        QTableWidgetItem * ac = new QTableWidgetItem(tr("%1").arg(refname));
        QTableWidgetItem * amt = new QTableWidgetItem(tr("%1").arg(number));

        ui->m_bankTotalTable->setItem(row,0,ac);
        ui->m_bankTotalTable->setItem(row,1,amt);
    }

    return status;
}

EtransErrorType CnewTransactionDlg::checkForValidityOfRow(int row,  QVector<QVariant>& rowData)
{
    EtransErrorType status = TRANS_NO_ERROR;

    QTableWidgetItem* fromTo = ui->m_transactionTable->item(row, TRANS_FROM_TO_COL);
    QTableWidgetItem* amount = ui->m_transactionTable->item(row, TRANS_AMOUNT_COL);
    QTableWidgetItem* account = ui->m_transactionTable->item(row, TRANS_ACCOUNT_COL);
    QTableWidgetItem* mode = ui->m_transactionTable->item(row, TRANS_MODE_COL);
    QTableWidgetItem* ref = ui->m_transactionTable->item(row, TRANS_REF_COL);
    QTableWidgetItem* remark = ui->m_transactionTable->item(row, TRANS_PARTICULAR_REMARK_COL);
    QTableWidgetItem* type = ui->m_transactionTable->item(row, TRANS_TYPE_COL);

    if (isEmptyOrNull(fromTo) && isEmptyOrNull(amount) && isEmptyOrNull(account) && isEmptyOrNull(mode)) {
        return TRANS_IGNORE_ROW;
    }
    status = checkFromToCol(fromTo);
    if (status != TRANS_NO_ERROR) {
        return status;
    }
    if (fromTo == nullptr) {
        return TRANS_EMPTY_NAME_ERROR;
    }
    status = checkAmount(amount);
    if (status != TRANS_NO_ERROR) {
        return status;
    }
    if (amount == nullptr) {
        return TRANS_AMOUNT_EMPTY_ERROR;
    }
    status = checkAccount(account);
    if (status != TRANS_NO_ERROR) {
        return status;
    }
    if (account == nullptr) {
        return TRANS_ACCOUNT_EMPTY_ERROR;
    }
    status = checkMode(mode);
    if (status != TRANS_NO_ERROR) {
        return status;
    }
    if (mode == nullptr) {
        return TRANS_MODE_EMPTY_ERROR;
    }
    status = checkRef(ref);
    if (status != TRANS_NO_ERROR) {
        return status;
    }
    if (ref == nullptr && getIsModeRequireRef(row)) {
        return TRANS_REF_EMPTY_ERROR;
    }
    status = checkType(type);
    if (status != TRANS_NO_ERROR) {
        return status;
    }
    QString accountName = account->text().trimmed();
    QString transType = type->text().trimmed();
    double  value = amount->text().toDouble();
    if (accountName.compare(gBankAccountName) == 0) {
        accountName = transType;
    } else if (accountName.compare("Loan Account") == 0) {
        accountName = transType;
    }
    if (m_accountSummaryMap.contains(accountName)) {
        int count = m_accountSummaryMap[accountName].first + 1;
        double accountValue = m_accountSummaryMap[accountName].second + value;
        m_accountSummaryMap[accountName].first = count;
        m_accountSummaryMap[accountName].second = accountValue;
    } else {
        m_accountSummaryMap.insert(accountName, qMakePair(1, value));
    }

    rowData[TRANS_FROM_TO_COL] = fromTo->text().trimmed();
    rowData[TRANS_AMOUNT_COL]  = value;
    rowData[TRANS_ACCOUNT_COL] = accountName;
    rowData[TRANS_TYPE_COL] = transType;
    QString modeStr = mode->text().trimmed();
    int modeInt = CtransactionUtils::Object()->getTransactionMode(modeStr);
    rowData[TRANS_MODE_COL] = modeInt;
    if (ref){
        QString refernce = ref->text().trimmed();
        rowData[TRANS_REF_COL] = refernce;
        if (refernce.isEmpty() == false) {
            if (m_bankTotalMap.contains(refernce)) {
                m_bankTotalMap[refernce] += value;
            } else {
                m_bankTotalMap.insert(refernce, value);
            }
        }
    } else {
        rowData[TRANS_REF_COL] = "";
    }

    if (remark) {
        rowData[TRANS_PARTICULAR_REMARK_COL] = remark->text().trimmed().simplified();
    } else {
        rowData[TRANS_PARTICULAR_REMARK_COL] = "";
    }

    return TRANS_NO_ERROR;
}

bool CnewTransactionDlg::isEmptyOrNull(QTableWidgetItem *item)
{
    bool ret = false;
    if (item) {
        if (item->text().isEmpty()) {
            ret = true;
        }
    } else {
        ret = true;
    }
    return ret;
}

void CnewTransactionDlg::addRefAndModeForRow(int row)
{
    if (m_bankImport && m_bankTransDetail.size() && (m_currentBankTransDetailIdx < m_bankTransDetail.size())) {
        // add ref
        m_disableCheckRef = true;
        QTableWidgetItem * ref = ui->m_transactionTable->item(row,TRANS_REF_COL);
        const SbanktransDetail& bankTransDetail = m_bankTransDetail.at(m_currentBankTransDetailIdx);
        if(ref == NULL){
             ref =   new QTableWidgetItem();
            ui->m_transactionTable->setItem(row,TRANS_REF_COL,ref);
            ref->setText(bankTransDetail.m_refID);
        }
        ref->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled );

        //add mode
        QString modeStr =  CtransactionUtils::Object()->getTransactionModeStr(INTERNET_TRANSACTION_MODE);//"Internet Transfer";
        if (bankTransDetail.m_type == CHEQUE) {
            modeStr = CtransactionUtils::Object()->getTransactionModeStr(CHEQUE_TRANSACTION_MODE);
           //modeStr = "Cheque";
        } else if (bankTransDetail.m_type == BANK_CASH_DEPOSIT){
            modeStr = CtransactionUtils::Object()->getTransactionModeStr(BANK_CASH_DEPOSIT_TRANSACTION_MODE);
            //modeStr = "Bank Cash Deposit";
        } else if (bankTransDetail.m_type == BANK_CHARGES) {
            modeStr = CtransactionUtils::Object()->getTransactionModeStr(BANK_CHARGES_TRANSACTION_MODE);
            //modeStr = "Bank Charges";
        } else if (bankTransDetail.m_type == BANK_INTEREST) {
            modeStr = CtransactionUtils::Object()->getTransactionModeStr(BANK_INTEREST_TRANSACTION_MODE);
            //modeStr = "Bank Interest";
        }

        QTableWidgetItem * type = ui->m_transactionTable->item(row,TRANS_TYPE_COL);
        bool updateMode = false;
        if (type) {
            int transType = CtransactionUtils::Object()->getTransactionType(type->text());
            if (transType == (int)BANK_CASH_DEPOSIT_TRANSACTION_TYPE) {
                modeStr = CtransactionUtils::Object()->getTransactionModeStr(BANK_CASH_DEPOSIT_TRANSACTION_MODE);
                updateMode = true;
            } else if (transType == (int)BANK_CHARGES_TRANSACTION_TYPE) {
                modeStr = CtransactionUtils::Object()->getTransactionModeStr(BANK_CHARGES_TRANSACTION_MODE);
                updateMode = true;
            }
            if (transType == (int)BANK_INTEREST_TRANSACTION_TYPE) {
                modeStr = CtransactionUtils::Object()->getTransactionModeStr(BANK_INTEREST_TRANSACTION_MODE);
                updateMode = true;
            }
        }

        QTableWidgetItem * mode = ui->m_transactionTable->item(row, TRANS_MODE_COL);
        if(mode == NULL){
            mode =   new QTableWidgetItem();
            ui->m_transactionTable->setItem(row,TRANS_MODE_COL, mode);
            mode->setText(modeStr);
        } else if (updateMode) {
            if (modeStr != mode->text().trimmed()) {
                mode->setText(modeStr);
            }
        }
        mode->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled );
        m_disableCheckRef = false;
    }
}

void CnewTransactionDlg::populateBankImport(int importID)
{
    if (m_bankImport == false) {
        return;
    }
    QVector<SimportBankTransactionData> importName = CimportBankTransactionTable::Object()->getAllBankImport();
    ui->m_bankImportTable->clearContents();
    ui->m_bankImportTable->setRowCount(0);
    m_importTableId.clear();
    for (int i = 0; i < importName.size(); ++i) {
        int id = importName.at(i).m_id;
        QString daterange = importName.at(i).m_dateRange;
        QTableWidgetItem * row = new QTableWidgetItem(tr("%1").arg(daterange));

        row->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        if (importID > 0 && (id ==	 importID)) {
            row->setCheckState(Qt::Checked);
        } else {
            row->setCheckState(Qt::Unchecked);
        }
        //row->setText(daterange);
        int rowcount = ui->m_bankImportTable->rowCount();
        ui->m_bankImportTable->insertRow(rowcount);
        ui->m_bankImportTable->setItem(i, 0, row);
        m_importTableId.push_back(importName.at(i).m_id);


    }

}

void CnewTransactionDlg::displayCurrentBankImport()
{

    if (m_bankImport && m_bankTransDetail.size() &&
            m_currentBankTransDetailIdx > -1 && (m_currentBankTransDetailIdx < m_bankTransDetail.size())) {
        const SbanktransDetail& bankTransDetail = m_bankTransDetail.at(m_currentBankTransDetailIdx);
        QString line = bankTransDetail.toString();
        ScontactData* contact1 = CcontactTable::Object()->getContactFromBankName(bankTransDetail.m_min);
        ScontactData* contact2 = CcontactTable::Object()->getContactFromPhone(bankTransDetail.m_phone);
        QString name1 = contact1 ? contact1->m_fullNameWithSpouse : "";
        QString name2 = contact2 ? contact2->m_fullNameWithSpouse : "";

        if (name1.isEmpty() == false && name2.isEmpty() == false) {
            if (name1 == name2) {
                line = "[" + name1 + "] " + line;
            } else {
                line = "[" + name1 + "/" + name2 + "] " + line;
            }
        }else if (name2.isEmpty() == false) {
            line = "[" + name2 + "] " + line;

        } else if (name1.isEmpty() == false) {
           line = "[" + name1 + "] " + line;
        }
        ui->m_importLine->setText(line);
        if (checkRefAlreadyHasTrans() == false) {
            addInPrevTable(contact1 ? contact1->m_idx : 0 , contact2 ? contact2->m_idx : 0);
        } else {
           moveToNextImport();
        }
        delete contact1;
        delete contact2;
    } else {
        m_currentBankTransDetailIdx = -1;
    }


}

void CnewTransactionDlg::populateBankTransDetail()
{
    if (m_bankImport == false) {
        return;
    }
   m_bankTransDetail = CbankTransactionTable::Object()->getAllbankTransForSameImport(m_importID);
   if (m_bankTransDetail.isEmpty() == false) {
        m_currentBankTransDetailIdx = 0;
        displayCurrentBankImport();
   } else {
       m_currentBankTransDetailIdx = -1;
   }


}

void CnewTransactionDlg::moveToNextImport(bool skip)
{
    if (m_bankImport) {
        if (m_currentBankTransDetailIdx > -1 && m_currentBankTransDetailIdx < m_bankTransDetail.size()) {
            if (skip == false) {
                SbanktransDetail bankTrabsDetail = m_bankTransDetail[m_currentBankTransDetailIdx];
                bankTrabsDetail.deleteObj();
            }
            ++m_currentBankTransDetailIdx;
            if (m_currentBankTransDetailIdx < m_bankTransDetail.size()) {
                displayCurrentBankImport();
            } else {
                QMessageBox::warning(this, "SaptuamData",
                                     "Reach end of data");
                m_currentBankTransDetailIdx = -1;
                m_importID = 0;
                ui->m_importLine->setText("");
            }

        }
    }

}

void CnewTransactionDlg::addBankName(QSet<int> &fromIDs, QString bankName)
{
    for (int id : fromIDs) {
        CcontactTable::Object()->addBankName(id, bankName);

    }
}

bool CnewTransactionDlg::checkRefAlreadyHasTrans()
{
    bool ret = false;
    if (m_bankImport && m_bankTransDetail.size() &&
            m_currentBankTransDetailIdx > -1 && (m_currentBankTransDetailIdx < m_bankTransDetail.size())) {
        const SbanktransDetail& bankTransDetail = m_bankTransDetail.at(m_currentBankTransDetailIdx);
        //double amount = bankTransDetail.m_amount;
        QString ref = bankTransDetail.m_refID;

        QVector<StransactionData*> allTrns = CtransactionTable::Object()->getAllTransactionForRefId(ref);
        if (allTrns.isEmpty() == false) {
            addInPrevTable(allTrns);
            double total = 0;
            for (StransactionData* data : allTrns) {
                total += data->m_amount;
                delete data;
            }
            QMessageBox::StandardButton reply;
            QString msg = "Bank transaction with reference \"" + ref + "\" already process. Ignore this transaction?";
            reply = QMessageBox::question(this, "SaptuamData", msg ,
                                               QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::Yes) {
                ret = true;
            }
        }
    }
    return ret;
}

void CnewTransactionDlg::addInPrevTable(QVector<StransactionData*> &allTrns)
{
    ui->m_previousTable->clearContents();
    ui->m_previousTable->setRowCount(0);

    for (StransactionData* data : allTrns) {
        QTableWidgetItem * row = new QTableWidgetItem(); //tr("%1").arg(daterange));
        QString str =  CcontactMap::Object()->getContanceName(data->m_fromId);
        str += " " + CaccountMap::Object()->getAccountName(data->m_accountId);
        QString amount = "₹ " + QLocale(QLocale::English,QLocale::India).toString(data->m_amount, 'f', 2) ;
        str += " " + amount;
        int rowCount = ui->m_previousTable->rowCount();
        ui->m_previousTable->insertRow(rowCount);
        row->setText(str);
        ui->m_previousTable->setItem(rowCount, 0, row);


    }
}

void CnewTransactionDlg::addInPrevTable(int contactID1, int contactID2)
{
    QVector<StransactionData*> allTrns;
    if (contactID1 > 0 && (contactID1 == contactID2)) {
        QVector<StransactionData*> trans = CtransactionTable::Object()->getAllTransactionForContact(contactID1);
        for (StransactionData* data : trans) {
            if (allTrns.size() > 5) {
                delete data;
            } else {
                allTrns.push_back(data);
            }
        }
    } else {
        //bool add1 = false;
        //bool add2 = false;
        if (contactID1 > 0) {
             QVector<StransactionData*> trans = CtransactionTable::Object()->getAllTransactionForContact(contactID1);
             for (StransactionData* data : trans) {
                 if (allTrns.size() > 5) {
                     delete data;
                 } else {
                     allTrns.push_back(data);
                 }
             }
            //add1 = true;
        }
        if (contactID2 > 0) {
            int limit = allTrns.size() + 5;
            QVector<StransactionData*> allTrans2 = CtransactionTable::Object()->getAllTransactionForContact(contactID2);
            for (StransactionData* trans : allTrans2) {
                if (limit > 0) {
                    allTrns.push_back(trans);
                    --limit;
                } else {
                    delete trans;
                }
            }
        }
    }

    addInPrevTable(allTrns);
    for (StransactionData* data : allTrns) {
        delete data;
    }
}
EtransErrorType CnewTransactionDlg::checkFromToCol(QTableWidgetItem *item)
{
    EtransErrorType status = TRANS_NO_ERROR;
    if (item) {
        QString name = item->text().trimmed();
        if (!name.isEmpty()) {
            if (CcontactMap::Object()->getContactID(name)) {
                item->setBackground(QColor(Qt::white));
                //item->setBackground(QColor(Qt::white));
                QString emptyTooltip;
                item->setToolTip(emptyTooltip);
                addRefAndModeForRow(item->row());
            } else {
                item->setBackground(QColor(Qt::red));
                status = TRANS_NAME_NOT_FOUND_ERROR;
                item->setToolTip(getErrorString(status));
            }
        } else {

        }
    }
    return status;
}

EtransErrorType CnewTransactionDlg::checkAmount(QTableWidgetItem *item)
{
    EtransErrorType status = TRANS_NO_ERROR;
    if (item) {
        if(item->text().isEmpty()){
            item->setBackground(QColor(Qt::red));
            status = TRANS_AMOUNT_EMPTY_ERROR;
            item->setToolTip(getErrorString(status));
        } else {
            bool ok;
            double f = item->text().toDouble(&ok);
            if(ok == false ||  f == 0){
                item->setBackground(QColor(Qt::red));
                status = TRANS_AMOUNT_NOT_NUMBER_ERROR;
                item->setToolTip(getErrorString(status));
             } else {

                item->setBackground(QColor(Qt::white));
                QString emptyTooltip;
                item->setToolTip(emptyTooltip);
                addRefAndModeForRow(item->row());
            }
        }
    }

    if (status == TRANS_NO_ERROR && item) {
        QTableWidgetItem * fromTo = ui->m_transactionTable->item(item->row(), TRANS_FROM_TO_COL);
        if (fromTo) {
            if (fromTo->text().isEmpty()){
                fromTo->setBackground(QColor(Qt::red));
                status = TRANS_EMPTY_NAME_ERROR;
                fromTo->setToolTip(getErrorString(status));
            }
        }

    }
    return status;
}

EtransErrorType CnewTransactionDlg::checkAccount(QTableWidgetItem *item)
{
    EtransErrorType status = TRANS_NO_ERROR;
    if (item) {
        QString name = item->text().trimmed();
        if(name.isEmpty()){
            item->setBackground(QColor(Qt::red));
            status = TRANS_ACCOUNT_EMPTY_ERROR;
            item->setToolTip(getErrorString(status));

        } else {
            if (CaccountMap::Object()->getAccountID(name)) {
                    item->setBackground(QColor(Qt::white));
                    QString emptyTooltip;
                    item->setToolTip(emptyTooltip);
            } else {
                item->setBackground(QColor(Qt::red));
                status = TRANS_ACCOUNT_NOT_FOUND_ERROR;
                item->setToolTip(getErrorString(status));

            }
        }
    }
    return status;
}

EtransErrorType CnewTransactionDlg::checkMode(QTableWidgetItem *item)
{
    EtransErrorType status = TRANS_NO_ERROR;
    if (item) {
        if(item->text().isEmpty()){
            item->setBackground(QColor(Qt::red));
            status = TRANS_MODE_EMPTY_ERROR;
            item->setToolTip(getErrorString(status));

        } else {
            int row = item->row();
            QTableWidgetItem* account = ui->m_transactionTable->item(row, TRANS_ACCOUNT_COL);
            if (account) {
                QString mode = item->text();
                bool isBankMode = false;
                if (mode == "Bank Cash Deposit" ||
                        mode == "Bank Charges" ||
                        mode == "Bank Interest") {
                    isBankMode = true;
                }

                QString accountName = account->text();
                int accountType = CaccountMap::Object()->getAccountType(accountName);
                if (BANK_ACCOUNT_TYPE == accountType) {
                    if (isBankMode == false) {
                        status = TRANS_MODE_AND_ACCOUNT_TYPE_MISMATCH;
                    }
                } else {
                    if (isBankMode) {
                        status =TRANS_MODE_AND_ACCOUNT_TYPE_MISMATCH;
                    }
                }
            }
            if (status == TRANS_NO_ERROR) {
                item->setBackground(QColor(Qt::white));
                QString emptyTooltip;
                item->setToolTip(emptyTooltip);

            } else {
                item->setBackground(QColor(Qt::red));
                item->setToolTip(getErrorString(status));
            }

        }
    }
    return status;
}

EtransErrorType CnewTransactionDlg::checkRef(QTableWidgetItem *item)
{
    EtransErrorType status = TRANS_NO_ERROR;
    bool isModeRequireRef = false;
    if (item) {
        int row = item->row();
        isModeRequireRef = getIsModeRequireRef(row);
    }

    if (item == nullptr && isModeRequireRef) {
        status = TRANS_REF_EMPTY_ERROR;

    } else if (item) {

        if (item->text().trimmed().isEmpty() == false) {
            status = TRANS_NO_ERROR;
            QString emptyTooltip;
            item->setToolTip(emptyTooltip);
        } else {
            status = TRANS_NO_ERROR;
            if (isModeRequireRef) {

                status = TRANS_REF_EMPTY_ERROR;
                item->setBackground(QColor(Qt::red));
                item->setToolTip(getErrorString(status));
            }
        }
    }
    if (status == TRANS_NO_ERROR && item) {

        item->setBackground(QColor(Qt::white));
        QString emptyTooltip;
        item->setToolTip(emptyTooltip);


    }
    return status;
}

EtransErrorType CnewTransactionDlg::checkType(QTableWidgetItem *item)
{
    if (item == nullptr || item->text().trimmed().isEmpty()){
        return TRANS_TYPE_EMPTY_ERROR;
    }

    if (m_bankImport) {
        EtransactionType type = CtransactionUtils::Object()->getTransactionType(item->text().trimmed());
        bool isIncome = CtransactionUtils::Object()->getIsTransactionIncome(type);
        bool isPayment = CtransactionUtils::Object()->getIsTransactionPayment(type);
        SbanktransDetail bankTrans = m_bankTransDetail.at(m_currentBankTransDetailIdx);
        if (isIncome && isPayment) {
            Q_ASSERT(0);
            /*if (bankTrans.m_type != BANK_CASH_DEPOSIT) {
                return TRANS_BANK_AND_ENTRY_MISMATCH;
            }*/
        }
        if (bankTrans.m_isIncome && isPayment) {
            return TRANS_PAYMENT_ACCOUNT_FOR_INCOME;
        }

        if (isIncome && !bankTrans.m_isIncome) {
            return TRANS_INCOME_ACCOUNT_FOR_PAYMENT;
        }

        EbankTransType bankTransType = bankTrans.m_type;
        if (!(type == BANK_CASH_DEPOSIT_TRANSACTION_TYPE && bankTransType == NOTASIGN)) {
            if ((bankTransType == BANK_CASH_DEPOSIT && type != BANK_CASH_DEPOSIT_TRANSACTION_TYPE) ||
                (bankTransType != BANK_CASH_DEPOSIT && type == BANK_CASH_DEPOSIT_TRANSACTION_TYPE))  {
                return TRANS_TYPE_MISMATCH;
            }
        }
        if ((bankTransType == BANK_CHARGES && type != BANK_CHARGES_TRANSACTION_TYPE) ||
             (bankTransType != BANK_CHARGES && type == BANK_CHARGES_TRANSACTION_TYPE)) {
            return TRANS_TYPE_MISMATCH;

        }
        if ((bankTransType == BANK_INTEREST && type != BANK_INTEREST_TRANSACTION_TYPE) ||
                (bankTransType != BANK_INTEREST && type == BANK_INTEREST_TRANSACTION_TYPE)){
            return TRANS_TYPE_MISMATCH;

        }



    }

    return TRANS_NO_ERROR;
}

bool CnewTransactionDlg::getIsModeRequireRef(int row)
{
    bool isModeRequireRef = false;
    QTableWidgetItem * mode = ui->m_transactionTable->item(row, TRANS_MODE_COL);
    if (mode) {
        //list<<"Cash"<<"Cheque"<<"Internet Transfer"<<"Bank Cash Deposit"<<"Bank Charges"<<"Bank Interest"<<"Contra";
        QString name = mode->text().trimmed();
        isModeRequireRef = CtransactionUtils::Object()->isModeRequireRef(name);
    }
    return isModeRequireRef;
}

void CnewTransactionDlg::resizeEvent(QResizeEvent *)
{
    resize();
}

void CnewTransactionDlg::on_m_cancelButton_clicked()
{
   emit reject();
}

void CnewTransactionDlg::on_m_okButton_clicked()
{
   m_calcTotalCall = false;
    EtransErrorType error = calcTotal();
    if (error != TRANS_NO_ERROR) {
        QString msg = getErrorString(error);
        QMessageBox::warning(this, "SaptuamData",
                             msg);
        return;
    }
    int rowCount = m_allRowData.size();

    QSet<int> fromIDSet;
    for (int i = 0; i < rowCount; ++i) {
        const QVector<QVariant>& rowData = m_allRowData.at(i);
        StransactionData transData;


        transData.m_id = 0;
        int fromID = CcontactMap::Object()->getContactID(rowData.at(TRANS_FROM_TO_COL).toString());
        transData.m_fromId = fromID;
        fromIDSet.insert(fromID);
        transData.m_amount = rowData.at(TRANS_AMOUNT_COL).toDouble();
        transData.m_accountId = CaccountMap::Object()->getAccountID(rowData.at(TRANS_ACCOUNT_COL).toString());
        transData.m_date = ui->m_date->date();
        transData.m_type = CtransactionUtils::Object()->getTransactionType(rowData.at(TRANS_TYPE_COL).toString());
        transData.m_mode = rowData.at(TRANS_MODE_COL).toInt();
        transData.m_ref = rowData.at(TRANS_REF_COL).toString();
        transData.m_remittanceID = 0;
        transData.m_parentID = 0;
        transData.m_closingID = 0;
        transData.m_bankID = 0;
        transData.m_status = true;
        transData.m_reeiptBook = "";
        int totalTransactin = CtransactionTable::Object()->totalTransaction(CaccountMap::Object()->getAccountID(
                                                                                rowData.at(TRANS_ACCOUNT_COL).toString()));

        transData.m_receiptSlNo = CsdUtils::abbravate(rowData.at(TRANS_ACCOUNT_COL).toString()) + QString::number(totalTransactin);
        transData.m_particular = rowData.at(TRANS_PARTICULAR_REMARK_COL).toString();
        transData.save();

    }
    if (m_bankImport) {
        SbanktransDetail bankTrans = m_bankTransDetail.at(m_currentBankTransDetailIdx);
        QString bankName = bankTrans.m_min;
        if (bankName.isEmpty() == false) {
            addBankName(fromIDSet, bankName);
        }
        moveToNextImport();

    }
    if ((m_bankImport == false || (m_bankTransDetail.size() <= m_currentBankTransDetailIdx) || (m_currentBankTransDetailIdx ==-1))) {
        emit accept();
    }
    if (m_bankImport) {
        ui->m_transactionTable->clearContents();
        ui->m_transactionTable->setRowCount(0);

        for (int i =0 ; i < 10; ++i) {
            int row =  ui->m_transactionTable->rowCount();
            ui->m_transactionTable->insertRow(row);
        }
    }
}

void CnewTransactionDlg::on_m_transactionTable_itemChanged(QTableWidgetItem *item)
{
    int col = item->column();
    int row = item->row();

    if(row == (ui->m_transactionTable->rowCount() -2)){
        ui->m_transactionTable->insertRow(row + 2);
        ui->m_transactionTable->insertRow(row + 3);
        ui->m_transactionTable->scrollToBottom();
    }

    if (col == TRANS_FROM_TO_COL) {
        checkFromToCol(item);
        return;
    }

    if (col == TRANS_AMOUNT_COL) {
        checkAmount(item);
        calcTotal();
        return;
    }
    if (col == TRANS_TYPE_COL) {
        addRefAndModeForRow(item->row());

    }
    if (col == TRANS_ACCOUNT_COL) {
        checkAccount(item);

        calcTotal();
        return;
    }

    if (col == TRANS_MODE_COL) {
        checkMode(item);
        return;
    }

    if (col == TRANS_REF_COL) {
        if (m_disableCheckRef == false) {
            checkRef(item);

            calcTotal();
        }
        return;
    }
}

void CnewTransactionDlg::on_m_transactionTable_customContextMenuRequested(const QPoint &pos)
{
    m_rowNumber = -1;
    QModelIndex index =  ui->m_transactionTable->indexAt(pos);
    if(index.isValid()){
        m_rowNumber = index.row();
        QMenu *menu=new QMenu(this);
        menu->addAction(m_deleteRow);
        menu->addAction(m_deleteAllRow);
        menu->popup(ui->m_transactionTable->viewport()->mapToGlobal(pos));
    }
}




void CnewTransactionDlg::on_m_bankImportTable_itemClicked(QTableWidgetItem *item)
{
   if (item) {
      if (item->checkState() == Qt::Checked) {
          int row = item->row();
          m_importID = m_importTableId.at(row);
          populateBankTransDetail();
      }
   }
}


void CnewTransactionDlg::on_m_skipBtn_clicked()
{
    moveToNextImport(true);
}


void CnewTransactionDlg::on_m_rejectBtn_clicked()
{
   moveToNextImport(false);
}

