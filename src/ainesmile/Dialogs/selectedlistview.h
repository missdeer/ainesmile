#ifndef SELECTEDLISTVIEW_H
#define SELECTEDLISTVIEW_H

#include <QListView>

class SelectedListView : public QListView
{
    Q_OBJECT
public:
    explicit SelectedListView(QWidget *parent = 0);
    
signals:
    void itemSelected( const QItemSelection & selected, const QItemSelection & deselected );
public slots:
    void selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected );
};

#endif // SELECTEDLISTVIEW_H
