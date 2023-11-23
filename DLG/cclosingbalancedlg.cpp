#include "cclosingbalancedlg.h"
#include "ui_cclosingbalancedlg.h"
#include "MISC/cclosingcalculator.h"
#include "caccountmap.h"
#include "cclosingbalancetable.h"

CclosingBalanceDlg::CclosingBalanceDlg(QWidget *parent) :
    CdlgBase(parent),
    ui(new Ui::CclosingBalanceDlg)
{
    ui->setupUi(this);
    m_prefix = "   ";
    QDate date = QDate::currentDate();
    ui->m_closingDate->setDate(date);


}

CclosingBalanceDlg::~CclosingBalanceDlg()
{
    delete ui;
}


void CclosingBalanceDlg::on_m_saveCloseBtn_clicked()
{
    int rowCount = ui->m_table->rowCount();
    if (rowCount > 0) {
        std::vector<SclosingDetailData> rowData;
        for (int row = 0; row < rowCount; ++row) {
            QTableWidgetItem* accOrDeptItem = ui->m_table->item(row, 0);
            QTableWidgetItem* amountItem = ui->m_table->item(row, 1);
            if (accOrDeptItem == nullptr || amountItem == nullptr) {
                continue;
            }
            std::pair<int, bool> & idIsAccount = m_rowAccountMap[row];
            SclosingDetailData detailData;

            detailData.m_id = 0;
            detailData.m_accountOrDeptTableID = idIsAccount.first;
            detailData.m_closingTableID = 0;
            detailData.m_isAccount = idIsAccount.second;
            detailData.m_amount = 0;
            if (amountItem) {
                bool ok;
                detailData.m_amount = amountItem->text().toDouble(&ok);
                if (ok == false) {
                    continue;
                }
            }
            QString name = accOrDeptItem->text().trimmed();
            if (detailData.m_amount == 0 || name.isEmpty()) {
                continue;
            }
            detailData.m_cashOrBankName = "";
            if (name == gBankAccountName || name == gCashAccountName) {
                detailData.m_cashOrBankName = name;
            }
            rowData.push_back(detailData);
        }
        if (rowData.size() > 0) {
            SclosingData closingData;
            closingData.m_closingDate = ui->m_closingDate->date();
            int id = closingData.save();
            for (auto data : rowData) {
                data.m_closingTableID = id;
                data.save();
            }
            emit accept();
        }
    }
}


void CclosingBalanceDlg::on_m_cancelBtn_clicked()
{
    emit reject();
}

void CclosingBalanceDlg::resize()
{
    int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();

    int x = ui->m_table->geometry().x();
    int y = ui->m_table->geometry().y();
    int buttonheight = ui->m_saveCloseBtn->geometry().height();
    int buttonwidth = ui->m_saveCloseBtn->geometry().width();

    int width = windowwidth - (2*x);
    int height = windowheight - y - (2*buttonheight);
    ui->m_table->setGeometry(x, y, width, height);
    ui->m_table->horizontalHeader()->resizeSection(0,width*0.5);
    ui->m_table->horizontalHeader()->resizeSection(1,width*0.5);
    //ui->m_table->horizontalHeader()->resizeSection(2,width*0.25);

    x = windowwidth/2 - buttonwidth - 2;
    y = y + height + (0.5*buttonheight);
    ui->m_saveCloseBtn->setGeometry(x, y, buttonwidth, buttonheight);
    x = windowwidth/2 + 1;
    ui->m_cancelBtn->setGeometry(x, y, buttonwidth, buttonheight);

}

void CclosingBalanceDlg::populateTable(QDate date)
{
    CclosingCalculator calc;
    calc.calculateClosingAsOn(date);
    // const accountDeptAmountType& getClosing()
   //         double cashClosing() const;
    //double bankClosing() const;

    m_rowAccountMap.clear();
    m_cashBankRow.clear();
    ui->m_table->clearContents();
    ui->m_table->setRowCount(0);
    m_accounts.clear();
    const QStringList& accounts = CaccountMap::Object()->getAccountList();
    for (auto account: accounts){
        int id = CaccountMap::Object()->getAccountID(account);
        if (id > 0) {
            m_accounts.insert(id);
        }
    }
    const QStringList& depList = CaccountMap::Object()->getDeptList();
    for (auto dept : depList) {

        int deptID = CaccountMap::Object()->getDeptId(dept);

        int row = ui->m_table->rowCount();
        ui->m_table->insertRow(row);
        m_rowAccountMap[row] = std::make_pair(deptID, false);
        QTableWidgetItem * ac = new QTableWidgetItem(tr("%1").arg(dept));
        ac->setFlags(ac->flags() & ~Qt::ItemIsEditable);

        QTableWidgetItem * amtIncome = new QTableWidgetItem(tr("%1").arg(""));
        //QTableWidgetItem * amtPayment = new QTableWidgetItem(tr("%1").arg(""));

        ui->m_table->setItem(row, 0, ac);
        ui->m_table->setItem(row, 1, amtIncome);
        //ui->m_table->setItem(row, 2, amtPayment);

        QSet<int> accounts;
        if (CaccountMap::Object()->getAccounsForDept(deptID, accounts)){
            //QStringList accountList;
            for (auto accountID : accounts) {
                m_accountDeptMap[accountID] = deptID;
                m_accounts.erase(accountID);
                //accountList.push_back(CaccountMap::Object()->getAccountName(accountID));
            }
            /*accountList.sort();
            for (auto accountName: accountList) {
                QString name = m_prefix + accountName;
                int row = ui->m_table->rowCount();
                ui->m_table->insertRow(row);
                m_nameRowMap[name] = std::make_pair(row, accountName);
                QTableWidgetItem * ac = new QTableWidgetItem(tr("%1").arg(name));
                QTableWidgetItem * amtIncome = new QTableWidgetItem(tr("%1").arg(""));
                QTableWidgetItem * amtPayment = new QTableWidgetItem(tr("%1").arg(""));

                ui->m_table->setItem(row, 0, ac);
                ui->m_table->setItem(row, 1, amtIncome);
                ui->m_table->setItem(row, 2, amtPayment);
            }*/
        } else {

        }
    }
    int bankAccountID = 0;
    for(auto accountID : m_accounts) {
        QString accountName = CaccountMap::Object()->getAccountName(accountID);
        if (accountName == gBankAccountName) {
            bankAccountID = accountID;
            continue;
        }
        int row = ui->m_table->rowCount();
        ui->m_table->insertRow(row);
        m_rowAccountMap[row] = std::make_pair(accountID, true);

        QTableWidgetItem * ac = new QTableWidgetItem(tr("%1").arg(accountName));
        QTableWidgetItem * amtIncome = new QTableWidgetItem(tr("%1").arg(""));
        //QTableWidgetItem * amtPayment = new QTableWidgetItem(tr("%1").arg(""));
        ac->setFlags(ac->flags() & ~Qt::ItemIsEditable);

        ui->m_table->setItem(row, 0, ac);
        ui->m_table->setItem(row, 1, amtIncome);
        //ui->m_table->setItem(row, 2, amtPayment);
    }


    int row = ui->m_table->rowCount();
    ui->m_table->insertRow(row);
    m_rowAccountMap[row] = std::make_pair(bankAccountID, true);
    m_cashBankRow.insert({gBankAccountName, row});
    QTableWidgetItem * ac = new QTableWidgetItem(tr("%1").arg(gBankAccountName));
    QTableWidgetItem * amtIncome = new QTableWidgetItem(tr("%1").arg(""));
    //QTableWidgetItem * amtPayment = new QTableWidgetItem(tr("%1").arg(""));
    ac->setFlags(ac->flags() & ~Qt::ItemIsEditable);

    ui->m_table->setItem(row, 0, ac);
    ui->m_table->setItem(row, 1, amtIncome);
    //ui->m_table->setItem(row, 2, amtPayment);


    row = ui->m_table->rowCount();
    ui->m_table->insertRow(row);
    m_rowAccountMap[row] = std::make_pair(0, true);
    m_cashBankRow.insert({gCashAccountName, row});
    QTableWidgetItem * acCash = new QTableWidgetItem(tr("%1").arg(gCashAccountName));
    QTableWidgetItem * amtIncomeCash = new QTableWidgetItem(tr("%1").arg(""));
    //QTableWidgetItem * amtPaymentCash = new QTableWidgetItem(tr("%1").arg(""));
    acCash->setFlags(acCash->flags() & ~Qt::ItemIsEditable);

    ui->m_table->setItem(row, 0, acCash);
    ui->m_table->setItem(row, 1, amtIncomeCash);
    //ui->m_table->setItem(row, 2, amtPaymentCash);


}


void CclosingBalanceDlg::on_m_closingDate_dateChanged(const QDate &date)
{
    populateTable(date);
}

