#include "cimportbanktransactiontable.h"
#include "cbanktransactiontable.h"



QVector<SimportBankTransactionData>
CimportBankTransactionTable::getAllBankImport()
{
    QVector<SimportBankTransactionData> imports;
    QSet<int> todelete;
    //QString whenstatement = " WHERE " +  getColName(BANK_TRANSACTION_TABLE_IMPORT_ID) +"=" + QString::number(importID);
    QString query = "SELECT * FROM " + getTableName();
    QSqlQuery q(getDataBase());
    if(q.exec(query)){
        while(q.next()){
            int id = q.value(IMPORT_BANK_TRANSACTION_TABLE_ID_IDX).toInt();
            QString dateRange = q.value(IMPORT_BANK_TRANSACTION_TABLE_DATE_RANGE_IDX).toString();
            QVector<SbanktransDetail> banks = CbankTransactionTable::Object()->getAllbankTransForSameImport(id);
            if (banks.isEmpty() == false) {
                SimportBankTransactionData data;
                data.m_id = id;
                data.m_dateRange = dateRange;
                imports.push_back(data);
            } else {
               todelete.insert(id);
            }
        }
    }
    for (int i : todelete) {
        deleteValue(i,IMPORT_BANK_TRANSACTION_TABLE_ID_IDX);
    }
    return imports;
}

CimportBankTransactionTable::CimportBankTransactionTable():
    CdbTableBase(IMPORT_BANK_TRANSACTION_TABLE)
{

    addTableCol("importBankTransactionTableID", FIELD_TYPE_PRIMARY_KEY, "ID", true, IMPORT_BANK_TRANSACTION_TABLE_START_NO);
    addTableCol("importBankTransactionTableDateRange", FIELD_TYPE_STRING, "date");
}

CimportBankTransactionTable::~CimportBankTransactionTable()
{

}

int SimportBankTransactionData::save()
{
    if (m_id == 0) {
        Tobjects obj;
        obj.push_back(m_id);
        obj.push_back(m_dateRange);
        m_id = CimportBankTransactionTable::Object()->insertInTable(obj).toInt();
        //qDebug()<<"Save import bank transaction table "<<m_id;
    }

    return m_id;
}

void SimportBankTransactionData::deleteObj()
{
    if (m_id > 0) {
        CimportBankTransactionTable::Object()->deleteValue(m_id,IMPORT_BANK_TRANSACTION_TABLE_ID_IDX);
        m_id = 0;
    }
}
