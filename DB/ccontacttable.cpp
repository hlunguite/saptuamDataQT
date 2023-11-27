#include "ccontacttable.h"
#include "csdutils.h"
#include "cpersontable.h"
#include "cothercontacttable.h"
CcontactTable::CcontactTable():
    CdbTableBase(CONTACT_TABLE)
{
    addTableCol("contactID", FIELD_TYPE_PRIMARY_KEY, "Contact ID", true, CONTACT_TABLE_START_NO);
    addTableCol("contactFullName", FIELD_TYPE_STRING, "Full Name", false);
    addTableCol("contactFullNameWithSpouse", FIELD_TYPE_STRING, "Full Name with Spouse", false);
    addTableCol("contactFullNameWithoutTitle", FIELD_TYPE_STRING, "Full name without title", true);
    addTableCol("contactPersonID", FIELD_TYPE_INT, "Person", true);
    addTableCol("contactOtherID", FIELD_TYPE_INT, "Other Contact", true);
    addTableCol("contactPhone", FIELD_TYPE_STRING, "Phone", false);
    addTableCol("contactBankName", FIELD_TYPE_STRING, "Bank Name", true);
}

CcontactTable::~CcontactTable()
{

}


QVector<QVariant> CcontactTable::findName(QString name)
{
    QVector<QVariant> ids;
    QString whenstatement = " WHERE " +  CcontactTable::Object()->getColName(CONTACT_FULL_NAME_IDX);
    whenstatement += " LIKE \"%" + CsdUtils::ucfirst(name) +"%\"";
    QString query = "SELECT * FROM " + getTableName() + whenstatement;
    //qDebug()<<"Contact table search:"<<query;
    QSqlQuery q(getDataBase());
    if(q.exec(query)){
        while(q.next()){
            int pid = q.value(CONTACT_ID_IDX).toInt();
            ids.push_back(pid);
        }

    }

    return ids;
}

void CcontactTable::addBankName(int contactID, QString bankName)
{
    QString whenstatement = " WHERE " +  getColName(CONTACT_ID_IDX);
    whenstatement += "=" + QString::number(contactID);

    QString query = "SELECT * FROM " + getTableName() + whenstatement;
    ScontactData* contact =  getContact(query);
    if (contact) {
        contact->m_bankName = bankName;
        contact->save();
        delete contact;
    }
}

QVector<ScontactData *>
CcontactTable::getContactFromBankName(QString bankName)
{
    if (bankName.isEmpty() == false) {
        QString whenstatement = " WHERE " +  getColName(CONTACT_BANK_NAME_IDX);
        whenstatement += " LIKE \"%" + bankName +"%\"";
        QString query = "SELECT * FROM " + getTableName() + whenstatement;
        return getContacts(query);

    }
    return  QVector<ScontactData *>();
}

 QVector<ScontactData *>
CcontactTable::getContactFromPhone(QString phone)
{
    if (phone.isEmpty() == false) {
        QString whenstatement = " WHERE " +  CcontactTable::Object()->getColName(CONTACT_PHONE_IDX);
        whenstatement += " LIKE \"%" + phone +"%\"";
        QString query = "SELECT * FROM " + getTableName() + whenstatement;
        return getContacts(query);
    }
    return QVector<ScontactData *>();
}

ScontactData*
CcontactTable::getContactFromPersonId(int id)
{
    if (id > 0) {
        QString whenstatement = " WHERE " +  CcontactTable::Object()->getColName(CONTACT_PERSON_ID_IDX);
        whenstatement += "=" + QString::number(id);
        QString query = "SELECT * FROM " + getTableName() + whenstatement;
        return getContact(query);
    }
    return nullptr;
}

ScontactData *CcontactTable::getContactFromOtherId(int id)
{
    if (id > 0) {
        QString whenstatement = " WHERE " +  CcontactTable::Object()->getColName(CONTACT_OTHER_ID_IDX);
        whenstatement += "=" + QString::number(id);
        QString query = "SELECT * FROM " + getTableName() + whenstatement;
        return getContact(query);
    }
    return nullptr;
}

ScontactData *CcontactTable::getContactFromID(int id)
{
    if (id > 0) {
        QString whenstatement = " WHERE " +  CcontactTable::Object()->getColName(CONTACT_ID_IDX);
        whenstatement += "=" + QString::number(id);
        QString query = "SELECT * FROM " + getTableName() + whenstatement;
        return getContact(query);
    }
    return nullptr;
}

void CcontactTable::serializeTable(QDataStream &out, const Tobjects &obj)
{
    ScontactData contact;
    contact.m_idx = obj.at(CONTACT_ID_IDX).toInt();
    contact.m_fullName = obj.at(CONTACT_FULL_NAME_IDX).toString();
    contact.m_fullNameWithSpouse = obj.at(CONTACT_FULL_NAME_WITH_SPOUSE_IDX).toString();
    contact.m_fullNameWithoutTitle = obj.at(CONTACT_FULL_NAME_IDX_WITHOUT_TITLE).toString();
    contact.m_personTableID = obj.at(CONTACT_PERSON_ID_IDX).toInt();
    contact.m_otherContactTableId = obj.at(CONTACT_OTHER_ID_IDX).toInt();
    contact.m_phone = obj.at(CONTACT_PHONE_IDX).toString();
    contact.m_bankName = obj.at(CONTACT_BANK_NAME_IDX).toString();
    contact.serialize(out);

}

void CcontactTable::desserializeTable(QDataStream &in)
{
    ScontactData contact;
    contact.desserialize(in);
}



ScontactData*
CcontactTable::getContact(QString query)
{
    ScontactData* contact = nullptr;
    if (query.isEmpty() == false) {
    QSqlQuery q(getDataBase());
    if(q.exec(query)){
        while(q.next()){
            contact =  new ScontactData;
            contact->m_idx = q.value(CONTACT_ID_IDX).toInt();
            contact->m_fullName = q.value(CONTACT_FULL_NAME_IDX).toString();
            contact->m_fullNameWithSpouse = q.value(CONTACT_FULL_NAME_WITH_SPOUSE_IDX).toString();
            contact->m_fullNameWithoutTitle = q.value(CONTACT_FULL_NAME_IDX_WITHOUT_TITLE).toString();
            contact->m_personTableID = q.value(CONTACT_PERSON_ID_IDX).toInt();
            contact->m_otherContactTableId = q.value(CONTACT_OTHER_ID_IDX).toInt();
            contact->m_phone = q.value(CONTACT_PHONE_IDX).toString();
            contact->m_bankName = q.value(CONTACT_BANK_NAME_IDX).toString();
            break;
        }

    }
    }

    return contact;
}

QVector<ScontactData *> CcontactTable::getContacts(QString query)
{
    QVector<ScontactData*> contacts;
    if (query.isEmpty() == false) {
        QSqlQuery q(getDataBase());
        if(q.exec(query)){
            while(q.next()){
                 ScontactData*contact =  new ScontactData;
                contact->m_idx = q.value(CONTACT_ID_IDX).toInt();
                contact->m_fullName = q.value(CONTACT_FULL_NAME_IDX).toString();
                contact->m_fullNameWithSpouse = q.value(CONTACT_FULL_NAME_WITH_SPOUSE_IDX).toString();
                contact->m_fullNameWithoutTitle = q.value(CONTACT_FULL_NAME_IDX_WITHOUT_TITLE).toString();
                contact->m_personTableID = q.value(CONTACT_PERSON_ID_IDX).toInt();
                contact->m_otherContactTableId = q.value(CONTACT_OTHER_ID_IDX).toInt();
                contact->m_phone = q.value(CONTACT_PHONE_IDX).toString();
                contact->m_bankName = q.value(CONTACT_BANK_NAME_IDX).toString();
                contacts.push_back(contact);
            }

        }
    }

    return contacts;
}


int ScontactData::save()
{
    Tobjects data(CONTACT_MAX_FIELD);

     data[CONTACT_ID_IDX] = m_idx ;
     data[CONTACT_FULL_NAME_IDX] = m_fullName ;
     data[CONTACT_FULL_NAME_WITH_SPOUSE_IDX] = m_fullNameWithSpouse ;
     data[CONTACT_FULL_NAME_IDX_WITHOUT_TITLE] = m_fullNameWithoutTitle ;
     data[CONTACT_PERSON_ID_IDX] = m_personTableID ;
     data[CONTACT_OTHER_ID_IDX] = m_otherContactTableId ;
     data[CONTACT_PHONE_IDX] = m_phone ;
     data[CONTACT_BANK_NAME_IDX] = m_bankName ;

    if (m_idx == 0) {
        //new object
        if (m_fullNameWithSpouse.isEmpty()) {
            return 0;
        }
        TfilterObjType filter;
        filter.push_back(qMakePair(CONTACT_FULL_NAME_WITH_SPOUSE_IDX, m_fullNameWithSpouse));
        if (CcontactTable::Object()->isValueExist(filter) == false) {
            data[CONTACT_ID_IDX] = 0;
            m_idx = CcontactTable::Object()->insertInTable(data).toInt();
        }

    } else {
        //update
        for (unsigned int i = CONTACT_FULL_NAME_IDX; i < CONTACT_MAX_FIELD; ++i) {
            CcontactTable::Object()->updateValue(data.at(i), i , m_idx, CONTACT_ID_IDX);
        }
    }
    return m_idx;
}

void ScontactData::deleteObj()
{
    if (m_idx != 0) {
        CcontactTable::Object()->deleteValue(m_idx, CONTACT_ID_IDX);
    }
}

void ScontactData::serialize(QDataStream &out)
{
    bool isPerson = m_personTableID > 0 ? true : false;
    out<<m_idx<<isPerson<<m_bankName;
    if (isPerson) {
        SpersonData* data = CpersonTable::Object()->getPersonData(m_personTableID);
        data->serialize(out);
        delete data;
    } else {
        SotherContactData * data = CotherContactTable::Object()->getOtherContactByID(m_otherContactTableId);
        data->serialize(out);
        delete data;
    }
}

void ScontactData::desserialize(QDataStream &in)
{
    bool isPerson;
    in>>m_idx>>isPerson>>m_bankName;
    if (isPerson) {
        SpersonData data;
        data.desserialize(in);
        ScontactData* d1 = data.getContactDataFromPersonData();
        m_fullName = d1->m_fullName;
        m_fullNameWithSpouse = d1->m_fullNameWithSpouse;
        m_fullNameWithoutTitle = d1->m_fullNameWithoutTitle;
        m_personTableID = data.m_id;
        m_otherContactTableId = 0;
        m_phone = d1->m_phone;
        delete d1;

    }else {
        SotherContactData data;
        data.desserialize(in);
        m_fullName = data.m_name;
        m_fullNameWithSpouse = m_fullName;
        m_fullNameWithoutTitle = m_fullName;
        m_personTableID = 0;
        m_otherContactTableId = data.m_idx;
        m_phone = data.m_phone;
    }

    //qDebug()<<"reading contact "<<m_idx<<" "<<m_fullNameWithSpouse<<" "<<m_otherContactTableId<<" "<<m_personTableID;
}
