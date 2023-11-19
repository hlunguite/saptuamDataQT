#include "cclosingbalancetable.h"

/*    CLOSING_TABLE_ID_IDX,
    CLOSING_DATE_IDX,
    MAX_CLOSING_TABLE_FIELD*/
void CclosingBalanceTable::serializeTable(QDataStream &out, const Tobjects &obj)
{
    SclosingData data;
    data.m_id = obj.at(CLOSING_TABLE_ID_IDX).toInt();
    data.m_closingDate = obj.at(CLOSING_DATE_IDX).toDate();
    data.serialize(out);
}

void CclosingBalanceTable::desserializeTable(QDataStream &in)
{
    SclosingData data;
    data.desserialize(in);
}

CclosingBalanceTable::CclosingBalanceTable():
    CdbTableBase(CLOSING_TABLE)
{
    addTableCol("closingID", FIELD_TYPE_PRIMARY_KEY, "closing ID", true, CLOSING_TABLE_START_NO);
    addTableCol("closingDate", FIELD_TYPE_DATE, "Closing Date");

}

SclosingData CclosingBalanceTable::getClosingForDate(QDate date, bool dateOnOrBefore)
{
    QString condition = getColName(CLOSING_DATE_IDX);
    if (dateOnOrBefore) {
        condition += "<=" + date.toString("yyyy-MM-dd");

    } else {
        condition += "=" + date.toString("yyyy-MM-dd");
    }
    condition += " ORDER BY " + getColName(CLOSING_DATE_IDX) + " DESC";
    TobjectList values;
    SclosingData data;
    data.m_id = 0;
    if (getTableValues(condition, values)) {
        data.m_id = values.at(0).at(CLOSING_TABLE_ID_IDX).toInt();
        data.m_closingDate = values.at(0).at(CLOSING_DATE_IDX).toDate();

    }
    return data;
}

int SclosingData::save()
{
    QVector<QVariant> data(MAX_CLOSING_TABLE_FIELD);
    data[CLOSING_TABLE_ID_IDX] = m_id;
    data[CLOSING_DATE_IDX] = m_closingDate;


    if (m_id == 0) {
        TfilterObjType filter;
        TobjectList values;
        filter.push_back(QPair<int, QVariant>(CLOSING_DATE_IDX, m_closingDate));

        if (CclosingBalanceTable::Object()->isValueExist(filter, values) == false) {
            m_id = CclosingBalanceTable::Object()->insertInTable(data).toInt();
            //return m_id;
        } else {
            m_id = values.at(0).at(CLOSING_TABLE_ID_IDX).toInt();
        }
    } else {

    }
    return m_id;
}

void SclosingData::deleteObj()
{
    Q_ASSERT(0);
}

void SclosingData::serialize(QDataStream &out)
{
    out<<m_id<<m_closingDate;
}

void SclosingData::desserialize(QDataStream &in)
{
    in>>m_id>>m_closingDate;
}



/*
    CLOSING_DETAIL_TABLE_ID_IDX,
    CLOSING_DETAIL_CLOSING_TABLE_ID_IDX,
    CLOSING_DETAIL_ACCOUNT_DEPET_TABLE_ID_IDX,
    CLOSING_DETAIL_IS_ACCOUNT_ID_IDX,
    CLOSING_DETAIL_AMOUNT_TABLE_ID_IDX,
    CLOSING_DETAIL_ACCOUNT_NAME_ID_IDX, // if accuont id is zero. mostly in caseh of bank or cash
    MAX_CLOSING_DETAIL_TABLE_FIELD*/
CclosingBalanceDetailTable::CclosingBalanceDetailTable():
    CdbTableBase(CLOSING_DETAIL_TABLE)
{
    addTableCol("closingDetailID", FIELD_TYPE_PRIMARY_KEY, "closing Detail ID", true, CLOSING_DETAIL_TABLE_START_NO);
    addTableCol("closingTableID", FIELD_TYPE_INT, "closing table id");
    addTableCol("accountordeptTableID", FIELD_TYPE_INT, "account or dept table id");
    addTableCol("closingisAccount", FIELD_TYPE_BOOL, "Is Account");
    addTableCol("closingAmount", FIELD_TYPE_DECIMAL, "Amount");
    addTableCol("closingaccname", FIELD_TYPE_STRING, "Cash or Bank account");

}
void CclosingBalanceDetailTable::serializeTable(QDataStream &out, const Tobjects &obj)
{
    SclosingDetailData data;
    data.m_id = obj.at(CLOSING_DETAIL_TABLE_ID_IDX).toInt();
    data.m_closingTableID = obj.at(CLOSING_DETAIL_CLOSING_TABLE_ID_IDX).toInt();
    data.m_accountOrDeptTableID = obj.at(CLOSING_DETAIL_ACCOUNT_DEPET_TABLE_ID_IDX).toInt();
    data.m_isAccount = obj.at(CLOSING_DETAIL_IS_ACCOUNT_ID_IDX).toBool();
    data.m_amount = obj.at(CLOSING_DETAIL_AMOUNT_TABLE_ID_IDX).toDouble();
    data.m_cashOrBankName = obj.at(CLOSING_DETAIL_ACCOUNT_NAME_ID_IDX).toString();

    data.serialize(out);
}

void CclosingBalanceDetailTable::desserializeTable(QDataStream &in)
{
    SclosingDetailData data;
    data.desserialize(in);
}



QVector<SclosingDetailData *> CclosingBalanceDetailTable::getClosingDetailForID(int closingID)
{
    QVector<SclosingDetailData *> dataV;
    QString condition = getColName(CLOSING_DETAIL_CLOSING_TABLE_ID_IDX) + "=" + QString::number(closingID);
    TobjectList values;
    if(getTableValues(condition, values)) {

    }

    return dataV;

}

int SclosingDetailData::save()
{
    QVector<QVariant> data(MAX_CLOSING_DETAIL_TABLE_FIELD);

    data[CLOSING_DETAIL_TABLE_ID_IDX] = m_id;
    data[CLOSING_DETAIL_CLOSING_TABLE_ID_IDX] =  m_closingTableID;
    data[CLOSING_DETAIL_ACCOUNT_DEPET_TABLE_ID_IDX] = m_accountOrDeptTableID;
    data[CLOSING_DETAIL_IS_ACCOUNT_ID_IDX] = m_isAccount;
    data[CLOSING_DETAIL_AMOUNT_TABLE_ID_IDX] =  m_amount;
    data[CLOSING_DETAIL_ACCOUNT_NAME_ID_IDX] = m_cashOrBankName;
    if (m_id == 0) {
        TfilterObjType filter;
        TobjectList values;
        filter.push_back(QPair<int, QVariant>(CLOSING_DETAIL_CLOSING_TABLE_ID_IDX, m_closingTableID));
        filter.push_back(QPair<int, QVariant>(CLOSING_DETAIL_ACCOUNT_DEPET_TABLE_ID_IDX,m_accountOrDeptTableID ));

        if (CclosingBalanceDetailTable::Object()->isValueExist(filter, values) == false) {
            m_id = CclosingBalanceDetailTable::Object()->insertInTable(data).toInt();
            //return m_id;
        } else {
            m_id = values.at(0).at(CLOSING_DETAIL_TABLE_ID_IDX).toInt();
        }
    } else {
        CclosingBalanceDetailTable::Object()->updateValue(m_closingTableID, CLOSING_DETAIL_CLOSING_TABLE_ID_IDX, m_id, CLOSING_DETAIL_TABLE_ID_IDX);
        CclosingBalanceDetailTable::Object()->updateValue(m_accountOrDeptTableID, CLOSING_DETAIL_ACCOUNT_DEPET_TABLE_ID_IDX, m_id, CLOSING_DETAIL_TABLE_ID_IDX);
        CclosingBalanceDetailTable::Object()->updateValue(m_isAccount, CLOSING_DETAIL_IS_ACCOUNT_ID_IDX, m_id, CLOSING_DETAIL_TABLE_ID_IDX);
        CclosingBalanceDetailTable::Object()->updateValue(m_amount, CLOSING_DETAIL_AMOUNT_TABLE_ID_IDX, m_id, CLOSING_DETAIL_TABLE_ID_IDX);
        CclosingBalanceDetailTable::Object()->updateValue(m_cashOrBankName, CLOSING_DETAIL_ACCOUNT_NAME_ID_IDX, m_id, CLOSING_DETAIL_TABLE_ID_IDX);

    }

    return m_id;
}

void SclosingDetailData::deleteObj()
{
    if (m_id > 0) {
        CclosingBalanceDetailTable::Object()->deleteValue(m_id, CLOSING_DETAIL_TABLE_ID_IDX);
        m_id = 0;
    }
}

void SclosingDetailData::serialize(QDataStream &out)
{
    out<<m_id<<m_closingTableID<<m_accountOrDeptTableID<<m_isAccount<<m_amount<<m_cashOrBankName;
}

void SclosingDetailData::desserialize(QDataStream &in)
{
    in>>m_id>>m_closingTableID>>m_accountOrDeptTableID>>m_isAccount>>m_amount>>m_cashOrBankName;

}

