#include "cpaymentaccounttable.h"
#include <QVector>
#include <QString>
#include "caccounttable.h"

CpaymentAccountTable::CpaymentAccountTable():
    CdbTableBase(PAYMENT_ACCOUNT_TABLE)
{
    addTableCol("paymentAccountID", FIELD_TYPE_PRIMARY_KEY, "ID", true, PAYMENT_ACCOUNT_TABLE_START_NO);
    addTableCol("paymentAccountAccountName", FIELD_TYPE_STRING, "Account Name");
    addTableCol("paymentAccountName", FIELD_TYPE_STRING,"Payment Account Name");
    addTableCol("paymentAcountNatureOfExpense", FIELD_TYPE_STRING, "Description");


}

CpaymentAccountTable::~CpaymentAccountTable()
{

}

SpaymentAccountData *CpaymentAccountTable::getPaymentAccountDataForId(int id)
{
    SpaymentAccountData* data = nullptr;
    if (id > 0) {
        QString tosearch = QString::number(id);
        QString condition = getSearchString(tosearch, PAYMENT_ACCOUNT_ID_IDX);
        TobjectList values;
        if (getTableValues(condition, values)) {
            const Tobjects& obj = values.at(0);
            data = new SpaymentAccountData;
            data->m_id = obj.at(PAYMENT_ACCOUNT_ID_IDX).toInt();
            data->m_paymentAccountName = obj.at(PAYMENT_ACCOUNT_ACCOUNT_NAME_IDX).toString();
            data->m_accountName = obj.at(PAYMENT_ACCOUNT_NAME_IDX).toString();
            data->m_natureOfExpense = obj.at(PAYMENT_ACCOUNT_NATURE_OF_EXPENSE_IDX).toString();

        }
    }

    return data;
}

SpaymentAccountData *CpaymentAccountTable::getPaymentAccountDataForPaymentAccountName(QString name)
{


    SpaymentAccountData* data = nullptr;
    if (name.isEmpty() == false) {

        QString condition = getSearchString(name, PAYMENT_ACCOUNT_NAME_IDX);
        TobjectList values;
        if (getTableValues(condition, values)) {
            const Tobjects& obj = values.at(0);
            data = new SpaymentAccountData;
            data->m_id = obj.at(PAYMENT_ACCOUNT_ID_IDX).toInt();
            data->m_paymentAccountName = obj.at(PAYMENT_ACCOUNT_ACCOUNT_NAME_IDX).toString();
            data->m_accountName = obj.at(PAYMENT_ACCOUNT_NAME_IDX).toString();
            data->m_natureOfExpense = obj.at(PAYMENT_ACCOUNT_NATURE_OF_EXPENSE_IDX).toString();

        }
    }

    return data;
}

void CpaymentAccountTable::serializeTable(QDataStream &out, const Tobjects &obj)
{
    SpaymentAccountData data;
    data.m_id = obj.at(PAYMENT_ACCOUNT_ID_IDX).toInt();
    data.m_paymentAccountName = obj.at(PAYMENT_ACCOUNT_ACCOUNT_NAME_IDX).toString();
    data.m_accountName = obj.at(PAYMENT_ACCOUNT_NAME_IDX).toString();
    data.m_natureOfExpense = obj.at(PAYMENT_ACCOUNT_NATURE_OF_EXPENSE_IDX).toString();
    data.serialize(out);

}

void CpaymentAccountTable::desserializeTable(QDataStream &in)
{
    SpaymentAccountData data;
    data.desserialize(in);
}


QSqlError
CpaymentAccountTable::createTable()
{
    QSqlError error = CdbTableBase::createTable();
    if (error.isValid()) {
        return error;
    }
    // pre populate table
    QVector<QString> paymentAccountVec;
    paymentAccountVec.push_back("Church Growth,EXP/B/1(01.8) Church Growth,Local Project ding toh kisai a HQ a Loan/Advance khempeuh");
    paymentAccountVec.push_back("Project Support,EXP/B/1(01.5) Project Support,");
    paymentAccountVec.push_back("Assistance/Social Care,EXP/B/1(02.2) Assistance/Social Care,Kithuahpihna; Panpihna leh Departmental Aid");
    paymentAccountVec.push_back("Vehicle Mainteance,EXP/B/1(03.1) Vehicle Mainteance,Local Saptuam Gari maintenance a zat ding");
    paymentAccountVec.push_back("Printing & Stationery,EXP/B/1(04.1) Printing & Stationery,Laisutman taw kisai");
    paymentAccountVec.push_back("Computer Maintenance,EXP/B/1(04.4) Computer Maintenance,Computer taw kisan himhim te");
    paymentAccountVec.push_back("Meetings,EXP/B/1(04.5) Meetings,Meeting toh kisai a sum senna te");
    paymentAccountVec.push_back("Hospitality/Staff Canteen,EXP/B/1(04.6) Hospitality/Staff Canteen,Refreshment");
    paymentAccountVec.push_back("Miscellaneous,EXP/B/1(04.8) Miscellaneous,Hiai ah expense head a om lou senna tampi hilou te");
    paymentAccountVec.push_back("Tel/Elect/Water Fees etc,EXP/B/1(05.1) Tel/Elect/Water Fees etc,Fees piak ding toh kisai himhimte");
    paymentAccountVec.push_back("Bank Charges,EXP/B/1(05.3) Bank Charges,Bank toh kisai a senna te");
    paymentAccountVec.push_back("Generator Maintenance,EXP/B/1(06.2) Generator Maintenance, Generator toh kisai a sennate");
    paymentAccountVec.push_back("Sound System,EXP/B/2(01.3) Sound System,Saptuam sound system toh kisai a senna te himhim.");
    paymentAccountVec.push_back("Missions Nasepna,EXP/B/1(02.5) Missions Nasepna,Local in Missionary vakna dia apiak te gelh pawtna ding");
    paymentAccountVec.push_back("Gaalna,EXP/B/2(05.1)  Gaalna,");
    paymentAccountVec.push_back("Local Church Supports,EXP/B/2(05.2) Local Church Supports,Expense related to maintenance of pastor quarters and local church buldings");
    paymentAccountVec.push_back("Pastoral Care,EXP/B/2(05.3) Pastoral Care,Seminar; Crusade; Assistance and expense of Local Pastor");
    paymentAccountVec.push_back("Contingencies,EXP/B/2(08.1) Contingencies,Gel kholh louh a senna om thei te hiai a gelh lut ding");
    paymentAccountVec.push_back("Honorarium,EXP/B/2(09.1) Honorarium,Honorarium paid every month to volunteers under ministries");
    paymentAccountVec.push_back("Office Equipment,EXP/B/3(09.1) Office Equipment,Local Office toh kisai a sum senna khem peuh gelh lutna ding");
    paymentAccountVec.push_back(",EXP/B/2(03.1) Baptist Children Department,Tualsung Baptist Children Department te sum zatna");
    paymentAccountVec.push_back(",EXP/B/2(03.2) Baptist Dorcas Committee,Tualsung Baptist Dorcas Committee te sum zatna");
    paymentAccountVec.push_back(",EXP/B/2(03.3) Baptist Youth Fellowship,Tualsung Baptist Youth Fellowship Department te sum zatna");
    paymentAccountVec.push_back(",EXP/B/2(03.4) Baptist Missions Committee,Tualsung Baptist Missions Committee te sum zatna");
    paymentAccountVec.push_back("Centenary Hall Project,EXP/B/2(03.5) Centenary Hall Project,");
    paymentAccountVec.push_back("Jubilee Prayer Hall,EXP/B/2(03.6) Jubilee Prayer Hall,");
    unsigned int size = paymentAccountVec.size();

    for (unsigned int i = 0; i < size; ++i) {
        QString line = paymentAccountVec.at(i);
        QStringList paymentAccounts = line.split(",");


        SpaymentAccountData data;
        data.m_id = 0;
        data.m_accountName = paymentAccounts.at(0).trimmed().simplified();
        data.m_paymentAccountName = paymentAccounts.at(1).trimmed().simplified() ;
        data.m_natureOfExpense = paymentAccounts.at(2).trimmed().simplified();
        data.save();
    }
    return QSqlError();

}


int SpaymentAccountData::save()
{

    Tobjects data(PAYMENT_ACCOUNT_MAX_IDX);

    data[PAYMENT_ACCOUNT_ID_IDX] = m_id;
    data[PAYMENT_ACCOUNT_ACCOUNT_NAME_IDX] = m_accountName;
    data[PAYMENT_ACCOUNT_NAME_IDX] = m_paymentAccountName;
    data[PAYMENT_ACCOUNT_NATURE_OF_EXPENSE_IDX] = m_natureOfExpense;

    if (m_id == 0) {
        TfilterObjType filter;
        if (data.at(PAYMENT_ACCOUNT_ACCOUNT_NAME_IDX).toString().isEmpty()) {
            filter.push_back(qMakePair(PAYMENT_ACCOUNT_NAME_IDX, data.at(PAYMENT_ACCOUNT_NAME_IDX)));
        } else {
            filter.push_back(qMakePair(PAYMENT_ACCOUNT_ACCOUNT_NAME_IDX, data.at(PAYMENT_ACCOUNT_ACCOUNT_NAME_IDX)));
        }
        if (CpaymentAccountTable::Object()->isValueExist(filter) == false) {
            m_id = CpaymentAccountTable::Object()->insertInTable(data).toInt();
        }
    } else {
        for (int i = PAYMENT_ACCOUNT_ACCOUNT_NAME_IDX; i < PAYMENT_ACCOUNT_MAX_IDX; ++i) {
            CpaymentAccountTable::Object()->updateValue(data.at(i), i, m_id, PAYMENT_ACCOUNT_ID_IDX );
        }
        CaccountTable::Object()->updateValue(data[PAYMENT_ACCOUNT_ACCOUNT_NAME_IDX], ACCOUNT_NAME_IDX, m_id, ACCOUNT_PAYMENT_ACCOUNT_ID_IDX);
    }
    return m_id;
}

void SpaymentAccountData::deleteObj()
{
    assert(0);
}

void SpaymentAccountData::serialize(QDataStream &out)
{
    out<<m_id<<m_accountName<<m_paymentAccountName<<m_natureOfExpense;
    //qDebug()<<"dumping "<<m_id<<" "<<m_paymentAccountName;
}

void SpaymentAccountData::desserialize(QDataStream &in)
{
    in>>m_id>>m_accountName>>m_paymentAccountName>>m_natureOfExpense;
    //qDebug()<<"reading "<<m_id<<" "<<m_paymentAccountName;

}
