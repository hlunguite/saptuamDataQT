#include "caccounttable.h"
#include <QPair>
CaccountTable::CaccountTable():
    CdbTableBase(ACCOUNT_TABLE)
{

    addTableCol("accountID", FIELD_TYPE_PRIMARY_KEY, "Account ID", true, ACCOUNT_TABLE_START_NO);
    addTableCol("accountName", FIELD_TYPE_STRING, "Account Name", false);
    addTableCol("accountType", FIELD_TYPE_INT, "Account Type", false);
    addTableCol("accountIsUseHqReceipt", FIELD_TYPE_BOOL, "Use Hq Receipt", true);
    addTableCol("accountPaymentAccountId", FIELD_TYPE_INT, "Payment Account", false);
    addTableCol("accountIsActive", FIELD_TYPE_BOOL, "Is Account Active?", false);
    addTableCol("accountBankAccountNo", FIELD_TYPE_STRING, "Bank Account", false);

}

CaccountTable::~CaccountTable()
{

}

void CaccountTable::serializeTable(QDataStream &out, const Tobjects &obj)
{
    SaccountData data;
    data.m_id = obj.at(ACCOUNT_ID_IDX).toInt();
    data.m_accountName = obj.at(ACCOUNT_NAME_IDX).toString();
    data.m_accountType = obj.at(ACCOUNT_TYPE_IDX).toInt();
    data.m_isUseHqReceipt = obj.at(ACCOUNT_USE_HQ_RECEIPT_IDX).toBool();
    data.m_paymentAccountID = obj.at(ACCOUNT_PAYMENT_ACCOUNT_ID_IDX).toInt();
    data.m_isActiveAccount = obj.at(ACCOUNT_IS_ACTIVE_IDX).toBool();
    data.m_bankAccountName = obj.at(ACCOUNT_BANK_ACCOUNT_NO_IDX).toString();
    data.serialize(out);
}

void CaccountTable::desserializeTable(QDataStream &in)
{
    SaccountData data;
    data.desserialize(in);
}

SaccountData *CaccountTable::getAccountDataForId(int accountID)
{
    SaccountData* data = nullptr;
    QString tosearch = QString::number(accountID);
    QString condition = getSearchString(tosearch, ACCOUNT_ID_IDX);
    TobjectList values;
    if (getTableValues(condition, values)) {
        const Tobjects& obj = values.at(0);
        data = new SaccountData;
        data->m_id = obj.at(ACCOUNT_ID_IDX).toInt();
        data->m_accountName = obj.at(ACCOUNT_NAME_IDX).toString();
        data->m_accountType = obj.at(ACCOUNT_TYPE_IDX).toInt();
        data->m_isUseHqReceipt = obj.at(ACCOUNT_USE_HQ_RECEIPT_IDX).toBool();
        data->m_paymentAccountID = obj.at(ACCOUNT_PAYMENT_ACCOUNT_ID_IDX).toInt();
        data->m_isActiveAccount = obj.at(ACCOUNT_IS_ACTIVE_IDX).toBool();
        data->m_bankAccountName = obj.at(ACCOUNT_BANK_ACCOUNT_NO_IDX).toString();
    }

    return data;
}

SaccountData *CaccountTable::getAccountDataForName(QString accountName)
{

    SaccountData* data = nullptr;
    TobjectList values;
    QString condition = getSearchString(accountName, ACCOUNT_NAME_IDX);

    if (getTableValues(condition, values)) {
        const Tobjects& obj = values.at(0);
        data = new SaccountData;
        data->m_id = obj.at(ACCOUNT_ID_IDX).toInt();
        data->m_accountName = obj.at(ACCOUNT_NAME_IDX).toString();
        data->m_accountType = obj.at(ACCOUNT_TYPE_IDX).toInt();
        data->m_isUseHqReceipt = obj.at(ACCOUNT_USE_HQ_RECEIPT_IDX).toBool();
        data->m_paymentAccountID = obj.at(ACCOUNT_PAYMENT_ACCOUNT_ID_IDX).toInt();
        data->m_isActiveAccount = obj.at(ACCOUNT_IS_ACTIVE_IDX).toBool();
        data->m_bankAccountName = obj.at(ACCOUNT_BANK_ACCOUNT_NO_IDX).toString();
    }

    return data;
}

SaccountData::SaccountData():SdbObject(){}
SaccountData::~SaccountData(){}
int SaccountData::save()
{
    QVector<QVariant> data(MAX_ACCOUNT_TABLE_FIELD);
    data[ACCOUNT_ID_IDX] = m_id;
    data[ACCOUNT_NAME_IDX] = m_accountName;
    data[ACCOUNT_TYPE_IDX] = m_accountType;
    data[ACCOUNT_USE_HQ_RECEIPT_IDX] = m_isUseHqReceipt;
    data[ACCOUNT_PAYMENT_ACCOUNT_ID_IDX] = m_paymentAccountID;
    data[ACCOUNT_IS_ACTIVE_IDX] = m_isActiveAccount;
    data[ACCOUNT_BANK_ACCOUNT_NO_IDX] = m_bankAccountName;

    if (m_id == 0) {
        TfilterObjType filter;
        filter.push_back(QPair<int, QVariant>(ACCOUNT_NAME_IDX, m_accountName));
        if (CaccountTable::Object()->isValueExist(filter) == false) {
            data[ACCOUNT_ID_IDX] = 0;
            m_id = CaccountTable::Object()->insertInTable(data).toInt();
        }
    } else {
        // update
        for (unsigned int i = ACCOUNT_NAME_IDX; i < MAX_ACCOUNT_TABLE_FIELD; ++i) {
            CaccountTable::Object()->updateValue(data.at(i), i, m_id, ACCOUNT_ID_IDX);
        }
    }
    return m_id;
}

void SaccountData::deleteObj()
{
    if (m_id > 0) {
        CaccountTable::Object()->deleteValue(m_id, ACCOUNT_ID_IDX);
    }
}

void SaccountData::serialize(QDataStream &out)
{
    out<<m_id<<m_accountName<<m_accountType<<m_isUseHqReceipt<<m_paymentAccountID<<m_isActiveAccount<<m_bankAccountName;
}

void SaccountData::desserialize(QDataStream &in)
{
    in>>m_id>>m_accountName>>m_accountType>>m_isUseHqReceipt>>m_paymentAccountID>>m_isActiveAccount>>m_bankAccountName;
    //qDebug()<<"reading account "<<m_id<<" "<<m_accountName;
}


QString CaccountUtils::getAccountType(int accountType)
{

    QMap<int, QString>::iterator fn = m_accountTypeIdStringMap.find(accountType);
    if (fn != m_accountTypeIdStringMap.end()) {
        return fn.value();
    }
    return "";
}

int CaccountUtils::getAccountTypeId(QString accountType)
{
    if (accountType.isEmpty() == false) {
        QMap<QString, int>::iterator fn = m_accountTypeStringIdMap.find(accountType);
        if (fn != m_accountTypeStringIdMap.end()) {
            return fn.value();
        }
    }
    return INVALID_ACOUNT_TYPE;
}

void CaccountUtils::getAccountTypesForNewAccountType(QVector<QString> &accountType)
{
    QMap<int, QString>::iterator beg = m_accountTypeIdStringMap.begin();
    QMap<int, QString>::iterator end = m_accountTypeIdStringMap.end();
    while (beg != end) {
        if (beg.key() == INCOME_ACCOUNT_TYPE ||
                beg.key() == PAYMENT_ACCOUNT_TYPE ||
                beg.key() == LOAN_ACCOUNT_TYPE) {
            accountType.push_back(beg.value());
        }
        ++beg;
    }
}

void CaccountUtils::serialize(QDataStream &out)
{
    int size = m_accountTypeIdStringMap.size();
    out<<size;
    for (auto keyValue : m_accountTypeIdStringMap.asKeyValueRange()) {
        out<<(int)keyValue.first<<(QString)keyValue.second;
    }
}

void CaccountUtils::desserialize(QDataStream &in)
{
    int size = 0;
    in>>size;
    for (int i = 0; i < size; ++i) {
        int type;
        QString typeStr;
        in>>type>>typeStr;
    }
}

CaccountUtils::CaccountUtils()
{
    m_accountTypeIdStringMap.insert(INCOME_ACCOUNT_TYPE, "Income Account");
    m_accountTypeIdStringMap.insert(PAYMENT_ACCOUNT_TYPE, "Payment Account");
    m_accountTypeIdStringMap.insert(BANK_ACCOUNT_TYPE, "Bank Account");
    m_accountTypeIdStringMap.insert(REMITTANCE_ACCOUNT_TYPE, "Remittance Account");
    m_accountTypeIdStringMap.insert(REQUEST_ACCOUNT_TYPE, "Request Account");
    m_accountTypeIdStringMap.insert(LOAN_ACCOUNT_TYPE, "Loan Account");
    QMap<int, QString>::iterator beg = m_accountTypeIdStringMap.begin();
    QMap<int, QString>::iterator end = m_accountTypeIdStringMap.end();
    while (beg != end) {
        m_accountTypeStringIdMap.insert(beg.value(), beg.key());
        ++beg;
    }



}
