#ifndef CPAYMENTACCOUNTDLG_H
#define CPAYMENTACCOUNTDLG_H

#include <QDialog>
#include "cdlgbase.h"
namespace Ui {
class CpaymentAccountDlg;
}

class CpaymentAccountDlg : public CdlgBase
{
    Q_OBJECT

public:
    explicit CpaymentAccountDlg(QWidget *parent = nullptr);
    ~CpaymentAccountDlg();
    bool setIDToEdit(int idtoedit);
private slots:
    void on_m_okBtn_clicked();

    void on_m_cancelBtn_clicked();

private:
    Ui::CpaymentAccountDlg *ui;
    int     m_idToEdit;
    void resize();
    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event) override {resize();}
};

#endif // CPAYMENTACCOUNTDLG_H
