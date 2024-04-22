#ifndef CCONTACTMAP_H
#define CCONTACTMAP_H
#include "csdsingleton.h"
#include <QString>
#include <QMap>
#include <QStringList>
class CcontactDLG;
class CcontactMap : public CsdSingleton<CcontactMap>
{
    friend class CsdSingleton<CcontactMap>;

public:
    void populateContactMap();
    int getContactID(const QString& name) const;

    QString getContanceName(int id) const;
    QString getContactNameOnly(int id) const;
    int getPersonIdFromContactID(int id) const ;
    QString getCreateContactNameWithParent(int id) const;
    const QStringList& getNameList() const { return m_fullNameList;}
    int addEditContact(CcontactDLG* contactDlg);
    QString getContactPhone(int contactID);
    QString getPersonName(int personID);
private:
    CcontactMap();
    ~CcontactMap();
    QMap<int, QString>	m_contactIDWithFullNameWithSpouse;
    QMap<int, QString>	m_contactIDWithFullNameOnly;

    QMap<int, QString>  m_contactIDwithNameAndParent;
    QMap<QString, int>	m_contactIDWithFullName;
    QMap<int, int>      m_contactToPersonId;
    QMap<int, int>      m_contactToOtherID;
    QStringList			m_fullNameList;
    QMap<int, QString>  m_contactPhone;
    QMap<int, QString> m_personIDNameMap;

};

#endif // CCONTACTMAP_H
