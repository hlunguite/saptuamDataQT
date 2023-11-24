#include "cimporttransdlg.h"
#include "ui_cimporttransdlg.h"
#include "csdutils.h"
#include <QFileDialog>
#include "cimporttransactiontable.h"
#include "ctransactionutils.h"
#include "cimportmintable.h"
#include "cimportaccounttable.h"
#include "ccontactmap.h"
#include "caccountmap.h"
#include "cedittransactiondlg.h"
#include "creceiptbookmap.h"
CimportTransDlg::CimportTransDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CimportTransDlg)
{
    ui->setupUi(this);
    m_data = nullptr;
    resize();


    QVector<SimportMinData *> *  mins = CimportMinTable::Object()->getImportedMin(false);
    if (mins) {
        for (auto min: *mins) {
            //qDebug()<<"Mappint "<<min->m_firstName<<" "<<CcontactMap::Object()->getContanceName(min->m_contactTableIdx)<<"\n";
            m_contactMap[min->m_importIdx] = min->m_contactTableIdx;
            QString comment = min->m_transRemark;
            if (comment.isEmpty() == false) {
                m_contactComment[min->m_importIdx] = comment;
            }
            delete min;

        }
        delete mins;

    }
    QVector<SimportAccountData*>* accounts = CimportAccountTable::Object()->getImportAccount(false);
    if (accounts) {
        for (auto account:*accounts) {
            int importid = account->m_importIdx;
            int accountID = account->m_accountID;
            m_accountMap[importid] = accountID;
        }
        delete accounts;
    }
    ui->m_progressBar->hide();
    ui->m_progressBar->setRange(0, 100);
    ui->m_progressBar->setOrientation(Qt::Horizontal);
    populateTable();
}

CimportTransDlg::~CimportTransDlg()
{
    clear();
    delete ui;
}

void CimportTransDlg::resize()
{
    int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();

    int x = ui->m_table->geometry().x();
    int y = ui->m_table->geometry().y();
    int buttonheight = ui->m_processBtn->geometry().height();
    int buttonwidth = ui->m_processBtn->geometry().width();

    int width = windowwidth - x - x;
    int height = windowheight - y - 3*buttonheight;
    ui->m_table->setGeometry(x, y, width, height);
    ui->m_table->horizontalHeader()->resizeSection(0,width*.10);
    ui->m_table->horizontalHeader()->resizeSection(1,width*.20);
    ui->m_table->horizontalHeader()->resizeSection(2,width*.10);
    ui->m_table->horizontalHeader()->resizeSection(3,width*.10);
    ui->m_table->horizontalHeader()->resizeSection(4,width*.10);
    ui->m_table->horizontalHeader()->resizeSection(5,width*.10);
    ui->m_table->horizontalHeader()->resizeSection(6,width*.10);
    ui->m_table->horizontalHeader()->resizeSection(7,width*.10);
    ui->m_table->horizontalHeader()->resizeSection(8,width*.10);



    y = y + height + (1.5* buttonheight);
    //ui->m_progressBar->setGeometry(x, y, width*0.4, buttonheight);
    x = windowwidth/2 - buttonwidth - 1;
    ui->m_processBtn->setGeometry(x, y, buttonwidth, buttonheight);
    x += buttonwidth + 2;
    ui->m_cancelBtn->setGeometry(x, y, buttonwidth, buttonheight);
}

void CimportTransDlg::populateTable()
{
    clear();
    ui->m_progressBar->show();
    ui->m_progressBar->setValue(0);
    m_data = CimportTransactionTable::Object()->getImportedTransaction();
    std::map<int, std::set<int> > receiptBooks;
    if (m_data) {
        int size = m_data->size();
        for (int idx = 0; idx < size; ++idx) {
            int value = ((float)(idx+1)/float(size))*100;
            ui->m_progressBar->setValue(value);
            SimportTransData* data = m_data->at(idx);
            QVector<QTableWidgetItem*> cells(9);
            cells[0] = new QTableWidgetItem(tr("%1").arg(data->m_date.toString("d MMM yyyy")));
            cells[1] = new QTableWidgetItem(tr("%1").arg(data->m_from));
            cells[2] = new QTableWidgetItem(tr("%1").arg(data->m_account));
            QString amountStr = CsdUtils::convertAmountToStringWithSign(data->m_amount);
            cells[3] = new QTableWidgetItem(tr("%1").arg(amountStr));
            cells[4] = new QTableWidgetItem(tr("%1").arg(CtransactionUtils::Object()->getTransactionModeStr((EtransactionMode)data->m_mode)));
            cells[5] = new QTableWidgetItem(tr("%1").arg(data->m_ref));
            cells[6] = new QTableWidgetItem(tr("%1").arg(data->m_reciptBook));
            cells[7] = new QTableWidgetItem(tr("%1").arg(data->m_receiptNo));
            cells[8] = new QTableWidgetItem(tr("%1").arg(QString::number(data->m_transID)));
            if (data->m_reciptBook.isEmpty() == false) {
                int book = data->m_reciptBook.toInt();
                int sl = data->m_receiptNo.toInt();
                if (book > 0 && sl > 0) {
                    receiptBooks[book].insert(sl);
                }
            }

            int row = ui->m_table->rowCount();
            ui->m_table->insertRow(row);
            for (int i = 0; i < 9; ++i) {
                ui->m_table->setItem(row, i, cells[i]);
            }

        }

    }

    for (auto receiptBook : receiptBooks){
        int bookNo = receiptBook.first;
        std::set<int>& slNumbers = receiptBook.second;
        SreceiptBook book = CreceiptBookTable::Object()->getReciptBookByBookNo(bookNo);
        if (book.isValid() && book.m_id) {
            unsigned int count = book.m_receiptNo.size();
            for (auto no : slNumbers) {
                book.m_receiptNo.insert(no);
            }
            if (count != book.m_receiptNo.size()) {
                book.m_startNo = *book.m_receiptNo.begin();
                book.m_endNo = *book.m_receiptNo.rbegin();
                book.m_available = book.m_receiptNo.size();
                book.save();
            }

        } else {
            book.m_bookNo = bookNo;
            for (auto no : slNumbers) {
                book.m_receiptNo.insert(no);
            }
            book.m_startNo = *book.m_receiptNo.begin();
            book.m_endNo = *book.m_receiptNo.rbegin();
            book.m_available = book.m_receiptNo.size();
            book.save();
        }


    }


    ui->m_progressBar->hide();
}

void CimportTransDlg::clear()
{
    if (m_data) {
        for (auto data : *m_data){
            delete data;
        }
        delete m_data;
        m_data = nullptr;
    }
}

void CimportTransDlg::on_m_selectBtn_clicked()
{
    QString currentDirectory = CsdUtils::getCurrentWdir();
    QString fileName =  QFileDialog::getOpenFileName(nullptr,
                                                    "Open min list", currentDirectory,
                                                    "SaptuamData files (*.csv)");
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        //QString error = "Not able to open file " + fileName;
        //message(this,error);
        return ;
    }
    QVector<QString> lines;
    QTextStream in(&file);
    ui->m_progressBar->show();
    QString line ;
    while (!in.atEnd()) {
        line = in.readLine();
        if (line.contains("fromid")) {
            continue;
        }
        lines.push_back(line);
    }
    file.close();
    int no = 0;
    int size = lines.size();
    //qDebug()<<"size is "<<size;
    ui->m_progressBar->setValue(0);
    for(auto line: lines){
        ++no;
        int value = ((float)no/(float)size)*100;
        ui->m_progressBar->setValue(value);
        QRegularExpression rx("(\\,)");
        QStringList query = line.split(rx);
        if (query.size() < 14) {
            continue;
        }
        QString fullString;
        SimportTransData data;
        QString dateStr = query.at(0);
        fullString += dateStr;
        data.m_idx = 0;
        QDate date = QDate::fromString(dateStr, "dd MMM yyyy");
        data.m_date = date;//QDate(date.year() + 100, date.month(), date.day());
        data.m_from = query.at(1);
        data.m_fromID = query.at(2).toInt();
        fullString += query.at(2);
        data.m_account = query.at(5);
        data.m_accountID = query.at(6).toInt();
        fullString += query.at(6);
        QString amount = query.at(7);
        if (amount.isEmpty()) {
            amount = query.at(8);
        }
        fullString += amount;
        data.m_amount = amount.toDouble();
        /*    m_modeModeTwoTypeStrMap.insert(CASH_TRANSACTION_MODE, "Cash");
    m_modeModeTwoTypeStrMap.insert(CHEQUE_TRANSACTION_MODE, "Bank");
    m_modeModeTwoTypeStrMap.insert(INTERNET_TRANSACTION_MODE, "Bank");
    m_modeModeTwoTypeStrMap.insert(BANK_CASH_DEPOSIT_TRANSACTION_MODE, "Bank");
    m_modeModeTwoTypeStrMap.insert(BANK_CHARGES_TRANSACTION_MODE, "Bank");
    m_modeModeTwoTypeStrMap.insert(BANK_INTEREST_TRANSACTION_MODE, "Bank");*/
        QString mode = query.at(9);
        if (mode.contains("cash", Qt::CaseInsensitive)) {
            data.m_mode = CASH_TRANSACTION_MODE;
        } else if (mode.contains("cheque", Qt::CaseInsensitive)) {
            data.m_mode = CHEQUE_TRANSACTION_MODE;
        } else if (mode.contains("internet", Qt::CaseInsensitive)) {
            data.m_mode = INTERNET_TRANSACTION_MODE;
        } else {
            data.m_mode = CASH_TRANSACTION_MODE;
        }
        fullString += mode;
        data.m_ref = query.at(10);
        fullString += data.m_ref;
        data.m_reciptBook = query.at(11);
        data.m_receiptNo = query.at(12);
        fullString += data.m_reciptBook + data.m_receiptNo;
        data.m_remark = query.at(13);
        data.m_transID = 0;
        data.m_fullString = fullString;
        data.save();
        //qDebug()<<data.m_date.toString("d MMM yyyy")<<" "<<data.m_from<<" "<<data.m_account<<"  "<<data.m_amount<<" "<<data.m_mode<<" "<<data.m_ref<<" "<<data.m_reciptBook<<" "<<data.m_receiptNo<<" "<<data.m_remark;
        /*auto accountFn = m_accountMap.find(data.m_accountID);
        auto contactFn = m_contactMap.find(data.m_fromID);
        if ((accountFn != m_accountMap.end()) && (contactFn != m_contactMap.end())) {
            qDebug()<<data.m_date.toString("d MMM yyyy")<<data.m_amount<<" "<<data.m_from<<" "
                     <<CcontactMap::Object()->getContanceName(contactFn->second)<<" "<<data.m_account<<" "
                     <<CaccountMap::Object()->getAccountName(accountFn->second);
        } else {
            qDebug()<<"not found";
        }*/

        //auto comment = m_contactComment.find(data.m_fromID);
    }

    ui->m_progressBar->hide();
    populateTable();
}


void CimportTransDlg::on_m_processBtn_clicked()
{
    populateTable();
    int idx = 0;
    int size = m_data->size();
    ui->m_progressBar->show();

    while(idx < size) {
        SimportTransData* data = m_data->at(idx);
        int value = ((float)idx/float(size))*100;
        ui->m_progressBar->setValue(value);

        QTableWidgetItem* firstNameItem = ui->m_table->item(idx, 2);
        if (firstNameItem) {
            ui->m_table->scrollToItem(firstNameItem);
        }

        ++idx;

        StransactionData* transData  = new StransactionData;
        transData->m_id = 0;
        auto accountFn = m_accountMap.find(data->m_accountID);
        auto contactFn = m_contactMap.find(data->m_fromID);
        if (accountFn == m_accountMap.end() || contactFn == m_contactMap.end()) {
            continue;
        }
        auto comment = m_contactComment.find(data->m_fromID);


        transData->m_fromId = contactFn->second;
        transData->m_amount = data->m_amount;
        transData->m_accountId = accountFn->second;
        transData->m_date = data->m_date;
        transData->m_mode = data->m_mode;
        transData->m_ref = data->m_ref;
        transData->m_remittanceID = 0;
        transData->m_parentID = 0;
        transData->m_closingID = 0;
        transData->m_bankID = 0;
        transData->m_status = 1;
        transData->m_reeiptBook = data->m_reciptBook;
        transData->m_receiptSlNo = data->m_receiptNo;
        transData->m_particular = data->m_remark;
        if (comment != m_contactComment.end()) {
            if (transData->m_particular.isEmpty() == false) {
                transData->m_particular += " ";
            }
            transData->m_particular += comment->second;
        }

        QString name = CcontactMap::Object()->getContanceName(transData->m_fromId);
        QString accountName = CaccountMap::Object()->getAccountName(transData->m_accountId);
        int accountType = CaccountMap::Object()->getAccountType(accountName);
        int type = INVALID_TRANSACTION_TYPE;
        bool toCheck = false;
        if (accountType == INCOME_ACCOUNT_TYPE || accountType == REQUEST_ACCOUNT_TYPE) {
            type = INCOME_TRANSACTION_TYPE;
        } else if (accountType == PAYMENT_ACCOUNT_TYPE || accountType == REMITTANCE_ACCOUNT_TYPE) {
            type = PAYMENT_TRANSACTION_TYPE;
        } else if (accountType == BANK_ACCOUNT_TYPE) {
            toCheck = true;
        } else if (accountType == LOAN_ACCOUNT_TYPE) {
            toCheck = true;
        }
        if (name.contains("bank", Qt::CaseInsensitive) || accountName.contains("bank", Qt::CaseInsensitive)) {
            toCheck = true;
        }
        if (accountType == REQUEST_ACCOUNT_TYPE || accountType == REMITTANCE_ACCOUNT_TYPE) {
            toCheck = true;
        }
   /*
        trans type
    INVALID_TRANSACTION_TYPE = 0,
    INCOME_TRANSACTION_TYPE, //INCOME_ACCOUNT_TYPE, REQUEST_ACCOUNT_TYPE
    PAYMENT_TRANSACTION_TYPE, //PAYMENT_ACCOUNT_TYPE , REMITTANCE_ACCOUNT_TYPE
    LOAN_RECOVERY_TRANSACTION_TYPE, //LOAN_ACCOUNT_TYPE,
    LOAN_DISBURSE_TRANSACTION_TYPE, // LOAN_ACCOUNT_TYPE
    BANK_CHARGES_TRANSACTION_TYPE, //BANK_ACCOUNT_TYPE
    BANK_INTEREST_TRANSACTION_TYPE, // BANK_ACCOUNT_TYPE
    BANK_CASH_DEPOSIT_TRANSACTION_TYPE, // BANK_ACCOUNT_TYPE
*/
        //transData->m_mode = data->m_mode;
        int accountID = transData->m_accountId;
        int mode = data->m_mode;
        if (name == gBankCashDepositName) {
            type = BANK_CASH_DEPOSIT_TRANSACTION_TYPE;
            mode = BANK_CASH_DEPOSIT_TRANSACTION_MODE;
            accountID = CaccountMap::Object()->getAccountID(gBankAccountName);
        }
        if (name == gBankChargesName) {
            type = BANK_CHARGES_TRANSACTION_TYPE;
            mode = BANK_CHARGES_TRANSACTION_MODE;
            accountID = CaccountMap::Object()->getAccountID(gBankAccountName);

        }
        if (name == gBankInterestName) {
            type = BANK_INTEREST_TRANSACTION_TYPE;
            mode = BANK_INTEREST_TRANSACTION_MODE;
            accountID = CaccountMap::Object()->getAccountID(gBankAccountName);

        }
        if (type == INVALID_TRANSACTION_TYPE) {
            toCheck = true;
            type = INCOME_TRANSACTION_TYPE;
        }
        if (transData->m_amount > 100000) {
            toCheck = true;
        }
        if (accountID) {
            transData->m_accountId = accountID;
        }
        transData->m_type = type;
        transData->m_mode = mode;




        CeditTransactionDlg dlg;
        dlg.setForImport(transData);
        if (toCheck == false) {
            if (dlg.checkValue() == false) {
                toCheck = true;
                //qDebug()<<name <<" "<<accountName<<" "<<transData->m_amount;
            } else {
                if (dlg.checkAndSave()) {
                    data->m_transID = dlg.newTransID();
                    data->save();
                     QTableWidgetItem* contactTableID = ui->m_table->item(idx -1, 8);
                    if (contactTableID) {
                         contactTableID->setText(QString::number(data->m_transID));
                     }
                    emit updateTransaction();
                } else {
                    toCheck = true;
                }

            }
        }
        if (toCheck) {
            dlg.setWindowTitle("Import Transaction");
            if (dlg.exec()) {
                data->m_transID = dlg.newTransID();
                data->save();
                QTableWidgetItem* contactTableID = ui->m_table->item(idx -1, 9);
                if (contactTableID) {
                    contactTableID->setText(QString::number(data->m_transID));
                }
                emit updateTransaction();
            }else {
                break;
            }
        }

    }
    ui->m_progressBar->hide();

}


void CimportTransDlg::on_m_cancelBtn_clicked()
{
    emit reject();
}

