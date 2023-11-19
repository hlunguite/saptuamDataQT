#include "cimportaccounttable.h"
/*    IMPORT_ACCOUNT_TABLE_IDX,
    IMPORT_ACCOUNT_IMPORTED_ID_IDX,
    IMPORT_ACCOUNT_TYPE_IDX,
    IMPORT_ACCOUNT_ID_IDX,
    IMPORT_ACCOUNT_NAME_IDX,
    IMPORT_ACCOUNT_TABLE_MAX_IDX*/

CimportAccountTable::CimportAccountTable():
 CdbTableBase(ACCOUNT_IMPORT_TABLE)
{
    addTableCol("importAccountTableID", FIELD_TYPE_PRIMARY_KEY, "Import ID", true, IMPORT_TABLE_START_NO);
    addTableCol("importAccountImportedID", FIELD_TYPE_INT, "Imported Account ID");
    addTableCol("importAccountType", FIELD_TYPE_INT, "Imported Account Type");
    addTableCol("importAccountID", FIELD_TYPE_INT, "Account ID");
    addTableCol("importAccountName", FIELD_TYPE_STRING, "Imported Account Name");
}

QVector<SimportAccountData *> *CimportAccountTable::getImportAccount(bool toprocess)
{

    QString whenstatement = " WHERE " +  getColName(IMPORT_ACCOUNT_ID_IDX);
    if (toprocess) {
        whenstatement += "=0";
    } else {
        whenstatement += ">0";

    }
    QString query = "SELECT * FROM " + getTableName() + whenstatement;
    QVector<SimportAccountData *> * dataV = nullptr;
    if (query.isEmpty() == false) {
        QSqlQuery q(getDataBase());
        if(q.exec(query)){
            dataV = new QVector<SimportAccountData *>;
            while(q.next()){
                SimportAccountData* data = new SimportAccountData;
                /* contact =  new ScontactData;
                contact->m_idx = q.value(CONTACT_ID_IDX).toInt();*/
                data->m_idx = q.value(IMPORT_ACCOUNT_TABLE_IDX).toInt();
                data->m_importIdx = q.value(IMPORT_ACCOUNT_IMPORTED_ID_IDX).toInt();
                data->m_accountType = q.value(IMPORT_ACCOUNT_TYPE_IDX).toInt();
                data->m_accountID = q.value(IMPORT_ACCOUNT_ID_IDX).toInt();
                data->m_accountName = q.value(IMPORT_ACCOUNT_NAME_IDX).toString();


                dataV->push_back(data);
            }

        }
    }

    return dataV;

}

int SimportAccountData::save()
{
    Tobjects data(IMPORT_ACCOUNT_TABLE_MAX_IDX);
    data[IMPORT_ACCOUNT_TABLE_IDX] =  m_idx;
    data[IMPORT_ACCOUNT_IMPORTED_ID_IDX] = m_importIdx;
    data[IMPORT_ACCOUNT_TYPE_IDX] = m_accountType;
    data[IMPORT_ACCOUNT_ID_IDX] = m_accountID;
    data[IMPORT_ACCOUNT_NAME_IDX] = m_accountName;

    if (m_idx == 0) {
        TfilterObjType filter;
        filter.push_back(qMakePair(IMPORT_ACCOUNT_IMPORTED_ID_IDX, m_importIdx));
        if (m_importIdx == 0 || CimportAccountTable::Object()->isValueExist(filter) == false) {
            m_idx = CimportAccountTable::Object()->insertInTable(data).toInt();
        }
    } else {
        for (int i = IMPORT_ACCOUNT_IMPORTED_ID_IDX; i < IMPORT_ACCOUNT_TABLE_MAX_IDX; ++i) {
            CimportAccountTable::Object()->updateValue(data.at(i), i , m_idx, IMPORT_ACCOUNT_TABLE_IDX);
        }
    }

    return m_idx;
}
