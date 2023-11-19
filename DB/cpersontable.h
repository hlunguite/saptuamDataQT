#ifndef CPERSONTABLE_H
#define CPERSONTABLE_H
#include "cdbtablebase.h"
#include "ccontacttable.h"
#include <QVector>
enum EpersonTableFieldIndex{
    PERSON_ID_IDX,
    PERSON_TITLE_IDX,
    PERSON_FIRST_NAME_IDX,
    PERSON_LAST_NAME_IDX,
    PERSON_RELATIONSHIP_IDX,
    PERSON_PARENT_OR_SPOUSE_PERSON_ID_IDX,
    PERSON_GENDER_IDX,
    PERSON_PHONE_IDX,
    PERSON_IS_SAPTUAM_MEMBER_IDX,
    PERSON_IS_FAMILY_HEAD_IDX,
    PERSON_CHURCH_MEMBER_HIHNA_IDX,
    PERSON_FAMILY_HEAD_PERSON_ID_IDX,
    PERSON_REMARK_IDX,
    PERSON_ADDRESS_IDX,
    PERSON_USE_FULL_NAME_WITH_SPOUSE,
    PERSON_MAX_IDX
};

struct SpersonData: public SdbObject
{
private:
    QString     createFullName(int id);

 public:
    int m_id = 0;
    QString m_title;
    QString m_firstName;
    QString m_lastName;
    QString m_relationship;
    int		m_parentOrSpousePersonId = 0;
    QString m_gender;
    QString m_phone;
    bool	m_isSaptuamMember = false;
    bool	m_isFamilyHead = false;
    QString m_saptuamMembrHihna;
    int		m_familyHeadPersonId = 0;
    QString m_remark;
    QString m_address;
    bool    m_useFullNameWithSpouse = false;

    SpersonData(){}
    ~SpersonData(){}
    QString getFullName();
    QString getParentFullName();
    ScontactData* getContactDataFromPersonData();
    int save();
    void deleteObj();
    void serialize(QDataStream& out) ;
    void desserialize(QDataStream& in);
};

class CpersonTable : public CdbTableBase, public CsdSingleton<CpersonTable>
{
    friend class CsdSingleton<CpersonTable>;
private:
public:
    CpersonTable();
    ~CpersonTable();
    SpersonData* getPersonData(int id);
    QVector<SpersonData*>* getAllPresonData();


};

#endif // CPERSONTABLE_H
