#include "cothercontacttable.h"
#include "ccontacttable.h"

SotherContactData *CotherContactTable::findName(QString name)
{
    //CotherContactTable::Object()

    SotherContactData* data = nullptr;
    TobjectList values;
    QString condition = getSearchString(name, OTHER_NAME_IDX);

    if (getTableValues(condition, values)) {
        const Tobjects& obj = values.at(0);
        data = new SotherContactData;
        data->m_idx = obj.at(OTHER_CONTACT_ID_IDX).toInt();
        data->m_name = obj.at(OTHER_NAME_IDX).toString();
        data->m_phone = obj.at(OTHER_PHONE_IDX).toString();
    }

    return data;
}

SotherContactData *CotherContactTable::getOtherContactByID(int id)
{
    if (id > 0) {
        QString whenstatement = " WHERE " +  CotherContactTable::Object()->getColName(OTHER_CONTACT_ID_IDX);
        whenstatement += "=" + QString::number(id);
        QString query = "SELECT * FROM " + getTableName() + whenstatement;
        SotherContactData* contact = nullptr;
        if (query.isEmpty() == false) {
            QSqlQuery q(getDataBase());
            if(q.exec(query)){
                while(q.next()){
                    contact =  new SotherContactData;
                    contact->m_idx = q.value(OTHER_CONTACT_ID_IDX).toInt();
                    contact->m_name = q.value(OTHER_NAME_IDX).toString();
                    contact->m_phone = q.value(OTHER_PHONE_IDX).toString();
                    break;
                }

            }
        }
        return contact;
    }
    return nullptr;
//return data;
}

CotherContactTable::CotherContactTable():
    CdbTableBase(OTHER_CONTACT_TABLE)
{
    addTableCol("otherContactID", FIELD_TYPE_PRIMARY_KEY, "Other Contact ID", true, OTHER_CONTACT_TABLE_START_NO);
    addTableCol("otherName", FIELD_TYPE_STRING, "Name", false);
    addTableCol("otherPhone", FIELD_TYPE_STRING, "Phone", false);
}

CotherContactTable::~CotherContactTable()
{

}




SotherContactData::SotherContactData(){}
SotherContactData::~SotherContactData(){}
int SotherContactData::save()
{

    Tobjects data(OTHER_CONTACT_MAX_IDX);
    data[OTHER_CONTACT_ID_IDX] = m_idx;
    data[OTHER_NAME_IDX] = m_name;
    data[OTHER_PHONE_IDX] = m_phone;

    if (m_idx == 0) {
        TfilterObjType filter;
        filter.push_back(qMakePair(OTHER_NAME_IDX, m_name));
        if (CotherContactTable::Object()->isValueExist(filter) == false) {
            m_idx = CotherContactTable::Object()->insertInTable(data).toInt();
            if (m_idx > 0) {
                ScontactData contact;
                contact.m_idx = 0;
                contact.m_fullName = m_name;
                contact.m_fullNameWithSpouse = m_name;
                contact.m_fullNameWithoutTitle = m_name;
                contact.m_phone = m_phone;
                contact.m_personTableID = 0;
                contact.m_otherContactTableId = m_idx;
                contact.m_bankName = "";
                contact.save();
            }
        }
    } else {
        CotherContactTable::Object()->updateValue(m_name, OTHER_NAME_IDX, m_idx, OTHER_CONTACT_ID_IDX);
        CotherContactTable::Object()->updateValue(m_phone, OTHER_PHONE_IDX, m_idx, OTHER_CONTACT_ID_IDX);

        ScontactData* contact = CcontactTable::Object()->getContactFromOtherId(m_idx);
        if (contact) {
            contact->m_fullName = m_name;
            contact->m_fullNameWithSpouse = m_name;
            contact->m_fullNameWithoutTitle = m_name;
            contact->m_phone = m_phone;
            contact->save();
            delete contact;
        }

    }
    return m_idx;
}

void SotherContactData::deleteObj()
{

    if (m_idx != 0) {
        CotherContactTable::Object()->deleteValue(m_idx, OTHER_CONTACT_ID_IDX);
    }
}

void SotherContactData::serialize(QDataStream &out)
{
    out<<m_idx<<m_name<<m_phone;
}

void SotherContactData::desserialize(QDataStream &in)
{
    in>>m_idx>>m_name>>m_phone;
}
