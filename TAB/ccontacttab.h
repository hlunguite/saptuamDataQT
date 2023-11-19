#ifndef CDIRECTORYTAB_H
#define CDIRECTORYTAB_H

#include <QWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QAction>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>

class CaddressTab;
class CcontactTab : public QWidget
{
    Q_OBJECT
public:
    explicit CcontactTab(QWidget *parent = 0);
    ~CcontactTab();

signals:

public slots:
    void search(QString txt);
    void refreshTableAddress();
private:
    QToolBar*		m_toolBar;
    QAction*  		m_addContact;
    QAction*        m_editAction;
    QLabel* 		m_searchLabel;
    QLabel*			m_searchIn;
    QComboBox* 		m_searchCombo;
    QLineEdit* 		m_searchLine;
    CaddressTab*	m_addressTab;
    QVBoxLayout* 	m_vboxLayout;

    void search();


};

#endif // CDIRECTORYTAB_H
