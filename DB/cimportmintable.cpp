#include "cimportmintable.h"


CimportMinTable::CimportMinTable():
    CdbTableBase(MIN_IMPORT_TABLE)
{
    addTableCol("importMinTableID", FIELD_TYPE_PRIMARY_KEY, "Contact ID", true, IMPORT_TABLE_START_NO);
    addTableCol("importMinID", FIELD_TYPE_INT, "Import ID");
    addTableCol("importMinTitle", FIELD_TYPE_STRING, "Import Title");
    addTableCol("importMinFirstName", FIELD_TYPE_STRING, "Import First Name");
    addTableCol("importMinLastName", FIELD_TYPE_STRING, "Import Last");
    addTableCol("importMinRelationShip", FIELD_TYPE_STRING, "Import Relationship");
    addTableCol("importMinParentFirstName", FIELD_TYPE_STRING, "Import Parent First Name");
    addTableCol("importMinParentLastName", FIELD_TYPE_STRING, "Import Parent Last");
    addTableCol("importMinPhone", FIELD_TYPE_STRING, "Import Phone");
    addTableCol("importMinBankName", FIELD_TYPE_STRING, "Import Bank Name");
    addTableCol("importMinContactTableID", FIELD_TYPE_INT, "Import Contact Table ID");
    addTableCol("importMinIsPerson", FIELD_TYPE_BOOL, "is Person");
    addTableCol("importMinTransRemark", FIELD_TYPE_STRING, "Trans remark");

}

QVector<SimportMinData *> *CimportMinTable::getImportedMin(bool toprocess)
{
    QString whenstatement = " WHERE " +  getColName(IMPORT_MIN_CONTACT_TABLE_INDEX_IDX);
    if (toprocess) {
        whenstatement += "=0";
    } else {
        whenstatement += ">0";

    }
    QString query = "SELECT * FROM " + getTableName() + whenstatement;
    QVector<SimportMinData *> * dataV = nullptr;
    if (query.isEmpty() == false) {
        QSqlQuery q(getDataBase());
        if(q.exec(query)){
            dataV = new QVector<SimportMinData *>;
            while(q.next()){
                SimportMinData* data = new SimportMinData;
               /* contact =  new ScontactData;
                contact->m_idx = q.value(CONTACT_ID_IDX).toInt();*/
                data->m_idx = q.value(IMPORT_MIN_TABLE_IDX).toInt();
                data->m_importIdx = q.value(IMPORT_MIN_INDEX_IDX).toInt();
                data->m_title = q.value(IMPORT_MIN_TITLE_IDX).toString();
                data->m_firstName = q.value(IMPORT_MIN_FIRSTNAME_IDX).toString();
                data->m_lastName = q.value(IMPORT_MIN_LASTNAME_IDX).toString();
                data->m_relationship = q.value(IMPORT_MIN_RELATIONSHIP_IDX).toString();
                data->m_parentFirstName = q.value(IMPORT_MIN_PARENT_FIRST_NAME_IDX).toString();
                data->m_parentLastName = q.value(IMPORT_MIN_PARENT_LAST_NAME_IDX).toString();
                data->m_phone = q.value(IMPORT_MIN_PHONE_IDX).toString();
                data->m_bankName = q.value(IMPORT_MIN_BANK_NAME_IDX).toString();
                data->m_contactTableIdx = q.value(IMPORT_MIN_CONTACT_TABLE_INDEX_IDX).toInt();
                data->m_isPerson = q.value(IMPORT_MIN_IS_PERSON_IDX).toBool();
                data->m_transRemark = q.value(IMPORT_MIN_TRANS_REMARK_IDX).toString();
                dataV->push_back(data);
            }

        }
    }

    return dataV;

}

int SimportMinData::save()
{

    Tobjects data(IMPORT_MIN_TABLE_MAX_IDX);
    data[IMPORT_MIN_TABLE_IDX] = m_idx;
    data[IMPORT_MIN_INDEX_IDX] = m_importIdx;
    data[IMPORT_MIN_TITLE_IDX] = m_title;
    data[IMPORT_MIN_FIRSTNAME_IDX] = m_firstName;
    data[IMPORT_MIN_LASTNAME_IDX] = m_lastName;
    data[IMPORT_MIN_RELATIONSHIP_IDX] = m_relationship;
    data[IMPORT_MIN_PARENT_FIRST_NAME_IDX] = m_parentFirstName;
    data[IMPORT_MIN_PARENT_LAST_NAME_IDX] = m_parentLastName;
    data[IMPORT_MIN_PHONE_IDX] = m_phone;
    data[IMPORT_MIN_BANK_NAME_IDX] = m_bankName;
    data[IMPORT_MIN_CONTACT_TABLE_INDEX_IDX] = m_contactTableIdx;
    data[IMPORT_MIN_IS_PERSON_IDX] = m_isPerson;
    data[IMPORT_MIN_TRANS_REMARK_IDX] = m_transRemark;

    if (m_idx == 0) {
        TfilterObjType filter;
        filter.push_back(qMakePair(IMPORT_MIN_INDEX_IDX, m_importIdx));
        if (m_importIdx == 0 || CimportMinTable::Object()->isValueExist(filter) == false) {
            m_idx = CimportMinTable::Object()->insertInTable(data).toInt();
        }

    } else {
        for (int i = IMPORT_MIN_INDEX_IDX; i < IMPORT_MIN_TABLE_MAX_IDX; ++i) {
            CimportMinTable::Object()->updateValue(data.at(i), i , m_idx, IMPORT_MIN_TABLE_IDX);
        }
    }
    return m_idx;
}
