#ifndef CDEPARTMENTTABLE_H
#define CDEPARTMENTTABLE_H
#include "cdbtablebase.h"

enum EdepartmentTableFieldIndex {
    DEPARTMENT_ID_IDX,
    DEPARTMENT_NAME_IDX,
    DEPARTMENT_SHORT_NAME_IDX,
    DEPARTMENT_PAYMENT_ACCOUNT_ID_IDX,
    DEPARTMENT_MAX_ID
};

struct SdepartmentData: public SdbObject
{
private:
 public:
    SdepartmentData(){}
    ~SdepartmentData(){}
    int save();
    void deleteObj();
    void serialize(QDataStream& out) ;
    void desserialize(QDataStream& in);
    int m_id = 0;
    QString m_deptName;
    QString m_deptShortName;
    int		m_paymentAccuntID = 0;

};


class CdepartmentTable : public CdbTableBase, public CsdSingleton<CdepartmentTable>
{
    friend class CsdSingleton<CdepartmentTable>;
public:
    SdepartmentData* getDepartmentDataByName(QString deptName);
    SdepartmentData* getDepartmentDataByShortName(QString deptName);
    SdepartmentData* getDepartmentDataByID(int id);
protected:
    virtual void serializeTable(QDataStream& out, const Tobjects& obj) override;
    virtual void desserializeTable(QDataStream& in) override;
;

private:
    ~CdepartmentTable();
    CdepartmentTable();

};



#endif // CDEPARTMENTTABLE_H
