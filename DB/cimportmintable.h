#ifndef CIMPORTMINTABLE_H
#define CIMPORTMINTABLE_H

#include "cdbtablebase.h"
#include <QVector>

enum EimportMinTableIndex {
    IMPORT_MIN_TABLE_IDX,
    IMPORT_MIN_INDEX_IDX,
    IMPORT_MIN_TITLE_IDX,
    IMPORT_MIN_FIRSTNAME_IDX,
    IMPORT_MIN_LASTNAME_IDX,
    IMPORT_MIN_RELATIONSHIP_IDX,
    IMPORT_MIN_PARENT_FIRST_NAME_IDX,
    IMPORT_MIN_PARENT_LAST_NAME_IDX,
    IMPORT_MIN_PHONE_IDX,
    IMPORT_MIN_BANK_NAME_IDX,
    IMPORT_MIN_CONTACT_TABLE_INDEX_IDX,
    IMPORT_MIN_IS_PERSON_IDX,
    IMPORT_MIN_TRANS_REMARK_IDX,
    IMPORT_MIN_TABLE_MAX_IDX
};

struct SimportMinData : public SdbObject {
    int m_idx;
    int m_importIdx;
    QString m_title;
    QString m_firstName;
    QString m_lastName;
    QString m_relationship;
    QString m_parentFirstName;
    QString m_parentLastName;
    QString m_phone;
    QString m_bankName;
    int m_contactTableIdx;
    bool m_isPerson;
    QString m_transRemark;
    SimportMinData() {
        m_idx = 0;
        m_importIdx = 0;
        m_contactTableIdx = 0;
    }
    ~SimportMinData(){}
    int save();
    void deleteObj(){}
};

class CimportMinTable : public CdbTableBase , public CsdSingleton<CimportMinTable>
{
    friend class CsdSingleton<CimportMinTable>;

public:
    CimportMinTable();
    QVector<SimportMinData*>* getImportedMin(bool toprocess = true);

};

#endif // CIMPORTMINTABLE_H
