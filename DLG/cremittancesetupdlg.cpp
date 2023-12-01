#include "cremittancesetupdlg.h"
#include "ui_cremittancesetupdlg.h"
#include <QRadioButton>
#include <QDebug>
#include "MISC/CdlgDefine.h"
#include "cremittancesetuptable.h"
#include "caccountmap.h"
#include <QMessageBox>
CremittanceSetupDlg::CremittanceSetupDlg(QWidget *parent) :
    CdlgBase(parent),
    ui(new Ui::CremittanceSetupDlg)
{
    m_populateTable = false;
    ui->setupUi(this);
    QStringList remitList = CremittanceNameTable::Object()->getAllRemittanceName();
    remitList.sort();
    m_remitNameCompleter = new QCompleter(remitList, this);
    m_remitNameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_remitNameCompleter->setWrapAround(true);
    m_remitNameCompleter->setFilterMode(Qt::MatchContains);
    ui->m_remitNameLineEdit->setCompleter(m_remitNameCompleter);
    ui->m_remitNameList->addItem("");
    ui->m_remitNameList->addItems(remitList);
    resize();
    m_populateTable = true;
    populateTable();
}

CremittanceSetupDlg::~CremittanceSetupDlg()
{
    delete ui;
}

bool CremittanceSetupDlg::addEditRemittancePcForAccount(QString accountName, double localPC, double hqPC, bool isAdd, bool isEdit)
{
    int accountID = CaccountMap::Object()->getAccountID(accountName);
    if (isEdit) {
        SremittanceSetupTableData* setupData = CremittanceSetupTable::Object()->getRemittanceSetupForAccount(accountID);
        if (setupData) {
            int percentID =   setupData->m_percentageID;
            SremitrancePercentData* pcData = CremittancePercentTable::Object()->getRemittancePercentageForID(percentID);
            bool update = false;
            if (pcData) {
                if (hqPC != pcData->m_hqShare) {
                    update = true;
                }
                if (localPC !=  pcData->m_localShare ){
                    update = true;
                }
                delete pcData;
            }
            if (update) {
                if (localPC < 1 && hqPC < 1) {
                    int pcID = setupData->m_percentageID;
                    setupData->deleteObj();
                    CremittanceSetupTable::Object()->deleteREmitanceNameIfNotUSe(ui->m_remitNameList->currentText());
                    CremittanceSetupTable::Object()->deleteRemittanePCIfNotUSe(pcID);




                } else {
                    SremitrancePercentData pc;
                    pc.m_hqShare = hqPC;
                    pc.m_localShare = localPC;
                    int id = pc.save();
                    setupData->m_percentageID = id;
                    setupData->save();
                }

            }

            delete setupData;

        }
    } else if (isAdd) {
        SremitrancePercentData pc;
        pc.m_hqShare = hqPC;
        pc.m_localShare = localPC;
        int id = pc.save();
        SremittanceNameData nameData;
        nameData.m_name = ui->m_remitNameLineEdit->text().simplified();
        int nameid = nameData.save();
        SremittanceSetupTableData data;
        data.m_nameID = nameid;
        data.m_accountID = accountID;
        data.m_percentageID = id;
        data.save();



    }

}


void CremittanceSetupDlg::resizeEvent(QResizeEvent *)
{
    resize();
}

void CremittanceSetupDlg::resize()
{
    int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();

    int x = XCORD;//ui->m_accountTable->geometry().x();
    int y = YCORD;ui->m_accountTable->geometry().y();
    int btnWidth = BUTTON_SIZE.width();
    int btnheight = BUTTON_SIZE.height();
    int width = 2*DATE_SIZE.width();

    ui->m_remitNameLbl->setGeometry(x, y, width, DEFAULT_HEIGHT);
    x += GAP +  width;
    ui->m_orLbl->setGeometry(x, y, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    x += GAP + DEFAULT_WIDTH;
    ui->m_selectremitnalelbl->setGeometry(x, y, width, DEFAULT_HEIGHT);
    x = XCORD;
    y = YCORD + GAP + DEFAULT_HEIGHT;
    ui->m_remitNameLineEdit->setGeometry(x, y, width, DEFAULT_HEIGHT);
    x += GAP +  width + GAP + DEFAULT_WIDTH;
    ui->m_remitNameList->setGeometry(x, y,  width, DEFAULT_HEIGHT);
    x = XCORD;
    y += DEFAULT_HEIGHT + GAP;

    width = windowwidth - 2*XCORD;
    ui->m_selectAccountLbl->setGeometry(x, y, width, DEFAULT_HEIGHT);

    y += GAP + DEFAULT_HEIGHT;
    int height = windowheight  - y - GAP - btnheight - YCORD;

    ui->m_accountTable->setGeometry(x, y, width, height);
    ui->m_accountTable->horizontalHeader()->resizeSection(0, width*.25);
    ui->m_accountTable->horizontalHeader()->resizeSection(1, width*.10);
    ui->m_accountTable->horizontalHeader()->resizeSection(2, width*.10);
    ui->m_accountTable->horizontalHeader()->resizeSection(3, width*0.25);
    ui->m_accountTable->horizontalHeader()->resizeSection(4, width*.10);
    ui->m_accountTable->horizontalHeader()->resizeSection(5, width*0.10);


    y += height + GAP ;
    x = windowwidth/2 - btnWidth - GAP/2;
    ui->m_okBtn->setGeometry(x, y, btnWidth, btnheight);
    x += GAP + btnWidth;
    //ui->gridLayout->setGeometry(QRect(x, y, 2*btnWidth, btnheight));
    ui->m_cancelBtn->setGeometry(x, y, btnWidth, btnheight);


}

void CremittanceSetupDlg::populateTable()
{
    if (m_populateTable == false) {
        return;
    }
    ui->m_accountTable->clearContents();
    ui->m_accountTable->setRowCount(0);
    QString remitName = ui->m_remitNameList->currentText();
    QString newName = ui->m_remitNameLineEdit->text().simplified();
    if (remitName.isEmpty() == false && newName.isEmpty()){
        populateRemittance(remitName);
    } else if (remitName.isEmpty()) {
        populateAllAccount();
    }

}

void CremittanceSetupDlg::populateAllAccount()
{
    if (m_populateTable == false) {
        return;
    }

    //TODO also dlg for payment account
    QString msg = "Select account to add in new remittance list. Add percentage detail";
    ui->m_selectAccountLbl->setText(msg);
    QStringList accountWithHqReceipt = CaccountMap::Object()->getAccountListWithHqReceipt();
    accountWithHqReceipt.sort();
    int rowcount = 0;
    int col = 5;
    for (auto accountName : accountWithHqReceipt) {
        int accountID = CaccountMap::Object()->getAccountID(accountName);
        SremittanceSetupTableData* setupData = CremittanceSetupTable::Object()->getRemittanceSetupForAccount(accountID);
        float hqShare = 0;
        float localShare = 0;

        if (setupData) {
            delete setupData;
            continue;
            /*int percentID =   setupData->m_percentageID;
            SremitrancePercentData* pcData = CremittancePercentTable::Object()->getRemittancePercentageForID(percentID);
            if (pcData) {
                hqShare = pcData->m_hqShare;
                localShare = pcData->m_localShare;
                delete pcData;
            }
            delete setupData;*/

        }
        // display account name only if not assign any percentage
        if (col == 5) { // insert row
            col = -1;
            rowcount = ui->m_accountTable->rowCount();
            ui->m_accountTable->insertRow(rowcount);
        }
        QTableWidgetItem *rowAccount = new QTableWidgetItem();

        rowAccount->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        rowAccount->setCheckState(Qt::Unchecked);
        rowAccount->setText(accountName);
        //rowAccount->setFlags(Qt::ItemIsEditable | Qt::ItemIsUserCheckable);

        QTableWidgetItem * rowLocal = new QTableWidgetItem(tr("%1").arg(QString::number(localShare)));
        QTableWidgetItem * rowHQ = new QTableWidgetItem(tr("%1").arg(QString::number(hqShare)));
        ++col;
        ui->m_accountTable->setItem(rowcount, col, rowAccount);
        ++col;
        ui->m_accountTable->setItem(rowcount, col, rowLocal);
        ++col;
        ui->m_accountTable->setItem(rowcount, col, rowHQ);
    }


}

void CremittanceSetupDlg::populateRemittance(QString remitName)
{
    QString msg = "Select account to modify. Change the precentage. Set percentage for local and Hq share to zero if you want to remove";
    ui->m_selectAccountLbl->setText(msg);

    QVector<SremittanceSetupTableData*>* remitData = CremittanceSetupTable::Object()->getRemitanceSetupForName(remitName);
    if (remitData) {
        int rowcount = 0;
        int col = 5;
        for (auto data: *remitData) {
            int accountID = data->m_accountID;
            QString accountName = CaccountMap::Object()->getAccountName(accountID);
            int percentID = data->m_percentageID;
            delete data;
            float hqShare = 0;
            float localShare = 0;
            SremitrancePercentData* pcData = CremittancePercentTable::Object()->getRemittancePercentageForID(percentID);
            if (pcData) {
                hqShare = pcData->m_hqShare;
                localShare = pcData->m_localShare;
                delete pcData;
            }


            if (col == 5) { // insert row
                col = -1;
                rowcount = ui->m_accountTable->rowCount();
                ui->m_accountTable->insertRow(rowcount);
            }
            QTableWidgetItem *rowAccount = new QTableWidgetItem();

            rowAccount->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            rowAccount->setCheckState(Qt::Unchecked);
            rowAccount->setText(accountName);
            //rowAccount->setFlags(Qt::ItemIsEditable | Qt::ItemIsUserCheckable);

            QTableWidgetItem * rowLocal = new QTableWidgetItem(tr("%1").arg(QString::number(localShare)));
            QTableWidgetItem * rowHQ = new QTableWidgetItem(tr("%1").arg(QString::number(hqShare)));
            ++col;
            ui->m_accountTable->setItem(rowcount, col, rowAccount);
            ++col;
            ui->m_accountTable->setItem(rowcount, col, rowLocal);
            ++col;
            ui->m_accountTable->setItem(rowcount, col, rowHQ);
        }

        delete remitData;
    }


}

void CremittanceSetupDlg::on_m_remitNameLineEdit_editingFinished()
{   if (m_populateTable == false) {
        return;
    }
    m_populateTable = false;
    QString newName = ui->m_remitNameLineEdit->text().simplified();
    if (newName.isEmpty()) {
        ui->m_remitNameList->setEnabled(true);
        ui->m_remitNameList->setCurrentIndex(0);
    } else {
        ui->m_remitNameList->setCurrentIndex(0);
         ui->m_remitNameList->setEnabled(false);
    }
    m_populateTable = true;

    populateTable();
}


void CremittanceSetupDlg::on_m_remitNameList_currentTextChanged(const QString &arg1)
{
    if (m_populateTable == false) {
         return;
    }
    m_populateTable = false;
    if (arg1.isEmpty()) {
         ui->m_remitNameLineEdit->setEnabled(true);
         ui->m_remitNameLineEdit->setText("");
    } else {
         ui->m_remitNameLineEdit->setEnabled(false);
         ui->m_remitNameLineEdit->setText("");
    }
    m_populateTable = true;
   populateTable();
}


void CremittanceSetupDlg::on_m_cancelBtn_clicked()
{
   emit reject();
}


void CremittanceSetupDlg::on_m_okBtn_clicked()
{
   //SremittanceSetupTableData
    //            m_remitData = CremittanceSetupTable::Object()->getRemittanceSetupForAccount(m_accountIDForEdit);
   //SremittanceNameData
   //m_nameData = CremittanceNameTable::Object()->getRemittanceNameData(nameID);
   //SremitrancePercentData
//                m_pcData = CremittancePercentTable::Object()->getRemittancePercentageForID(percentID);

   bool isNew = false;
   bool isEdit = false;
   if (ui->m_remitNameLineEdit->text().simplified().isEmpty() && ui->m_remitNameList->currentText().isEmpty() == false) {
         isEdit = true;
   }
   if (ui->m_remitNameLineEdit->text().simplified().isEmpty() == false && ui->m_remitNameList->currentText().isEmpty()) {
         isNew = true;
   }

   if (isNew && isEdit) {
         QString msg = "Remittance canot be new and edit";
         QMessageBox::warning(this, "SaptuamData",
                              msg);
         return ;
   }
   if (isNew == false && isEdit == false) {
         QString msg = "Select remittance setup to modify";
         QMessageBox::warning(this, "SaptuamData",
                              msg);
         return ;
   }
   bool hasError = false;
   int numReceiptRow = ui->m_accountTable->rowCount();
   std::map<QString, std::pair<double, double> > selectedAccounts;
   for (int i = 0; i < numReceiptRow && hasError == false; ++i) {
         QTableWidgetItem * accountNameItem1 = ui->m_accountTable->item(i,0);
         if (accountNameItem1 && accountNameItem1->checkState() == Qt::Checked) {
            QString accountName = accountNameItem1->text();
            double localPC = 0;
            double hqPC = 0;
            int accountID = CaccountMap::Object()->getAccountID(accountName);
            if (accountID == 0) {
                hasError = true;
                QString msg = "Account " + accountName + " at row " + QString::number(i + 1) + " not correct";
                message(msg);
                continue;
            }
            QTableWidgetItem * localVal = ui->m_accountTable->item(i,1);
            QTableWidgetItem * hqVal = ui->m_accountTable->item(i,2);
            bool okLocal = false;
            bool okHq = false;
            if (localVal) {

                double value = localVal->text().toDouble(&okLocal);
                localPC = value;
            }
            if (hqVal) {

                double value = hqVal->text().toDouble(&okHq);
                hqPC = value;
            }
            if (okLocal && okHq) {
                int sum = localPC + hqPC;
                QString msg;
                if ((sum > 0 && sum < 100) || sum > 100) {
                    msg = "Sum of hq and local share at row " + QString::number(i + 1) + " is not equal to 100";
                }

                if (isNew && sum == 0) {
                    msg = "Sum of hq and local share at row " + QString::number(i + 1) + " is zero";

                }

                if (msg.isEmpty()) {
                    selectedAccounts[accountName] = std::make_pair(localPC, hqPC);
                } else {
                    hasError = true;

                    message(msg);
                }

            } else {
                if (okLocal == false) {
                    QString msg = "Local share at row " + QString::number(i + 1) + " is not number";
                    message(msg);
                    hasError = true;
                }
                if (okHq == false) {
                    QString msg = "Hq share at row " + QString::number(i + 1) + " is not number";
                    message(msg);
                    hasError = true;
                }
            }
         }
         QTableWidgetItem * accountNameItem2 = ui->m_accountTable->item(i,3);
         if (accountNameItem2 && accountNameItem2->checkState() == Qt::Checked) {
            QString accountName = accountNameItem1->text();
            int accountID = CaccountMap::Object()->getAccountID(accountName);
            if (accountID == 0) {
                hasError = true;
                QString msg = "Account " + accountName + " at row " + QString::number(i + 1) + " not correct";
                message(msg);
                continue;
            }
            QTableWidgetItem * localVal = ui->m_accountTable->item(i,4);
            QTableWidgetItem * hqVal = ui->m_accountTable->item(i,5);
            bool okLocal = false;
            bool okHq = false;
            double localPC = 0;
            double hqPC = 0;
            if (localVal) {

                double value = localVal->text().toDouble(&okLocal);
                localPC = value;
            }
            if (hqVal) {

                double value = hqVal->text().toDouble(&okHq);
                hqPC = value;
            }
            if (okLocal && okHq) {
                int sum = localPC + hqPC;
                QString msg;
                if ((sum > 0 && sum < 100) || sum > 100) {
                    msg = "Sum of hq and local share at row " + QString::number(i + 1) + " is not equal to 100";
                }

                if (isNew && sum == 0) {
                    msg = "Sum of hq and local share at row " + QString::number(i + 1) + " is zero";

                }

                if (msg.isEmpty()) {
                    selectedAccounts[accountName] = std::make_pair(localPC, hqPC);
                } else {
                    hasError = true;

                    message(msg);
                }

            } else {
                if (okLocal == false) {
                    QString msg = "Local share at row " + QString::number(i + 1) + " is not number";
                    message(msg);
                    hasError = true;
                }
                if (okHq == false) {
                    QString msg = "Hq share at row " + QString::number(i + 1) + " is not number";
                    message(msg);
                    hasError = true;
                }
            }
        }
   }
   if (hasError == false) {
        if (numReceiptRow > 0 && selectedAccounts.empty()) {
            QString msg = "No account selected";
            message(msg);
            return;
        }
    //std::map<QString, std::pair<double, double> > selectedAccounts
        for (auto selectAccount: selectedAccounts) {
            addEditRemittancePcForAccount(selectAccount.first, selectAccount.second.first,  selectAccount.second.second, isNew, isEdit);
        }

        emit accept();

   }

}

