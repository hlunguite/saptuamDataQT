#ifndef CDEPARTMENTACCOUNTTABLE_H
#define CDEPARTMENTACCOUNTTABLE_H
#include "cdbtablebase.h"

enum EdepartmentAccountTableFieldIndex {
    DEPARTMENT_ACCOUNT_DEPARTMENT_OR_ACCOUNT_ID,
    DEPARTMENT_ACCOUNT_ID_IDX,
    DEPARTMENT_ACCOUNT_IS_DEPT,
    DEPARTMENT_ACCOUNT_PARENT_ID,
    DEPARTMENT_ACCOUNT_MAX_IDX

};

struct SdepartmentAccountData: public SdbObject
{
private:
 public:
    SdepartmentAccountData(){}
    ~SdepartmentAccountData(){}
    int save();
    void deleteObj();

    int m_deptOrAccountID = 0;
    int m_id = 0;
    bool m_isDept = 0;
    int m_parentID = 0;
};

class CdepartmentAccountTable : public CdbTableBase, public CsdSingleton<CdepartmentAccountTable>
{
    friend class CsdSingleton<CdepartmentAccountTable>;
public:
    bool addOrUpdateDepartmentAccount(int deptID, const QVector<int>& accountIDForDept);
    //bool removeFromDeptAccount(int id);
    SdbObjects* getAllDepartAccountWithParent(int parentid);
    SdepartmentAccountData* getDeptAccountDataForDepOrAccID(int id);
    SdepartmentAccountData* getDeptAccountDataForID(int id);
private:
    int addDepartmentAccount(int deptOrAccountID, bool isDept, int parentId);
    int addAccountToDeptAccount(int accountID, int parentId);
    CdepartmentAccountTable();
    ~CdepartmentAccountTable();
};

#endif // CDEPARTMENTACCOUNTTABLE_H
