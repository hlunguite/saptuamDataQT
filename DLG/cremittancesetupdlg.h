#ifndef CREMITTANCESETUPDLG_H
#define CREMITTANCESETUPDLG_H

#include <QDialog>
#include <QButtonGroup>

namespace Ui {
class CremittanceSetupDlg;
}

class CremittanceSetupDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CremittanceSetupDlg(QWidget *parent = nullptr);
    ~CremittanceSetupDlg();

private:
   Ui::CremittanceSetupDlg *ui;

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
};

#endif // CREMITTANCESETUPDLG_H
