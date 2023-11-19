#ifndef CIMPORTACCOUNTDLG_H
#define CIMPORTACCOUNTDLG_H

#include <QDialog>
#include "cimportaccounttable.h"
#include <QVector>
namespace Ui {
class CimportAccountDlg;
}

class CimportAccountDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CimportAccountDlg(QWidget *parent = nullptr);
    ~CimportAccountDlg();
signals:
    void updateAccountTab();
private:
    Ui::CimportAccountDlg *ui;
    QVector<SimportAccountData *> * m_importData;
    void resize();
    void clear();
    void populateTable();
    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override {resize();}
private slots:
    void on_m_processBtn_clicked();
    void on_m_cancelBtn_clicked();
    void on_m_selectBtn_clicked();
};

#endif // CIMPORTACCOUNTDLG_H
