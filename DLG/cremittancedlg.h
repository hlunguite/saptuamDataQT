#ifndef CREMITTANCEDLG_H
#define CREMITTANCEDLG_H

#include <QDialog>
#include <QCheckBox>
#include <map>
#include <set>
#include <vector>
#include "MISC/chtmlutils.h"
#include <QTableWidgetItem>
namespace Ui {
class CremittanceDlg;
}
class CcustomDeligateForRemittance;
struct StransactionData;
class CremittanceDlg : public QDialog {
  Q_OBJECT

public:
    explicit CremittanceDlg(QWidget *parent = nullptr);
    ~CremittanceDlg();

private slots:
    void on_m_remittanceNameCombo_currentTextChanged(const QString &arg1);

    void on_m_periodFrom_dateChanged(const QDate &date);

    void on_m_periodTo_dateChanged(const QDate &date);

    void on_m_submitDate_dateChanged(const QDate &date);

    void on_m_modeCombo_currentTextChanged(const QString &arg1);

    void on_m_referene_editingFinished();

    void on_m_transDate_dateChanged(const QDate &date);

    void on_m_toAddressBox_textChanged();

    void on_m_submitByBox_textChanged();

    void accountCheckBoxClicked();
    void on_m_cancelBtn_clicked();

    void on_m_pdfBtn_clicked();

    void on_m_csvBtn_clicked();

    void deleteAllRow();
    void addRow();
    void deleteRow();
    void on_m_table_customContextMenuRequested(const QPoint &pos);

    void on_m_table_itemChanged(QTableWidgetItem *item);

private:
    struct SremitTransDetail {
        int m_transID;
        QString m_from;
        QString m_amount;
        QString m_receiptBook;
        QString m_receiptNo;
        int     m_accountID;
        SremitTransDetail(QString str);
        SremitTransDetail(int id, QString name, QString amount, QString book, QString no, int accountID):
            m_transID(id),
            m_from(name), m_amount(amount), m_receiptBook(book), m_receiptNo(no), m_accountID(accountID){}
        QString toString();

    };
    struct StransForRemittance{
        double                          m_hqShare;
        double                          m_localShare;

        std::map<int, std::set<int> >   m_receipts;
        std::vector<SremitTransDetail>  m_allTrans;
        StransForRemittance() {
            m_hqShare = 0;
            m_localShare = 0;
            m_receipts.clear();
            m_allTrans.clear();
        }

    };

    Ui::CremittanceDlg *ui;
    bool                                        m_constructor;
    QVector<QCheckBox*>                         m_accountCheckBox;
    QVector<int>                                m_accontIDs;
    std::map<int, std::pair<float, float>>      m_accountPct;
    std::map<int, StransForRemittance>          m_accountRemittance;
    std::map<int, StransForRemittance>          m_deptRemittance;
    bool                                        m_allHq;
    int                                         m_selectRow;
    QAction*                                    m_deleteRowAction;
    QAction*                                    m_addRowAction;
    QString                                     m_html;
    QStringList                                 m_csvList;
    CcustomDeligateForRemittance*               m_customDeligateForRemittance;
    double                                      m_remittanceAmount;
    int                                         m_remittanceIndex;
    int saveRemittance();
    QString getQueryString(int remittanceID , bool isEqual);

    void populateAccount();
    void populateTable();
    void populateHTMLTable();
    bool populateTableUsingTransaction();
    bool populateTransaction(QVector<StransactionData*>&results);
    void calclateTotal();
    void calculateTotalAndPopulateHtmlTable();


    bool isEmptyOrNull(QTableWidgetItem *item);
    bool checkInputValidity();
    void populateSummaryTable(ChtmlUtils& htmlUtisl,
                              QStringList& csvList,
                              std::map<int, StransForRemittance>::iterator & keyvalue,
                              int &rowCount,
                              bool isAccount,
                              double& total,
                              double& local,
                              double& hq);
    void populateTransacltionListTable(int accountDetp, const StransForRemittance& transForRemittance, bool isAccount);


    void resize();
    void disableEnable(bool enable);
    void setFieldBaseOnremittanceID();

    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *);
};

#endif // CREMITTANCEDLG_H
