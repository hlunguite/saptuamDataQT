#include "creceiptsetupdlg.h"
#include "ui_creceiptsetupdlg.h"
#include <QDebug>
#include "creceiptbooktable.h"
#include "caccountmap.h"
#include "creceiptbookmap.h"
#include "MISC/CdlgDefine.h"
CreceiptSetupDlg::CreceiptSetupDlg(QWidget *parent) :CdlgBase(parent),
    ui(new Ui::CreceiptSetupDlg)

{
    ui->setupUi(this);
    QStringList deptList = CaccountMap::Object()->getDeptList();
    deptList.sort();
    ui->m_selDeptBox->addItem("");
    ui->m_selDeptBox->addItems(deptList);

    auto allAccountWithHqReceipt = CaccountMap::Object()->getAccountIDListWithHqReceipt();
    for (auto id : allAccountWithHqReceipt) {
        m_allAccountWithHqReceipt.insert(id);
    }

    updateAccountForAvailableReceiptTable("");
    resize();

}

CreceiptSetupDlg::~CreceiptSetupDlg()
{
    delete ui;
}

void CreceiptSetupDlg::resize()
{
    int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();


    int x = XCORD;//ui->m_addNewReceiptGroup->geometry().x();
    int y = YCORD;//ui->m_addNewReceiptGroup->geometry().x();
    int width = (4*BUTTON_SIZE.width()) + (3*DATE_SIZE.width()) + 8*GAP;
    int height = 2*DEFAULT_HEIGHT;
    ui->m_addNewReceiptGroup->setGeometry(x, y, width, height);
    x = GAP;

    ui->m_bookNolbl->setGeometry(x, y, BUTTON_SIZE.width(), BUTTON_SIZE.height());
    x += GAP + BUTTON_SIZE.width();
    ui->m_bookNo->setGeometry(x, y, DATE_SIZE.width(), DATE_SIZE.height());

    x += GAP +  DATE_SIZE.width();
    ui->m_startNolbl->setGeometry(x, y, BUTTON_SIZE.width(), BUTTON_SIZE.height());
    x += GAP + BUTTON_SIZE.width();
    ui->m_startNo->setGeometry(x, y, DATE_SIZE.width(), DATE_SIZE.height());

    x += GAP +  DATE_SIZE.width();
    ui->m_endNolbl->setGeometry(x, y, BUTTON_SIZE.width(), BUTTON_SIZE.height());
    x += GAP + BUTTON_SIZE.width();
    ui->m_endNo->setGeometry(x, y, DATE_SIZE.width(), DATE_SIZE.height());
    x += GAP +  DATE_SIZE.width();
    ui->m_addReceiptBtn->setGeometry(x, y, BUTTON_SIZE.width(), BUTTON_SIZE.height());


    int buttonheight = BUTTON_SIZE.height();
    int buttonwidth = BUTTON_SIZE.width();
    //qDebug()<<"buttonheight "<<buttonheight<<" buttonwidth "<<buttonwidth;
    y = y + GAP + height;
    x = XCORD;//ui->m_addNewReceiptGroup->geometry().x();

    ui->m_selectlbl->setGeometry(x, y /*+ buttonheight/2*/,
                                 DEPT_ACCOUNT_COMBO_SIZE.width(), DEPT_ACCOUNT_COMBO_SIZE.height());



    int bx = windowwidth/2 - buttonwidth/2;
    ui->m_clearBtn->setGeometry(bx, y, buttonwidth, buttonheight);
    bx =  windowwidth/2 + buttonwidth/2 + GAP;
    int addRemovBtnWidth = buttonwidth*3;
    ui->m_removeReceiptAccountBtn->setGeometry(bx, y, addRemovBtnWidth, buttonheight);

    ui->m_accountReceiptlbl->setGeometry( windowwidth - x - DEPT_ACCOUNT_COMBO_SIZE.width(), y,
                                         DEPT_ACCOUNT_COMBO_SIZE.width(),
                                         DEPT_ACCOUNT_COMBO_SIZE.height());


    bx =  windowwidth/2 - buttonwidth/2 - GAP - addRemovBtnWidth;
    ui->m_addReceiptAccountBtn->setGeometry(bx, y, addRemovBtnWidth, buttonheight);

    bx = bx - (GAP + DEPT_ACCOUNT_COMBO_SIZE.width());

    ui->m_selDeptBox->setGeometry(bx, y, DEPT_ACCOUNT_COMBO_SIZE.width(), DEPT_ACCOUNT_COMBO_SIZE.height());





    y += buttonheight + GAP;
    width = windowwidth*0.5 - x;
    height = windowheight - y - buttonheight - GAP - GAP;
    if (height < 0) {
        height = width;
    }

    ui->m_availableReceiptTable->setGeometry(x,y,width,height);


    ui->m_receiptAccountMap->setGeometry((windowwidth - width - x), y, width, height);

    ui->m_cancelBtn->setGeometry( (windowwidth*.5 - buttonwidth/2), (y + GAP + height) , buttonwidth, buttonheight);
    //ui->m_okBtn->setGeometry( (x + width), (y + buttonheight/2 + height) , buttonwidth, buttonheight);

    ui->m_availableReceiptTable->horizontalHeader()->resizeSection(0 ,width*.40);
    ui->m_availableReceiptTable->horizontalHeader()->resizeSection(1 ,width*.60);
    //ui->m_availableReceiptTable->horizontalHeader()->resizeSection(RECEIPT_SETUP_ACCOUNT ,width*.5);

    ui->m_receiptAccountMap->horizontalHeader()->resizeSection(0, width*.60);
    ui->m_receiptAccountMap->horizontalHeader()->resizeSection(1, width*.40);




}

void CreceiptSetupDlg::clear()
{
    CreceiptBookMap::Object()->populate()  ;

    addAvailableTable();
    addAccountReceiptTable();
    //ui->m_availableReceiptTable->setItem(0, 1, item2);

}

void CreceiptSetupDlg::updateAccountForAvailableReceiptTable(QString deptName)
{

    m_currentAccountNameInAvailableReceiptTable.clear();
    m_currentAccountIDInAvailableReceiptTable.clear();
    if (deptName.isEmpty() == false) {
        QSet<int> accounts;
        int deptID = CaccountMap::Object()->getDeptId(deptName);
        if (CaccountMap::Object()->getAccounsForDept(deptID, accounts)) {
            for (auto account: accounts) {
                auto fn = m_allAccountWithHqReceipt.find(account);
                if (fn != m_allAccountWithHqReceipt.end()) {
                    m_currentAccountNameInAvailableReceiptTable.push_back(CaccountMap::Object()->getAccountName(account));
                    m_currentAccountIDInAvailableReceiptTable.insert(account);
                }
            }
        }
    } else {
        for (auto account : m_allAccountWithHqReceipt) {
            m_currentAccountNameInAvailableReceiptTable.push_back(CaccountMap::Object()->getAccountName(account));
            m_currentAccountIDInAvailableReceiptTable.insert(account);
        }
    }
    m_currentAccountNameInAvailableReceiptTable.sort();


    clear();
}



void CreceiptSetupDlg::resizeEvent(QResizeEvent * e)
{
    Q_UNUSED(e)
    resize();
}

void CreceiptSetupDlg::on_m_addReceiptBtn_clicked()
{
    int receiptbookNo = ui->m_bookNo->text().toInt();
    int start = ui->m_startNo->text().toInt();
    int end = ui->m_endNo->text().toInt();
    /*if (receiptbook == 0 || start == 0 || end == 0) {
        return;
    }*/

    SreceiptBook receiptbook;
    receiptbook.m_id = 0;
    receiptbook.m_bookNo = receiptbookNo;
    receiptbook.m_startNo = start;
    receiptbook.m_endNo = end;
    for (int i = start; i <= end; ++i) {
        receiptbook.m_receiptNo.insert(i);
    }
    receiptbook.m_available =  receiptbook.m_receiptNo.size();
    if (receiptbook.save() > 0) {

        ui->m_bookNo->setText("");
        ui->m_startNo->setText("");
        ui->m_endNo->setText("");
        clear();
    } else {
       message(receiptbook.m_error);
    }


}


void CreceiptSetupDlg::on_m_clearBtn_clicked()
{
   clear();
}


void CreceiptSetupDlg::on_m_cancelBtn_clicked()
{
    emit reject();
}

void CreceiptSetupDlg::addAvailableRow(QString receiptName, QString accountName)
{
    int rowcount = ui->m_availableReceiptTable->rowCount();
    ui->m_availableReceiptTable->insertRow(rowcount);

    QTableWidgetItem *item1 = new QTableWidgetItem();
    if (receiptName.isEmpty() == false) {
        item1->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item1->setCheckState(Qt::Unchecked);
        item1->setText(receiptName);
    } else {
        item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
    }
    ui->m_availableReceiptTable->setItem(rowcount, 0, item1);

    QTableWidgetItem *item2 = new QTableWidgetItem();
    if (accountName.isEmpty() == false) {
        item2->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item2->setCheckState(Qt::Unchecked);
        item2->setText(accountName);
    } else {
        item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
    }
    ui->m_availableReceiptTable->setItem(rowcount, 1, item2);
}

void CreceiptSetupDlg::addAvailableTable()
{

    //qDebug()<<"\nadd available table";

    m_rowToAvailableReceiptMap.clear();
    int row = 0;
    QVector<SreceiptBook> receipts  =  CreceiptBookMap::Object()->getAvailableReceiptBook() ;// CreceiptBookTable::Object()->getAvailableReceiptBook();
    QVector<QString> availableReceipt;
    int receiptIndex = 0;
    for (auto receipt: receipts) {
        int bookno = receipt.m_bookNo;
        const std::set<int> & accontIDs = CreceiptBookMap::Object()->getAccountIDsForReceiptBook(bookno);
        bool toAdd = false;
        QString receiptName = receipt.name();
        for (auto accountID : accontIDs) {
            auto fn = m_currentAccountIDInAvailableReceiptTable.find(accountID);
            bool isAssign = fn != m_currentAccountIDInAvailableReceiptTable.cend();
            if (isAssign) {
                toAdd = true;
                if (accontIDs.size() == 1) {
                    QString accountName = CaccountMap::Object()->getAccountName(accountID);
                    receiptName += " ("  + accountName + ")";
                } else {
                    int dept = CaccountMap::Object()->getDeptForAccount(accountID);
                    QString deptName = CaccountMap::Object()->getDeptName(dept);
                    if (deptName.isEmpty()) {
                        deptName = CaccountMap::Object()->getAccountName(accountID);
                    }

                    receiptName += " ("  + deptName + ")";


                }
                break;
            }
        }
        if (accontIDs.empty()) {
            toAdd = true;
        }

        if (toAdd) {
            //            int dept = CaccountMap::Object()->getDeptForAccount(accountId);

            m_rowToAvailableReceiptMap[row] = receiptIndex;
            availableReceipt.push_back(receiptName);
            ++row;
        }

        ++receiptIndex;
    }

    int accountSize = m_currentAccountNameInAvailableReceiptTable.size();

    int receiptBookCount = availableReceipt.size();
    ui->m_availableReceiptTable->clearContents();
    ui->m_availableReceiptTable->setRowCount(0);
    int i = 0;
    while((accountSize > 0)  && (receiptBookCount > 0)) {

        addAvailableRow(availableReceipt.at(i), m_currentAccountNameInAvailableReceiptTable.at(i));
        ++i;
        --accountSize;
        --receiptBookCount;
    }
    while (accountSize > 0) {
        addAvailableRow("", m_currentAccountNameInAvailableReceiptTable.at(i));
        ++i;
        --accountSize;

    }
    while(receiptBookCount > 0) {
        addAvailableRow(availableReceipt.at(i), "");
        ++i;
        --receiptBookCount;
    }
}

void CreceiptSetupDlg::addAccountReceiptTable()
{
    m_accountReceipt.clear();
    QVector<SaccountReceipt> accountReceipts = CreceiptBookMap::Object()->getAllAccountReceipt();
    ui->m_receiptAccountMap->setRowCount(0);
    int size = accountReceipts.size();
    for (int i = 0; i < size; ++i) {
        SaccountReceipt accountReceipt = accountReceipts.at(i);
        int accountID = accountReceipt.m_accountTableID;
        if (m_currentAccountIDInAvailableReceiptTable.find(accountID) != m_currentAccountIDInAvailableReceiptTable.end()) {
            addAccoutMapRow(accountReceipt);
        }
    }
}

void CreceiptSetupDlg::addAccoutMapRow(SaccountReceipt& accountReceipt)
{

    int accountID = accountReceipt.m_accountTableID;
    int receiptBookID = accountReceipt.m_receiptBoookTableID;
    SreceiptBook receiptBook  =  CreceiptBookMap::Object()->getReceiptBookFromID(receiptBookID);
    int rowcount = ui->m_receiptAccountMap->rowCount();
    ui->m_receiptAccountMap->insertRow(rowcount);
    QString accountName = CaccountMap::Object()->getAccountName(accountID);
    QString receiptName = receiptBook.name();

    int rowno = m_accountReceipt.size();

    QMap<int, int>& accountRow = m_receiptAccountRow[receiptBookID];
    accountRow.insert(accountID, rowno);

    m_accountReceipt.push_back(accountReceipt);

    QTableWidgetItem *item1 = new QTableWidgetItem();
    if (receiptName.isEmpty() == false) {
        //item1->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        //item1->setCheckState(Qt::Unchecked);
        item1->setText(receiptName);
    } else {
        item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
    }
    ui->m_receiptAccountMap->setItem(rowcount, 1, item1);

    QTableWidgetItem *item2 = new QTableWidgetItem();
    if (accountName.isEmpty() == false) {
        item2->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item2->setCheckState(Qt::Unchecked);
        item2->setText(accountName);
    } else {
        item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
    }
    ui->m_receiptAccountMap->setItem(rowcount, 0, item2);
}


void CreceiptSetupDlg::on_m_addReceiptAccountBtn_clicked()
{
    QVector<SreceiptBook> receipts  = CreceiptBookMap::Object()->getAvailableReceiptBook() ;
    int deptID = 0;
    bool error = false;
    QVector<int> accountSelected;
    QVector<int> receiptSelected;
    QSet<int> receiptAccount;
    int numrow = ui->m_availableReceiptTable->rowCount();
    for (int i = 0; i < numrow; ++i) {
        QTableWidgetItem * receipt = ui->m_availableReceiptTable->item(i,0);
        QTableWidgetItem * account = ui->m_availableReceiptTable->item(i,1);


        if (receipt && receipt->text().isEmpty() == false && (receipt->checkState() == Qt::Checked)) {
            int id = m_rowToAvailableReceiptMap[i];
            SreceiptBook receiptBook = receipts.at(id) ;
            //qDebug()<<"selected receipt "<<receiptBook.name();
            receiptSelected.push_back(receiptBook.m_id);
            //receipt->setCheckState(Qt::Unchecked);
            QMap<int, QMap<int, int> >::iterator fn = m_receiptAccountRow.find(receiptBook.m_id);
            if (fn != m_receiptAccountRow.end()) {
                QMap<int, int>& accountRow = fn.value();
                QMap<int, int>::iterator begin = accountRow.begin();
                QMap<int, int>::iterator end = accountRow.end();
                while (begin != end) {
                    int accountID = begin.key();
                    receiptAccount.insert(accountID);
                    ++begin;
                    int dept = CaccountMap::Object()->getDeptForAccount(accountID);
                    if (deptID == 0) {
                        deptID = dept;
                    } else if (deptID != dept) {
                        message("Department for selectd receipt are not same");
                        error = true;
                        break;
                    }

                }
            }

        }
        if (error) {
            return;
        }

        if (account && account->text().isEmpty() == false && (account->checkState() == Qt::Checked)) {
            int accountId = CaccountMap::Object()->getAccountID(account->text().trimmed());
            int dept = CaccountMap::Object()->getDeptForAccount(accountId);
            if (deptID == 0) {
                deptID = dept;
            } else if (deptID != dept) {
                message("Department for selectd account are not same");
                error = true;
            }
            accountSelected.push_back(accountId);
            account->setCheckState(Qt::Unchecked);
        }

        if (error) {
            return;
        }
    }
    //check if receipt already assign account and the department is same as current
    for (auto accountID: receiptAccount) {
        int dept = CaccountMap::Object()->getDeptForAccount(accountID);
        if (deptID != dept) {
            message("Department for selectd receipt are not same");
            return;
        }
    }
    if (deptID == 0) {
        message("Selected Account is not part of any Department");
        return;
    }

    for (auto accountID : accountSelected) {
        for (int receiptID: receiptSelected) {
            SaccountReceipt accountReceipt;
            accountReceipt.m_accountReceiptID = 0;
            accountReceipt.m_accountTableID = accountID;
            accountReceipt.m_receiptBoookTableID = receiptID;
            QMap<int, QMap<int, int> >::iterator fn = m_receiptAccountRow.find(receiptID);
            bool tosave = false;
            //check if account receipt pair already exist
            if (fn != m_receiptAccountRow.end()) {
                auto fnAccount = fn.value().find(accountID);
                if (fnAccount == fn.value().end()) {
                    tosave = true;
                }

            } else {
                tosave = true;
            }
            if (tosave) {
                accountReceipt.save();
            }
        }
    }

    clear();
}


void CreceiptSetupDlg::on_m_removeReceiptAccountBtn_clicked()
{

    int numrow = ui->m_receiptAccountMap->rowCount();
    for (int i = 0; i < numrow; ++i) {
        //QTableWidgetItem * receipt = ui->m_receiptAccountMap->item(i,1);
        QTableWidgetItem * account = ui->m_receiptAccountMap->item(i,0);

        if (account && account->text().isEmpty() == false && (account->checkState() == Qt::Checked)) {
                SaccountReceipt accountReceipt = m_accountReceipt.at(i);
                accountReceipt.deleteAccountReceipt();
                //qDebug()<<" receipt available "<<accountReceipt.m_receiptBook.m_available;
                //qDebug()<<" start and end "<<accountReceipt.m_receiptBook.m_startNo<<" "<<accountReceipt.m_receiptBook.m_endNo;

        }
    }

   clear();
}


void CreceiptSetupDlg::on_m_selDeptBox_currentTextChanged(const QString &arg1)
{
   updateAccountForAvailableReceiptTable(arg1);

}

