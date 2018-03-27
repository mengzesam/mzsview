#ifndef COMBOXDELEGATE_H
#define COMBOXDELEGATE_H
#include <QStyledItemDelegate>
#include <QComboBox>

class ComboboxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ComboboxDelegate(const QStringList& itemlist,QObject *parent = 0);
    ComboboxDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setItemlist(const QStringList& itemlist);

private:
    QStringList m_itemlist;
};


#endif // COMBOXDELEGATE_H
