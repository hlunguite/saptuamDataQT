#include "cdeselectabletreeview.h"

CdeselectableTreeView::CdeselectableTreeView(QWidget * parent):
    QTreeView(parent)
{

}

CdeselectableTreeView::~CdeselectableTreeView()
{

}

void
CdeselectableTreeView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex item = indexAt(event->pos());
    bool selected = selectionModel()->isSelected(indexAt(event->pos()));
    QTreeView::mousePressEvent(event);
    // QString accountname = "";
    //qDebug()<<"selected "<<selected <<" "<<item.row()<<" "<<item.column();
    // if(item.row() > 0) accountname = model()->index(item.row(),0,item.parent()).data().toString();
    if ((item.row() == 0 && item.column() == 0) || selected)
    {
        clearSelection();
        const QModelIndex index;
        selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);

    }
}

