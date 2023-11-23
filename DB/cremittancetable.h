#ifndef CREMITTANCE_H
#define CREMITTANCE_H
#include "cdbtablebase.h"

enum EremittanceTableFieldIndex {
    REMITTANCE_TABLE_ID_IDX,
    REMITTANCE_TABLE_TRANSACTION_ID_IDX,
    REMITTANCE_TABLE_NAME_TABLE_ID_IDX,
    REMITTANCE_TABLE_PERIOD_FROM_IDX,
    REMITTANCE_TABLE_PERIOD_TO_IDX,
    REMITTANCE_TABLE_TRANSACTION_DATE_IDX,
    REMITTANCE_TABLE_TO_ADDRESS_IDX,
    REMITTANCE_TABLE_TO_SUBMIT_BY_IDX,
    REMITTANCE_TABLE_MAX_IDX
};

struct SremittanceData: public SdbObject{
public:
    SremittanceData() {}
    ~SremittanceData() {}
   int save();
   void deleteObj();
   void serialize(QDataStream& out) ;
   void desserialize(QDataStream& in);
   int 		m_id = 0;
   int 		m_transactionID = 0;
   int 		m_remitNameTableID =0;
   QDate	m_fromDate;
   QDate	m_toDate;
   QDate	m_transactionDate;
   QString	m_toAddress;
   QString  m_submitBy;

};

class CremittanceTable : public CdbTableBase, public CsdSingleton<CremittanceTable>
{
    friend class CsdSingleton<CremittanceTable>;
public:
    SremittanceData* getRemittanceData(int tableOD);
    SremittanceData* getRemittanceDataFromTransID(int transID);
protected:
    virtual void serializeTable(QDataStream& out, const Tobjects& obj) override;
    virtual void desserializeTable(QDataStream& in) override;
private:
    CremittanceTable();
    ~CremittanceTable();
};

enum EremittanceDetailTableFieldIndex {
    REMITTANCE_DETAIL_TABLE_ID_IDX,
    REMITTANCE_DETAIL_TABLE_REMIT_TABLE_ID_IDX,
    REMITTANCE_DETAIL_TABLE_ACCOUNT_ID_IDX,
    REMITTANCE_DETAIL_TABLE_LOCAL_SHARE_AMOUNT_IDX,
    REMITTANCE_DETAIL_TABLE_HQ_SHARE_AMOUNT_IDX,
    REMITTANCE_DETAIL_TABLE_MAX_IDX
};

struct SremittanceDetailData: public SdbObject{
public:
    SremittanceDetailData() {}
    ~SremittanceDetailData() {}
   int save();
   void deleteObj();
   void serialize(QDataStream& out) ;
   void desserialize(QDataStream& in);
   int 		m_id = 0;
   int 		m_remitanceTableID = 0;
   int		m_accuntTableID  =0;
   double	m_localShare = 0;
   double   m_hqShare = 0;
};

class CremittanceDetailTable : public CdbTableBase, public CsdSingleton<CremittanceDetailTable>
{
    friend class CsdSingleton<CremittanceDetailTable>;
public:
    QVector<SremittanceDetailData*>* getRemittanceDetailForTableID(int id);
protected:
    virtual void serializeTable(QDataStream& out, const Tobjects& obj) override;
    virtual void desserializeTable(QDataStream& in) override;


private:
    CremittanceDetailTable();
    ~CremittanceDetailTable() {}
};



enum EremittanceReconcileFieldIndex {
    REMITTANCE_RECONCILE_TABLE_ID_IDX,
    REMITTANCE_RECONCILE_TABLE_REMIT_TABLE_ID_IDX,
    REMITTANCE_RECONCILE_TABLE_ACCOUNT_DEPT_TABLE_ID_IDX,
    REMITTANCE_RECONCILE_TABLE_IS_ACCOUNT_IDX,
    REMITTANCE_RECONCILE_TABLE_STRING_IDX,
    REMITTANCE_RECONCILE_TABLE_MAX_IDX
};

struct SremittanceReconcileData: public SdbObject{
public:
    SremittanceReconcileData() {}
    ~SremittanceReconcileData() {}
    int save();
    void deleteObj();
    void serialize(QDataStream& out) ;
    void desserialize(QDataStream& in);
    int 	m_id = 0;
    int 	m_remitanceTableID = 0;
    int		m_accontOrDeptTableID  = 0;
    bool    m_isAccount = false;
    QString	m_str;

};

class CremittanceReconcileTable : public CdbTableBase, public CsdSingleton<CremittanceReconcileTable>
{
    friend class CsdSingleton<CremittanceReconcileTable>;
public:
    QVector<SremittanceReconcileData*>* getRemittanceReconcileForTableID(int id);
protected:
    virtual void serializeTable(QDataStream& out, const Tobjects& obj) override;
    virtual void desserializeTable(QDataStream& in) override;

private:
    CremittanceReconcileTable();
    ~CremittanceReconcileTable() {}
};
struct SremitTransDetail {
    int m_transID;
    QString m_from;
    QString m_amount;
    QString m_receiptBook;
    QString m_receiptNo;
    int     m_accountID;
    SremitTransDetail(QString str);
    SremitTransDetail(int id, QString name, QString amount, QString book, QString no, int accountID):
        m_transID(id),
        m_from(name), m_amount(amount), m_receiptBook(book), m_receiptNo(no), m_accountID(accountID){}
    QString toString();

};
#endif // CREMITTANCE_H
