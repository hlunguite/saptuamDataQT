#ifndef CCONTACTDLG_H
#define CCONTACTDLG_H

#include <QDialog>
#include <QCompleter>
#include <QMap>
#include <QSet>
#include "cdlgbase.h"
namespace Ui {
class CcontactDLG;
}
struct SimportMinData;
class CcontactDLG : public CdlgBase
{
    Q_OBJECT

public:
    explicit CcontactDLG(QWidget *parent = 0);
    ~CcontactDLG();

    int contactEditID() const;
    bool setContactEditID(int contactEditID);

    QString title() const;
    void setTitle(const QString &title);

    QString firstName() const;
    void setFirstName(const QString &firstName);

    QString lastName() const;
    void setLastName(const QString &lastName);

    QString relationShip() const;
    void setRelationShip(const QString &relationShip);

    QString parentSpouse() const;
    void setParentSpouse(const QString &parentSpouse);
    //int suggestedNameContactID() const;

    QString gender() const;
    void setGender(const QString &gender);

    QString phone() const;
    void setPhone(const QString &phone);

   // QString nameWithSpouse() const;
    void setNameWithSpouse(const QString &nameWithSpouse);


    QString fullName() const;
    void setFullName(const QString &fullName);

    bool isPerson() const;
    void setIsPerson(bool isPerson);

    QString parentTitle() const;
    void setParentTitle(const QString &parentTitle);

    QString parentFirstName() const;
    void setParentFirstName(const QString &parentFirstName);

    QString parentLastName() const;
    void setParentLastName(const QString &parentLastName);

    int personEditID() const;
    void setPersonEditID(int personEditID);

    int otherEditID() const;
    void setOtherEditID(int otherEditID);
    int contactIDFullNameToUpdate() const { return m_contactIDFullNameToUpdate;}

    bool forImport() const;
    void setForImport(SimportMinData* data);

    QString suggestedName() const;

    int contactParentFullNameToUpdate() const;

    int parentSpouseContactID() const;
    int suggestedNameContactID() const { return m_suggestedNameContactID;}
private slots:
    void on_m_dataForPersoncheckBox_clicked(bool checked);
    void on_m_cancelButton_clicked();

    void on_m_okButton_clicked();

    void on_m_parentSpouse_editingFinished();

    void on_m_relationShipCombo_currentTextChanged(const QString &arg1);

    void on_m_titleComboBox_currentTextChanged(const QString &arg1);

    void on_m_name_editingFinished();

private:
    Ui::CcontactDLG *ui;
    QCompleter*	m_parentSpouseCompleter;
    int 		m_contactEditID;
    int			m_personEditID;
    int			m_otherEditID;
    int         m_contactIDFullNameToUpdate;
    int         m_contactParentFullNameToUpdate;
    int         m_parentSpouseContactID;
    int         m_suggestedNameContactID;
    bool		m_isPerson;
    bool       m_forImport;

    QString 	m_title;
    QString     m_suggestedName;
    QString 	m_firstName;
    QString 	m_lastName;
    QString		m_relationShip;
    QString		m_parentSpouse;
    //QString 	m_nameWithSpouse;
    QString		m_fullName;
    QString		m_gender;
    QString		m_phone;

    QString    m_parentTitle;
    QString	   m_parentFirstName;
    QString	   m_parentLastName;

    QMap<QString, QSet<int> > m_nameIDMap;
    QMap<int, QString> m_displayName;
    QMap<int, QString>  m_allFullName;
    QMap<QString, int> m_nameWithSpouseMap;
    QStringList         m_minList;
    QStringList         m_otherContactList;
    int findContactID(QString name);
    int findContactID(QString title, QString firstName, QString lastName,
                      QString relationShip, QString parentTitle, QString parentFirstName, QString parentLastName);
    QString getDisplayName(int id);
    QString getFullName(int id);
    void populatePersonForEdit();
    void populateOtherForEdit();
    void resize();


    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override {resize();}
};

#endif // CCONTACTDLG_H
