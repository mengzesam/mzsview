#ifndef CHECKDELEGATE_H
#define CHECKDELEGATE_H
#include "mzsviewglobal.h"
#include <QStyledItemDelegate>
#include <QCheckBox>

class CheckDelegate:public QStyledItemDelegate
{
    Q_OBJECT

public:
    CheckDelegate(Qt::CheckState defaultstate,QWidget *parent = Q_NULLPTR);
    CheckDelegate(QWidget *parent = Q_NULLPTR);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                               const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setDefaultState(Qt::CheckState defaultstate){
        m_defaultstate=defaultstate;
    }
private:
    Qt::CheckState m_defaultstate;
};


#endif // CHECKDELEGATE_H
