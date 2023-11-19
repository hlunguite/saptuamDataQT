#ifndef CACCOUNTTABLE_H
#define CACCOUNTTABLE_H
#include "cdbtablebase.h"

enum EaccountType {
    INCOME_ACCOUNT_TYPE,
    PAYMENT_ACCOUNT_TYPE,
    BANK_ACCOUNT_TYPE,
    LOAN_ACCOUNT_TYPE,
    REQUEST_ACCOUNT_TYPE,
    REMITTANCE_ACCOUNT_TYPE,
    INVALID_ACOUNT_TYPE,
};


enum EaccountTableFieldIndex {
    ACCOUNT_ID_IDX,
    ACCOUNT_NAME_IDX,
    ACCOUNT_TYPE_IDX,
    ACCOUNT_USE_HQ_RECEIPT_IDX,
    ACCOUNT_PAYMENT_ACCOUNT_ID_IDX,
    ACCOUNT_IS_ACTIVE_IDX,
    ACCOUNT_BANK_ACCOUNT_NO_IDX,
    MAX_ACCOUNT_TABLE_FIELD
};

struct SaccountData: public SdbObject
{
 public:
    SaccountData();
    ~SaccountData();
    int save();
    void deleteObj();
    virtual void serialize(QDataStream& out);
    virtual void desserialize(QDataStream& in);
    int 	m_id = 0;
    QString m_accountName;
    int		m_accountType = INVALID_ACOUNT_TYPE;
    bool	m_isUseHqReceipt = false;
    int		m_paymentAccountID =  0;
    bool	m_isActiveAccount = true;
    QString	m_bankAccountName;

};

class CaccountTable : public CdbTableBase, public CsdSingleton<CaccountTable>
{
    friend class CsdSingleton<CaccountTable>;
private:

    CaccountTable();
    ~CaccountTable();
protected:
    virtual void serializeTable(QDataStream& out, const Tobjects& obj) override;
    virtual void desserializeTable(QDataStream& in) override;
public:
    SaccountData* getAccountDataForId(int accountID);
    SaccountData* getAccountDataForName(QString accountName);


};

class CaccountUtils : public CsdSingleton<CaccountUtils>
{
    friend class CsdSingleton<CaccountUtils>;
public:
    QString getAccountType(int accountType);
    int     getAccountTypeId(QString accountType);
    void 	getAccountTypesForNewAccountType(QVector<QString>& accountType);
    void serialize(QDataStream& out);
    void desserialize(QDataStream& in);
private:
    CaccountUtils();

    QMap<int, QString> m_accountTypeIdStringMap;
    QMap<QString, int> m_accountTypeStringIdMap;
};

#endif // CACCOUNTTABLE_H
