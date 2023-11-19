#include "cpersontable.h"
#include "ccontacttable.h"
#include "csdutils.h"

CpersonTable::CpersonTable():
    CdbTableBase(PERSON_TABLE)
{

    addTableCol("personID", FIELD_TYPE_PRIMARY_KEY, "Person ID", true, PERSON_TABLE_START_NO);
    addTableCol("personTitle", FIELD_TYPE_STRING, "Title", false);
    addTableCol("personFirstName", FIELD_TYPE_STRING, "First Name", false);
    addTableCol("personLastName", FIELD_TYPE_STRING, "Last Name", false);
    addTableCol("personRelationShip", FIELD_TYPE_STRING, "Relationship", false);
    addTableCol("personParentSpousePersonID", FIELD_TYPE_INT, "Parent/Spouse", false);
    addTableCol("personGender", FIELD_TYPE_STRING, "Gender", false);
    addTableCol("personPhone", FIELD_TYPE_STRING, "Phone", false);
    addTableCol("personIsSaptuamMember", FIELD_TYPE_BOOL, "Saptuam Member?", false);
    addTableCol("personIsFamilyHead", FIELD_TYPE_BOOL, "Family Head", false);
    addTableCol("personChurchtMemberHihna", FIELD_TYPE_STRING, "Saptuam Member hihna", false);
    addTableCol("personFamilyHeadPersonID", FIELD_TYPE_INT, "Head of Family", true);
    addTableCol("personRemark", FIELD_TYPE_STRING, "Remark", false);
    addTableCol("personAddress", FIELD_TYPE_STRING, "Address", false);
    addTableCol("useFullNameWithSpouse", FIELD_TYPE_BOOL, "Use full name with spouse", false);


}

CpersonTable::~CpersonTable()
{

}

SpersonData *CpersonTable::getPersonData(int id)
{
    SpersonData* data = nullptr;

    if (id > 0) {
        QString query;
        QString whenstatement = " WHERE " +  getColName(PERSON_ID_IDX);
        whenstatement += "=" + QString::number(id);
        query = "SELECT * FROM " + getTableName() + whenstatement;
        if (query.isEmpty() == false) {
            QSqlQuery q(getDataBase());
            if(q.exec(query)){
                while(q.next()){
                    data = new SpersonData();
                    data->m_id = q.value(PERSON_ID_IDX).toInt();
                    data->m_title = q.value(PERSON_TITLE_IDX).toString();
                    data->m_firstName = q.value(PERSON_FIRST_NAME_IDX).toString();
                    data->m_lastName = q.value(PERSON_LAST_NAME_IDX).toString();
                    data->m_relationship = q.value(PERSON_RELATIONSHIP_IDX).toString();
                    data->m_parentOrSpousePersonId = q.value(PERSON_PARENT_OR_SPOUSE_PERSON_ID_IDX).toInt();
                    data->m_gender = q.value(PERSON_GENDER_IDX).toString();
                    data->m_phone = q.value(PERSON_PHONE_IDX).toString();
                    data->m_isSaptuamMember =  q.value(PERSON_IS_SAPTUAM_MEMBER_IDX).toBool();
                    data->m_isFamilyHead =  q.value(PERSON_IS_FAMILY_HEAD_IDX).toBool();
                    data->m_saptuamMembrHihna = q.value(PERSON_CHURCH_MEMBER_HIHNA_IDX).toString();
                    data->m_familyHeadPersonId =  q.value(PERSON_FAMILY_HEAD_PERSON_ID_IDX).toInt();
                    data->m_remark = q.value(PERSON_REMARK_IDX).toString();
                    data->m_address = q.value(PERSON_ADDRESS_IDX).toString();
                    data->m_useFullNameWithSpouse = q.value(PERSON_USE_FULL_NAME_WITH_SPOUSE).toBool();
                    break;
                }
            }
        }

    }

    return data;
}

QVector<SpersonData *>* CpersonTable::getAllPresonData()
{
    QString query = "SELECT * FROM " + getTableName();
    QVector<SpersonData *>* dataV  = nullptr;
    if (query.isEmpty() == false) {
        QSqlQuery q(getDataBase());
        if(q.exec(query)){
            dataV = new QVector<SpersonData *>;
            while(q.next()){
                SpersonData* data = new SpersonData();
                data->m_id = q.value(PERSON_ID_IDX).toInt();
                data->m_title = q.value(PERSON_TITLE_IDX).toString();
                data->m_firstName = q.value(PERSON_FIRST_NAME_IDX).toString();
                data->m_lastName = q.value(PERSON_LAST_NAME_IDX).toString();
                data->m_relationship = q.value(PERSON_RELATIONSHIP_IDX).toString();
                data->m_parentOrSpousePersonId = q.value(PERSON_PARENT_OR_SPOUSE_PERSON_ID_IDX).toInt();
                data->m_gender = q.value(PERSON_GENDER_IDX).toString();
                data->m_phone = q.value(PERSON_PHONE_IDX).toString();
                data->m_isSaptuamMember =  q.value(PERSON_IS_SAPTUAM_MEMBER_IDX).toBool();
                data->m_isFamilyHead =  q.value(PERSON_IS_FAMILY_HEAD_IDX).toBool();
                data->m_saptuamMembrHihna = q.value(PERSON_CHURCH_MEMBER_HIHNA_IDX).toString();
                data->m_familyHeadPersonId =  q.value(PERSON_FAMILY_HEAD_PERSON_ID_IDX).toInt();
                data->m_remark = q.value(PERSON_REMARK_IDX).toString();
                data->m_address = q.value(PERSON_ADDRESS_IDX).toString();
                data->m_useFullNameWithSpouse = q.value(PERSON_USE_FULL_NAME_WITH_SPOUSE).toBool();
                dataV->push_back(data);
            }
        }
    }
    return dataV;
}



ScontactData*
SpersonData::getContactDataFromPersonData()
{
    ScontactData* contact = new ScontactData;
    QString fullName = CsdUtils::createFullName(m_title, m_firstName, m_lastName).trimmed();
    QString spouseFullName = createFullName(m_parentOrSpousePersonId).trimmed();
    QString fullNameWithSpouse = CsdUtils::createFullNameWithSpouse(fullName, m_relationship, spouseFullName);

    contact->m_idx = 0;
    contact->m_fullName = fullName;
    contact->m_fullNameWithSpouse = m_useFullNameWithSpouse ? fullNameWithSpouse : fullName;
    contact->m_fullNameWithoutTitle = CsdUtils::createFullName("", m_firstName, m_lastName).simplified();
    contact->m_personTableID = m_id;
    contact->m_otherContactTableId = 0;
    contact->m_phone = m_phone;
    contact->m_bankName = "";
    return contact;
}

QString SpersonData::getFullName()
{
    return CsdUtils::createFullName(m_title, m_firstName, m_lastName);
}

QString SpersonData::getParentFullName()
{
    return createFullName(m_parentOrSpousePersonId);
}


QString SpersonData::createFullName(int id)
{
    QString fullName;
    if (id > 0) {
        SpersonData * personData =  CpersonTable::Object()->getPersonData(id);
        if (personData) {
            fullName = personData->getFullName();
            delete personData;
        }
    }
    return fullName;
}

int SpersonData::save()
{
    Tobjects data(PERSON_MAX_IDX);


    data[PERSON_ID_IDX] = m_id ;
    data[PERSON_TITLE_IDX] = m_title ;
    data[PERSON_FIRST_NAME_IDX] = m_firstName ;
    data[PERSON_LAST_NAME_IDX] = m_lastName ;
    data[PERSON_RELATIONSHIP_IDX] = m_relationship ;
    data[PERSON_PARENT_OR_SPOUSE_PERSON_ID_IDX] = m_parentOrSpousePersonId ;
    data[PERSON_GENDER_IDX] = m_gender ;
    data[PERSON_PHONE_IDX] = m_phone ;
    data[PERSON_IS_SAPTUAM_MEMBER_IDX] = m_isSaptuamMember ;
    data[PERSON_IS_FAMILY_HEAD_IDX] = m_isFamilyHead ;
    data[PERSON_CHURCH_MEMBER_HIHNA_IDX] = m_saptuamMembrHihna ;
    data[PERSON_FAMILY_HEAD_PERSON_ID_IDX] = m_familyHeadPersonId ;
    data[PERSON_REMARK_IDX] = m_remark ;
    data[PERSON_ADDRESS_IDX] = m_address ;
    data[PERSON_USE_FULL_NAME_WITH_SPOUSE] = m_useFullNameWithSpouse;


    if (m_id == 0) {
        //new object
        TfilterObjType filter;
        filter.push_back(qMakePair(PERSON_FIRST_NAME_IDX, data.at(PERSON_FIRST_NAME_IDX)));
        if (data.at(PERSON_LAST_NAME_IDX).toString().isEmpty() == false) {
            filter.push_back(qMakePair(PERSON_LAST_NAME_IDX, data.at(PERSON_LAST_NAME_IDX)));
        }
        /*if (data.at(PERSON_PARENT_OR_SPOUSE_PERSON_ID_IDX).toInt() > 0) {
            filter.push_back(qMakePair(PERSON_PARENT_OR_SPOUSE_PERSON_ID_IDX, data.at(PERSON_PARENT_OR_SPOUSE_PERSON_ID_IDX)));
        }*/
        TobjectList values;
        bool toadd = true;
        if(CpersonTable::Object()->isValueExist(filter, values) == true) {
            toadd = false;
            int size = values.size();
            /*if (size) {
                toadd = true;
            }*/
            bool sameParent = false;
            for (int i = 0; i < size; ++i) {
                const Tobjects& obj  = values.at(i);
                if (m_parentOrSpousePersonId > 0 && obj.at(PERSON_PARENT_OR_SPOUSE_PERSON_ID_IDX).toInt() == m_parentOrSpousePersonId) {
                    sameParent = true;
                    m_id = obj.at(PERSON_ID_IDX).toInt();
                    break;
                }
            }
            if (sameParent == false) {
                toadd  = true;
            }
        }
        if (toadd) {
            data[PERSON_ID_IDX] = 0;
            m_id = CpersonTable::Object()->insertInTable(data).toInt();
            if (m_id > 0) {
                data[PERSON_ID_IDX] = m_id;
                ScontactData* contactData = getContactDataFromPersonData();
                contactData->save();
                delete contactData;
            }
            return m_id;
        } else {
           // Q_ASSERT(0);
            //qDebug()<<"ame "<<values.at(0).at(PERSON_PARENT_OR_SPOUSE_PERSON_ID_IDX).toInt()<<" "<<values.at(0).at(PERSON_FIRST_NAME_IDX).toString();

        }
    } else {
        for (unsigned int i = PERSON_TITLE_IDX; i < PERSON_MAX_IDX; ++i) {
            CpersonTable::Object()->updateValue(data.at(i), i, m_id, PERSON_ID_IDX);
        }

        ScontactData* contact = CcontactTable::Object()->getContactFromPersonId(m_id);
        if (contact) {
            ScontactData* contactData = getContactDataFromPersonData();
            contact->m_fullName = contactData->m_fullName;
            contact->m_fullNameWithSpouse = contactData->m_fullNameWithSpouse;
            contact->m_fullNameWithoutTitle = contactData->m_fullNameWithoutTitle;
            contact->m_phone = contactData->m_phone;
            contact->save();
            delete contactData;
            delete contact;
        }
    }
    return m_id;
}

void SpersonData::deleteObj()
{
    if (m_id != 0) {
        CpersonTable::Object()->deleteValue(m_id, PERSON_ID_IDX);
    }
}

void SpersonData::serialize(QDataStream &out)
{
    out<<m_id<<m_title<<m_firstName<<m_lastName<<m_relationship<<m_parentOrSpousePersonId
        <<m_gender<<m_phone<<m_isSaptuamMember<<m_isFamilyHead<<m_saptuamMembrHihna
        <<m_familyHeadPersonId<<m_remark<<m_address<<m_useFullNameWithSpouse;
}

void SpersonData::desserialize(QDataStream &in)
{
    in>>m_id>>m_title>>m_firstName>>m_lastName>>m_relationship>>m_parentOrSpousePersonId
        >>m_gender>>m_phone>>m_isSaptuamMember>>m_isFamilyHead>>m_saptuamMembrHihna
        >>m_familyHeadPersonId>>m_remark>>m_address>>m_useFullNameWithSpouse;

}
