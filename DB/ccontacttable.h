#ifndef CCONTACTTABLE_H
#define CCONTACTTABLE_H
#include "cdbtablebase.h"

enum EcontactTableFieldIndex {
    CONTACT_ID_IDX,
    CONTACT_FULL_NAME_IDX,
    CONTACT_FULL_NAME_WITH_SPOUSE_IDX,
    CONTACT_FULL_NAME_IDX_WITHOUT_TITLE,
    CONTACT_PERSON_ID_IDX,
    CONTACT_OTHER_ID_IDX,
    CONTACT_PHONE_IDX,
    CONTACT_BANK_NAME_IDX,
    CONTACT_MAX_FIELD
};

struct ScontactData : public SdbObject {
    int m_idx;
    QString m_fullName;
    QString m_fullNameWithSpouse;
    QString m_fullNameWithoutTitle;
    int m_personTableID;
    int m_otherContactTableId;
    QString m_phone;
    QString m_bankName;
    ScontactData() {
        m_idx = 0;
        m_personTableID = 0;
        m_otherContactTableId = 0;
    }
    ~ScontactData(){}
    int save();
    void deleteObj();
    void serialize(QDataStream& out) ;
    void desserialize(QDataStream& in);
};


class CcontactTable : public CdbTableBase, public CsdSingleton<CcontactTable>
{
    friend class CsdSingleton<CcontactTable>;
public:
    CcontactTable();
    ~CcontactTable();
    QVector<QVariant> findName(QString name);
    void addBankName(int contactID, QString bankName);
    QVector<ScontactData *>     getContactFromBankName(QString bankName);
     QVector<ScontactData *>    getContactFromPhone(QString phone);
    ScontactData* getContactFromPersonId(int id);
    ScontactData* getContactFromOtherId(int id);
    ScontactData* getContactFromID(int id);
protected:
    virtual void serializeTable(QDataStream& out, const Tobjects& obj) override;
    virtual void desserializeTable(QDataStream& in) override;
private:
    ScontactData* getContact(QString query);
    QVector<ScontactData*> getContacts(QString query);

};

#endif // CCONTACTTABLE_H
