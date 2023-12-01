#ifndef CREMITTANCESETUPDLG_H
#define CREMITTANCESETUPDLG_H

#include <QDialog>
#include <QButtonGroup>
#include <QCompleter>
#include "cdlgbase.h"

namespace Ui {
class CremittanceSetupDlg;
}

class CremittanceSetupDlg : public CdlgBase
{
    Q_OBJECT

public:
    explicit CremittanceSetupDlg(QWidget *parent = nullptr);
    ~CremittanceSetupDlg();

private:
   Ui::CremittanceSetupDlg *ui;
    bool m_populateTable = false;
   QCompleter* m_remitNameCompleter;
    bool addEditRemittancePcForAccount(QString accountName, double localPC, double hqPC, bool isAdd, bool isEdit);
   // QWidget interface
protected:
   virtual void resizeEvent(QResizeEvent *);
   void resize();
private slots:
   void on_m_remitNameLineEdit_editingFinished();
   void on_m_remitNameList_currentTextChanged(const QString &arg1);
   void populateTable();
   void populateAllAccount();
   void populateRemittance(QString remitName);
   void on_m_cancelBtn_clicked();
   void on_m_okBtn_clicked();
};

#endif // CREMITTANCESETUPDLG_H
