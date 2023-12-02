#ifndef CDBTABLEBASE_H
#define CDBTABLEBASE_H
#include <QString>
#include <QVector>
#include <QPair>
#include <QtSql>
#include "csdsingleton.h"
#include "cdbdefine.h"
#include <QDataStream>


//{"", "account", "transaction"});
typedef QVector<QVariant> Tobjects;
typedef QVector<Tobjects > TobjectList;
typedef QVector<QPair<int, QVariant> > TfilterObjType;

class CdbTableBase;
struct SdbObject
{
   // QString 	m_error;
    SdbObject(){}
    virtual ~SdbObject() {}
    virtual int save() { Q_ASSERT(0); return 0;}
    virtual void deleteObj() { Q_ASSERT(0);}
    virtual void serialize(QDataStream& out) {Q_ASSERT(0);}
    virtual void desserialize(QDataStream& in){Q_ASSERT(0);}
    //QString error() { return m_error;}
};

struct SdbObjects {
    QVector<SdbObject*>	m_listofObject;
    ~SdbObjects();
    int size() {return m_listofObject.size();}
    const SdbObject* at(int i ) { return m_listofObject.at(i);}
};

class CdbTableBase
{
public:
    enum EfieldType {
        FIELD_TYPE_INT,
        FIELD_TYPE_STRING,
        FIELD_TYPE_PRIMARY_KEY,
        FIELD_TYPE_BOOL,
        FIELD_TYPE_DECIMAL,
        FIELD_TYPE_FLOAT,
        FIELD_TYPE_UNSIGNED_INT,
        FIELD_TYPE_DATE,
    };
    static QVector<QString> m_tableFieldTypeStr;


    CdbTableBase(EtableType type);
    virtual ~CdbTableBase();
    virtual void serialize(QDataStream& out);
    virtual void desserialize(QDataStream& in);
    QString 			getTableName();
    const QVector<QString>& getTableHeader() const { return m_tableHeader; }
    virtual QSqlError 	createTable();
    void                deleteTable();
    //virtual SdbObjects* all();
    //virtual SdbObjects* filter(QString& query) = 0;
    virtual bool 		isValueExist(const TfilterObjType& filterField);
    virtual bool 		isValueExist(const TfilterObjType& filterField, TobjectList& values);
    virtual bool        isValueExist(const QString& condition, TobjectList& values);


    EfieldType 			getColType(int index);
    QString 			getColName(int index);
    QString				getColTypeString(int index);
    bool				getIsHeaderHidden(int index);


protected:
    EtableType 							m_tableType;
    QString 							m_tableName;
    int									m_autoIncColNo;
    int									m_autoIccStartAt;
    QVector<QPair<QString,EfieldType> > m_tableDataType;
    QVector<QString> 					m_tableHeader;
    QVector<bool> 						m_tableColumnHidden;
    virtual void serializeTable(QDataStream& out, const Tobjects& obj) {Q_ASSERT(0);}
    virtual void desserializeTable(QDataStream& in) {Q_ASSERT(0);};
public:
    void 			addTableCol(QString field,
                                EfieldType type,
                                QString header,
                                bool isHidden = false,
                                int autoIncStartAt=0);

    QString			formatSearch(QString & tosearch, EfieldType type);
    QString         getSearchString(QString tosearch, int colNo);

    QSqlDatabase	getDataBase();
    bool 		    getTableValues(const QString& condition, TobjectList& values);
    QVariant 		insertInTable(Tobjects & data);
    void 			updateValue(QVariant newValue, int updateFieldId,
                                QVariant findValue, int findFieldId);
    bool		 	deleteValue( QVariant findValue, int fieldIndex);



};

#endif // CDBTABLEBASE_H
