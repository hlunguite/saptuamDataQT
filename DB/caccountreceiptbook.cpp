#include "creceiptbookmap.h"
#include "caccountreceiptbook.h"

CaccountReceiptBook::CaccountReceiptBook():
    CdbTableBase(RECEIPT_ACCOUNT_RECEIPT_BOOK_TABLE)
{
    //    ACCOUNT_RECEIPT_BOOK_TABLE_ID_IDX,
    //ACCOUNT_RECEIPT_BOOK_TABLE_ACCOUNT_ID_IDX,
    //ACCOUNT_RECEIPT_BOOK_TABLE_RECEIPT_BOOK_TABLE_ID_IDX,
    addTableCol("accountReceiptBookID", FIELD_TYPE_PRIMARY_KEY, "Account Receipt Book", true, RECEIPT_ACCOUNT_RECEIPT_BOOK_TABLE_START_NO);
    addTableCol("accountID", FIELD_TYPE_INT, "Account ID", true);
    addTableCol("receiptBookID", FIELD_TYPE_INT, "Receipt book ID", true);


}


QVector<SaccountReceipt> CaccountReceiptBook::getAllAccountReceipt()
{
    QVector<SaccountReceipt> accountReceipts;
    QString query = "SELECT * FROM " + getTableName() + " ORDER BY " + getColName(ACCOUNT_RECEIPT_BOOK_TABLE_RECEIPT_BOOK_TABLE_ID_IDX) + " ASC";
    //qDebug()<<"qeery "<<query;
    QSqlQuery q(getDataBase());
    if(q.exec(query)){
        while(q.next()){
            int pid = q.value(ACCOUNT_RECEIPT_BOOK_TABLE_ID_IDX).toInt();
            int accountTableId = q.value(ACCOUNT_RECEIPT_BOOK_TABLE_ACCOUNT_ID_IDX).toInt();
            int receiptBookID = q.value(ACCOUNT_RECEIPT_BOOK_TABLE_RECEIPT_BOOK_TABLE_ID_IDX).toInt();

            SaccountReceipt accountReceipt;
            accountReceipt.m_accountReceiptID = pid;
            accountReceipt.m_accountTableID = accountTableId;
            accountReceipt.m_receiptBoookTableID = receiptBookID;
            accountReceipts.push_back(accountReceipt);

        }

    }
    return accountReceipts;
}


SaccountReceipt::SaccountReceipt(): m_accountReceiptID(0),
    m_accountTableID(0),
    m_receiptBoookTableID(0){}

SaccountReceipt::SaccountReceipt(const SaccountReceipt &that)
{
    m_accountReceiptID = that.m_accountReceiptID;
    m_accountTableID = that.m_accountTableID;
    m_receiptBoookTableID = that.m_receiptBoookTableID;
}

SaccountReceipt &SaccountReceipt::operator =(const SaccountReceipt &that)
{
    if (this != &that) {
        m_accountReceiptID = that.m_accountReceiptID;
        m_accountTableID = that.m_accountTableID;
        m_receiptBoookTableID = that.m_receiptBoookTableID;
    }
    return *this;
}

int SaccountReceipt::save()
{
    Tobjects data(ACCOUNT_RECEIPT_BOOK_TABLE_MAX_IDX);
    data[ACCOUNT_RECEIPT_BOOK_TABLE_ID_IDX] = m_accountReceiptID;
    data[ACCOUNT_RECEIPT_BOOK_TABLE_ACCOUNT_ID_IDX] = m_accountTableID;
    data[ACCOUNT_RECEIPT_BOOK_TABLE_RECEIPT_BOOK_TABLE_ID_IDX] = m_receiptBoookTableID;

    if (m_accountReceiptID == 0) {
        return CaccountReceiptBook::Object()->insertInTable(data).toInt();
    } else {
        CaccountReceiptBook::Object()->updateValue(m_accountTableID,
                                                   ACCOUNT_RECEIPT_BOOK_TABLE_ACCOUNT_ID_IDX,
                                                   m_accountReceiptID, ACCOUNT_RECEIPT_BOOK_TABLE_ID_IDX);
        CaccountReceiptBook::Object()->updateValue( m_receiptBoookTableID,
                                                   ACCOUNT_RECEIPT_BOOK_TABLE_RECEIPT_BOOK_TABLE_ID_IDX,
                                                   m_accountReceiptID, ACCOUNT_RECEIPT_BOOK_TABLE_ID_IDX);
    }

    return m_accountReceiptID;
}

void SaccountReceipt::deleteAccountReceipt()
{
    if (m_accountReceiptID > 0) {
        CaccountReceiptBook::Object()->deleteValue(m_accountReceiptID,ACCOUNT_RECEIPT_BOOK_TABLE_ID_IDX);
        m_accountReceiptID = 0;
        m_accountTableID = 0;
        m_receiptBoookTableID = 0;
    }
}
