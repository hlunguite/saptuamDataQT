#ifndef CASSIGNRECEIPTDLG_H
#define CASSIGNRECEIPTDLG_H

#include <QDialog>
#include "VIEWSANDMODEL/ctransactiontablemodel.h"
#include "creceiptbooktable.h"
#include "cdlgbase.h"
#include "creceiptbookmap.h"

namespace Ui {
class CassignReceiptDlg;
}

class CassignReceiptDlg : public CdlgBase
{
    Q_OBJECT

public:
    explicit CassignReceiptDlg(QWidget *parent = nullptr);
    ~CassignReceiptDlg();

private slots:
    void on_m_cancelBtn_clicked();

    void on_m_okBtn_clicked();

    void on_m_startDate_dateChanged(const QDate &date);

    void on_m_endDate_dateChanged(const QDate &date);

    void on_m_accountComboBox_currentTextChanged(const QString &arg1);

    void on_m_departmentComboBox_currentTextChanged(const QString &arg1);

    void on_m_clearBtn_clicked();

    void on_m_autoFillBtn_clicked();

    void on_m_sameReceiptAccount_clicked();

private:
    Ui::CassignReceiptDlg *ui;
    CtransactionTableModel*		m_transationTableModel;
    QMap<QString, QStringList> 	m_departmentAccount;
    QMap<int, std::set<int> >	m_accountReceipts;
    QMap<int, std::set<int> >	m_accountWithSameReceipt;

    QVector<int>        		m_currentReceiptBooks;
    std::set<int>               m_transAccounts;
    void resize();
    void populateDeptAccountReceiptDetail();
    void populateReceiptTable();
    void populateTransTable();
    bool checkTransTable();
    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *);

};
#endif // CASSIGNRECEIPTDLG_H
