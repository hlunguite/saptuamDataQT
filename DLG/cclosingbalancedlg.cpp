#include "cclosingbalancedlg.h"
#include "ui_cclosingbalancedlg.h"
#include "MISC/cclosingcalculator.h"
#include "caccountmap.h"
#include "cclosingbalancetable.h"
#include "cformatforamountdeligate.h"
#include "csdutils.h"
#include "MISC/CdlgDefine.h"
const double gDiffValue = 1;
CclosingBalanceDlg::CclosingBalanceDlg(QWidget *parent) :
    CdlgBase(parent),
    ui(new Ui::CclosingBalanceDlg)
{
    ui->setupUi(this);
    ui->m_table->setItemDelegateForColumn(1, new CformatForAmountDeligate(this));
    ui->m_table->setItemDelegateForColumn(2, new CformatForAmountDeligate(this));

    m_calcTotal = false;

    m_dateToUse = QDate::currentDate();
    ui->m_closingDate->setDate(m_dateToUse);


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

        double cashBankTotal = 0;
        double othertotal = 0;
        int lastOtherRow = -1;
        int row = 0;
        for ( ;row < rowCount; ++row) {
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
                cashBankTotal += detailData.m_amount;
            } else {
                lastOtherRow = row;
                othertotal += detailData.m_amount;
            }
            rowData.push_back(detailData);
        }

        if (cashBankTotal != othertotal) {
            double diff = cashBankTotal - othertotal;
            if(abs(diff) > gDiffValue) {
                QColor bgColor(225, 0, 0);
                if (lastOtherRow > -1) {
                    QTableWidgetItem* cumulativeItem = ui->m_table->item(lastOtherRow, 2);
                    if (cumulativeItem) {
                        cumulativeItem->setBackground(bgColor);
                    }
                }
                QTableWidgetItem* bankCashItem = ui->m_table->item(row, 2);
                if (bankCashItem) {
                    bankCashItem->setBackground(bgColor);
                }

                QString cashBankTotalStr = CsdUtils::convertAmountToStringWithSign(cashBankTotal);
                QString otherTotalStr = CsdUtils::convertAmountToStringWithSign(othertotal);
                QString diffStr = CsdUtils::convertAmountToStringWithSign(diff);
                QString msg = "Sum of Cash and Bank " + cashBankTotalStr + " not equal Account/Dept total " + otherTotalStr + " by " + diffStr;
                message(msg);
                return;
            }
        }
        if (rowData.size() > 0) {


            SclosingData closingData;
            closingData.m_closingDate = m_dateToUse;
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

    int x = XCORD;
    int y = YCORD;
    int buttonheight = BUTTON_SIZE.height();
    int buttonwidth = BUTTON_SIZE.width();

    ui->m_closingasonlbl->setGeometry(x, y, DATE_SIZE.width(), DATE_SIZE.height());
    x += GAP + DATE_SIZE.width();
    ui->m_closingDate->setGeometry(x, y, DATE_SIZE.width(), DATE_SIZE.height());
    x = XCORD;
    y += GAP + DATE_SIZE.height();

    int width = windowwidth - (2*XCORD);
    int height = windowheight - y - YCORD - buttonheight - GAP;
    ui->m_table->setGeometry(x, y, width, height);
    ui->m_table->horizontalHeader()->resizeSection(0,width*0.5);
    ui->m_table->horizontalHeader()->resizeSection(1,width*0.24);
    ui->m_table->horizontalHeader()->resizeSection(2,width*0.24);

    x =  windowwidth/2 - buttonwidth - GAP/2;
    y += height + GAP;
    ui->m_saveCloseBtn->setGeometry(x, y, buttonwidth, buttonheight);
    x += buttonwidth + GAP;
    ui->m_cancelBtn->setGeometry(x, y, buttonwidth, buttonheight);

}

void CclosingBalanceDlg::populateTable(QDate date)
{
    m_dateToUse = date;
    m_calcTotal = false;
    CclosingCalculator calc;
    calc.calculateClosingAsOn(date);
    const accountDeptAmountType& accountDeptClosing = calc.getClosing();
    double cashClosing = calc.cashClosing();
    double bankClosing = calc.bankClosing();

    if (m_dateToUse > calc.lastTransDate()) {
        //qDebug()<<"Select date "<<date<<" last trans date "<<calc.lastTransDate();
        m_dateToUse = calc.lastTransDate();
    }

    m_rowAccountMap.clear();
    m_cashBankRow.clear();
    ui->m_table->clearContents();
    ui->m_table->setRowCount(0);
    m_accounts.clear();
    int lastOtherRow = -1;
    const QStringList& accounts = CaccountMap::Object()->getAccountList();
    for (auto account: accounts){
        int id = CaccountMap::Object()->getAccountID(account);
        if (id > 0) {
            m_accounts.insert(id);
        }
    }
    double cashBankTotal = 0;
    double otherTotal = 0;
    const QStringList& depList = CaccountMap::Object()->getDeptList();
    for (auto dept : depList) {

        int deptID = CaccountMap::Object()->getDeptId(dept);
        auto fn = accountDeptClosing.find(deptID);
        QString amtStr;
        if (fn != accountDeptClosing.end()) {
            amtStr = QString::number(fn->second, 'f', 2);
            otherTotal += fn->second;

            //qDebug()<<"Other "<<deptID<<" "<<QString::number(otherTotal, 'f', 2)<<" "<<QString::number(fn->second, 'f', 2);
        } else {
            //qDebug()<<"not found for "<<dept;
        }
        int row = ui->m_table->rowCount();
        lastOtherRow = row;
        ui->m_table->insertRow(row);
        m_rowAccountMap[row] = std::make_pair(deptID, false);
        QTableWidgetItem * ac = new QTableWidgetItem(tr("%1").arg(dept));
        ac->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
        ac->setFlags(ac->flags() & ~Qt::ItemIsEditable);

        QTableWidgetItem * amtIncome = new QTableWidgetItem(tr("%1").arg(amtStr));
        amtIncome->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

        QTableWidgetItem * cumulativeTotal = new QTableWidgetItem(tr("%1").arg(QString::number(otherTotal, 'f', 2)));
        cumulativeTotal->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        cumulativeTotal->setFlags(ac->flags() & ~Qt::ItemIsEditable);


        ui->m_table->setItem(row, 0, ac);
        ui->m_table->setItem(row, 1, amtIncome);
        ui->m_table->setItem(row, 2, cumulativeTotal);



        QSet<int> accounts;
        if (CaccountMap::Object()->getAccounsForDept(deptID, accounts)){
            //QStringList accountList;
            for (auto accountID : accounts) {
                m_accountDeptMap[accountID] = deptID;
                m_accounts.erase(accountID);
            }

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
        /*EaccountType accType = CaccountMap::Object()->getAccountType(accountID);
        if (accType == REMITTANCE_ACCOUNT_TYPE) {
            m_remittanceID = accountID;
        } else if (accType == REQUEST_ACCOUNT_TYPE) {
            m_requestID = accountID;
        }*/

        QString amtStr;
        auto fn = accountDeptClosing.find(accountID);
        if (fn != accountDeptClosing.end()) {
            double amount = fn->second;
            amtStr = QString::number(amount, 'f', 2);

            otherTotal += amount;
           // qDebug()<<"Other1 "<<accountID<<" "<<QString::number(otherTotal, 'f', 2)<<" "<<QString::number(amount, 'f', 2);


        } else {
            qDebug()<<"not found for "<<accountName;
        }

        int row = ui->m_table->rowCount();
        lastOtherRow = row;

        ui->m_table->insertRow(row);
        m_rowAccountMap[row] = std::make_pair(accountID, true);

        QTableWidgetItem * ac = new QTableWidgetItem(tr("%1").arg(accountName));
        ac->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);

        QTableWidgetItem * amtIncome = new QTableWidgetItem(tr("%1").arg(amtStr));
        amtIncome->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        ac->setFlags(ac->flags() & ~Qt::ItemIsEditable);

        QTableWidgetItem * cumulativeTotal = new QTableWidgetItem(tr("%1").arg(QString::number(otherTotal, 'f', 2)));
        cumulativeTotal->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        cumulativeTotal->setFlags(ac->flags() & ~Qt::ItemIsEditable);

        ui->m_table->setItem(row, 0, ac);
        ui->m_table->setItem(row, 1, amtIncome);
        ui->m_table->setItem(row, 2, cumulativeTotal);

    }


    int row = ui->m_table->rowCount();
    ui->m_table->insertRow(row);
    m_rowAccountMap[row] = std::make_pair(bankAccountID, true);
    m_cashBankRow.insert({gBankAccountName, row});
    QTableWidgetItem * ac = new QTableWidgetItem(tr("%1").arg(gBankAccountName));
    ac->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    ac->setFlags(ac->flags() & ~Qt::ItemIsEditable);

    QTableWidgetItem * amtIncome = new QTableWidgetItem(tr("%1").arg(QString::number(bankClosing, 'f',2)));
    amtIncome->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    cashBankTotal += bankClosing;

    QTableWidgetItem * cumulativeTotal = new QTableWidgetItem(tr("%1").arg(QString::number(cashBankTotal, 'f', 2)));
    cumulativeTotal->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    cumulativeTotal->setFlags(ac->flags() & ~Qt::ItemIsEditable);

    ui->m_table->setItem(row, 0, ac);
    ui->m_table->setItem(row, 1, amtIncome);
    ui->m_table->setItem(row, 2, cumulativeTotal);


    row = ui->m_table->rowCount();
    ui->m_table->insertRow(row);
    m_rowAccountMap[row] = std::make_pair(0, true);
    m_cashBankRow.insert({gCashAccountName, row});
    QTableWidgetItem * acCash = new QTableWidgetItem(tr("%1").arg(gCashAccountName));
    acCash->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    acCash->setFlags(acCash->flags() & ~Qt::ItemIsEditable);

    QTableWidgetItem * amtIncomeCash = new QTableWidgetItem(tr("%1").arg(QString::number(cashClosing, 'f',2)));
    amtIncomeCash->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    cashBankTotal += cashClosing;
    QTableWidgetItem * cumulativeTotal1 = new QTableWidgetItem(tr("%1").arg(QString::number(cashBankTotal, 'f', 2)));
    cumulativeTotal1->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
    cumulativeTotal1->setFlags(ac->flags() & ~Qt::ItemIsEditable);

    ui->m_table->setItem(row, 0, acCash);
    ui->m_table->setItem(row, 1, amtIncomeCash);
    ui->m_table->setItem(row, 2, cumulativeTotal1);

    QColor bgColor(Qt::white);
    QString diffStr;
    double diff = cashBankTotal - otherTotal;

    if (abs(diff) > 0.009) {

        QString cash = CsdUtils::convertAmountToStringWithSign(cashBankTotal);
        QString other = CsdUtils::convertAmountToStringWithSign(otherTotal);
        diffStr = "Cash/Bank " + cash;
        diffStr += " Others " + other + " Difference amount " + CsdUtils::convertAmountToStringWithSign(diff);

        if (fabs(diff) > 1) {
            bgColor.setRgb(255, 0,0);
        } else {
            bgColor.setRgb(255, 219, 255);
        }

    }
    if (lastOtherRow > -1) {
        QTableWidgetItem* cumulativeItem = ui->m_table->item(lastOtherRow, 2);
        if (cumulativeItem) {
            cumulativeItem->setBackground(bgColor);
            if (diffStr.isEmpty() == false) {
                cumulativeItem->setToolTip(diffStr);
            }
        }
    }
    QTableWidgetItem* bankCashItem = ui->m_table->item(row, 2);
    if (bankCashItem) {
        bankCashItem->setBackground(bgColor);
        if (diffStr.isEmpty() == false) {
            bankCashItem->setToolTip(diffStr);
        }
    }


    m_calcTotal = true;
}


void CclosingBalanceDlg::on_m_closingDate_dateChanged(const QDate &date)
{
    populateTable(date);
}


void CclosingBalanceDlg::on_m_table_itemChanged(QTableWidgetItem *item)
{
    if (m_calcTotal == false) {
        return;
    }
    if (item->column() != 1) {
        return;
    }


}

