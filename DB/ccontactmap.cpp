#include "ccontactmap.h"
#include "ccontacttable.h"
#include "cpersontable.h"
#include "csdutils.h"
#include "DLG/ccontactdlg.h"
#include "cothercontacttable.h"
#include <QMessageBox>
#include "ctransactiontable.h"
#include "cimportmintable.h"
void CcontactMap::populateContactMap()
{
    m_contactIDWithFullNameWithSpouse.clear();
    m_contactIDWithFullName.clear();
    m_fullNameList.clear();
    TobjectList values;
    QString condition;


    if (CcontactTable::Object()->getTableValues(condition, values)) {
        int size = values.size();
        for (int i = 0; i < size; ++i) {
            int id = values.at(i).at(CONTACT_ID_IDX).toInt();
            QString name = values.at(i).at(CONTACT_FULL_NAME_WITH_SPOUSE_IDX).toString();
            QString nameOnly = values.at(i).at(CONTACT_FULL_NAME_IDX).toString();
            QString phone = values.at(i).at(CONTACT_PHONE_IDX).toString();

            if (phone.isEmpty() == false) {
                m_contactPhone.insert(id, phone);
            }
            m_contactIDWithFullNameWithSpouse.insert(id, name);
            m_contactIDWithFullNameOnly.insert(id, nameOnly);
            m_contactIDWithFullName.insert(name, id);
            m_fullNameList.push_back(name);
            //QString fullName = values.at(i).at(CONTACT_FULL_NAME_IDX).toString();
            int personTableID = values.at(i).at(CONTACT_PERSON_ID_IDX).toInt();

            /*{
               // qDebug()<<name;
                if (name.contains("Chingthianmawi")) {
                    //qDebug()<<name<<" contact id "<<id<<" person id "<<personTableID;
                    CcontactTable::Object()->deleteValue(id, CONTACT_ID_IDX);
                    CpersonTable::Object()->deleteValue(personTableID, PERSON_ID_IDX);
                    continue;
                }
            }*/
            if (personTableID) {



                m_contactToPersonId.insert(id, personTableID);
                SpersonData* data = CpersonTable::Object()->getPersonData(personTableID);
                if (data) {
                    QString relationShip = data->m_relationship;
                    QString title = data->m_title;
                    QString firstName = data->m_firstName;
                    QString lastName = data->m_lastName;
                    QString fullName = CsdUtils::createFullName(title, firstName, lastName);
                    int parentSouseID = data->m_parentOrSpousePersonId;
                    m_personIDNameMap.insert(personTableID, fullName);
                    delete data;
                    data = CpersonTable::Object()->getPersonData(parentSouseID);
                    if (data) {
                        QString parentSpouse = CsdUtils::createFullName(data->m_title,
                                                                    data->m_firstName,
                                                                    data->m_lastName);


                        QString nameWithParent = CsdUtils::createFullNameWithSpouse(fullName, relationShip, parentSpouse);
                        m_contactIDwithNameAndParent.insert(id, nameWithParent);

                        delete data;
                    }

                }
            } else {

                int otherContactID = values.at(i).at(CONTACT_OTHER_ID_IDX).toInt();
                m_contactToOtherID.insert(id, otherContactID);
                m_contactIDwithNameAndParent.insert(id, name);


            }

        }
        m_fullNameList.sort();
   }



}

int CcontactMap::getContactID(const QString &name) const
{
    QMap<QString, int>::const_iterator fn =	m_contactIDWithFullName.find(name);
    if (fn != m_contactIDWithFullName.end()) {
        return fn.value();
    }
    return 0;
}

QString CcontactMap::getContanceName(int id) const
{
    QMap<int, QString>::const_iterator fn =	m_contactIDWithFullNameWithSpouse.find(id);
    if (fn != m_contactIDWithFullNameWithSpouse.end()) {
        return fn.value();
    }
    return QString();
}

QString CcontactMap::getContactNameOnly(int id) const
{
    //m_contactIDWithFullNameOnly
    QMap<int, QString>::const_iterator fn =	m_contactIDWithFullNameOnly.find(id);
    if (fn != m_contactIDWithFullNameOnly.end()) {
        return fn.value();
    }
    return QString();
}

int CcontactMap::getPersonIdFromContactID(int id) const
{
    QMap<int, int>::const_iterator fn = m_contactToPersonId.find(id);
    if (fn != m_contactToPersonId.end()) {
        return fn.value();
    }

    return 0;
}

QString CcontactMap::getCreateContactNameWithParent(int id) const
{
    //there is no use of this
    // it add name with parent/spouse
    QMap<int, QString>::const_iterator fn = m_contactIDwithNameAndParent.find(id);
    if (fn != m_contactIDwithNameAndParent.end()) {
        return fn.value();
    }
    return QString("");
}

int CcontactMap::addEditContact(CcontactDLG *contactDlg)
{

    bool isPerson = contactDlg->isPerson();
    QString firstname = contactDlg->firstName();
    QString phone = contactDlg->phone();
    //QString bankName = ;
    int contactID = 0;

    int suggestNameID = 0;
    bool forImport = contactDlg->forImport();
    if (forImport) {
        suggestNameID = contactDlg->suggestedNameContactID();
    }

    if (isPerson) {
        if (suggestNameID) {
            ScontactData* contactData = CcontactTable::Object()->getContactFromID(suggestNameID);
            contactID = contactData->m_idx;
            bool save = false;
            if (contactData->m_phone.isEmpty() && phone.isEmpty() == false) {
                contactData->m_phone = phone;
                save = true;
            } else if (contactData->m_phone.isEmpty() == false && phone.isEmpty() == false) {
                if (contactData->m_phone != phone) {
                    QString msg = "Replace phone no " + contactData->m_phone;
                    msg += " with " + phone + " for " + contactData->m_fullName;
                    QMessageBox::StandardButton reply = QMessageBox::question(nullptr,"Update", msg, QMessageBox::Yes|QMessageBox::No);
                    if (reply == QMessageBox::Yes) {
                        contactData->m_phone = phone;
                        save = true;
                    }
                }

            }
            if (save) {
                contactData->save();
            }
            delete contactData;
        } else {

            int parentId = 0;
            QString parentSpouse = contactDlg->parentSpouse();
            if (parentSpouse.isEmpty() == false) {
                int parentSpouseId = contactDlg->parentSpouseContactID();
                if (parentSpouseId == 0) {
                    QString parentTitle = contactDlg->parentTitle();
                    QString parentFirstName = contactDlg->parentFirstName();
                    QString parentLastName = contactDlg->parentLastName();

                    SpersonData person;
                    person.m_id = 0;
                    person.m_title = CsdUtils::ucfirst(parentTitle.trimmed().simplified());
                    person.m_firstName= CsdUtils::ucfirst(parentFirstName.trimmed().simplified());
                    person.m_lastName = CsdUtils::ucfirst(parentLastName.trimmed().simplified());
                    person.m_relationship = "";
                    person.m_parentOrSpousePersonId = 0;
                    person.m_gender = contactDlg->gender();
                    person.m_phone = phone;
                    person.m_isSaptuamMember = 0;
                    person.m_isFamilyHead = 0;
                    person.m_saptuamMembrHihna = "";
                    person.m_familyHeadPersonId = 0;
                    person.m_remark = "";
                    person.m_address = "";
                    /*if ((contactDlg->contactIDFullNameToUpdate() || contactDlg->contactParentFullNameToUpdate()) && person.m_lastName.isEmpty()) {
                        person.m_lastName = "LNU";
                    }*/
                    person.m_useFullNameWithSpouse = (contactDlg->contactIDFullNameToUpdate() || contactDlg->contactParentFullNameToUpdate()) > 0? true: false;
                    //qDebug()<<"saving parent contact";
                    parentId = person.save();

                } else {
                    ScontactData* contactData = CcontactTable::Object()->getContactFromID(parentSpouseId);
                    if (contactData) {
                        parentId = contactData->m_personTableID; // obj->at(0).at(CONTACT_PERSON_ID_IDX).toInt();
                        delete contactData;
                    }
                }

            }
            QString title = contactDlg->title();
            QString lastname = contactDlg->lastName();
            QString relationship = contactDlg->relationShip();
            QString gender = contactDlg->gender();
            SpersonData person;
            person.m_id = contactDlg->personEditID();;
            person.m_title = CsdUtils::ucfirst(title.trimmed().simplified());
            person.m_firstName= CsdUtils::ucfirst(firstname.trimmed().simplified());
            person.m_lastName = CsdUtils::ucfirst(lastname.trimmed().simplified());
            person.m_relationship = relationship;
            person.m_parentOrSpousePersonId = parentId;
            person.m_gender = gender;
            person.m_phone = phone;
            person.m_isSaptuamMember = 0;
            person.m_isFamilyHead = 0;
            person.m_saptuamMembrHihna = "";
            person.m_familyHeadPersonId = 0;
            person.m_remark = "";
            person.m_address = "";
            person.m_useFullNameWithSpouse = (contactDlg->contactParentFullNameToUpdate() || contactDlg->contactIDFullNameToUpdate()) > 0? true: false;

            //qDebug()<<"saving contact"<<person.m_firstName<<" "<<person.m_phone;
            person.save();

            ScontactData* contact = CcontactTable::Object()->getContactFromPersonId(person.m_id);
            contactID = contact->m_idx;
            delete contact;
        }

    } else {

        if (suggestNameID) {
            ScontactData* contactData = CcontactTable::Object()->getContactFromID(suggestNameID);
            if (contactData) {
                contactID = contactData->m_idx;
                delete contactData;
            }
        } else {

            SotherContactData contactData;
            contactData.m_idx = contactDlg->otherEditID();
            contactData.m_name = CsdUtils::ucfirst(firstname.trimmed().simplified());
            contactData.m_phone = phone.trimmed().simplified();
            contactData.save();
            populateContactMap();
            ScontactData* contact = CcontactTable::Object()->getContactFromOtherId(contactData.m_idx);
            contactID = contact->m_idx;
            delete contact;
        }
    }

    if (contactDlg->contactIDFullNameToUpdate() > 0) {
        int personID = CcontactMap::Object()->getPersonIdFromContactID(contactDlg->contactIDFullNameToUpdate());
        if (personID > 0) {
            SpersonData* data = CpersonTable::Object()->getPersonData(personID);
            if (data) {
                data->m_useFullNameWithSpouse = true;
                data->save();
            }

        }
    }
    if (contactDlg->contactParentFullNameToUpdate() > 0) {
        int personID = CcontactMap::Object()->getPersonIdFromContactID(contactDlg->contactParentFullNameToUpdate());
        if (personID > 0) {
            SpersonData* data = CpersonTable::Object()->getPersonData(personID);
            if (data) {
                data->m_useFullNameWithSpouse = true;
                data->save();
            }

        }
    }



    populateContactMap();

    return contactID;
}

QString CcontactMap::getContactPhone(int contactID)
{
    auto fn = m_contactPhone.find(contactID);
    if (fn != m_contactPhone.end()) {
        return fn.value();
    }
    return "";
}

QString CcontactMap::getPersonName(int personID)
{
    auto fn = m_personIDNameMap.find(personID);
    if (fn != m_personIDNameMap.end()) {
        return fn.value();
    }
    return "";
}



CcontactMap::CcontactMap()
{
    populateContactMap();
}

CcontactMap::~CcontactMap()
{

}

