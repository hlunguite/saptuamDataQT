#include "cfiltertransactiondlg.h"
#include "ui_cfiltertransactiondlg.h"
#include "ctransactionutils.h"
#include "caccountmap.h";
#include "ctransactiontable.h"
#include <set>
CfilterTransactionDlg::CfilterTransactionDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CfilterTransactionDlg)
{
    ui->setupUi(this);
    ui->m_filterDateStart->setEnabled(false);
    ui->m_filterDateEnd->setEnabled(false);
    m_filter.clear();
    QDate date = QDate::currentDate();
    ui->m_filterDateStart->setDate(date);
    ui->m_filterDateEnd->setDate(date);
    QStringList depts = CaccountMap::Object()->getDeptList();
    depts.sort();
    ui->m_deptBox->addItem("");
    ui->m_deptBox->addItems(depts);
    QStringList accounts = CaccountMap::Object()->getAccountList();
    accounts.sort();
    ui->m_accountBox->addItem("");
    ui->m_accountBox->addItems(accounts);
    ui->m_transTypeBox->clear();
    ui->m_transTypeBox->addItem("");
    QStringList types = CtransactionUtils::Object()->getAllTransctionTypeStr();
    types.sort();
    ui->m_transTypeBox->addItems(types);


}

CfilterTransactionDlg::~CfilterTransactionDlg()
{
    delete ui;
}

void CfilterTransactionDlg::on_pushButton_clicked()
{

    QString dept = ui->m_deptBox->currentText();
    QString account = ui->m_accountBox->currentText();
    if (account.isEmpty()) {
        if (dept.isEmpty() == false) {
            int deptID = CaccountMap::Object()->getDeptId(dept);
            QSet<int> accounts;
            QString accountFilter;
            if (CaccountMap::Object()->getAccounsForDept(deptID, accounts)) {
                for (auto id : accounts) {
                    if (accountFilter.isEmpty() == false) {
                        accountFilter += " OR ";
                    }
                    accountFilter += CtransactionTable::Object()->getColName(TRANSACTION_ACCOUNT_IDX) + "=" + QString::number(id);
                }
            }
            if (accountFilter.isEmpty() == false) {
                QString open;
                QString close;
                if (m_filter.isEmpty() == false) {
                    open = " AND (";
                    close = ")";
                }
                m_filter += open + accountFilter + close;
            }
        }
    } else {
        int id = CaccountMap::Object()->getAccountID(account);
        if (id > 0) {
            QString accountFilter = CtransactionTable::Object()->getColName(TRANSACTION_ACCOUNT_IDX) + "=" + QString::number(id);
            QString open;
            QString close;
            if (m_filter.isEmpty() == false) {
                open = " AND (";
                close = ")";
            }
            m_filter += open + accountFilter + close;
        }
    }

    QString type = ui->m_transTypeBox->currentText();
    int typeID = CtransactionUtils::Object()->getTransactionType(type);
    if (typeID != INVALID_TRANSACTION_TYPE) {
        QString typeFilter =  CtransactionTable::Object()->getColName(TRANSACTION_TYPE_IDX) + "=" + QString::number(typeID);
        QString open;
        QString close;
        if (m_filter.isEmpty() == false) {
            open = " AND (";
            close = ")";
        }
        m_filter += open + typeFilter + close;
    }
    //date

    if (ui->m_filterDateCheckbox->isChecked()) {

        QString startDate = ui->m_filterDateStart->date().toString("yyyy-MM-dd");
        QString toDate = ui->m_filterDateEnd->date().toString("yyyy-MM-dd");

    //QString query = "SELECT * FROM " +  CtransactionTable::Object()->getTableName();
    //query += " WHERE (" + CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1) AND (";

        QString dateFilter = CtransactionTable::Object()->getColName(TRANSACTION_DATE_IDX) + " BETWEEN \"" + startDate  + "\" AND \"";
        dateFilter += toDate + "\""   ;
        QString open;
        QString close;
        if (m_filter.isEmpty() == false) {
            open = " AND (";
            close = ")";
        }
        m_filter += open + dateFilter + close;
    }

    emit accept();
}


void CfilterTransactionDlg::on_m_filterDateCheckbox_clicked()
{
    ui->m_filterDateStart->setEnabled(false);
    ui->m_filterDateEnd->setEnabled(false);
    if (ui->m_filterDateCheckbox->isChecked()) {
        ui->m_filterDateStart->setEnabled(true);
        ui->m_filterDateEnd->setEnabled(true);
    }
}

QString CfilterTransactionDlg::filter() const
{
    return m_filter;
}


void CfilterTransactionDlg::on_m_deptBox_currentTextChanged(const QString &arg1)
{
    if (arg1.isEmpty()) {
        ui->m_accountBox->clear();
        QStringList accounts = CaccountMap::Object()->getAccountList();
        accounts.sort();
        ui->m_accountBox->addItem("");
        ui->m_accountBox->addItems(accounts);
        ui->m_transTypeBox->clear();
        ui->m_transTypeBox->addItem("");
        QStringList types = CtransactionUtils::Object()->getAllTransctionTypeStr();
        types.sort();
        ui->m_transTypeBox->addItems(types);
        return;
    }
    ui->m_accountBox->clear();
    ui->m_accountBox->addItem("");
    int deptID = CaccountMap::Object()->getDeptId(arg1);
    QSet<int> accounts;
    std::set<QString> transType;
    if (CaccountMap::Object()->getAccounsForDept(deptID, accounts)) {

        for (auto id : accounts) {
            ui->m_accountBox->addItem(CaccountMap::Object()->getAccountName(id));
            QStringList types = CtransactionUtils::Object()->getTransactionTypesForAccountType(CaccountMap::Object()->getAccountType(id));
            types.sort();
            for (auto type: types) {
                transType.insert(type);
            }
        }
    }

    ui->m_transTypeBox->clear();
    ui->m_transTypeBox->addItem("");

    for (auto type: transType) {
        ui->m_transTypeBox->addItem(type);
    }
}


void CfilterTransactionDlg::on_m_accountBox_currentTextChanged(const QString &arg1)
{
    if (arg1.isEmpty()) {
        return;
    }
    QStringList types = CtransactionUtils::Object()->getTransactionTypesForAccountType(CaccountMap::Object()->getAccountType(arg1));
    ui->m_transTypeBox->clear();
    ui->m_transTypeBox->addItem("");

    for (auto type: types) {
        ui->m_transTypeBox->addItem(type);
    }

}

void CfilterTransactionDlg::setFilter(const QString &newFilter)
{
    m_filter = newFilter;
}

