#ifndef CADDRESSTAB_H
#define CADDRESSTAB_H
#include <QTabWidget>
#include <QVector>
#include <QAction>
#include <QModelIndex>
class CcontactDLG;
class CaddressTab : public QTabWidget
{
    Q_OBJECT

public:
    CaddressTab(QWidget *parent = 0);
    ~CaddressTab();
    void search(QString searchText, QString searchIn);
    void refreshTable();
public slots:
    void addContact();
    void editSelectMember();
    void getAllTransaction();
    void customContextMenu(QPoint point);
    void doubleClick(QModelIndex index);
private:
    void setupTab();

    void refreshForSearch(QVector<QPair<int,QString> >& idNamePair);
    int			m_selectedID;
    QAction*	m_addContact;
    QAction*	m_editContact;
    QAction*	m_getAllTransaction;

    QVector<QString> m_originalFilter;
    QVector<QString> m_tabText;
    QVector<int> 	 m_tabIndex;



    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent * e);
};

#endif // CADDRESSTAB_H
