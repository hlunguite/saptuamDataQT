#ifndef CSELECTREMITTANCE_H
#define CSELECTREMITTANCE_H

#include <QDialog>
#include <set>
#include <vector>
#include <QRadioButton>
namespace Ui {
class CselectRemittance;
}

class CselectRemittance : public QDialog
{
    Q_OBJECT

public:
    explicit CselectRemittance(QWidget *parent = nullptr);
    ~CselectRemittance();

    int selectID() const;

    void setAllID(const std::set<int> &newAllID);

private slots:


    void on_m_cancelBtn_clicked();

    void on_m_okBtn_clicked();

private:
    Ui::CselectRemittance *ui;
    int              m_selectID;
    std::vector<int> m_allID;
    QVector<QRadioButton*>  m_remitRadioBox;

    void resize();

    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event) {resize();}
};

#endif // CSELECTREMITTANCE_H
