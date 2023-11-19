#ifndef CREMITTANCESETUPTABLE_H
#define CREMITTANCESETUPTABLE_H
#include "cdbtablebase.h"

enum EremittancePercentTableIndex {
    REMITTANCE_PERCENT_TABLE_ID_IDX,
    REMITTANCE_PERCENT_TABLE_LOCAL_SHARE_IDX,
    REMITTANCE_PERCENT_TABLE_HQ_SHARE_IDX,
    REMITTANCE_PERCENT_TABLE_MAX_FIELD
};
struct SremitrancePercentData: public SdbObject
{
 public:
    SremitrancePercentData(){}
    ~SremitrancePercentData(){}
    int save();
    void deleteObj();
    void serialize(QDataStream& out) ;
    void desserialize(QDataStream& in);

    int m_id = 0;
    int m_localShare = 0;
    int m_hqShare = 0;
   // QString m_remitName;

};

class CremittancePercentTable : public CdbTableBase, public CsdSingleton<CremittancePercentTable>
{
    friend class CsdSingleton<CremittancePercentTable>;

public:
    CremittancePercentTable();
    SremitrancePercentData * getRemittancePercentageForID(int ID);
    SremitrancePercentData*  getRemittancePercentage(int hqShare, int localShare);
protected:
    virtual void serializeTable(QDataStream& out, const Tobjects& obj) override;
    virtual void desserializeTable(QDataStream& in) override;
};

enum EremittanceNameTableIndex {
    REMITTANCE_NAME_TABLE_ID_IDX,
    REMITTANCE_NAME_TABLE_NAME_IDX,
    REMITTANCE_NAME_TABLE_MAX_IDX
};

struct SremittanceNameData: public SdbObject
{
 public:
    SremittanceNameData(){}
    ~SremittanceNameData(){}
    int save();
    void deleteObj();
    void serialize(QDataStream& out) ;
    void desserialize(QDataStream& in);

    int m_id = 0;
    QString m_name;
};

class CremittanceNameTable : public CdbTableBase, public CsdSingleton<CremittanceNameTable>
{
    friend class CsdSingleton<CremittanceNameTable>;
   public:
    CremittanceNameTable();
    SremittanceNameData* getRemittanceNameData(QString name);
    SremittanceNameData* getRemittanceNameData(int id);
    QStringList getAllRemittanceName();
protected:
    virtual void serializeTable(QDataStream& out, const Tobjects& obj) override;
    virtual void desserializeTable(QDataStream& in) override;

};
enum EremittanceSetupTableIndex {
    REMITTANCE_SETUP_TABLE_ID_IDX,
    REMITTANCE_SETUP_TABLE_ACCOUNT_ID_IDX,
    REMITTANCE_SETUP_TABLE_NAME_ID_IDX,
    REMITTANCE_SETUP_TABLE_PERCENTAGE_ID_IDX,
    REMITTANCE_SETUP_TABLE_MAX_IDX
};

struct SremittanceSetupTableData {
public:
    SremittanceSetupTableData(){}
    ~SremittanceSetupTableData(){}
    int save();
    void deleteObj();
    int m_id = 0;
    int m_accountID = 0;
    int m_nameID = 0;
    int m_percentageID = 0;
    void serialize(QDataStream& out) ;
    void desserialize(QDataStream& in);
};

class CremittanceSetupTable : public CdbTableBase, public CsdSingleton<CremittanceSetupTable>
{
    friend class CsdSingleton<CremittanceSetupTable>;
   public:
    CremittanceSetupTable();
    SremittanceSetupTableData* getRemittanceSetupForAccount(int acountID);
    QVector<SremittanceSetupTableData*>* getRemitanceSetupForPercentID(int percentID);

    QVector<SremittanceSetupTableData*>* getRemitanceSetupForName(QString name);
protected:
    virtual void serializeTable(QDataStream& out, const Tobjects& obj) override;
    virtual void desserializeTable(QDataStream& in) override;

};
#endif // CREMITTANCESETUPTABLE_H
