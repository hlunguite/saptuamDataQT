#ifndef COTHERCONTACTTABLE_H
#define COTHERCONTACTTABLE_H
#include "cdbtablebase.h"

enum EotherContactTableFieldIndex {
    OTHER_CONTACT_ID_IDX,
    OTHER_NAME_IDX,
    OTHER_PHONE_IDX,
    OTHER_CONTACT_MAX_IDX
};

struct SotherContactData: public SdbObject
{
private:
 public:
    SotherContactData();
    ~SotherContactData();
    int save();
    void deleteObj();
    void serialize(QDataStream& out) ;
    void desserialize(QDataStream& in);
    int m_idx;
    QString m_name;
    QString m_phone;
};

class CotherContactTable : public CdbTableBase, public CsdSingleton<CotherContactTable>
{
    friend class CsdSingleton<CotherContactTable>;

public:
    SotherContactData* findName(QString name);
    SotherContactData* getOtherContactByID(int id);
    CotherContactTable();
    ~CotherContactTable();


};

#endif // COTHERCONTACTTABLE_H
