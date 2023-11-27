#include "ctransactiontable.h"

CtransactionTable::CtransactionTable():
    CdbTableBase(TRANSACTION_TABLE)
{
    addTableCol("transactionID", FIELD_TYPE_PRIMARY_KEY, "ID", false, TRANSACTION_TABLE_START_NO);
    addTableCol("transactionFrom", FIELD_TYPE_INT, "From/To");
    addTableCol("transactionAmount", FIELD_TYPE_DECIMAL, "Amount");
    addTableCol("transactionAccount", FIELD_TYPE_INT, "Account");
    addTableCol("transactionDate", FIELD_TYPE_DATE, "Date");
    addTableCol("transactionType", FIELD_TYPE_INT, "Transaction Type");
    addTableCol("transactionMode", FIELD_TYPE_INT, "Transaction Mode");
    addTableCol("transactionRef",FIELD_TYPE_STRING, "Reference");
    addTableCol("transactionRemitId", FIELD_TYPE_INT, "Remittance Id", true);
    addTableCol("transactionParentId", FIELD_TYPE_INT,"Parent Id", true);
    addTableCol("transactionClosingId", FIELD_TYPE_INT,"Closing Id", true);
    addTableCol("transactionBankId", FIELD_TYPE_INT, "Bank Id", true);
    addTableCol("transactionStatus", FIELD_TYPE_INT, "Status", true);
    addTableCol("transactionReceiptBook", FIELD_TYPE_STRING, "Receipt Book");
    addTableCol("transactionReceiptNo", FIELD_TYPE_STRING, "Receipt No");
    addTableCol("transactionParticular", FIELD_TYPE_STRING, "Remark");
}

CtransactionTable::~CtransactionTable()
{

}

int CtransactionTable::totalTransaction(int accountID)
{
    QString condition;
    if (accountID > 0) {
        condition = getColName(TRANSACTION_ACCOUNT_IDX) + "=" + QString::number(accountID);
    }
    int count = 0;
    TobjectList values;

    if (getTableValues(condition, values)) {
        count = values.size();
    }

    return count;
}

QVector<StransactionData*> CtransactionTable::getAllTransactionForRefId(QString refId)
{
    QString query;
    if (refId.isEmpty() == false) {
        QString whenstatement = " WHERE " +  getColName(TRANSACTION_REF_IDX);
        whenstatement += " LIKE \"%" + refId +"%\"";
        query = "SELECT * FROM " + getTableName() + whenstatement;
    }
    return getAllTransaction(query);


}

QVector<StransactionData*> CtransactionTable::getAllTransactionForContact(int contactID)
{

    QString query;
    if (contactID > 0) {
        QString whenstatement = " WHERE " +  getColName(TRANSACTION_FROM_IDX);
        whenstatement += "=" + QString::number(contactID);
        query = "SELECT * FROM " + getTableName() + whenstatement +  + " ORDER BY " + getColName(TRANSACTION_DATE_IDX) + " DESC LIMIT 15";
    }
   // qDebug()<<"get all trans "<<query;
    return getAllTransaction(query);
}

StransactionData *CtransactionTable::getTransaction(int transId)
{
    QString query;
    if (transId > 0) {
        QString whenstatement = " WHERE " +  getColName(TRANSACTION_ID_IDX);
        whenstatement += "=" + QString::number(transId);
        query = "SELECT * FROM " + getTableName() + whenstatement;
    }
    QVector<StransactionData*> val = getAllTransaction(query);
    if (val.isEmpty() == false) {
        return val[0];
    }
    return nullptr;
}

QVector<StransactionData*> CtransactionTable::getAllTransaction(QString query)
{
    QVector<StransactionData*> transData;
    if (query.isEmpty() == false) {
        QSqlQuery q(getDataBase());
        if(q.exec(query)){
            while(q.next()){
                StransactionData* trans = new StransactionData;

                trans->m_id =  q.value(TRANSACTION_ID_IDX).toInt();
                trans->m_fromId = q.value(TRANSACTION_FROM_IDX).toInt();
                trans->m_amount = q.value(TRANSACTION_AMOUNT_IDX).toDouble();
                trans->m_accountId = q.value(TRANSACTION_ACCOUNT_IDX).toInt();
                trans->m_date = q.value(TRANSACTION_DATE_IDX).toDate();
                trans->m_type = q.value(TRANSACTION_TYPE_IDX).toInt();
                trans->m_mode = q.value(TRANSACTION_MODE_IDX).toInt();
                trans->m_ref = q.value(TRANSACTION_REF_IDX).toString();
                trans->m_remittanceID = q.value(TRANSACTION_REMITTANCE_ID_IDX).toInt();
                trans->m_parentID = q.value(TRANSACTION_PARENT_ID_IDX).toInt();
                trans->m_closingID = q.value(TRANSACTION_CLOSING_ID_IDX).toInt();
                trans->m_bankID = q.value(TRANSACTION_BANK_ID_IDX).toInt();
                trans->m_status = q.value(TRANSACTION_STATUS_IDX).toInt();
                trans->m_reeiptBook = q.value(TRANSACTION_RECTIPT_BOOK_IDX).toString();
                trans->m_receiptSlNo = q.value(TRANSACTION_RECTIPT_NO_IDX).toString();
                trans->m_particular = q.value(TRANSACTION_PARTICULAR_IDX).toString();

                transData.push_back(trans);
            }

        }
    }

    return transData;
}

void CtransactionTable::serialize(QDataStream &out)
{
    QString conditon = "(" +CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1)";

    TobjectList values;
    if (getTableValues(conditon, values)) {
        out<<(qint32)values.size();
        qDebug()<<"dumpint for trans "<<values.size();
        for (int i = 0; i < values.size(); ++i) {
            const Tobjects& obj = values.at(i);
            serializeTable(out, obj);

        }
    }else {
        out<<(qint32)0;
    }
}

void CtransactionTable::serializeTable(QDataStream &out, const Tobjects &obj)
{
    StransactionData trans;
    trans.m_id =  obj.at(TRANSACTION_ID_IDX).toInt();
    trans.m_fromId = obj.at(TRANSACTION_FROM_IDX).toInt();
    trans.m_amount = obj.at(TRANSACTION_AMOUNT_IDX).toDouble();
    trans.m_accountId = obj.at(TRANSACTION_ACCOUNT_IDX).toInt();
    trans.m_date = obj.at(TRANSACTION_DATE_IDX).toDate();
    trans.m_type = obj.at(TRANSACTION_TYPE_IDX).toInt();
    trans.m_mode = obj.at(TRANSACTION_MODE_IDX).toInt();
    trans.m_ref = obj.at(TRANSACTION_REF_IDX).toString();
    trans.m_remittanceID = obj.at(TRANSACTION_REMITTANCE_ID_IDX).toInt();
    trans.m_parentID = obj.at(TRANSACTION_PARENT_ID_IDX).toInt();
    trans.m_closingID = obj.at(TRANSACTION_CLOSING_ID_IDX).toInt();
    trans.m_bankID = obj.at(TRANSACTION_BANK_ID_IDX).toInt();
    trans.m_status = obj.at(TRANSACTION_STATUS_IDX).toInt();
    trans.m_reeiptBook = obj.at(TRANSACTION_RECTIPT_BOOK_IDX).toString();
    trans.m_receiptSlNo = obj.at(TRANSACTION_RECTIPT_NO_IDX).toString();
    trans.m_particular = obj.at(TRANSACTION_PARTICULAR_IDX).toString();

    trans.serialize(out);
}

void CtransactionTable::desserializeTable(QDataStream &in)
{
    StransactionData trans;
    trans.desserialize(in);
}


int StransactionData::save()
{
    Tobjects data(TRANSACTION_MAX_IDX);
    data[TRANSACTION_ID_IDX] = m_id ;
    data[TRANSACTION_FROM_IDX] =     m_fromId ;
    data[TRANSACTION_AMOUNT_IDX] =     m_amount ;
    data[TRANSACTION_ACCOUNT_IDX] =     m_accountId ;
    data[TRANSACTION_DATE_IDX] =     m_date ;
    data[TRANSACTION_TYPE_IDX] =     m_type ;
    data[TRANSACTION_MODE_IDX] =     m_mode ;
    data[TRANSACTION_REF_IDX] =     m_ref ;
    data[TRANSACTION_REMITTANCE_ID_IDX] =     m_remittanceID ;
    data[TRANSACTION_PARENT_ID_IDX] =     m_parentID ;
    data[TRANSACTION_CLOSING_ID_IDX] =     m_closingID ;
    data[TRANSACTION_BANK_ID_IDX] =     m_bankID ;
    data[TRANSACTION_STATUS_IDX] =     m_status ;
    data[TRANSACTION_RECTIPT_BOOK_IDX] =     m_reeiptBook ;
    data[TRANSACTION_RECTIPT_NO_IDX] =     m_receiptSlNo ;
    data[TRANSACTION_PARTICULAR_IDX] =     m_particular ;

    if (m_id == 0) {
        m_id = CtransactionTable::Object()->insertInTable(data).toInt();
    } else {
        for (unsigned int i = TRANSACTION_FROM_IDX; i < TRANSACTION_MAX_IDX; ++i) {
            CtransactionTable::Object()->updateValue(data.at(i), i , m_id, TRANSACTION_ID_IDX);
       }
    }

    return m_id;
}

void StransactionData::deleteObj()
{

}

StransactionData::StransactionData(const StransactionData &that)
{
    if (this != &that) {
       m_id = that.m_id;
       m_fromId = that.m_fromId;
       m_amount = that.m_amount;
       m_accountId = that.m_accountId;
       m_date = that.m_date;
       m_type = that.m_type;
       m_mode = that.m_mode;
       m_ref = that.m_ref;
       m_remittanceID = that.m_remittanceID;
       m_parentID = that.m_parentID;
       m_closingID = that.m_closingID;
       m_bankID = that.m_bankID;
       m_status = that.m_status;
       m_reeiptBook = that.m_reeiptBook;
       m_receiptSlNo = that.m_receiptSlNo;
       m_particular = that.m_particular;
    }
}

void StransactionData::serialize(QDataStream &out)
{
    //QString date =m_date.toString("dd/MM/yyyy");
    out<<m_id<<m_fromId<<m_amount<<m_accountId
        <<m_date<<m_type<<m_mode<<m_ref
        <<m_remittanceID<<m_parentID<<m_closingID<<m_bankID
        <<m_status<<m_reeiptBook<<m_receiptSlNo<<m_particular;
    //qDebug()<<"dumping trans "<<m_id<<" "<<m_amount;
}

void StransactionData::desserialize(QDataStream &in)
{
    //QString date;
    in>>m_id>>m_fromId>>m_amount>>m_accountId
        >>m_date>>m_type>>m_mode>>m_ref
        >>m_remittanceID>>m_parentID>>m_closingID>>m_bankID
        >>m_status>>m_reeiptBook>>m_receiptSlNo>>m_particular;
    //m_date = QDate::fromString(date, "dd/MM/yyyy");
    //qDebug()<<"reading rans "<<m_id<<" "<<m_amount<<" "<<m_date;
}
