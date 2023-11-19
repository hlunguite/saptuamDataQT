#ifndef CIMPORTTRANSACTIONTABLE_H
#define CIMPORTTRANSACTIONTABLE_H
#include <QVector>
#include "cdbtablebase.h"
enum EimportTransTableIndex {
    IMPORT_TRANS_TABLE_IDX,
    IMPORT_TRANS_DATE_IDX,
    IMPORT_TRANS_FROM_IDX,
    IMPORT_TRANS_FROM_ID_IDX,
    IMPORT_TRANS_ACCOUNT_IDX,
    IMPORT_TRANS_ACCOUNT_ID_IDX,
    IMPORT_TRANS_AMOUNT_IDX,
    IMPORT_TRANS_MODE_IDX,
    IMPORT_TRANS_REF_IDX,
    IMPORT_TRANS_RECEIPT_BOOK_IDX,
    IMPORT_TRANS_RECEIPT_NO_IDX,
    IMPORT_TRANS_REMARK_IDX,
    IMPORT_TRANS_TRANS_ID_IDX,
    IMPORT_TRANS_FULL_STR_IDX,
    IMPORT_TRANS_TABLE_MAX_IDX
};

struct SimportTransData : public SdbObject {
    int     m_idx;
    QDate   m_date;
    QString m_from;
    int     m_fromID;
    QString m_account;
    int     m_accountID;
    double  m_amount;
    int     m_mode;
    QString m_ref;
    QString m_reciptBook;
    QString m_receiptNo;
    QString m_remark;
    int     m_transID;
    QString m_fullString;

    SimportTransData() {
        m_idx = 0;
        m_fromID = 0;
        m_accountID = 0;
        m_amount = 0;
        m_mode = 1;
        m_transID = 0;


    }
    ~SimportTransData(){}
    int save();
    void deleteObj(){}
};

class CimportTransactionTable : public CdbTableBase, public CsdSingleton<CimportTransactionTable>
{
    friend  CsdSingleton<CimportTransactionTable>;
public:
    CimportTransactionTable();
    QVector<SimportTransData*>* getImportedTransaction(bool process = true);
};

#endif // CIMPORTTRANSACTIONTABLE_H
