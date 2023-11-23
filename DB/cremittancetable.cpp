#include "cremittancetable.h"


SremittanceData *CremittanceTable::getRemittanceData(int tableOD)
{
    SremittanceData* data = nullptr;
    if (tableOD > 0) {
        QString tosearch = QString::number(tableOD);
        QString condition = getSearchString(tosearch, REMITTANCE_TABLE_ID_IDX);
        TobjectList values;
        if (getTableValues(condition, values)) {
            int size = values.size();
            for (int i = 0; i < size; ++i) {
                const Tobjects& obj = values.at(i);
                data = new SremittanceData;
                data->m_id = obj.at(REMITTANCE_TABLE_ID_IDX).toInt();
                data->m_transactionID = obj.at(REMITTANCE_TABLE_TRANSACTION_ID_IDX).toInt();
                data->m_remitNameTableID = obj.at(REMITTANCE_TABLE_NAME_TABLE_ID_IDX).toInt();
                data->m_fromDate = obj.at(REMITTANCE_TABLE_PERIOD_FROM_IDX).toDate();
                data->m_toDate = obj.at(REMITTANCE_TABLE_PERIOD_TO_IDX).toDate();
                data->m_transactionDate = obj.at(REMITTANCE_TABLE_TRANSACTION_DATE_IDX).toDate();
                data->m_toAddress = obj.at(REMITTANCE_TABLE_TO_ADDRESS_IDX).toString();
                data->m_submitBy = obj.at(REMITTANCE_TABLE_TO_SUBMIT_BY_IDX).toString();

                break;

            }


        }
    }

    return data;
}

SremittanceData *CremittanceTable::getRemittanceDataFromTransID(int transID)
{
    SremittanceData* data = nullptr;
    if (transID > 0) {
        QString tosearch = QString::number(transID);
        QString condition = getSearchString(tosearch, REMITTANCE_TABLE_TRANSACTION_ID_IDX);
        TobjectList values;
        if (getTableValues(condition, values)) {
            int size = values.size();
            for (int i = 0; i < size; ++i) {
                const Tobjects& obj = values.at(i);
                data = new SremittanceData;
                data->m_id = obj.at(REMITTANCE_TABLE_ID_IDX).toInt();
                data->m_transactionID = obj.at(REMITTANCE_TABLE_TRANSACTION_ID_IDX).toInt();
                data->m_remitNameTableID = obj.at(REMITTANCE_TABLE_NAME_TABLE_ID_IDX).toInt();
                data->m_fromDate = obj.at(REMITTANCE_TABLE_PERIOD_FROM_IDX).toDate();
                data->m_toDate = obj.at(REMITTANCE_TABLE_PERIOD_TO_IDX).toDate();
                data->m_transactionDate = obj.at(REMITTANCE_TABLE_TRANSACTION_DATE_IDX).toDate();
                data->m_toAddress = obj.at(REMITTANCE_TABLE_TO_ADDRESS_IDX).toString();
                data->m_submitBy = obj.at(REMITTANCE_TABLE_TO_SUBMIT_BY_IDX).toString();

                break;

            }


        }
    }

    return data;
}

void CremittanceTable::serializeTable(QDataStream &out, const Tobjects &obj)
{
    SremittanceData data;
    data.m_id = obj.at(REMITTANCE_TABLE_ID_IDX).toInt();
    data.m_transactionID = obj.at(REMITTANCE_TABLE_TRANSACTION_ID_IDX).toInt();
    data.m_remitNameTableID = obj.at(REMITTANCE_TABLE_NAME_TABLE_ID_IDX).toInt();
    data.m_fromDate = obj.at(REMITTANCE_TABLE_PERIOD_FROM_IDX).toDate();
    data.m_toDate = obj.at(REMITTANCE_TABLE_PERIOD_TO_IDX).toDate();
    data.m_transactionDate = obj.at(REMITTANCE_TABLE_TRANSACTION_DATE_IDX).toDate();
    data.m_toAddress = obj.at(REMITTANCE_TABLE_TO_ADDRESS_IDX).toString();
    data.m_submitBy = obj.at(REMITTANCE_TABLE_TO_SUBMIT_BY_IDX).toString();
    data.serialize(out);

}

void CremittanceTable::desserializeTable(QDataStream &in)
{
    SremittanceData data;
    data.desserialize(in);
}

CremittanceTable::CremittanceTable():
    CdbTableBase(REMITTANCE_TABLE)
{
    addTableCol("remittanceTableID", FIELD_TYPE_PRIMARY_KEY, "Remittance ID", true, REMITTANCE_TABLE_START_NO);
    addTableCol("remittanceTransID", FIELD_TYPE_INT, "Trans id");
    addTableCol("remittanceTableNameID", FIELD_TYPE_INT, "Name id");
    addTableCol("remittanceFromDate", FIELD_TYPE_DATE, "From date");
    addTableCol("remittanceToDate", FIELD_TYPE_DATE, "To Date");
    addTableCol("remittanceTransactionDate", FIELD_TYPE_DATE, "Trasaction date");
    addTableCol("remittanceToAddress", FIELD_TYPE_STRING, "To address");
    addTableCol("remittanceSubmtiBy", FIELD_TYPE_STRING, "Submit By");
}

CremittanceTable::~CremittanceTable()
{

}


int SremittanceData::save()
{

    if (m_id == 0) {
        Tobjects obj(REMITTANCE_TABLE_MAX_IDX);
        obj[REMITTANCE_TABLE_ID_IDX] = m_id;
        obj[REMITTANCE_TABLE_TRANSACTION_ID_IDX] = m_transactionID;
        obj[REMITTANCE_TABLE_NAME_TABLE_ID_IDX] = m_remitNameTableID;
        obj[REMITTANCE_TABLE_PERIOD_FROM_IDX] = m_fromDate;
        obj[REMITTANCE_TABLE_PERIOD_TO_IDX] = m_toDate;
        obj[REMITTANCE_TABLE_TRANSACTION_DATE_IDX] = m_transactionDate;
        obj[REMITTANCE_TABLE_TO_ADDRESS_IDX] = m_toAddress;
        obj[REMITTANCE_TABLE_TO_SUBMIT_BY_IDX] = m_submitBy;
        m_id = CremittanceTable::Object()->insertInTable(obj).toInt();

    }  else {
        CremittanceTable::Object()->updateValue(m_transactionID, REMITTANCE_TABLE_TRANSACTION_ID_IDX, m_id, REMITTANCE_TABLE_ID_IDX);
        CremittanceTable::Object()->updateValue(m_remitNameTableID, REMITTANCE_TABLE_NAME_TABLE_ID_IDX, m_id, REMITTANCE_TABLE_ID_IDX);
        CremittanceTable::Object()->updateValue(m_fromDate, REMITTANCE_TABLE_PERIOD_FROM_IDX, m_id, REMITTANCE_TABLE_ID_IDX);
        CremittanceTable::Object()->updateValue(m_toDate, REMITTANCE_TABLE_PERIOD_TO_IDX, m_id, REMITTANCE_TABLE_ID_IDX);
        CremittanceTable::Object()->updateValue(m_transactionDate, REMITTANCE_TABLE_TRANSACTION_DATE_IDX, m_id, REMITTANCE_TABLE_ID_IDX);
        CremittanceTable::Object()->updateValue(m_toAddress, REMITTANCE_TABLE_TO_ADDRESS_IDX, m_id, REMITTANCE_TABLE_ID_IDX);
        CremittanceTable::Object()->updateValue(m_submitBy, REMITTANCE_TABLE_TO_SUBMIT_BY_IDX, m_id, REMITTANCE_TABLE_ID_IDX);

    }

    return m_id;
}

void SremittanceData::deleteObj()
{
    if (m_id) {
        CremittanceTable::Object()->deleteValue(m_id, REMITTANCE_TABLE_ID_IDX);
        QVector<SremittanceDetailData *>* details =  CremittanceDetailTable::Object()->getRemittanceDetailForTableID(m_id);
        if (details) {
            for (int i = 0; i < details->size(); ++i) {
                (*details)[i]->deleteObj();
                delete (*details)[i];
            }
            delete details;
        }


        m_id =0;
    }
}

void SremittanceData::serialize(QDataStream &out)
{
    out<<m_id<<m_transactionID<<m_remitNameTableID<<m_fromDate<<m_toDate<<m_transactionDate<<m_toAddress<<m_submitBy;
}

void SremittanceData::desserialize(QDataStream &in)
{
    in>>m_id>>m_transactionID>>m_remitNameTableID>>m_fromDate>>m_toDate>>m_transactionDate>>m_toAddress>>m_submitBy;
    qDebug()<<"reading remittance "<<m_id<<m_fromDate<<m_submitBy;
}

//

CremittanceDetailTable::CremittanceDetailTable():
    CdbTableBase(REMITTANCE_DETAIL_TABLE)
{
    addTableCol("remittanceDetailTableID", FIELD_TYPE_PRIMARY_KEY, "Remittance ID", true, REMITTANCE_DETAIL_TABLE_START_NO);
    addTableCol("remittanceTableID", FIELD_TYPE_INT, "Table id");
    addTableCol("remittanceAccountDeptID", FIELD_TYPE_INT, "Account or Dept id");
    addTableCol("remittanceLocalShareAmount", FIELD_TYPE_FLOAT, "Local Share");
    addTableCol("remittanceHqShare", FIELD_TYPE_FLOAT, "Hq Share");


}

QVector<SremittanceDetailData *> *CremittanceDetailTable::getRemittanceDetailForTableID(int id)
{
    QVector<SremittanceDetailData *>* dataV = nullptr;
    if (id > 0) {
        if (id > 0) {
            QString tosearch = QString::number(id);
            QString condition = getSearchString(tosearch, REMITTANCE_DETAIL_TABLE_REMIT_TABLE_ID_IDX);
            TobjectList values;
            if (getTableValues(condition, values)) {
                dataV = new QVector<SremittanceDetailData *>;
                int size = values.size();
                for (int i = 0; i < size; ++i) {
                    const Tobjects& obj = values.at(i);
                    SremittanceDetailData * data = new SremittanceDetailData;
                    data->m_id = obj.at(REMITTANCE_DETAIL_TABLE_ID_IDX).toInt();
                    data->m_remitanceTableID = obj.at(REMITTANCE_DETAIL_TABLE_REMIT_TABLE_ID_IDX).toInt();
                    data->m_accuntTableID = obj.at(REMITTANCE_DETAIL_TABLE_ACCOUNT_ID_IDX).toInt();
                    data->m_localShare = obj.at(REMITTANCE_DETAIL_TABLE_LOCAL_SHARE_AMOUNT_IDX).toInt();
                    data->m_hqShare = obj.at(REMITTANCE_DETAIL_TABLE_HQ_SHARE_AMOUNT_IDX).toInt();
                    dataV->push_back(data);
                }

            }
        }

    }

    return dataV;
}

void CremittanceDetailTable::serializeTable(QDataStream &out, const Tobjects &obj)
{
    SremittanceDetailData data;
    data.m_id = obj.at(REMITTANCE_DETAIL_TABLE_ID_IDX).toInt();
    data.m_remitanceTableID = obj.at(REMITTANCE_DETAIL_TABLE_REMIT_TABLE_ID_IDX).toInt();
    data.m_accuntTableID = obj.at(REMITTANCE_DETAIL_TABLE_ACCOUNT_ID_IDX).toInt();
    data.m_localShare = obj.at(REMITTANCE_DETAIL_TABLE_LOCAL_SHARE_AMOUNT_IDX).toInt();
    data.m_hqShare = obj.at(REMITTANCE_DETAIL_TABLE_HQ_SHARE_AMOUNT_IDX).toInt();
    data.serialize(out);
}

void CremittanceDetailTable::desserializeTable(QDataStream &in)
{
    SremittanceDetailData data;
    data.desserialize(in);
}

int SremittanceDetailData::save()
{
    if (m_id == 0) {
        Tobjects obj(REMITTANCE_DETAIL_TABLE_MAX_IDX);
        obj[REMITTANCE_DETAIL_TABLE_ID_IDX] = 0;
        obj[REMITTANCE_DETAIL_TABLE_REMIT_TABLE_ID_IDX] = m_remitanceTableID;
        obj[REMITTANCE_DETAIL_TABLE_ACCOUNT_ID_IDX] = m_accuntTableID;
        obj[REMITTANCE_DETAIL_TABLE_LOCAL_SHARE_AMOUNT_IDX] = m_localShare;
        obj[REMITTANCE_DETAIL_TABLE_HQ_SHARE_AMOUNT_IDX] = m_hqShare;
        m_id = CremittanceDetailTable::Object()->insertInTable(obj).toInt();
    } else {
        CremittanceDetailTable::Object()->updateValue(m_remitanceTableID, REMITTANCE_DETAIL_TABLE_REMIT_TABLE_ID_IDX, m_id,  REMITTANCE_DETAIL_TABLE_ID_IDX);
        CremittanceDetailTable::Object()->updateValue(m_accuntTableID, REMITTANCE_DETAIL_TABLE_ACCOUNT_ID_IDX, m_id,  REMITTANCE_DETAIL_TABLE_ID_IDX);
        CremittanceDetailTable::Object()->updateValue(m_localShare, REMITTANCE_DETAIL_TABLE_LOCAL_SHARE_AMOUNT_IDX, m_id,  REMITTANCE_DETAIL_TABLE_ID_IDX);
        CremittanceDetailTable::Object()->updateValue(m_hqShare, REMITTANCE_DETAIL_TABLE_HQ_SHARE_AMOUNT_IDX, m_id,  REMITTANCE_DETAIL_TABLE_ID_IDX);
    }

    return m_id;
}

void SremittanceDetailData::deleteObj()
{
    if (m_id > 0) {
        CremittanceDetailTable::Object()->deleteValue(m_id, REMITTANCE_DETAIL_TABLE_ID_IDX);
        m_id = 0;
    }

}

void SremittanceDetailData::serialize(QDataStream &out)
{
    out<<m_id<<m_remitanceTableID<<m_accuntTableID<<m_localShare<<m_hqShare;
}

void SremittanceDetailData::desserialize(QDataStream &in)
{
    in>>m_id>>m_remitanceTableID>>m_accuntTableID>>m_localShare>>m_hqShare;
   // qDebug()<<"reading remitance data "<<m_id<<" "<<m_accuntTableID<<" "<<m_localShare<<" "<<m_hqShare;

}


CremittanceReconcileTable::CremittanceReconcileTable():
    CdbTableBase(REMITTANCE_RECONCILE_TABLE)
{

    addTableCol("remittanceReconcileTableID", FIELD_TYPE_PRIMARY_KEY, "Remittance Reconcile ID", true, REMITTANCE_RECONCILE_TABLE_START_NO);
    addTableCol("remittanceTableID", FIELD_TYPE_INT, "Table id");
    addTableCol("remittanceAccountDeptID", FIELD_TYPE_INT, "Account or Dept id");
    addTableCol("remitanceReconcileIsAccount", FIELD_TYPE_BOOL, "Is account");
    addTableCol("remittanceReconcileDetail", FIELD_TYPE_STRING, "Reconcile Detail");


}

QVector<SremittanceReconcileData *> *CremittanceReconcileTable::getRemittanceReconcileForTableID(int id)
{
    QVector<SremittanceReconcileData *>* dataV = nullptr;
    if (id > 0) {
        QString tosearch = QString::number(id);
        QString condition = getSearchString(tosearch, REMITTANCE_RECONCILE_TABLE_REMIT_TABLE_ID_IDX);
        TobjectList values;
        if (getTableValues(condition, values)) {
            dataV = new QVector<SremittanceReconcileData *>;
            int size = values.size();
            for (int i = 0; i < size; ++i) {
                const Tobjects& obj = values.at(i);
                SremittanceReconcileData* data = new SremittanceReconcileData;
                data->m_id = obj.at(REMITTANCE_RECONCILE_TABLE_ID_IDX).toInt();
                data->m_remitanceTableID = obj.at(REMITTANCE_RECONCILE_TABLE_REMIT_TABLE_ID_IDX).toInt();
                data->m_accontOrDeptTableID = obj.at(REMITTANCE_RECONCILE_TABLE_ACCOUNT_DEPT_TABLE_ID_IDX).toInt();
                data->m_isAccount = obj.at(REMITTANCE_RECONCILE_TABLE_IS_ACCOUNT_IDX).toBool();
                data->m_str = obj.at(REMITTANCE_RECONCILE_TABLE_STRING_IDX).toString();
                dataV->push_back(data);

            }

        }
    }

    return dataV;
}

void CremittanceReconcileTable::serializeTable(QDataStream &out, const Tobjects &obj)
{
    SremittanceReconcileData data;
    data.m_id = obj.at(REMITTANCE_RECONCILE_TABLE_ID_IDX).toInt();
    data.m_remitanceTableID = obj.at(REMITTANCE_RECONCILE_TABLE_REMIT_TABLE_ID_IDX).toInt();
    data.m_accontOrDeptTableID = obj.at(REMITTANCE_RECONCILE_TABLE_ACCOUNT_DEPT_TABLE_ID_IDX).toInt();
    data.m_isAccount = obj.at(REMITTANCE_RECONCILE_TABLE_IS_ACCOUNT_IDX).toBool();
    data.m_str = obj.at(REMITTANCE_RECONCILE_TABLE_STRING_IDX).toString();
    data.serialize(out);
}

void CremittanceReconcileTable::desserializeTable(QDataStream &in)
{
    SremittanceReconcileData data;
    data.desserialize(in);
}

int SremittanceReconcileData::save()
{
    if (m_id == 0) {
        Tobjects obj(REMITTANCE_RECONCILE_TABLE_MAX_IDX);
        obj[REMITTANCE_RECONCILE_TABLE_ID_IDX] = 0;
        obj[REMITTANCE_RECONCILE_TABLE_REMIT_TABLE_ID_IDX] = m_remitanceTableID;
        obj[REMITTANCE_RECONCILE_TABLE_ACCOUNT_DEPT_TABLE_ID_IDX] = m_accontOrDeptTableID;
        obj[REMITTANCE_RECONCILE_TABLE_IS_ACCOUNT_IDX] = m_isAccount;
        obj[REMITTANCE_RECONCILE_TABLE_STRING_IDX] = m_str;
        m_id = CremittanceReconcileTable::Object()->insertInTable(obj).toInt();
    } else {
        CremittanceReconcileTable::Object()->updateValue(m_remitanceTableID, REMITTANCE_RECONCILE_TABLE_REMIT_TABLE_ID_IDX, m_id,  REMITTANCE_RECONCILE_TABLE_ID_IDX);
        CremittanceReconcileTable::Object()->updateValue(m_accontOrDeptTableID, REMITTANCE_RECONCILE_TABLE_ACCOUNT_DEPT_TABLE_ID_IDX, m_id,  REMITTANCE_RECONCILE_TABLE_ID_IDX);
        CremittanceReconcileTable::Object()->updateValue(m_isAccount, REMITTANCE_RECONCILE_TABLE_IS_ACCOUNT_IDX, m_id,  REMITTANCE_RECONCILE_TABLE_ID_IDX);
        CremittanceReconcileTable::Object()->updateValue(m_str, REMITTANCE_RECONCILE_TABLE_STRING_IDX, m_id,  REMITTANCE_RECONCILE_TABLE_ID_IDX);
    }

    return m_id;
}

void SremittanceReconcileData::deleteObj()
{
    if (m_id > 0) {
        CremittanceReconcileTable::Object()->deleteValue(m_id, REMITTANCE_RECONCILE_TABLE_ID_IDX);
        m_id = 0;
    }
}

void SremittanceReconcileData::serialize(QDataStream &out)
{
    out<<m_id<<m_remitanceTableID<<m_accontOrDeptTableID<<m_isAccount<<m_str;
}

void SremittanceReconcileData::desserialize(QDataStream &in)
{
    in>>m_id>>m_remitanceTableID>>m_accontOrDeptTableID>>m_isAccount>>m_str;
    qDebug()<<"reading remit concile "<<m_id<< ""<<m_str;

}

SremitTransDetail::SremitTransDetail(QString str)
{
    m_transID = 0;
    QStringList split = str.split("#");
    if (split.size() == 5) {
        m_from = split.at(0);
        m_amount = split.at(1);
        m_receiptBook = split.at(2);
        m_receiptNo = split.at(3);
        m_accountID = split.at(4).toInt();
    }
}

QString SremitTransDetail::toString()
{
    QString str = m_from + "#" + m_amount + "#" + m_receiptBook + "#" + m_receiptNo+ "#" + QString::number(m_accountID);
    /*QString m_from;
    QString m_amount;
    QString m_receiptBook;
    QString m_receiptNo;*/
    QStringList split = str.split("#");
    //qDebug()<<split;
    return str;
}
