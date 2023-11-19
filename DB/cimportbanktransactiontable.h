#ifndef CIMPORTBANKTRANSACTIONTABE_H
#define CIMPORTBANKTRANSACTIONTABE_H
#include "cdbtablebase.h"
#include <Qset>
enum EimportBankTransactionTableIndex {
    IMPORT_BANK_TRANSACTION_TABLE_ID_IDX,
    IMPORT_BANK_TRANSACTION_TABLE_DATE_RANGE_IDX,
    IMPORT_BANK_TRANSACTION_ITABLE_MAX_IDX
};

struct SimportBankTransactionData {
    int 	m_id;
    QString m_dateRange;
    SimportBankTransactionData() {
        m_id = 0;
        m_dateRange.clear();
    }
    int save();
    void deleteObj();
};

class CimportBankTransactionTable : public CdbTableBase, public CsdSingleton<CimportBankTransactionTable>
{
    friend class CsdSingleton<CimportBankTransactionTable>;
public:
    QVector<SimportBankTransactionData> getAllBankImport();
private:
    CimportBankTransactionTable();
    ~CimportBankTransactionTable();
};


#endif // CIMPORTBANKTRANSACTIONTABE_H
