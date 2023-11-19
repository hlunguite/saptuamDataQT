#ifndef CTRANSACTIONTABLE_H
#define CTRANSACTIONTABLE_H
#include "cdbtablebase.h"


enum EtransactionTableFieldIndex {
    TRANSACTION_ID_IDX,
    TRANSACTION_FROM_IDX,
    TRANSACTION_AMOUNT_IDX,
    TRANSACTION_ACCOUNT_IDX,
    TRANSACTION_DATE_IDX,
    TRANSACTION_TYPE_IDX,
    TRANSACTION_MODE_IDX,
    TRANSACTION_REF_IDX,
    TRANSACTION_REMITTANCE_ID_IDX,
    TRANSACTION_PARENT_ID_IDX,
    TRANSACTION_CLOSING_ID_IDX,
    TRANSACTION_BANK_ID_IDX,
    TRANSACTION_STATUS_IDX,
    TRANSACTION_RECTIPT_BOOK_IDX,
    TRANSACTION_RECTIPT_NO_IDX,
    TRANSACTION_PARTICULAR_IDX,
    TRANSACTION_MAX_IDX
};
struct StransactionData: public SdbObject {
    int 	m_id =0;
    int 	m_fromId =0;
    double 	m_amount = 0;
    int 	m_accountId = 0;
    QDate	m_date;
    int 	m_type = 0;
    int 	m_mode = 0;
    QString	m_ref;
    int		m_remittanceID = 0;
    int 	m_parentID = 0;
    int 	m_closingID = 0;
    int		m_bankID = 0;
    int		m_status = 0;
    QString	m_reeiptBook;
    QString	m_receiptSlNo;
    QString	m_particular;
    virtual int save();
    virtual void deleteObj();
    StransactionData(){}
    ~StransactionData(){}
    StransactionData(const StransactionData& that);
    void serialize(QDataStream& out) ;
    void desserialize(QDataStream& in);
    /*QString getFromTo() const;
    QString getAccount() const;
    QString getDept() const;*/

};


class CtransactionTable : public CdbTableBase, public CsdSingleton<CtransactionTable>
{
    friend class CsdSingleton<CtransactionTable>;

public:

    CtransactionTable();
    ~CtransactionTable();
    int totalTransaction(int accountID);
    QVector<StransactionData*> getAllTransactionForRefId(QString refId);
    QVector<StransactionData*> getAllTransactionForContact(int contactID);
    StransactionData* getTransaction(int transId);
    QVector<StransactionData*> getAllTransaction(QString query);
    virtual void serialize(QDataStream& out) override;
    virtual void serializeTable(QDataStream& out, const Tobjects& obj) override;
    virtual void desserializeTable(QDataStream& in) override;

private:

};

#endif // CTRANSACTIONTABLE_H
