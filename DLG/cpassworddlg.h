#ifndef CPASSWORDDLG_H
#define CPASSWORDDLG_H

#include <QDialog>

namespace Ui {
class CpasswordDlg;
}

class CpasswordDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CpasswordDlg(QWidget *parent = nullptr);
    ~CpasswordDlg();

    bool isPasswordCorrect() const;

private slots:
    void on_m_password_textChanged(const QString &arg1);

private:
    Ui::CpasswordDlg *ui;
    bool m_isPasswordCorrect;
};

#endif // CPASSWORDDLG_H
