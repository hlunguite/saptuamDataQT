#ifndef CPAYMENTACCOUNTTAB_H
#define CPAYMENTACCOUNTTAB_H

#include <QWidget>
#include <QTableView>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QVBoxLayout>
class QSqlTableModel;
class CpaymentAccountTab : public QWidget
{
    Q_OBJECT
public:
    explicit CpaymentAccountTab(QWidget *parent = nullptr);

signals:
public slots:
    void customContextMenu(QPoint);
    void newPaymentAccount();
    void editPaymentAccount();

private:
    QSqlTableModel*             m_tableModel;
    QAction*                     m_newPaymentAccount;
    QAction*                     m_editPaymentAccount;
    QToolBar*					m_toolBar;
    QTableView*					m_table;
    QVBoxLayout*				m_vlayout;
    int                         m_editID = 0;

    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event) override;
};

#endif // CPAYMENTACCOUNTTAB_H
