#ifndef CIMPORTACCOUNTTABLE_H
#define CIMPORTACCOUNTTABLE_H

#include "cdbtablebase.h"
#include <QVector>
enum EimportAccountTableIndex {
    IMPORT_ACCOUNT_TABLE_IDX,
    IMPORT_ACCOUNT_IMPORTED_ID_IDX,
    IMPORT_ACCOUNT_TYPE_IDX,
    IMPORT_ACCOUNT_ID_IDX,
    IMPORT_ACCOUNT_NAME_IDX,
    IMPORT_ACCOUNT_TABLE_MAX_IDX
};

struct SimportAccountData : public SdbObject {
    int m_idx;
    int m_importIdx;
    int m_accountType;
    int m_accountID;
    QString m_accountName;

    SimportAccountData() {
        m_idx = 0;
        m_importIdx = 0;
        m_accountType = 0;
        m_accountID = 0;
    }
    ~SimportAccountData(){}
    int save();
    void deleteObj(){}
};

class CimportAccountTable : public CdbTableBase , public CsdSingleton<CimportAccountTable>
{
    friend class CsdSingleton<CimportAccountTable>;

public:
    CimportAccountTable();
    QVector<SimportAccountData*> * getImportAccount(bool toprocess = true);
};

#endif // CIMPORTACCOUNTTABLE_H
