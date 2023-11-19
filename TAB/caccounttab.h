#ifndef CACCOUNTTAB_H
#define CACCOUNTTAB_H

#include <QWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QMenu>
#include "DB/cdbtablebase.h"
#include "cdepartmenttable.h"
#include "caccounttable.h"
class CdeselectableTreeView;
class CSortFilterProxyModelForTreeModel;
class CaccountTreeModel;
class CaccountTab : public QWidget
{
    Q_OBJECT
public:
    explicit CaccountTab(QWidget *parent = 0);
    ~CaccountTab();
    void setAccountTabWidth(int width);
    void refreshAccounts();
signals:

public slots:
    void customContextMenu(QPoint);
    void createNewAccount();
    void createNewDept();
    void editAccount();
    void editDept();
private:
    int									m_currentAccountID;
    int									m_currentDeptID;
    QToolBar*							m_toolBar ;
    QVBoxLayout*						m_virticalBox;
    CaccountTreeModel*					m_accountTreeModel;
    CdeselectableTreeView* 				m_treeView;
    CSortFilterProxyModelForTreeModel*	m_model;

    QAction*							m_newAccount;
    QAction*							m_newDept;
    QAction*							m_editAccount;
    QAction*							m_editDept;

    //void insertRow(QVector<QVariant>& data);
    void resizeTree();
    void addContextMenu(QMenu *menu , QString accountname);

};

#endif // CACCOUNTTAB_H
