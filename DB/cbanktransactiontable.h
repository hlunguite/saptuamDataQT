#ifndef CBANKTRANSACTIONTABLE_H
#define CBANKTRANSACTIONTABLE_H
#include "cdbtablebase.h"
enum EbankTransactionTableFieldId {
    BANK_TRANSACTION_TABLE_ID,
    BANK_TRANSACTION_TABLE_MIN,
    BANK_TRANSACTION_TABLE_REFID,
    BANK_TRANSACTION_TABLE_PHONE,
    BANK_TRANSACTION_TABLE_PIAKCHAN,
    BANK_TRANSACTION_TABLE_DATE,
    BANK_TRANSACTION_TABLE_AMOUNT,
    BANK_TRANSACTION_TABLE_IS_INCOME,
    BANK_TRANSACTION_TABLE_TYPE,
    BANK_TRANSACTION_TABLE_STATUS,
    BANK_TRANSACTION_TABLE_IMPORT_ID,
    BANK_TRANSACTION_TABLE_FULLSTRING,
    BANK_TRANSACTION_TABLE_MAX_IDX
};

enum EbankTransType {INVALID,  BANK_CASH_DEPOSIT, CHEQUE, BANK_CHARGES, BANK_INTEREST, NOTASIGN};
enum EbankTransStatus{IMPORT, PROCESSED, SKIPPED,};
struct SbanktransDetail {
    int			m_transID;
    QString 	m_min;
    QString		m_refID;
    QString		m_phone;
    QString		m_piakChan;
    QString		m_date;
    double		m_amount;
    bool		m_isIncome;
    EbankTransType	m_type;
    EbankTransStatus	m_transStatus;
    int 		m_importID;
    SbanktransDetail() {
        m_transID = 0;
        m_type = INVALID;
        m_amount = 0;
        m_isIncome = false;
        m_transStatus = IMPORT;
        m_importID = 0;
    }
    QString toString() const;
    int save();
    void deleteObj();

};

class CbankTransactionTable:  public CdbTableBase, public CsdSingleton<CbankTransactionTable>
{
    friend class CsdSingleton<CbankTransactionTable>;

public:
    CbankTransactionTable();
    QVector<SbanktransDetail> getAllbankTransForSameImport(int importID);
    int	getIsTransactionAvailable(QString findStr);

    // CdbTableBase interface
public:
    virtual SdbObjects *filter(QString& query) { return nullptr;}
};

#endif // CBANKTRANSACTIONTABLE_H
