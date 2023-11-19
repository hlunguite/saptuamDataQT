#ifndef CSAPTUAMDATADB_H
#define CSAPTUAMDATADB_H

#include <QtSql>
#include <QVector>
#include <QString>
class CdbTableBase;
class CsaptuamDataDBMgr
{
private:
    QString 				m_saptuamMin;
    QString 				m_dbName;
    QString 				m_dbfile;
    void 					addRemitRequestBankAccount();
    void                    addDefaultName();
public:
    CsaptuamDataDBMgr(QString saptuamMin);
    ~CsaptuamDataDBMgr();
    QSqlDatabase getDataBase();

};

#endif // CSAPTUAMDATADB_H
