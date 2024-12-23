#include "cbanktransactiontable.h"
#include "csdutils.h"
CbankTransactionTable::CbankTransactionTable():
    CdbTableBase(BANK_TRANSACTION_TABLE)
{
    addTableCol("bankTransactionTableId", FIELD_TYPE_PRIMARY_KEY, "ID", true, BANK_TRANSACTION_TABLE_START_NO);
    addTableCol("bankTransactionTableMin", FIELD_TYPE_STRING, "min");
    addTableCol("bankTransactionTableRefNo", FIELD_TYPE_STRING, "ref");
    addTableCol("bankTransactionTablePhone", FIELD_TYPE_STRING, "phone");
    addTableCol("bankTransactionTablePiakchan", FIELD_TYPE_STRING, "piakchan");
    addTableCol("bankTransactionTableDate", FIELD_TYPE_STRING, "date");
    addTableCol("bankTransactionTableAmount", FIELD_TYPE_DECIMAL, "amount");
    addTableCol("bankTransactionTableIsIncome", FIELD_TYPE_BOOL, "isIncome");
    addTableCol("bankTransactionTableType", FIELD_TYPE_INT, "type");
    addTableCol("bankTransactionTableStatus", FIELD_TYPE_INT, "status");
    addTableCol("bankTransactionTableImportId", FIELD_TYPE_INT, "importID");
    addTableCol("bankTransactionTableFullString", FIELD_TYPE_STRING, "fullstring");
}

QVector<SbanktransDetail> CbankTransactionTable::getAllbankTransForSameImport(int importID)
{
    QVector<SbanktransDetail> available;
    QString whenstatement = " WHERE " +  getColName(BANK_TRANSACTION_TABLE_IMPORT_ID) +"=" + QString::number(importID);
    QString query = "SELECT * FROM " + getTableName() + whenstatement;
    QSqlQuery q(getDataBase());
    if(q.exec(query)){
        while(q.next()){
            SbanktransDetail bankTrans;
            bankTrans.m_transID = q.value(BANK_TRANSACTION_TABLE_ID).toInt();
            bankTrans.m_min = q.value(BANK_TRANSACTION_TABLE_MIN).toString();
            bankTrans.m_refID = q.value(BANK_TRANSACTION_TABLE_REFID).toString();
            bankTrans.m_phone = q.value(BANK_TRANSACTION_TABLE_PHONE).toString();
            bankTrans.m_piakChan = q.value(BANK_TRANSACTION_TABLE_PIAKCHAN).toString();
            bankTrans.m_date = q.value(BANK_TRANSACTION_TABLE_DATE).toString();
            bankTrans.m_amount = q.value(BANK_TRANSACTION_TABLE_AMOUNT).toDouble();
            bankTrans.m_isIncome = q.value(BANK_TRANSACTION_TABLE_IS_INCOME).toBool();
            bankTrans.m_type = (EbankTransType)q.value(BANK_TRANSACTION_TABLE_TYPE).toInt();
            bankTrans.m_transStatus = (EbankTransStatus)q.value(BANK_TRANSACTION_TABLE_STATUS).toInt();
            bankTrans.m_importID = q.value(BANK_TRANSACTION_TABLE_IMPORT_ID).toInt();
            available.push_back(bankTrans);
        }
    }
    return available;
}

int CbankTransactionTable::getIsTransactionAvailable(QString findStr)
{
    QVector<SbanktransDetail> available;
    QString whenstatement = " WHERE " +  getColName(BANK_TRANSACTION_TABLE_FULLSTRING) +" LIKE \"%" + findStr +"%\"";
    QString query = "SELECT * FROM " + getTableName() + whenstatement;
    //qDebug()<<"query for bank trans table :"<<query;
    QSqlQuery q(getDataBase());
    if(q.exec(query)){
        while(q.next()){
            return q.value(BANK_TRANSACTION_TABLE_ID).toInt();
        }
    }
    return 0;
}



QString SbanktransDetail::toString() const
{
    QString txt;
    if (m_min.isEmpty() == false) {
        txt += "Min:" + m_min + " ";
    }
    if (m_refID.isEmpty() == false) {
        txt += "RefId:" +  m_refID + " ";
    }
    if (m_phone.isEmpty() == false) {
        txt  += "Phone:" + m_phone + " ";
    }
    if (m_piakChan.isEmpty() == false) {
        txt += "Piakchan:" + m_piakChan + " ";
    }
    if (m_amount > 0) {

        QString amount = CsdUtils::convertAmountToStringWithSign(m_amount);

        txt += "Amount:" + amount + " ";
        if (m_isIncome) {
            txt += "Muhna ";
        } else {
            txt += "Zatna ";
        }
    }
    if (m_date.isEmpty() == false) {
        txt += m_date + " ";
    }
    if (m_type > INVALID && m_type < NOTASIGN) {
        QString type [] = {"INVALID",  "Bank Cash Deposit", "Cheque", "Bank Charges", "Bank Interest", "Bank Cash Withdrawal", "NOTASIGN"};
        txt += "Type:"+ type[m_type];
    }

    return txt.trimmed();
}

int SbanktransDetail::save()
{
    if (m_transID == 0) {
        Tobjects obj(BANK_TRANSACTION_TABLE_MAX_IDX);
        obj[BANK_TRANSACTION_TABLE_ID] = m_transID;
        obj[BANK_TRANSACTION_TABLE_MIN] = m_min;
        obj[BANK_TRANSACTION_TABLE_REFID] = m_refID;
        obj[BANK_TRANSACTION_TABLE_PHONE] = m_phone;
        obj[BANK_TRANSACTION_TABLE_PIAKCHAN] = m_piakChan;
        obj[BANK_TRANSACTION_TABLE_DATE] = m_date;
        obj[BANK_TRANSACTION_TABLE_AMOUNT] = m_amount;
        obj[BANK_TRANSACTION_TABLE_IS_INCOME] = m_isIncome;
        obj[BANK_TRANSACTION_TABLE_TYPE] = m_type;
        obj[BANK_TRANSACTION_TABLE_STATUS] = m_transStatus;
        obj[BANK_TRANSACTION_TABLE_IMPORT_ID] = m_importID;
        obj[BANK_TRANSACTION_TABLE_FULLSTRING] = toString();
        int id = CbankTransactionTable::Object()->getIsTransactionAvailable(toString());
        if (id == 0) {
            m_transID = CbankTransactionTable::Object()->insertInTable(obj).toInt();
            //qDebug()<<"save bank transaction table "<<m_transID;
        } else {
            m_transID = id;
        }

    } else {
        CbankTransactionTable::Object()->updateValue(m_transStatus,
                                                     BANK_TRANSACTION_TABLE_STATUS,
                                                     m_transID,
                                                     BANK_TRANSACTION_TABLE_ID);
    }
    return m_transID;
}

void SbanktransDetail::deleteObj()
{
    if (m_transID > 0) {
        CbankTransactionTable::Object()->deleteValue(m_transID, BANK_TRANSACTION_TABLE_ID);

    }
}

