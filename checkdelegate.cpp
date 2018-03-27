#include "checkdelegate.h"
#include <QDebug>

CheckDelegate::CheckDelegate(Qt::CheckState defaultstate, QWidget *parent)
    : QStyledItemDelegate(parent)
{
    m_defaultstate=defaultstate;
}

CheckDelegate::CheckDelegate(QWidget *parent)
    : QStyledItemDelegate(parent)
    ,m_defaultstate(Qt::Unchecked)
{
}

QWidget *CheckDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/*option*/,
                                     const QModelIndex &/*index*/) const
{
    QCheckBox* editor=new QCheckBox(parent);
//    editor->setText(mzsview::CHECK_TEXTS[m_defaultstate]);
//    editor->setCheckState(m_defaultstate);
    return editor;
}

void CheckDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(QCheckBox* ckb = static_cast<QCheckBox*>(editor)){
        Qt::CheckState state=static_cast<Qt::CheckState>(index.data(Qt::EditRole).toInt());
        ckb->setText(mzsview::CHECK_TEXTS[state]);
        ckb->setCheckState(state);
    }else{
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void CheckDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(QCheckBox* ckb = static_cast<QCheckBox*>(editor)){
        int state= ckb->checkState();
        model->setData(index, state, Qt::EditRole);
    }else{
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void CheckDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                         const QModelIndex &/*index*/) const
{
    editor->setGeometry(option.rect);
}
