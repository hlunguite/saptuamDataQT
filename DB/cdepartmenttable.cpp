#include "cdepartmenttable.h"
#include "cdepartmentaccounttable.h"
CdepartmentTable::CdepartmentTable():
    CdbTableBase(DEPARTMENT_TABLE)
{
    addTableCol("departmentID", FIELD_TYPE_PRIMARY_KEY, "ID", true, DEPARTMENT_TABLE_START_NO);
    addTableCol("departmentName", FIELD_TYPE_STRING, "Department Name", false);
    addTableCol("departmentShortName", FIELD_TYPE_STRING, "Dept Short Name", false);
    addTableCol("departmentPaymentAccountID", FIELD_TYPE_INT, "Payment Account", false);
}

SdepartmentData *CdepartmentTable::getDepartmentDataByName(QString deptName)
{
    SdepartmentData* data = nullptr;
    QString conditon = getSearchString(deptName, DEPARTMENT_NAME_IDX);
    TobjectList values;
    if (getTableValues(conditon, values)) {
        const Tobjects& obj = values.at(0);
        data = new SdepartmentData;
        data->m_id = obj.at(DEPARTMENT_ID_IDX).toInt();
        data->m_deptName = obj.at(DEPARTMENT_NAME_IDX).toString();
        data->m_deptShortName = obj.at(DEPARTMENT_SHORT_NAME_IDX).toString();
        data->m_paymentAccuntID = obj.at(DEPARTMENT_PAYMENT_ACCOUNT_ID_IDX).toInt();
    }

    return data;
}

SdepartmentData *CdepartmentTable::getDepartmentDataByShortName(QString deptName)
{
    SdepartmentData* data = nullptr;
    QString conditon = getSearchString(deptName, DEPARTMENT_SHORT_NAME_IDX);

    TobjectList values;
    if (getTableValues(conditon, values)) {
        const Tobjects& obj = values.at(0);
        data = new SdepartmentData;
        data->m_id = obj.at(DEPARTMENT_ID_IDX).toInt();
        data->m_deptName = obj.at(DEPARTMENT_NAME_IDX).toString();
        data->m_deptShortName = obj.at(DEPARTMENT_SHORT_NAME_IDX).toString();
        data->m_paymentAccuntID = obj.at(DEPARTMENT_PAYMENT_ACCOUNT_ID_IDX).toInt();
    }

    return data;
}

SdepartmentData *CdepartmentTable::getDepartmentDataByID(int id)
{
    SdepartmentData* data = nullptr;
    if (id > 0) {
        QString tosearch = QString::number(id);
        QString conditon = getSearchString(tosearch, DEPARTMENT_ID_IDX);

        TobjectList values;
        if (getTableValues(conditon, values)) {
            const Tobjects& obj = values.at(0);
            data = new SdepartmentData;
            data->m_id = obj.at(DEPARTMENT_ID_IDX).toInt();
            data->m_deptName = obj.at(DEPARTMENT_NAME_IDX).toString();
            data->m_deptShortName = obj.at(DEPARTMENT_SHORT_NAME_IDX).toString();
            data->m_paymentAccuntID = obj.at(DEPARTMENT_PAYMENT_ACCOUNT_ID_IDX).toInt();
        }
    }
    return data;
}

void CdepartmentTable::serializeTable(QDataStream &out, const Tobjects& obj)
{

    SdepartmentData data;
    data.m_id = obj.at(DEPARTMENT_ID_IDX).toInt();
    data.m_deptName = obj.at(DEPARTMENT_NAME_IDX).toString();
    data.m_deptShortName = obj.at(DEPARTMENT_SHORT_NAME_IDX).toString();
    data.m_paymentAccuntID = obj.at(DEPARTMENT_PAYMENT_ACCOUNT_ID_IDX).toInt();
    data.serialize(out);
}

void CdepartmentTable::desserializeTable(QDataStream &in)
{

    SdepartmentData data;
    data.desserialize(in);

}

CdepartmentTable::~CdepartmentTable()
{

}


int SdepartmentData::save()
{
    Tobjects data(DEPARTMENT_MAX_ID);
    data[DEPARTMENT_ID_IDX] =  m_id;
    data[DEPARTMENT_NAME_IDX] = m_deptName;
    data[DEPARTMENT_SHORT_NAME_IDX] = m_deptShortName;
    data[DEPARTMENT_PAYMENT_ACCOUNT_ID_IDX] = m_paymentAccuntID;

    if (m_id == 0) {
        //new object
        TfilterObjType filter;
        filter.push_back(qMakePair(DEPARTMENT_NAME_IDX, m_deptName));
        if (CdepartmentTable::Object()->isValueExist(filter) == false) {
            m_id = CdepartmentTable::Object()->insertInTable(data).toInt();
            if (m_id > 0) {
                //update dept account table
                SdepartmentAccountData data;
                data.m_id = 0;
                data.m_deptOrAccountID = m_id;
                data.m_isDept = true;
                data.m_parentID = 0;
                data.save();
            }
        }
    } else {
        //update object
        CdepartmentTable::Object()->updateValue(m_deptName, DEPARTMENT_NAME_IDX, m_id, DEPARTMENT_ID_IDX);
        CdepartmentTable::Object()->updateValue(m_deptShortName, DEPARTMENT_SHORT_NAME_IDX, m_id, DEPARTMENT_ID_IDX);
        CdepartmentTable::Object()->updateValue(m_paymentAccuntID, DEPARTMENT_PAYMENT_ACCOUNT_ID_IDX, m_id, DEPARTMENT_ID_IDX);
    }

    return m_id;
}

void SdepartmentData::deleteObj()
{

    if (m_id > 0) {
        CdepartmentTable::Object()->deleteValue(m_id, DEPARTMENT_ID_IDX);
        m_id = 0;
    }
}

void SdepartmentData::serialize(QDataStream &out)
{
    out<<m_id<<m_deptName<<m_deptShortName<<m_paymentAccuntID;
    //qDebug()<<"dumping dep "<<m_id<<" "<<m_deptName;
}

void SdepartmentData::desserialize(QDataStream &in)
{
    in>>m_id>>m_deptName>>m_deptShortName>>m_paymentAccuntID;
    qDebug()<<"reading dep "<<m_id<<" "<<m_deptName;

}




