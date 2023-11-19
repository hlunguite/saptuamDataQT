#include "cimporttransactiontable.h"
/*
    IMPORT_TRANS_TABLE_IDX,
    IMPORT_TRANS_DATE_IDX,
    IMPORT_TRANS_FROM_IDX,
    IMPORT_TRANS_FROM_ID_IDX,
    IMPORT_TRANS_ACCOUNT_IDX,
    IMPORT_TRANS_ACCOUNT_ID_IDX,
    IMPORT_TRANS_AMOUNT_IDX,
    IMPORT_TRANS_MODE_IDX,
    IMPORT_TRANS_REF_IDX,
    IMPORT_TRANS_RECEIPT_BOOK_IDX,
    IMPORT_TRANS_RECEIPT_NO_IDX,
    IMPORT_TRANS_REMARK_IDX,
    IMPORT_TRANS_TRANS_ID_IDX,
    IMPORT_TRANS_FULL_STR_IDX,
    IMPORT_TRANS_TABLE_MAX_IDX
*/

CimportTransactionTable::CimportTransactionTable():
    CdbTableBase(TRANS_IMPORT_TABLE)
{
    addTableCol("importTransTableID", FIELD_TYPE_PRIMARY_KEY, "Import ID", true, IMPORT_TABLE_START_NO);
    addTableCol("imortTransDate", FIELD_TYPE_DATE, "Import Date");
    addTableCol("imortTransFom", FIELD_TYPE_STRING, "Import From");
    addTableCol("imortTransFromID", FIELD_TYPE_INT, "Import From ID");
    addTableCol("imortTransAccount", FIELD_TYPE_STRING, "Import Account");
    addTableCol("imortTransAccountID", FIELD_TYPE_INT, "Import Account ID");
    addTableCol("imortTransAmount", FIELD_TYPE_FLOAT, "Import Amouunt");
    addTableCol("imortTransMode", FIELD_TYPE_INT, "Import Mode");
    addTableCol("imortTransRef", FIELD_TYPE_STRING, "Import Ref");
    addTableCol("imortTransReciptBook", FIELD_TYPE_STRING, "Import receipt book");
    addTableCol("imortTransReciptSl", FIELD_TYPE_STRING, "Import receipt Sl");
    addTableCol("imortTransRemark", FIELD_TYPE_STRING, "Import remark");
    addTableCol("imortTransID", FIELD_TYPE_INT, "Import ID");
    addTableCol("imortTransFullString", FIELD_TYPE_STRING, "Import FullString");


}

QVector<SimportTransData *>* CimportTransactionTable::getImportedTransaction(bool process)
{
    QString whenstatement = " WHERE " +  getColName(IMPORT_TRANS_TRANS_ID_IDX);
    if (process) {
        whenstatement += "=0";
    } else {
         whenstatement += ">0";
    }

    QString query = "SELECT * FROM " + getTableName() + whenstatement;
    QVector<SimportTransData *> * dataV = nullptr;
    if (query.isEmpty() == false) {
        QSqlQuery q(getDataBase());
        if(q.exec(query)){
            dataV = new QVector<SimportTransData *>;
            while(q.next()){
                SimportTransData* data = new SimportTransData;
                data->m_idx = q.value(IMPORT_TRANS_TABLE_IDX).toInt();
                data->m_date = q.value(IMPORT_TRANS_DATE_IDX).toDate();
                data->m_from = q.value(IMPORT_TRANS_FROM_IDX).toString();
                data->m_fromID = q.value(IMPORT_TRANS_FROM_ID_IDX).toInt();
                data->m_account = q.value(IMPORT_TRANS_ACCOUNT_IDX).toString();
                data->m_accountID = q.value(IMPORT_TRANS_ACCOUNT_ID_IDX).toInt();
                data->m_amount = q.value(IMPORT_TRANS_AMOUNT_IDX).toDouble();
                data->m_mode = q.value(IMPORT_TRANS_MODE_IDX).toInt();
                data->m_ref = q.value(IMPORT_TRANS_REF_IDX).toString();
                data->m_reciptBook = q.value(IMPORT_TRANS_RECEIPT_BOOK_IDX).toString();
                data->m_receiptNo = q.value(IMPORT_TRANS_RECEIPT_NO_IDX).toString();
                data->m_remark = q.value(IMPORT_TRANS_REMARK_IDX).toString();
                data->m_transID = q.value(IMPORT_TRANS_TRANS_ID_IDX).toInt();
                data->m_fullString = q.value(IMPORT_TRANS_FULL_STR_IDX).toString();
                dataV->push_back(data);


            }
        }
    }
    return dataV;
}

int SimportTransData::save()
{
    Tobjects data(IMPORT_TRANS_TABLE_MAX_IDX);
    data[IMPORT_TRANS_TABLE_IDX] =  m_idx;
    data[IMPORT_TRANS_DATE_IDX] =  m_date;
    data[IMPORT_TRANS_FROM_IDX] = m_from;
    data[IMPORT_TRANS_FROM_ID_IDX] = m_fromID;
    data[IMPORT_TRANS_ACCOUNT_IDX] = m_account;
    data[IMPORT_TRANS_ACCOUNT_ID_IDX] = m_accountID;
    data[IMPORT_TRANS_AMOUNT_IDX] = m_amount;
    data[IMPORT_TRANS_MODE_IDX] = m_mode;
    data[IMPORT_TRANS_REF_IDX] = m_ref;
    data[IMPORT_TRANS_RECEIPT_BOOK_IDX] = m_reciptBook;
    data[IMPORT_TRANS_RECEIPT_NO_IDX] = m_receiptNo;
    data[IMPORT_TRANS_REMARK_IDX] =  m_remark;
    data[IMPORT_TRANS_TRANS_ID_IDX] = m_transID;
    data[IMPORT_TRANS_FULL_STR_IDX] = m_fullString;

    if (m_idx == 0) {
        TfilterObjType filter;
        filter.push_back(qMakePair(IMPORT_TRANS_FULL_STR_IDX, m_fullString));
        if (CimportTransactionTable::Object()->isValueExist(filter) == false || m_fullString.isEmpty()) {
            m_idx = CimportTransactionTable::Object()->insertInTable(data).toInt();
        }
    } else {
        for (int i = IMPORT_TRANS_DATE_IDX; i < IMPORT_TRANS_TABLE_MAX_IDX; ++i) {
            CimportTransactionTable::Object()->updateValue(data.at(i), i , m_idx, IMPORT_TRANS_TABLE_IDX);
        }
    }

    return m_idx;
}
