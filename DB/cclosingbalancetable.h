#ifndef CCLOSINGBALANCETABLE_H
#define CCLOSINGBALANCETABLE_H

#include "cdbtablebase.h"
enum EclosingBalanceTableFieldIndex {
    CLOSING_TABLE_ID_IDX,
    CLOSING_DATE_IDX,
    MAX_CLOSING_TABLE_FIELD
};

struct SclosingData: public SdbObject
{
public:
    SclosingData(){}
    ~SclosingData(){}
    int save();
    void deleteObj();
    virtual void serialize(QDataStream& out);
    virtual void desserialize(QDataStream& in);
    int 	m_id = 0;
    QDate   m_closingDate;


};

class CclosingBalanceTable : public CdbTableBase, public CsdSingleton<CclosingBalanceTable>
{
    friend class CsdSingleton<CclosingBalanceTable>;
protected:
    virtual void serializeTable(QDataStream& out, const Tobjects& obj) override;
    virtual void desserializeTable(QDataStream& in) override;
public:
    CclosingBalanceTable();
    SclosingData getClosingForDate(QDate date, bool dateOnOrBefore = false);
};

enum EclosingBalanceDetailTableFieldIndex {
    CLOSING_DETAIL_TABLE_ID_IDX,
    CLOSING_DETAIL_CLOSING_TABLE_ID_IDX,
    CLOSING_DETAIL_ACCOUNT_DEPET_TABLE_ID_IDX,
    CLOSING_DETAIL_IS_ACCOUNT_ID_IDX,
    CLOSING_DETAIL_AMOUNT_TABLE_ID_IDX,
    CLOSING_DETAIL_CASH_OR_BANK_NAME_IDX, // if accuont id is zero. mostly in caseh of bank or cash.
    MAX_CLOSING_DETAIL_TABLE_FIELD
};


struct SclosingDetailData: public SdbObject
{
public:
    SclosingDetailData(){}
    ~SclosingDetailData(){}
    int save();
    void deleteObj();
    virtual void serialize(QDataStream& out);
    virtual void desserialize(QDataStream& in);
    int 	m_id = 0;
    int     m_closingTableID = 0;
    int     m_accountOrDeptTableID = 0;
    bool    m_isAccount = false;
    double  m_amount;
    QString m_cashOrBankName;


};

class CclosingBalanceDetailTable : public CdbTableBase, public CsdSingleton<CclosingBalanceDetailTable>
{
    friend class CsdSingleton<CclosingBalanceDetailTable>;
protected:
    virtual void serializeTable(QDataStream& out, const Tobjects& obj) override;
    virtual void desserializeTable(QDataStream& in) override;
public:
    CclosingBalanceDetailTable();
    QVector<SclosingDetailData*> getClosingDetailForID(int closingID);
};

#endif // CCLOSINGBALANCETABLE_H
