#include "cassignreceiptdlg.h"
#include "ui_cassignreceiptdlg.h"
#include "ctransactiontable.h"
#include "caccountmap.h"
//#include "caccountreceiptbook.h"
#include "creceiptbookmap.h"
#include <set>
#include "MISC/CdlgDefine.h"
CassignReceiptDlg::CassignReceiptDlg(QWidget *parent) :
    CdlgBase(parent),
    ui(new Ui::CassignReceiptDlg), m_transationTableModel(nullptr)
{
    ui->setupUi(this);
    QDate date = QDate::currentDate();
    ui->m_startDate->setDate(date);
    ui->m_endDate->setDate(date);
    m_transationTableModel = new CtransactionTableModel(this, CtransactionTable::Object()->getDataBase());
    m_transationTableModel->setTable(CtransactionTable::Object()->getTableName());
    m_transationTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //m_transationTableModel->setFilter(m_filter);
    //m_transationTableModel->select();
    m_transationTableModel->sort(TRANSACTION_ACCOUNT_IDX,Qt::AscendingOrder);
    m_transationTableModel->setColEditable(TRANSACTION_RECTIPT_BOOK_IDX);
    m_transationTableModel->setColEditable(TRANSACTION_RECTIPT_NO_IDX);

    ui->m_transTable->setModel(m_transationTableModel);
    ui->m_transTable->setSortingEnabled(false);
    ui->m_transTable->horizontalHeader()->setStretchLastSection(true);
    ui->m_transTable->verticalHeader()->hide();
    //ui->m_transTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->m_transTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->m_transTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->m_transTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    const QVector<QString> & header  = CtransactionTable::Object()->getTableHeader();
    int headerCount = header.size();

    for (int j = 0; j < headerCount; ++j){
        if (CtransactionTable::Object()->getIsHeaderHidden(j)) {
            ui->m_transTable->hideColumn(j);
        } else if (j == TRANSACTION_FROM_IDX ||
                 j == TRANSACTION_ACCOUNT_IDX ||
                 j == TRANSACTION_AMOUNT_IDX ||
                 j == TRANSACTION_RECTIPT_BOOK_IDX ||
                 j == TRANSACTION_RECTIPT_NO_IDX) {
            m_transationTableModel->setHeaderData(j, Qt::Horizontal, header.at(j));
        } else {
            ui->m_transTable->hideColumn(j);
        }
    }
    ui->m_departmentComboBox->addItem("");
    ui->m_departmentComboBox->addItems(CaccountMap::Object()->getDeptList());
    ui->m_accountComboBox->clear();
    populateDeptAccountReceiptDetail();
    populateReceiptTable();
    populateTransTable();
    resize();
}

CassignReceiptDlg::~CassignReceiptDlg()
{
    delete m_transationTableModel;
    delete ui;
}

void CassignReceiptDlg::on_m_cancelBtn_clicked()
{
   emit reject();
}


void CassignReceiptDlg::on_m_okBtn_clicked()
{
    if (checkTransTable()) {

        emit accept();
    }
}

void CassignReceiptDlg::resize()
{
    int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();
    int x = XCORD;//ui->m_startDateLbl->geometry().x();
    int y = YCORD;//ui->m_startDateLbl->geometry().x();
    ui->m_startDateLbl->setGeometry(x, y, DATE_SIZE.width(), DATE_SIZE.height());
    ui->m_startDate->setGeometry(x, y + GAP + DATE_SIZE.height(), DATE_SIZE.width(), DATE_SIZE.height());
    x = x + GAP + DATE_SIZE.width();
    ui->m_endDateLbl->setGeometry(x, y, DATE_SIZE.width(), DATE_SIZE.height());
    ui->m_endDate->setGeometry(x, y + GAP + DATE_SIZE.height(), DATE_SIZE.width(), DATE_SIZE.height());
    x = x + GAP + DATE_SIZE.width();
    ui->m_departmentComboBox->setGeometry(x, y, DEPT_ACCOUNT_COMBO_SIZE.width(), DEPT_ACCOUNT_COMBO_SIZE.height() );
    ui->m_accountComboBox->setGeometry(x, y + GAP + DEPT_ACCOUNT_COMBO_SIZE.height(),
                                       DEPT_ACCOUNT_COMBO_SIZE.width(),
                                       DEPT_ACCOUNT_COMBO_SIZE.height());
    x = x + GAP + DEPT_ACCOUNT_COMBO_SIZE.width();
    int y1 = y + DEPT_ACCOUNT_COMBO_SIZE.height()/2;
    ui->m_autoFillBtn->setGeometry(x,
                                   y1,
                                   BUTTON_SIZE.width(),
                                   BUTTON_SIZE.height());
    x +=  BUTTON_SIZE.width() + GAP;
    int width = BUTTON_SIZE.width()*2;
    ui->m_sameReceiptAccount->setGeometry(x, y1, width, DEFAULT_HEIGHT);

    y = y + GAP + DEPT_ACCOUNT_COMBO_SIZE.height() + GAP + DEPT_ACCOUNT_COMBO_SIZE.height();
    x =  ui->m_startDateLbl->geometry().x();


    width = windowwidth*.25;//ui->m_availableReceiptTable->geometry().width();
    
    
    int height = windowheight - y - (2*DEFAULT_HEIGHT);

    ui->m_availableReceiptTable->setGeometry(x, y, width, height);
    ui->m_availableReceiptTable->horizontalHeader()->resizeSection(0, width);
    int xx = x;
    x = x + width;
    width = windowwidth - width - 2*xx;
    ui->m_transTable->setGeometry(x, y, width, height);
    ui->m_transTable->horizontalHeader()->resizeSection(TRANSACTION_FROM_IDX,0.3*width);
    ui->m_transTable->horizontalHeader()->resizeSection(TRANSACTION_ACCOUNT_IDX,0.2*width);
    ui->m_transTable->horizontalHeader()->resizeSection(TRANSACTION_AMOUNT_IDX,0.2*width);
    ui->m_transTable->horizontalHeader()->resizeSection(TRANSACTION_RECTIPT_BOOK_IDX,0.15*width);
    ui->m_transTable->horizontalHeader()->resizeSection(TRANSACTION_RECTIPT_NO_IDX,0.15*width);

    //x = ui->m_autoFillBtn->geometry().x();
    //y = ui->m_autoFillBtn->geometry().y();

    int buttonheight = BUTTON_SIZE.height();
    int buttonwidth = BUTTON_SIZE.width();
    y = y + height + buttonheight/2;
    x = windowwidth/2 - buttonwidth - buttonwidth;

    ui->m_clearBtn->setGeometry(x, y, buttonwidth, buttonheight);
    x = x + buttonwidth+GAP;

    ui->m_cancelBtn->setGeometry(x, y, buttonwidth, buttonheight);
    x = x + buttonwidth+GAP;
    ui->m_okBtn->setGeometry(x, y, buttonwidth, buttonheight);



}

void CassignReceiptDlg::populateDeptAccountReceiptDetail()
{
     const QStringList& accounts = CaccountMap::Object()->getAccountListWithHqReceipt();
     for (const QString& account: accounts) {
         int accountID = CaccountMap::Object()->getAccountID(account);
         int deptID = CaccountMap::Object()->getDeptForAccount(accountID);
         QString dept = CaccountMap::Object()->getDeptName(deptID);
         QStringList& deptAcc = m_departmentAccount[dept];
         deptAcc.push_back(account);
         deptAcc.sort();
     }
     QVector<SaccountReceipt> accountReceipt =  CreceiptBookMap::Object()->getAllAccountReceipt() ;
     std::map<int, std::set<int>> receiptAccounts;
     for (SaccountReceipt& accReceipt: accountReceipt ) {
         std::set<int>& receipts = m_accountReceipts[accReceipt.m_accountTableID];
         receipts.insert(accReceipt.m_receiptBoookTableID);
         std::set<int>& accountWithSameReceipt = receiptAccounts[accReceipt.m_receiptBoookTableID];//m_accountWithSameReceipt[accReceipt.m_receiptBoookTableID];
         accountWithSameReceipt.insert(accReceipt.m_accountTableID);
     }

     std::set<std::set<int> > accountGroup;
     for (auto receiptAccount: receiptAccounts) {
         accountGroup.insert(receiptAccount.second);
     }

     for (auto group: accountGroup) {
         //qDebug()<<"Account with same receipts:";
         std::set<int> accounts(group);
         //check if all account have the same receipt;
         std::set<int> receipts;
         bool first = true;
         bool isIgnore = false;
         for (auto account: group) {
             //qDebug()<<CaccountMap::Object()->getAccountName(account);
             std::set<int>& accountReceipt = m_accountReceipts[account];
             if (first) {
                 receipts.insert(accountReceipt.begin(), accountReceipt.end());
                 first = false;
             } else if (receipts != accountReceipt){
                 isIgnore = true;
             }
         }
         if (isIgnore == false) {
             for (auto account: group) {
                 m_accountWithSameReceipt[account] = accounts;
             }
         }
     }

}

void CassignReceiptDlg::populateReceiptTable()
{
     CreceiptBookMap::Object()->populate();
     ui->m_availableReceiptTable->setRowCount(0);
     m_currentReceiptBooks.clear();
     //get all acccount id
     QSet<int> accountIDs;
     QString accountName = ui->m_accountComboBox->currentText();
     //const std::set<int>& accountWithHq = CaccountMap::Object()->getAccountIDListWithHqReceipt();

     if (accountName.isEmpty() == false) {
         int accountID = CaccountMap::Object()->getAccountID(accountName);
         accountIDs.insert(accountID);
         //qDebug()<<"select account "<<accountName;
         bool populateAccountWithSameReceipt = true;
         if (ui->m_sameReceiptAccount->isChecked() == false) {
             populateAccountWithSameReceipt = false;
         }
         if (populateAccountWithSameReceipt == true) {
             const std::set<int> accountWithSameReceipt = m_accountWithSameReceipt[accountID];
             for (auto id: accountWithSameReceipt) {
                 accountIDs.insert(id);
             }
         }
     } else {
         QString deptName = ui->m_departmentComboBox->currentText();
         QMap<QString, QStringList>::iterator fn = m_departmentAccount.find(deptName);
         if (fn != m_departmentAccount.end()) {
             std::map<int, std::set<int> > receiptForAccount;
            for (QString & accountName: fn.value()) {
                int accountID = CaccountMap::Object()->getAccountID(accountName);
                auto fn = m_accountReceipts.find(accountID);
                if (fn != m_accountReceipts.end()) {
                    for (const int & receiptTableID : fn.value()) {
                        receiptForAccount[receiptTableID].insert(accountID);
                    }
                }
                accountIDs.insert(accountID);
            }
            if (receiptForAccount.size() > 1) {
                std::set<int> accounts;
                bool selectAccount = false;
                for (auto receiptacc: receiptForAccount) {
                    if (accounts.empty()) {
                        accounts.insert(receiptacc.second.begin(), receiptacc.second.end());
                    } else {
                        if (accounts != receiptacc.second) {
                            selectAccount = true;
                        }
                    }
                }

                if (selectAccount) {
                    accountIDs.clear();
                }
            }
         }
     }
     // get receipt for accounts
     QSet<int> alreadyAddReceipt;
     for (int accountID : accountIDs) {
         auto fn = m_accountReceipts.find(accountID);
         if (fn != m_accountReceipts.end()) {
            for (const int & receiptTableID : fn.value()) {
                if (alreadyAddReceipt.find(receiptTableID) == alreadyAddReceipt.end()) {
                    alreadyAddReceipt.insert(receiptTableID);
                    SreceiptBook receiptBook = CreceiptBookMap::Object()->getReceiptBookFromID(receiptTableID);
                    if (receiptBook.isValid() == false) {
                        continue;
                    }

                    m_currentReceiptBooks.push_back(receiptBook.m_bookNo);

                    int rowcount = ui->m_availableReceiptTable->rowCount();
                    ui->m_availableReceiptTable->insertRow(rowcount);
                    QString receiptName = receiptBook.name();
                    QTableWidgetItem *item1 = new QTableWidgetItem();
                    if (receiptName.isEmpty() == false) {
                        const std::set<int>& accounts = CreceiptBookMap::Object()->getAccountIDsForReceiptBook(receiptBook.m_bookNo);
                        QString accountName;
                        for (auto i : accounts){
                            if (accountName.isEmpty() == false) {
                                accountName += ", ";
                            }
                            accountName += CaccountMap::Object()->getAccountName(i);
                        }
                        if (accountName.isEmpty() == false) {
                            receiptName += " : " + accountName;
                        }

                        item1->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
                        item1->setCheckState(Qt::Unchecked);
                        item1->setText(receiptName);
                    } else {
                        item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
                    }
                    ui->m_availableReceiptTable->setItem(rowcount, 0, item1);
                }
            }
         }
     }

}

void CassignReceiptDlg::populateTransTable()
{
    if (m_transationTableModel == nullptr) {
        return;
    }

    const std::set<int>& accountWithHq = CaccountMap::Object()->getAccountIDListWithHqReceipt();
    QString filter = CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1";
    QString startdate = ui->m_startDate->date().toString("yyyy-MM-dd");
    QString enddate = ui->m_endDate->date().toString("yyyy-MM-dd");
    //get date
    QString date = CtransactionTable::Object()->getColName(TRANSACTION_DATE_IDX) + " BETWEEN \"" + startdate +"\"";
        date += " AND \"" + enddate +"\"";
    filter += " AND " + date;

    filter += " AND " + CtransactionTable::Object()->getColName(TRANSACTION_RECTIPT_BOOK_IDX) + " LIKE \"\"";

    std::set<int> accountIDs;
    QString accountName = ui->m_accountComboBox->currentText();
    if (accountName.isEmpty() == false) {
        int accountID = CaccountMap::Object()->getAccountID(accountName);
        accountIDs.insert(accountID);
        bool populateAccountWithSameReceipt = true;
        if (ui->m_sameReceiptAccount->isChecked() == false) {
            populateAccountWithSameReceipt = false;
        }
        if (populateAccountWithSameReceipt == true) {
            const std::set<int> accountWithSameReceipt = m_accountWithSameReceipt[accountID];
            for (auto id: accountWithSameReceipt) {
                accountIDs.insert(id);
            }
        }
    } else {
        QString deptName = ui->m_departmentComboBox->currentText();
        if (deptName.isEmpty() == false) {
            QMap<QString, QStringList>::iterator fn = m_departmentAccount.find(deptName);
            if (fn != m_departmentAccount.end()) {
                for (QString & accountName: fn.value()) {
                    accountIDs.insert(CaccountMap::Object()->getAccountID(accountName));
                }
            }
        } else {
            //accountIDs.insert(accountWithHq.begin(), accountWithHq.end());
        }
    }

    if (accountIDs.empty() == false) {
        filter += " AND (";
        bool first = true;
        for (const int& accountID: accountIDs ) {
            if (first == false) {
                filter += " OR ";
            }
            first = false;
            filter +=  CtransactionTable::Object()->getColName(TRANSACTION_ACCOUNT_IDX) + "=" + QString::number(accountID);
        }
        filter += ")";

    } else {
        filter = CtransactionTable::Object()->getColName(TRANSACTION_ACCOUNT_IDX) + ">10000000";
    }
    //filter += " ORDER BY " + CtransactionTable::Object()->getColName(TRANSACTION_ACCOUNT_IDX) + " ASC";

    m_transationTableModel->setFilter(filter);
    m_transationTableModel->select();
    m_transationTableModel->sort(TRANSACTION_ACCOUNT_IDX, Qt::AscendingOrder);

    m_transAccounts.clear();



    int numrow = m_transationTableModel->rowCount();
    for (int row = 0; row < numrow; ++row) {


        int account = m_transationTableModel->data(m_transationTableModel->index(row,TRANSACTION_ACCOUNT_IDX), Qt::EditRole).toInt();
        m_transAccounts.insert(account);

    }


}

bool CassignReceiptDlg::checkTransTable()
{
    QMap<int , QMap<int, int> > receiptmap;
    int numrow = m_transationTableModel->rowCount();
    for (int row = 0; row < numrow; ++row) {
       QString receiptBookNoStr = m_transationTableModel->data(m_transationTableModel->index(row,TRANSACTION_RECTIPT_BOOK_IDX), Qt::EditRole).toString();
       QString receiptSlNoStr = m_transationTableModel->data(m_transationTableModel->index(row,TRANSACTION_RECTIPT_NO_IDX), Qt::EditRole).toString();
        int accountID = m_transationTableModel->data(m_transationTableModel->index(row,TRANSACTION_ACCOUNT_IDX), Qt::EditRole).toInt();
        //check if receipt and no is a number
        bool ok;
        int receiptBookNo = receiptBookNoStr.toInt(&ok);
        if (!ok) {
            QString msg = "Recept book " + receiptBookNoStr + " at row " + QString::number(row + 1) + "is not a number";
            message(msg);
            return false;
        }
        int receiptSlNo = receiptSlNoStr.toInt(&ok);
        if (!ok) {
            QString msg = "Recept book Sl " + receiptSlNoStr + " at row " + QString::number(row + 1) + "is not a number";
            message(msg);
            return false;
        }
        if (receiptBookNo == 0 || receiptSlNo == 0) {
            QString msg = "Receipt cannot be empty at row " + QString::number(row + 1) ;
            message(msg);
            return false;
        }

        //check if receipt already use in current table
        if (receiptmap.contains(receiptBookNo)) {
            QMap<int, int>& receiptRow = receiptmap[receiptBookNo];
            if (receiptRow.contains(receiptSlNo)) {
                QString msg = "Receipt "  + QString::number(receiptBookNo) + ":" + QString::number(receiptSlNo);
                msg += " already use at row " + QString::number(receiptRow[receiptSlNo]);
                message(msg);
                return false;
            }
        }
        QMap<int, int>& receiptRow = receiptmap[receiptBookNo];
        receiptRow.insert(receiptSlNo, row);

        //check if receipt is valid
        if (CreceiptBookMap::Object()->isReceiptAvailable(receiptBookNo, receiptSlNo) == false) {
             QString msg = "Receipt "  + QString::number(receiptBookNo) + ":" + QString::number(receiptSlNo);
             msg += " is not available";
             message(msg);
            return false;
        }
        // check if receipt and account match
        if (CreceiptBookMap::Object()->getIsReceiptAssignToAccount(receiptBookNo, accountID) == false) {
            QString msg = "Recept book " + receiptBookNoStr + " at row " + QString::number(row + 1) + " is not assign to account ";
            msg += CaccountMap::Object()->getAccountName(accountID);

            message(msg);
            return false;
        }
    }

    QMap<int , QMap<int, int> >::iterator begin =  receiptmap.begin();
    QMap<int , QMap<int, int> >::iterator end =  receiptmap.end();
    while (begin != end) {
        int book = begin.key();
        QMap<int, int>& receipt = begin.value();
        ++begin;
        QMap<int, int>::iterator receiptBegin = receipt.begin();
        QMap<int, int>::iterator receiptEnd = receipt.end();
        while (receiptBegin != receiptEnd) {
            int no = receiptBegin.key();
            ++receiptBegin;
            CreceiptBookMap::Object()->markReceiptUse(book, no);

        }
    }

    m_transationTableModel->submitAll();

    return true;
}



void CassignReceiptDlg::resizeEvent(QResizeEvent * e)
{
    Q_UNUSED(e)
    resize();
}

void CassignReceiptDlg::on_m_startDate_dateChanged(const QDate &date)
{
    Q_UNUSED(date)
    populateTransTable();
}


void CassignReceiptDlg::on_m_endDate_dateChanged(const QDate &date)
{
    Q_UNUSED(date)
    populateTransTable();


}


void CassignReceiptDlg::on_m_accountComboBox_currentTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    populateReceiptTable();
    populateTransTable();
}


void CassignReceiptDlg::on_m_departmentComboBox_currentTextChanged(const QString &arg1)
{
    ui->m_accountComboBox->clear();
    ui->m_accountComboBox->addItem("");
    QMap<QString, QStringList>::iterator fn = m_departmentAccount.find(arg1);
    if (fn != m_departmentAccount.end()) {
        ui->m_accountComboBox->addItems(fn.value());
    }
    populateReceiptTable();
    populateTransTable();
}


void CassignReceiptDlg::on_m_clearBtn_clicked()
{
    populateReceiptTable();
    populateTransTable();
}


void CassignReceiptDlg::on_m_autoFillBtn_clicked()
{



    //get selected receipt
    //QString accuontName = ui->m_accountComboBox->currentText();
    //bool isDept = accuontName.isEmpty();
    std::set<int> selectedReceiptBooks;
    QVector<Sreceipt> selectedReceipt;

    int numReceiptRow = ui->m_availableReceiptTable->rowCount();
    for (int i = 0; i < numReceiptRow; ++i) {
        QTableWidgetItem * receipt = ui->m_availableReceiptTable->item(i,0);
        if (receipt && receipt->text().isEmpty() == false && (receipt->checkState() == Qt::Checked)) {
            //
            int receiptBookNo = m_currentReceiptBooks[i];
            auto isinsert = selectedReceiptBooks.insert(receiptBookNo);
            if (isinsert.second) {
                SreceiptBook receiptBook = CreceiptBookMap::Object()->getReceiptBookFromBookNo(receiptBookNo);
                const std::set<int>& accounts = CreceiptBookMap::Object()->getAccountIDsForReceiptBook(receiptBookNo);
                std::set<int> intersection;
                std::set_intersection(m_transAccounts.begin(), m_transAccounts.end(),
                                      accounts.begin(), accounts.end(), std::inserter(intersection, intersection.begin()));
                if (intersection != m_transAccounts) {
                    message("Select account from account box");
                    return;
                }
                if (receiptBook.isValid()) {
                    QVector<Sreceipt> receipts = receiptBook.getAllAvailableReceipt();
                    for (Sreceipt& receipt: receipts) {
                        selectedReceipt.push_back(receipt);
                    }
                }
            }

        }
    }
//    const std::set<int>& accounts = CreceiptBookMap::Object()->getAccountIDsForReceiptBook(receiptBook.m_bookNo);



    if (selectedReceipt.size() == 0) {
        message("No receipt selected");
        return;
    }
    int receiptCount = selectedReceipt.size();
    int receiptIdx = 0;
    int numrow = m_transationTableModel->rowCount();
    for (int row = 0; row < numrow; ++row) {
        if (receiptCount == 0 || receiptCount == receiptIdx) {

            break;
        }

        int receiptBookNo = m_transationTableModel->data(m_transationTableModel->index(row,TRANSACTION_RECTIPT_BOOK_IDX), Qt::EditRole).toInt();
        //int receiptSlNo = m_transationTableModel->data(m_transationTableModel->index(row,TRANSACTION_RECTIPT_NO_IDX), Qt::EditRole).toInt();
        if (receiptBookNo == 0) { // receipt not assign. pick from selected receipt receipt
            Sreceipt receipt = selectedReceipt.at(receiptIdx);
            ++receiptIdx;
            m_transationTableModel->setData(m_transationTableModel->index(row,
                                                                          TRANSACTION_RECTIPT_BOOK_IDX),
                                            receipt.m_receiptBookNo, Qt::EditRole);
            m_transationTableModel->setData(m_transationTableModel->index(row,
                                                                          TRANSACTION_RECTIPT_NO_IDX),
                                            receipt.m_receiptBookSlNo, Qt::EditRole);

        }


    }

}


void CassignReceiptDlg::on_m_sameReceiptAccount_clicked()
{
    populateReceiptTable();
    populateTransTable();
}

