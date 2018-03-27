#include "comboboxdelegate.h"

ComboboxDelegate::ComboboxDelegate(const QStringList& itemlist,QObject *parent)
    : QStyledItemDelegate(parent)
{
    for(int i=0;i<itemlist.size();i++)
        m_itemlist.append(itemlist.at(i));
}

ComboboxDelegate::ComboboxDelegate(QObject *parent)
    : QStyledItemDelegate(parent),
      m_itemlist(QStringList({""}))
{}

void ComboboxDelegate::setItemlist(const QStringList &itemlist)
{
    m_itemlist.clear();
    for(int i=0;i<itemlist.size();i++)
        m_itemlist.append(itemlist.at(i));
}

QWidget *ComboboxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & option,
                                        const QModelIndex & index) const
{
    if(m_itemlist.size()>0){
        QComboBox* editor = new QComboBox(parent);
        editor->setInsertPolicy(QComboBox::InsertAtTop);
        for(int i = 0; i <m_itemlist.size(); ++i)
          {
            editor->addItem(m_itemlist.at(i));
          }
        return editor;
    }else{
        return QStyledItemDelegate::createEditor(parent,option,index);
    }
}

void ComboboxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *cb;
    if(m_itemlist.size()>0 && (cb = static_cast<QComboBox*>(editor))){
        QString currentText = index.data(Qt::EditRole).toString();
        int cbIndex = cb->findText(currentText);
        if (cbIndex >= 0)
            cb->setCurrentIndex(cbIndex);
    }else{
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void ComboboxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *cb;
    if (m_itemlist.size()>0 && (cb = qobject_cast<QComboBox*>(editor))){
        model->setData(index, cb->currentText(), Qt::EditRole);
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}

void ComboboxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                            const QModelIndex & /*index*/) const
{
    editor->setGeometry(option.rect);
}
