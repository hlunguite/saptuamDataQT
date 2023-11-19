#ifndef CDESELECTABLETREEVIEW_H
#define CDESELECTABLETREEVIEW_H

#include<QTreeView>
#include <QMouseEvent>
class CdeselectableTreeView : public QTreeView
{

public:
    CdeselectableTreeView(QWidget * parent = 0);
    ~CdeselectableTreeView();
    virtual void mousePressEvent(QMouseEvent *event);
};

#endif // CDESELECTABLETREEVIEW_H
