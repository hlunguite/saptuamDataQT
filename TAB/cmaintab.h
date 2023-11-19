#ifndef CMAINTAB_H
#define CMAINTAB_H
#include<QTabWidget>

class CaccountTab;
class CcontactTab;
class CtransactionTab;
class CmainTab : public QTabWidget
{
    Q_OBJECT
public:
    CmainTab(QTabWidget * parent = 0);
    ~CmainTab();
    void setTabWidth(int width);

    void newAccount();
    void newDept();
signals:
    void signalFromMainTab(int);
public slots:
    void refreshTabs();
    void updateDirectory();
    void updateAccountTab();
private slots:
    void processSignal(int val);
private:
    CaccountTab*	m_accountTab;
    CcontactTab*	m_directoryTab;
    CtransactionTab* m_transactionTab;
};

#endif // CMAINTAB_H
