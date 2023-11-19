#include "cdbtablebase.h"
#include "csdutils.h"

//QSqlQuery q(getDataBase())
QVector<QString> CdbTableBase::m_tableFieldTypeStr(QVector<QString>()
                                                   <<"integer"
                                                   <<"VARCHAR"
                                                   <<"integer primary key autoincrement"
                                                   <<"TINYINT(1) "
                                                   <<"DECIMAL(20,2) "
                                                   <<"float"
                                                   <<"int unsigned"
                                                   <<"DATE ");



CdbTableBase::CdbTableBase(EtableType type):
        m_tableType(type),
        m_tableName(GtableName[type]),
        m_autoIncColNo(0),
        m_autoIccStartAt(0)
{

}

CdbTableBase::~CdbTableBase()
{


}

void CdbTableBase::serialize(QDataStream &out)
{
    QString conditon;
    TobjectList values;
    if (getTableValues(conditon, values)) {
        out<<(qint32)values.size();
        for (int i = 0; i < values.size(); ++i) {
            const Tobjects& obj = values.at(i);
            serializeTable(out, obj);

        }
    }else {
        out<<(qint32)0;
    }
}

void CdbTableBase::desserialize(QDataStream &in)
{
    qint32 count;
    in>>count;
    if (count > 0) {
        qDebug()<<"reading back "<<getTableName()<<" "<<count;
        for (int i = 0; i < count; ++i) {
            desserializeTable(in);

        }
    }
}

QString CdbTableBase::getTableName()
{
    return m_tableName;
}

void
CdbTableBase::addTableCol(QString field, EfieldType type, QString header, bool isHidden, int autoIncStartAt)
{
    if (type == FIELD_TYPE_PRIMARY_KEY && (m_autoIncColNo == 0)) {
        m_autoIncColNo = (int)m_tableDataType.size();
        if (autoIncStartAt > 0 ) {
            m_autoIccStartAt = autoIncStartAt;
        }
    }
    m_tableDataType.push_back(qMakePair(field, type));
    m_tableHeader.push_back(header);
    m_tableColumnHidden.push_back(isHidden);
}

QSqlDatabase CdbTableBase::getDataBase()
{
    return QSqlDatabase::database(CsdUtils::getDbName());
}
bool CdbTableBase::getTableValues(const QString &condition, TobjectList &values)
{

    QString query = "SELECT * FROM " + m_tableName;
    if (condition.isEmpty() == false) {
        query +=" WHERE " + condition ;
    }
    bool ret = false;
    values.clear();
    QSqlQuery q(getDataBase());
     if (q.exec(query)) {
         if (q.isActive() && q.isSelect()) {
             while(q.next()){
               ret = true;
               int col = q.record().count();
               Tobjects v;
               for (int i = 0; i < col; ++i) {
                   v.push_back(q.value(i));
               }
               values.push_back(v);
            }
         }
     }
     return ret;
}

QSqlError CdbTableBase::createTable()
{
    QSqlQuery q(getDataBase());

    QString cmd = "create table " + m_tableName;
    cmd += QString(" (");
    int size = m_tableDataType.size();
    QString primaryKey = "";
    for(int i = 0; i < size; ++i){
        if(i > 0) cmd += ", ";
        if (m_tableDataType.at(i).second == FIELD_TYPE_PRIMARY_KEY) {
            primaryKey = m_tableDataType.at(i).first;
        }
        cmd += m_tableDataType.at(i).first +" " + m_tableFieldTypeStr[m_tableDataType.at(i).second];
    }
    if (primaryKey.isEmpty()) {
        return QSqlError("No primary Key", m_tableName);
    }
    cmd += ")";
    //qDebug()<<"Craate table command :"<<cmd;
    if(!q.exec(cmd)){
        //qDebug()<<"error "<<q.lastError();
        return q.lastError();
    } else if (m_autoIccStartAt > 1) {
        //set the auto increment start value
        cmd = "INSERT INTO SQLITE_SEQUENCE(name,seq) VALUES(\'";
        cmd += m_tableName + "\',\'" + QString::number(m_autoIccStartAt) + "\')";
        //qDebug()<<"update sequence "<<cmd;
        if (!q.exec(cmd)) {
            return q.lastError();
        }
    }
    return QSqlError();

}

void CdbTableBase::deleteTable()
{

    QString cmd = "drop table " + m_tableName;
    QSqlQuery q(getDataBase());
    if(!q.exec(cmd)){
        qDebug()<<"Error drop table "<<m_tableName<<" "<<q.lastError().text();
    } else {
        qDebug()<<"Drop table "<<m_tableName;
    }

}


bool CdbTableBase::isValueExist(const TfilterObjType &filterField)
{
    bool ret = false;
    QString whenConditon;
    for (unsigned int i = 0; i < filterField.size(); ++i) {
        int fieldIndex = filterField.at(i).first;
        QString searchValue = filterField.at(i).second.toString();
        if (whenConditon.isEmpty() == false) {
            whenConditon += " AND ";
        }
        whenConditon += getSearchString(searchValue, fieldIndex);
    }


    QString query = "SELECT * FROM " + m_tableName;
    if (whenConditon.isEmpty() == false) {
        query +=" WHERE " + whenConditon ;
    }
    TobjectList values;
    if (isValueExist(query, values)) {
        ret = true;
    }
    /*QSqlQuery q(getDataBase());
     if (q.exec(query)) {
         if (q.isActive() && q.isSelect()) {
             if(q.next()){
               ret = true;
            }
         }
     }*/
    return ret;
}

bool CdbTableBase::isValueExist(const TfilterObjType &filterField, TobjectList &values)
{
    bool ret = false;
    QString whenConditon;
    for (unsigned int i = 0; i < filterField.size(); ++i) {
        int fieldIndex = filterField.at(i).first;
        QString searchValue = filterField.at(i).second.toString();
        if (whenConditon.isEmpty() == false) {
            whenConditon += " AND ";
        }
        whenConditon += getSearchString(searchValue, fieldIndex);
    }


    QString query = "SELECT * FROM " + m_tableName;
    if (whenConditon.isEmpty() == false) {
        query +=" WHERE " + whenConditon ;
    }

    if (isValueExist(query, values)) {
        ret = true;
    }
    return ret;
}

bool CdbTableBase::isValueExist(const QString &condition, TobjectList &values)
{
     bool ret = false;
     values.clear();
     QSqlQuery q(getDataBase());
     if (q.exec(condition)) {
         if (q.isActive() && q.isSelect()) {
            while(q.next()){
               ret = true;
               int col = q.record().count();
               Tobjects v;
               for (int i = 0; i < col; ++i) {
                   v.push_back(q.value(i));
               }
               values.push_back(v);
            }
         }
     }
     return ret;
}

CdbTableBase::EfieldType CdbTableBase::getColType(int index)
{
    //qDebug()<<"get col type "<<index<<" size "<<m_tableDataType.size()<<"\n";
    return  m_tableDataType.at(index).second;
    /*EfieldType type = typeString;
    if(str.contains("int")) {
           type = typeInt;
     } else if(str.contains("DATE")) {
           type = typeDate;
     } else if(str.contains("float")) {
           type = typeFloat;
    }
    return type;*/
}

QString CdbTableBase::getColName(int index)
{
    return m_tableDataType.at(index).first;
}

QString CdbTableBase::getColTypeString(int index)
{
    switch(getColType(index)) {
    case FIELD_TYPE_INT: return "INT_TYPE";
    case FIELD_TYPE_STRING: return "STRING_TYPE";
    case FIELD_TYPE_PRIMARY_KEY : return "PRIMARY_KEY";
    case FIELD_TYPE_BOOL : return "BOOL_TYPE";
    case FIELD_TYPE_DECIMAL : return "DECEMAL_TYPE";
    case FIELD_TYPE_FLOAT: return "FLOAT_TYPE";
    case FIELD_TYPE_UNSIGNED_INT : return "UNSIGNED_INT";
    case FIELD_TYPE_DATE : return "DATE_TYPE";
    }
    return "";
}

bool CdbTableBase::getIsHeaderHidden(int index)
{
    return m_tableColumnHidden.at(index);
}

QString  CdbTableBase::formatSearch(QString &tosearch, CdbTableBase::EfieldType type)
{
    QString tomatch;


    if(type == FIELD_TYPE_STRING || type == FIELD_TYPE_DATE){
        tomatch = " LIKE ";
        tosearch = "\"" + tosearch + "\"";

    }else {
        tomatch = "=";
    }

    return tomatch;
}

QString CdbTableBase::getSearchString(QString tosearch, int colNo)
{
    QString ret = getColName(colNo);
    EfieldType type = getColType(colNo);
    ret += formatSearch(tosearch,type);
    ret += tosearch;
    return  ret;
}

QVariant CdbTableBase::insertInTable( Tobjects &data)
{
    if (m_tableDataType.size() != data.size()) {
        qDebug()<<"size not equal "<<data.size()<<" table size "<<m_tableDataType.size() <<" for "<<m_tableName;
        Q_ASSERT(0);
    }

    QSqlQuery q(getDataBase());

    QString prepare = "insert into " + m_tableName + "(";
    QString colname;
    QString placeholder;
    int size = m_tableDataType.size();

    QVector<QString> nameAssociation(size);
    for (int i =0; i < size; ++i) {
        if(m_autoIncColNo == i) continue;
        QString field = m_tableDataType.at(i).first;
       // QString second = tableDetail.at(i).second;
        if(colname.isEmpty() == false){
            colname += ", ";
            placeholder += ", ";
        }
        colname += field;
        QString nameassoc = ":" +field;
        placeholder += nameassoc +" ";
        nameAssociation[i] = nameassoc;
    }

    prepare += colname +") values (" + placeholder +")";
    if(!q.prepare(prepare)){
        qDebug()<<"prepar fail: "<<q.lastError();
        return 0;
    }
    size = data.size();
    QString bind = "Bind:";
    for (int i = 0; i <size; ++i) {
        if(i == m_autoIncColNo) continue;
        QVariant v = data.at(i);
        if (QMetaType::Bool	 == v.userType()) {
            if (v.toBool()) {
                v = "1";
            } else {
                v = "0";
            }
        } else {
            //qDebug()<<"type "<<getColTypeString(i) <<" col name "<<getColName(i);
            EfieldType type = getColType(i);

            if (type == FIELD_TYPE_STRING) {
                QString s = v.toString();
                if (s.isEmpty()) {
                    v = "";
                }else if (v.isNull()) {
                    v = "";
                } else if (v.isValid() == false){
                    v = "";
                }
            }

        }

        QString n = nameAssociation[i];
        q.bindValue(n,v);
        bind += m_tableDataType.at( i).first + "#"+ n +":"+  v.toString() + "@" ;
    }

    if(!q.exec()){
        qDebug()<<"Insert fail "<<q.lastError()<<":"<<q.lastQuery();
        return 0;
    }

    return q.lastInsertId();
}

void CdbTableBase::updateValue(QVariant newValue,
                               int updateFieldId,  QVariant findValue, int findFieldId)
{
    if(m_autoIncColNo == updateFieldId) return;

     QSqlQuery q(getDataBase());

    QString query = "UPDATE " + m_tableName + " ";

    QString findVal = findValue.toString();
    //EfieldType findtype = getColType(findFieldId);

    EfieldType updatetype = getColType(updateFieldId);
    QString newVal = newValue.toString().trimmed().simplified();

    if (updatetype == FIELD_TYPE_BOOL) {
        if (newValue.toBool()) {
            newVal = "1";
        }else {
            newVal = "0";
        }
    }

    QString updateFieldName = getColName(updateFieldId);

    formatSearch(newVal, updatetype);
    QString setStr = "SET "+ updateFieldName + "=" + newVal;
    QString whereStr = " WHERE " + getSearchString(findVal, findFieldId);

    //qDebug()<<"\nUpdate for "<<query<<" "<<updateFieldName<<" field type "<<getColTypeString(updateFieldId);

    TobjectList values;
    QString condition = getSearchString(findVal, findFieldId);
    getTableValues(condition, values);
    //check if value re different. update only if value are different
    if (values.isEmpty() == false) {
        QVariant oldValue = values.at(0).at(updateFieldId);


        switch(updatetype) {
        case FIELD_TYPE_INT:
            if (newValue.toInt() == oldValue.toInt()) {
                return;
            }
            break;
        case FIELD_TYPE_STRING:{
            //qDebug()<<"Update fpr  "<<m_tableName<<" "<<updateFieldName<<": "<<oldValue<<" to "<<newValue;
            if (newValue.toString() == oldValue.toString()) {
                return;
            }
            QString val = newValue.toString();
            if (val.isEmpty()) {
                //qDebug()<<"new is empty";
                val = "";
                newValue = val;
            }}
            break;
         case FIELD_TYPE_PRIMARY_KEY:
            if (newValue.toUInt() == oldValue.toUInt()) {
                return;
            }
            break;
        case FIELD_TYPE_BOOL:
            if (newValue.toBool() == oldValue.toBool()) {
                return;
            }
            break;
        case FIELD_TYPE_DECIMAL:
            if (newValue.toDouble() == oldValue.toDouble()) {
                return;
            }
            break;
        case FIELD_TYPE_FLOAT:
            if (newValue.toFloat() == oldValue.toFloat()) {
                return;
            }
            break;
        case FIELD_TYPE_UNSIGNED_INT:
            if (newValue.toUInt() == oldValue.toUInt()) {
                return;
            }
            break;
        case FIELD_TYPE_DATE:
            if (newValue.toDate() == oldValue.toDate()) {
                return;
            }
            break;
        default: return;
        }


    } else if (newVal.isEmpty()) {
        return;
    }


    query += setStr + whereStr;
    //qDebug()<<"update string "<<query;
    if(!q.exec(query)){
        qDebug()<<"UPDATE faile for "<<query<<" "<<q.lastError().text();

    }else {

    }
}

bool CdbTableBase::deleteValue(QVariant findValue, int fieldIndex)
{

    QString searchValue = findValue.toString();
    EfieldType type = getColType(fieldIndex);
    QString match = formatSearch(searchValue, type);

    QString query = "DELETE FROM " + m_tableName + " WHERE " + getColName(fieldIndex) ;
    query += match + searchValue;
    QSqlQuery q(getDataBase());

    if (q.exec(query)) {
        return true;
    }

    return false;
}



SdbObjects::~SdbObjects()
{
    for (SdbObject* obj : m_listofObject) {
        delete obj;
    }
    m_listofObject.clear();
}
