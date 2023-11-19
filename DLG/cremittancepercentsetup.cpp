#include "cremittancepercentsetup.h"
#include "ui_cremittancepercentsetup.h"
#include "caccountmap.h"
#include "cremittancesetuptable.h"
#include <QDebug>
CremittancePercentSetup::CremittancePercentSetup(QWidget *parent) :
     CdlgBase(parent),
    ui(new Ui::CremittancePercentSetup), m_construct(true)
{
    ui->setupUi(this);

    int width = ui->m_percentTable->geometry().width();
    ui->m_percentTable->horizontalHeader()->resizeSection(0,width*0.0);
    ui->m_percentTable->horizontalHeader()->resizeSection(1,width*0.4);
    ui->m_percentTable->horizontalHeader()->resizeSection(2,width*0.3);
    ui->m_percentTable->horizontalHeader()->resizeSection(3,width*0.3);
    const QStringList& depts = CaccountMap::Object()->getDeptList();
    ui->m_deptComboBox->addItem("");
    ui->m_deptComboBox->addItems(depts);

    const QStringList& accounts = CaccountMap::Object()->getAccountListWithHqReceipt();
    ui->m_accountComboBox->addItem("");
    ui->m_accountComboBox->addItems(accounts);
    for (QString acc: accounts) {
        m_accountWithHqreceipt.insert(acc);
    }

    for (QString dept: depts ) {
        int deptID = CaccountMap::Object()->getDeptId(dept);
        QSet<int> accountIds;

        if (CaccountMap::Object()->getAccounsForDept(deptID, accountIds)) {
            QStringList& accountWithHqReceipt = m_deptAccounts[dept];
            for (int id : accountIds) {
                QString accountName = CaccountMap::Object()->getAccountName(id);
                if (m_accountWithHqreceipt.contains(accountName)) {
                    accountWithHqReceipt.push_back(accountName);
                }
            }
            accountWithHqReceipt.sort();
        }
    }
    QStringList remitList = CremittanceNameTable::Object()->getAllRemittanceName();
    m_remitNameCompleter = new QCompleter(remitList, this);
    m_remitNameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_remitNameCompleter->setWrapAround(true);
    m_remitNameCompleter->setFilterMode(Qt::MatchContains);
    ui->m_remitName->setCompleter(m_remitNameCompleter);
    m_construct = false;
    populateTable();
}

CremittancePercentSetup::~CremittancePercentSetup()
{
    delete ui;
}

void CremittancePercentSetup::on_m_deptComboBox_currentTextChanged(const QString &arg1)
{
    if (m_construct == false) {
        m_construct = true;
        ui->m_accountComboBox->clear();
        m_construct = false;
        ui->m_accountComboBox->addItem("");
        if (arg1.isEmpty()) {
            const QStringList& accounts = CaccountMap::Object()->getAccountListWithHqReceipt();
            ui->m_accountComboBox->addItems(accounts);
        } else {
            QStringList accountWithHqReceipt = m_deptAccounts.value(arg1);
            ui->m_accountComboBox->addItems(accountWithHqReceipt);

        }
    }

}


void CremittancePercentSetup::on_m_accountComboBox_currentTextChanged(const QString &arg1)
{
    if (m_construct == false) {
        populateTable();
    }

}

void CremittancePercentSetup::populateTable()
{/*
    ui->m_percentTable->clearContents();
    ui->m_percentTable->setRowCount(0);
    QString dept = ui->m_deptComboBox->currentText();
    QString account = ui->m_accountComboBox->currentText();
    QString remitName = ui->m_remitName->text().simplified();
    if (remitName.isEmpty()) {
        return;
    } else {
        SremittanceNameData* nameData = CremittanceNameTable::Object()->getRemittanceNameData(remitName);
        if (nameData) {
            m_construct = true;
            ui->m_deptComboBox->setCurrentText("");
            ui->m_accountComboBox->setCurrentText("");
            m_construct = false;
            int nameid = nameData->m_id;
            delete namedata;

        }

    }
    QStringList toaddAccount;
    if (dept.isEmpty()) {
        if (account.isEmpty()) {
            toaddAccount << CaccountMap::Object()->getAccountListWithHqReceipt();
        } else {
            toaddAccount<< account;
        }
    } else {
        if (account.isEmpty()) {
            toaddAccount <<m_deptAccounts.value(dept);
        } else {
            toaddAccount<< account;
        }
    }

   for (QString accountName : toaddAccount) {
       int rowcount = ui->m_percentTable->rowCount();
       ui->m_percentTable->insertRow(rowcount);
       int accountID = CaccountMap::Object()->getAccountID(accountName);
       SremitrancePercentData* data = CremittancePercentTable::Object()->getRemittancePercentageForAccount(accountID);

       QTableWidgetItem *id = new QTableWidgetItem();
       QTableWidgetItem *account = new QTableWidgetItem();
       QTableWidgetItem *localShare = new QTableWidgetItem();
       QTableWidgetItem *hqShare = new QTableWidgetItem();

       account->setText(accountName);
       account->setFlags(account->flags() & ~Qt::ItemIsEditable);

       if (data) {
           id->setText(QString::number(data->m_id));
           localShare->setText(QString::number(data->m_localShare));
           hqShare->setText(QString::number(data->m_hqShare));
           delete data;
       } else {
           id->setText("0");
       }
       ui->m_percentTable->setItem(rowcount, 0, id);
       ui->m_percentTable->setItem(rowcount, 1, account);
       ui->m_percentTable->setItem(rowcount, 2, localShare);
       ui->m_percentTable->setItem(rowcount, 3, hqShare);
   }*/
}


void CremittancePercentSetup::on_m_okBtn_clicked()
{
    Q_ASSERT(0);
    /*int rowcount = ui->m_percentTable->rowCount();

    for (int row = 0; row < rowcount; ++row) {
        QTableWidgetItem * id = ui->m_percentTable->item(row,0);
        QTableWidgetItem * account = ui->m_percentTable->item(row,1);
        QTableWidgetItem * localShare = ui->m_percentTable->item(row,2);
        QTableWidgetItem * hqShare = ui->m_percentTable->item(row,3);
        bool ok;
        int tableid = id->text().trimmed().toInt(&ok);
        if (ok == false) {
            tableid = 0;
        }
        int accountId = CaccountMap::Object()->getAccountID(account->text().trimmed());
        if (tableid > 0 && accountId == 0) {
            message("Wrong account name " + account->text().trimmed());
            return;
        }

        int localShareValue = localShare->text().trimmed().toInt(&ok);
        if (ok == false) {
            localShareValue = 0.0;
        }

        int hqShareValue = hqShare->text().trimmed().toInt(&ok);
        if (ok == false) {
            hqShareValue = 0.0;
        }
        int total = localShareValue + hqShareValue;
        //qDebug()<<"Total is "<<total;
        SremitrancePercentData* data = CremittancePercentTable::Object()->getRemittancePercentageForAccount(accountId);
        if (data) {
            if (data->m_id != tableid) {
                message("Wrong account name " + account->text().trimmed());
                return;
            }
            if ((total != 100) && (total != 0.0)) {
                QString msg = "Total for " + account->text() + " should equal 100";
                message(msg);
                return;
            }

            if (data->m_hqShare == hqShareValue && data->m_localShare == localShareValue) {
                delete data;
                continue;
            }

            data->m_hqShare = hqShareValue;
            data->m_localShare = localShareValue;
            data->save();
            delete data;

        } else {
            if (accountId == 0 || tableid > 0) {
                message("Wrong account name " + account->text().trimmed());
                return;
            }
            if (total == 0.0) {
                continue;
            }
            if (total == 100) {
                SremitrancePercentData pct;
                pct.m_id = 0;
                pct.m_hqShare = hqShareValue;
                pct.m_localShare = localShareValue;
                pct.save();

            } else {
                QString msg = "Total for " + account->text() + " should equal 100";
                message(msg);
                return ;
            }
        }

    }

    emit accept();*/

}


void CremittancePercentSetup::on_m_cancelBtn_clicked()
{
    emit reject();
}


void CremittancePercentSetup::on_m_remitName_editingFinished()
{
    QString name = ui->m_remitName->text();
    if (name.isEmpty()) {
        return;
    }
    populateTable();
}

