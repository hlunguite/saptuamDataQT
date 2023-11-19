#ifndef CPAYMENTACCOUNTTABLE_H
#define CPAYMENTACCOUNTTABLE_H
#include "cdbtablebase.h"

enum EpaymentAccountTableFieldIndex {
    PAYMENT_ACCOUNT_ID_IDX,
    PAYMENT_ACCOUNT_ACCOUNT_NAME_IDX,
    PAYMENT_ACCOUNT_NAME_IDX,
    PAYMENT_ACCOUNT_NATURE_OF_EXPENSE_IDX,
    PAYMENT_ACCOUNT_MAX_IDX

};
struct SpaymentAccountData: public SdbObject
{
private:
 public:
    SpaymentAccountData(){}
    ~SpaymentAccountData(){}
    int save();
    void deleteObj();
    void serialize(QDataStream& out) ;
    void desserialize(QDataStream& in);
    int m_id = 0;
    QString m_paymentAccountName;
    QString m_accountName;
    QString m_natureOfExpense;
};
class CpaymentAccountTable : public CdbTableBase, public CsdSingleton<CpaymentAccountTable>
{
    friend class CsdSingleton<CpaymentAccountTable>;
public:
    QSqlError 	createTable();
    SpaymentAccountData* getPaymentAccountDataForId(int id);
    virtual void serializeTable(QDataStream& out, const Tobjects& obj) override;
    virtual void desserializeTable(QDataStream& in) override;

private:
    CpaymentAccountTable();
    ~CpaymentAccountTable();

};

#endif // CPAYMENTACCOUNTTABLE_H
