#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QCloseEvent>
#include <QMenu>
#include "TAB/cmaintab.h"
#include<QDockWidget>

namespace Ui {
class MainWindow;
}
class CsaptuamDataDBMgr;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool getIsError();
private slots:
    void processSignals(int);
    void newAccountSlot();
    void newDeptSlot();
    void receiptSetup();
    void assignReceipt();
    void newTransaction();
    void importBankStatement();
    void processBankStatement();
    void addRemittance();
    //void remittancePCSetup();
    void remittanceSetup();
    void ledgerReport();
    void summaryReport();
    void sendSMS();
    void setting();
    void minImport();
    void accountImport();
    void transImport();
    void saveAll();
    void closing();
private:
    //Ui::MainWindow *ui;
    CsaptuamDataDBMgr*	m_saptuamDB;
    bool                m_isError;
    CmainTab*			m_mainTab;

    //Menu
    QMenu* 				m_fileMenu;
    QMenu*				m_actionMenu;
    QMenu*				m_reportMenu;
    QMenu*              m_settingsMenu;
    QMenu*				m_help;
    QMenu*				m_receiptMenu;
    QMenu*				m_transactionMenu;
    QMenu*				m_remittanceMenu;
    //Action
    QAction* 			m_newAccount;
    QAction*			m_newDept;
    QAction* 			m_newTransaction;
    QAction*			m_newBankImport;
    QAction*            m_processBankImport;
    QAction*			m_remittance;
    //QAction*			m_remitPercentSetup;
    QAction*			m_remittanceSetup;
    //
    QAction*            m_minImport;
    QAction*            m_accountImport;
    QAction*            m_transImport;
    QAction*			m_saveAll;
    //report
    QAction*			m_ledgerReport;
    QAction*			m_statementReport;
    QAction*			m_summaryReport;
    QAction*            m_closingReport;
    QAction*             m_sendSms;
    //settings
    QAction*			m_fySetting;
    QAction*			m_passwordSetting;
    //receipt action
    QAction*			m_receiptSetup;
    QAction*			m_assignReceiptToTransaction;

    QDockWidget* 		m_transactionDock;


    void createAction();
    // QWidget interface
    bool okToContinue();
    void writeSettings();
    void readSettings();
    void createMenu();
    void enableMenu(bool flag);
    void setupTab();

    void addDlgToDock(QDialog* dlg, QString title);



protected:
    virtual void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
