#ifndef CREMITTANCEPERCENTSETUP_H
#define CREMITTANCEPERCENTSETUP_H

#include <QDialog>
#include <QMap>
#include <QSet>
#include "cdlgbase.h"
#include <QCompleter>
namespace Ui {
class CremittancePercentSetup;
}

class CremittancePercentSetup : public CdlgBase
{
    Q_OBJECT

public:
    explicit CremittancePercentSetup(QWidget *parent = nullptr);
    ~CremittancePercentSetup();

private slots:
    void on_m_deptComboBox_currentTextChanged(const QString &arg1);

    void on_m_accountComboBox_currentTextChanged(const QString &arg1);

    void on_m_okBtn_clicked();

    void on_m_cancelBtn_clicked();

    void on_m_remitName_editingFinished();

private:
    Ui::CremittancePercentSetup *ui;
    QMap<QString, QStringList>	m_deptAccounts;
    QSet<QString>				m_accountWithHqreceipt;
    bool						m_construct;
    QCompleter*					m_remitNameCompleter;
    void populateTable();

};

#endif // CREMITTANCEPERCENTSETUP_H
