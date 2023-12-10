#include "csmsdlg.h"
#include "ui_csmsdlg.h"
#include "ctransactionutils.h"
#include "ctransactiontable.h"
#include "ccontactmap.h"
#include "caccountmap.h"
#include "csdutils.h"
#include "MISC/cprintUtils.h"
#include "MISC/CdlgDefine.h"
CsmsDlg::CsmsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CsmsDlg)
{
    ui->setupUi(this);
    m_query = "";
    QDate date = QDate::currentDate();
    ui->m_fromDate->setDate(date);
    ui->m_toDate->setDate(date);
    resize();
}

CsmsDlg::~CsmsDlg()
{
    delete ui;
}

void CsmsDlg::resize()
{

    int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();

    int btnheight = BUTTON_SIZE.height();
    int btnweight = BUTTON_SIZE.width();
    int x = XCORD;
    int y = YCORD;

    ui->m_fromDate->setGeometry(x, y, DATE_SIZE.width(), DATE_SIZE.height());
    x += GAP + DATE_SIZE.width();
    ui->m_toDate->setGeometry(x, y, DATE_SIZE.width(), DATE_SIZE.height());
    x = XCORD;
    y += GAP  + DATE_SIZE.height();

    int width = windowwidth - 2*XCORD;
    int height = windowheight - y - YCORD - GAP - btnheight;

    ui->m_table->setGeometry(x, y, width, height);
    ui->m_table->horizontalHeader()->resizeSection(0, width*0.2);
    ui->m_table->horizontalHeader()->resizeSection(1, width*0.2);
    ui->m_table->horizontalHeader()->resizeSection(2, width*0.2);
    ui->m_table->horizontalHeader()->resizeSection(3, width*0.2);
    ui->m_table->horizontalHeader()->resizeSection(4, width*0.2);


    y = y + height + GAP;
    x = windowwidth/2  - btnweight - GAP/2;
    ui->m_save->setGeometry(x, y, btnweight, btnheight);
    x += btnweight + GAP;
    ui->m_cancel->setGeometry(x, y, btnweight, btnheight);

}

void CsmsDlg::populateTable()
{
    m_query = "";
    QString filter = "(" +CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1)";
    filter += " AND (" + CtransactionTable::Object()->getColName(TRANSACTION_TYPE_IDX) + "=" + QString::number(INCOME_TRANSACTION_TYPE) + ")";
    filter += "AND (";
    QString startdate = ui->m_fromDate->date().toString("yyyy-MM-dd");
    QString enddate = ui->m_toDate->date().toString("yyyy-MM-dd");

    QString dateFilter = CtransactionTable::Object()->getColName(TRANSACTION_DATE_IDX) + " BETWEEN \"" + startdate +"\"";
    dateFilter += " AND \"" + enddate +"\"";
    filter += dateFilter + ")";

    QString query = "SELECT * FROM " + CtransactionTable::Object()->getTableName() + " WHERE ";
    query += filter;
    m_query = query;
    QVector<StransactionData*> results = CtransactionTable::Object()->getAllTransaction(query);
    ui->m_table->clearContents();
    ui->m_table->setRowCount(0);

    for (auto trans : results) {
        int rowCount = ui->m_table->rowCount();
        ui->m_table->insertRow(rowCount);
        QDate date = trans->m_date;
        QString from = CcontactMap::Object()->getContactNameOnly(trans->m_fromId);
        QString account = CaccountMap::Object()->getAccountName(trans->m_accountId);
        QString amountLocalStr = CsdUtils::converAmountToString(trans->m_amount,'f',2);
        QString phone = CcontactMap::Object()->getContactPhone(trans->m_fromId);
        delete trans;

        //date
        QTableWidgetItem *itemDate = new QTableWidgetItem();
        itemDate->setFlags(itemDate->flags() & ~Qt::ItemIsEditable);
        itemDate->setText(date.toString("d MMM yyyy"));
        ui->m_table->setItem(rowCount, 0, itemDate);
        //from
        QTableWidgetItem *itemFrom = new QTableWidgetItem();
        itemFrom->setFlags(itemFrom->flags() & ~Qt::ItemIsEditable);
        itemFrom->setText(from);
        ui->m_table->setItem(rowCount, 1, itemFrom);

        //account
        QTableWidgetItem *itemAccount = new QTableWidgetItem();
        itemAccount->setFlags(itemAccount->flags() & ~Qt::ItemIsEditable);
        itemAccount->setText(account);
        ui->m_table->setItem(rowCount, 2, itemAccount);

        //amount
        QTableWidgetItem *itemAmount = new QTableWidgetItem();
        itemAmount->setFlags(itemAmount->flags() & ~Qt::ItemIsEditable);
        itemAmount->setText(amountLocalStr);
        ui->m_table->setItem(rowCount, 3, itemAmount);

        //phone
        QTableWidgetItem *itemPhone = new QTableWidgetItem();
        //itemAmount->setFlags(itemAmount->flags() & ~Qt::ItemIsEditable);
        itemPhone->setText(phone);
        ui->m_table->setItem(rowCount, 4, itemPhone);
    }


}

void CsmsDlg::on_m_fromDate_dateChanged(const QDate &date)
{
    populateTable();
}


void CsmsDlg::on_m_toDate_dateChanged(const QDate &date)
{
    populateTable();
}


void CsmsDlg::on_m_cancel_clicked()
{
    emit reject();
}


void CsmsDlg::on_m_save_clicked()
{
    QMap<QString, QMap<QString, QMap<QString, QVector<QString> > > > msgMap;

    QVector<StransactionData*> results = CtransactionTable::Object()->getAllTransaction(m_query);
    for (auto trans : results) {
        QString date = trans->m_date.toString("d MMM yyyy");
        QString from = CcontactMap::Object()->getContactNameOnly(trans->m_fromId);
        QString account = CaccountMap::Object()->getAccountName(trans->m_accountId);
        QString amountLocalStr = CsdUtils::converAmountToString(trans->m_amount,'f',2);
        QString phone = CcontactMap::Object()->getContactPhone(trans->m_fromId);
        QString remark = trans->m_particular;
        delete trans;
        if (remark.isEmpty() == false) {
            account += "(" + remark + ")";
        }
        if (phone.isEmpty()) {
            continue;
        }
        if (phone.length() == 10) {
            phone = "91" + phone;
        }
        QMap<QString, QMap<QString, QVector<QString> > >& nameMap  = msgMap[phone];
        QMap<QString, QVector<QString> > & dateMap = nameMap[from];
        QVector<QString>& msgs = dateMap[date];
        QString msg = "_#" + account + " - Rs " + amountLocalStr + "_";
        msgs.push_back(msg);

    }


    QStringList csvList;
    int count = 0;
    for (auto keyvaluePhone :msgMap.asKeyValueRange() ){
        ++count;
        QString msg;
        QString phone = keyvaluePhone.first;
        for (auto keyValueName : keyvaluePhone.second.asKeyValueRange()) {
            QString name = keyValueName.first;
            for (auto keyValueDate: keyValueName.second.asKeyValueRange()) {
                QString date = keyValueDate.first;
                if (msg.isEmpty() == false) {
                    msg += " \n-----------------------\n";
                }
                msg += "Kris a unau ";
                msg += "*" + name + "*";
                msg += ", Pathian gam nasepna ding a na thilpiak \n";
                for (auto amount: keyValueDate.second) {
                    msg += amount + " \r";
                }
                msg += date + " niin kipak tak in ka sang hi.\n";
            }
        }
        msg += "\nToupa'n hon vualzawl hen\n";
        msg += "*Sum saite* \n";
        msg += "*" + CsdUtils::getSaptuamMin() + "*";
        msg = QString::number(count) + "," + phone + ",\"" + msg + "\"\n";
        csvList.push_back(msg);
    }
    if (csvList.isEmpty() == false) {
        CprintUtils print(csvList);
        print.printCSV();
    }
    emit accept();
}

