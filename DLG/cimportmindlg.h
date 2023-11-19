#ifndef CIMPORTMINDLG_H
#define CIMPORTMINDLG_H

#include <QDialog>
#include "cdlgbase.h"
#include "cimportmintable.h"

namespace Ui {
class CimportMinDlg;
}

class CimportMinDlg : public CdlgBase
{
    Q_OBJECT

public:
    explicit CimportMinDlg(QWidget *parent = nullptr);
    ~CimportMinDlg();

private slots:
    void on_m_minFileBtn_clicked();

    void on_m_closeBtn_clicked();

    void on_m_nextBtn_clicked();
signals:
    void updateContactTable();

private:
    Ui::CimportMinDlg *ui;
    void resize();
    QVector<SimportMinData *>* m_importMin;
    int m_index;
    void populateTable();
    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // CIMPORTMINDLG_H
