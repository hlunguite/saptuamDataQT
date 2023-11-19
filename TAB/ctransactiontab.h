#ifndef CTRANSACTIONTAB_H
#define CTRANSACTIONTAB_H
#include <QVBoxLayout>
#include <QWidget>
#include <QTableView>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include "VIEWSANDMODEL/ctransactiontablemodel.h"
class CtransactionTab : public QWidget
{
    Q_OBJECT
public:
    explicit CtransactionTab(QWidget *parent = 0);
    ~CtransactionTab();
    void refreshTable();
signals:
    void signalFromTransactionTab(int val);
public slots:
    void customContextMenu(QPoint);
    void editTrans();
    void markInvalid();
    void newTransaction();
    void addReceiptNo();
    void importBankStatment();
    void processBankStatement();
    void remittance();
    void summaryReport();
    void filterTable();
    void search();
    void search(int i);
    void sendSMS();
private:
    int							m_selectedId;
    QString						m_filter;
    QAction*					m_editTrans;
    QAction*					m_markInvalid;
    QAction*					m_newTransaction;
    QAction*					m_importBankStatment;
    QAction*                    m_processBankStatement;
    QAction*					m_enterReceiptNo;
    QAction*					m_hqRemittance;
    QAction*					m_summaryReport;
    QAction*					m_filterTable;
    QAction*                    m_sendSMS;
    QLabel*						m_searchLabel;
    QLineEdit*					m_searchLine;
    QLabel*						m_searchInLabel;
    QComboBox*					m_searchField;

    QToolBar*					m_toolBar;
    QTableView*					m_table;
    QVBoxLayout*				m_vlayout;
    CtransactionTableModel* 	m_transationTableModel;

    void addToolBar();
    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *);
};

#endif // CTRANSACTIONTAB_H
