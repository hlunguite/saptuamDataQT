#include "cdepartmentaccounttable.h"
#include "caccounttable.h"

CdepartmentAccountTable::CdepartmentAccountTable():
    CdbTableBase(DEPARTMENT_ACCOUNT_TABLE)
{
    addTableCol("departmentAccountDepartmentOrAccountID", FIELD_TYPE_INT, "Department/Account", false, DEPARTMENT_ACCOUNT_TABLE_START_NO);
    addTableCol("departmentAccountID", FIELD_TYPE_PRIMARY_KEY, "ID", true);
    addTableCol("departmentAccountIsDept", FIELD_TYPE_BOOL, "Is Dept?");
    addTableCol("departmentAccountParent", FIELD_TYPE_INT, "Parent");
}

CdepartmentAccountTable::~CdepartmentAccountTable()
{

}

bool
CdepartmentAccountTable::addOrUpdateDepartmentAccount(int deptID,
                                                      const QVector<int> &accountIDForDept)
{
    int count = accountIDForDept.size();
    int parentId = addDepartmentAccount(deptID, true, 0);
    if (parentId < 0) {
        return false;
    }
    for (int i = 0; i < count; ++i) {
        int accountId = accountIDForDept.at(i);
        addAccountToDeptAccount(accountId, parentId);


    }
    return true;
}
/*
bool CdepartmentAccountTable::removeFromDeptAccount(int id)
{
    bool ret = false;
    SdepartmentAccountData* data = //CdepartmentAccountTable::Object();
    if (data) {
        data->deleteObj();
        ret = true;
        delete data;
    }
    return ret;
}*/


SdbObjects *CdepartmentAccountTable::getAllDepartAccountWithParent(int parentid)
{
    SdbObjects* objects = nullptr;
    if (parentid >= 0) {
        QString condition = getColName(DEPARTMENT_ACCOUNT_PARENT_ID) + "=" + QString::number(parentid);
        condition += " ORDER BY "+ getColName(DEPARTMENT_ACCOUNT_ID_IDX);
        //qDebug()<<"account table query "<<condition;
        TobjectList values;
        if (getTableValues(condition, values)){
            //qDebug()<<"found size "<<values.size();
            objects = new SdbObjects;
            int size = values.size();
            for (int i = 0; i < size; ++i) {
                const Tobjects& obj = values.at(i);
                SdepartmentAccountData* data = new SdepartmentAccountData;
                data->m_deptOrAccountID = obj.at(DEPARTMENT_ACCOUNT_DEPARTMENT_OR_ACCOUNT_ID).toInt();
                data->m_id = obj.at(DEPARTMENT_ACCOUNT_ID_IDX).toInt();
                data->m_isDept = obj.at(DEPARTMENT_ACCOUNT_IS_DEPT).toBool();
                data->m_parentID = obj.at(DEPARTMENT_ACCOUNT_PARENT_ID).toInt();
                objects->m_listofObject.push_back(data);
                //qDebug()<<"found id "<<data->m_id <<" dept/accid  "<<data->m_deptOrAccountID;
            }
        }


    }

    return objects;
}

SdepartmentAccountData*
CdepartmentAccountTable::getDeptAccountDataForDepOrAccID(int id)
{
    SdepartmentAccountData* data = nullptr;
    if (id) {
        QString condition = getColName(DEPARTMENT_ACCOUNT_DEPARTMENT_OR_ACCOUNT_ID) + "=" + QString::number(id);
        TobjectList values;
        if (getTableValues(condition, values)){
            int size = values.size();
            for (int i = 0; i < size; ++i) {
                const Tobjects& obj = values.at(i);
                data = new SdepartmentAccountData;
                data->m_deptOrAccountID = obj.at(DEPARTMENT_ACCOUNT_DEPARTMENT_OR_ACCOUNT_ID).toInt();
                data->m_id = obj.at(DEPARTMENT_ACCOUNT_ID_IDX).toInt();
                data->m_isDept = obj.at(DEPARTMENT_ACCOUNT_IS_DEPT).toBool();
                data->m_parentID = obj.at(DEPARTMENT_ACCOUNT_PARENT_ID).toInt();
                break;
            }
        }
    }
    return data;
}

SdepartmentAccountData *CdepartmentAccountTable::getDeptAccountDataForID( int id)
{
    SdepartmentAccountData* data = nullptr;
    if (id) {
        QString condition = getColName(DEPARTMENT_ACCOUNT_ID_IDX) + "=" + QString::number(id);
        TobjectList values;
        if (getTableValues(condition, values)){
            int size = values.size();
            for (int i = 0; i < size; ++i) {
                const Tobjects& obj = values.at(i);
                data = new SdepartmentAccountData;
                data->m_deptOrAccountID = obj.at(DEPARTMENT_ACCOUNT_DEPARTMENT_OR_ACCOUNT_ID).toInt();
                data->m_id = obj.at(DEPARTMENT_ACCOUNT_ID_IDX).toInt();
                data->m_isDept = obj.at(DEPARTMENT_ACCOUNT_IS_DEPT).toBool();
                data->m_parentID = obj.at(DEPARTMENT_ACCOUNT_PARENT_ID).toInt();
                break;
            }
        }
    }
    return data;

}


int
CdepartmentAccountTable::addDepartmentAccount(int deptOrAccountID, bool isDept, int parentId)
{
    SdepartmentAccountData data;
    data.m_deptOrAccountID = deptOrAccountID;
    data.m_isDept = isDept;
    data.m_parentID = parentId;
    data.m_id = 0;
    SdepartmentAccountData* deptAccountObj = CdepartmentAccountTable::Object()->getDeptAccountDataForID(deptOrAccountID);
    if (deptAccountObj) {
        data.m_id = deptAccountObj->m_id;
        delete deptAccountObj;
    }

    return data.save();

}

int
CdepartmentAccountTable::addAccountToDeptAccount(int accountId, int parentId)
{
    parentId = addDepartmentAccount(accountId, false, parentId);
    return parentId;
}




int
SdepartmentAccountData::save()
{

    Tobjects data(DEPARTMENT_ACCOUNT_MAX_IDX);
    data[DEPARTMENT_ACCOUNT_DEPARTMENT_OR_ACCOUNT_ID] = m_deptOrAccountID;
    data[DEPARTMENT_ACCOUNT_ID_IDX] = m_id;
    data[DEPARTMENT_ACCOUNT_IS_DEPT] = m_isDept;
    data[DEPARTMENT_ACCOUNT_PARENT_ID] = m_parentID;


    if (m_id == 0) {
        TfilterObjType filter;
        filter.push_back(qMakePair(DEPARTMENT_ACCOUNT_DEPARTMENT_OR_ACCOUNT_ID, m_deptOrAccountID));
        if (CdepartmentAccountTable::Object()->isValueExist(filter) == false) {
            m_id = CdepartmentAccountTable::Object()->insertInTable(data).toInt();
        }

    } else {
        for (int i = DEPARTMENT_ACCOUNT_ID_IDX; i < DEPARTMENT_ACCOUNT_MAX_IDX; ++i) {
            CdepartmentAccountTable::Object()->updateValue(data.at(i), i, m_id, DEPARTMENT_ACCOUNT_ID_IDX);
        }

    }
    return m_id;
}

void
SdepartmentAccountData::deleteObj()
{
    if (m_id > 0) {
        //remove child

        SdbObjects* objects = CdepartmentAccountTable::Object()->getAllDepartAccountWithParent(m_id);
        if (objects) {
            int size = objects->m_listofObject.size();
            for (int i = 0; i < size; ++i) {
                objects->m_listofObject.at(i)->deleteObj();
            }
            delete objects;
        }
        CdepartmentAccountTable::Object()->deleteValue(m_id, DEPARTMENT_ACCOUNT_ID_IDX);
        m_id = 0;
    }
}
