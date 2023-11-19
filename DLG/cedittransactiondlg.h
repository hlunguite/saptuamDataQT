#ifndef CEDITTRANSACTIONDLG_H
#define CEDITTRANSACTIONDLG_H

#include <QDialog>
#include <QCompleter>
#include "ctransactiontable.h"
#include <QKeyEvent>
namespace Ui {
class CeditTransactionDlg;
}

class CeditTransactionDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CeditTransactionDlg(QWidget *parent = 0);
    ~CeditTransactionDlg();

    int idForEdit() const;
    void setIdForEdit(int idForEdit);
    void setForImport( StransactionData* data);

    bool markInvalid() const;
    void setMarkInvalid(bool markInvalid);

    int newTransID() const;
    bool  checkValue();
    bool  checkAndSave();

private slots:
    void on_m_okButton_clicked();

    void on_m_cancelButton_clicked();

    void on_m_account_editingFinished();

private:
    Ui::CeditTransactionDlg *ui;
    int 				m_idForEdit;
    int                 m_newTransID;
    bool				m_markInvalid;
    bool                m_import;
    QCompleter*			m_fromToCompleter;
    QCompleter* 		m_accountCompleter;
    StransactionData*	m_currentTrans;
    void  populateForm();

    void  invalidateRemittance();
    // QWidget interface
protected:
    virtual void keyPressEvent(QKeyEvent * e);
};

#endif // CEDITTRANSACTIONDLG_H
