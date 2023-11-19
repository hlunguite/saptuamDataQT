#include "cimportaccountdlg.h"
#include "ui_cimportaccountdlg.h"
#include <QRegularExpression>
#include "caccounttable.h"
#include <QFileDialog>
#include "csdutils.h"
#include "cnewaccountdlg.h"
CimportAccountDlg::CimportAccountDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CimportAccountDlg)
{
    ui->setupUi(this);
    m_importData = nullptr;
    populateTable();
}

CimportAccountDlg::~CimportAccountDlg()
{
    clear();
    delete ui;
}

void CimportAccountDlg::resize()
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
    ui->m_table->horizontalHeader()->resizeSection(0,width*.20);
    ui->m_table->horizontalHeader()->resizeSection(1,width*.40);
    ui->m_table->horizontalHeader()->resizeSection(2,width*.20);
    ui->m_table->horizontalHeader()->resizeSection(3,width*.20);

    y = y + height + (1.5* buttonheight);
    x = windowwidth/2 - buttonwidth - 1;
    ui->m_processBtn->setGeometry(x, y, buttonwidth, buttonheight);
    x += buttonwidth + 2;
    ui->m_cancelBtn->setGeometry(x, y, buttonwidth, buttonheight);
}

void CimportAccountDlg::clear()
{
    //QVector<SimportAccountData *> * m_importData;
    if (m_importData) {
        for (auto data : *m_importData) {
            delete data;
        }
        delete m_importData;
    }

}

void CimportAccountDlg::populateTable()
{
    clear();
    m_importData = CimportAccountTable::Object()->getImportAccount();
    ui->m_table->clearContents();
    ui->m_table->setRowCount(0);
    if (m_importData == nullptr) {
        return;
    }
    int size = m_importData->size();
    for (int idx = 0; idx < size; ++idx) {
        SimportAccountData* data = m_importData->at(idx);
        QVector<QTableWidgetItem*> cells(4);
        cells[0] = new QTableWidgetItem(tr("%1").arg(QString::number(data->m_importIdx)));
        cells[1] = new QTableWidgetItem(tr("%1").arg(data->m_accountName));
        cells[2] = new QTableWidgetItem(tr("%1").arg(CaccountUtils::Object()->getAccountType( data->m_accountType)));
        cells[3] = new QTableWidgetItem(tr("%1").arg(QString::number(data->m_accountID)));
        int row = ui->m_table->rowCount();
        ui->m_table->insertRow(row);
        for (int i = 0; i < 4; ++i) {
            ui->m_table->setItem(row, i, cells[i]);
        }

    }


}

void CimportAccountDlg::on_m_processBtn_clicked()
{
    populateTable();
    if (m_importData == nullptr) {
        return;
    }
    int size = m_importData->size();
    if (size == 0) {
        return;
    }
    int index = 0;

    while(true) {

        CnewAccountDlg editAccountDlg(true);
        editAccountDlg.setWindowTitle("Import Account");
        SimportAccountData* data = m_importData->at(index);
        editAccountDlg.setAccountToImport(data);
        if (editAccountDlg.exec()) {
            int id = editAccountDlg.accountID();
            if (id > 0) {
                data->m_accountID = id;
                data->save();

                 QTableWidgetItem* accountTableID = ui->m_table->item(index, 3);
                 if (accountTableID) {
                     accountTableID->setText(QString::number(id));

                 }

                emit updateAccountTab();
            }

        } else {
            break;
        }
        ++index;
        if (index >= size) {
            break;
        }


    }



}


void CimportAccountDlg::on_m_cancelBtn_clicked()
{
    emit reject();
}


void CimportAccountDlg::on_m_selectBtn_clicked()
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

    QTextStream in(&file);

    QString line ;
    while (!in.atEnd()) {
        line = in.readLine();
        if (line.contains("Account Name")) {
            continue;
        }

        QRegularExpression rx("(\\,)");
        QStringList query = line.split(rx);
        if (query.size() < 3) {
            continue;
        }
        QString id = query.at(0).trimmed().simplified();
        QString importAccName = query.at(1).trimmed().simplified();
        QString importAccType = query.at(2).trimmed().simplified();
        int type = INVALID_ACOUNT_TYPE;
        if (importAccType.contains("income", Qt::CaseInsensitive)) {
            type = INCOME_ACCOUNT_TYPE;
        } else if (importAccType.contains("payment", Qt::CaseInsensitive)) {
            type = PAYMENT_ACCOUNT_TYPE;
        } else if (importAccType.contains("bank", Qt::CaseInsensitive)) {
            type = BANK_ACCOUNT_TYPE;
        } else {
            Q_ASSERT(0);
        }
        /*m_accountTypeIdStringMap.insert(INCOME_ACCOUNT_TYPE, "Income Account");
        m_accountTypeIdStringMap.insert(PAYMENT_ACCOUNT_TYPE, "Payment Account");
        m_accountTypeIdStringMap.insert(BANK_ACCOUNT_TYPE, "Bank Account");
        m_accountTypeIdStringMap.insert(REMITTANCE_ACCOUNT_TYPE, "Remittance Account");
        m_accountTypeIdStringMap.insert(REQUEST_ACCOUNT_TYPE, "Request Account");
        m_accountTypeIdStringMap.insert(LOAN_ACCOUNT_TYPE, "Loan Account");*/
        //qDebug()<<id<<" "<<importAccName<<" "<<importAccType<<" type "<<type;
        SimportAccountData account;
        account.m_idx = 0;
        account.m_accountID = 0;
        account.m_importIdx = id.toInt();
        account.m_accountName = importAccName;
        account.m_accountType = type;
        account.save();

    }
    file.close();
    populateTable();
}

