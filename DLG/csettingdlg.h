#ifndef CSETTINGDLG_H
#define CSETTINGDLG_H

#include <QDialog>

namespace Ui {
class CsettingDlg;
}

class CsettingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CsettingDlg(QWidget *parent = nullptr);
    ~CsettingDlg();

private slots:
    void on_m_closeBtn_clicked();

    void on_m_bwroseDirBtn_clicked();

private:
    Ui::CsettingDlg *ui;
    bool m_isNew;
};

#endif // CSETTINGDLG_H
