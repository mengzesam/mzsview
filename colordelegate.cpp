#include "colordelegate.h"

ColorDelegate::ColorDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QWidget *ColorDelegate::createEditor(QWidget *parent,const QStyleOptionViewItem & /*option*/,
                                     const QModelIndex & index) const
{
    QPushButton* editor=new QPushButton(parent);
    editor->setText("");
    connect(editor, &QPushButton::clicked, this, &ColorDelegate::getColor);
    return editor;
}

void ColorDelegate::getColor()
{
    if(QPushButton *editor= static_cast<QPushButton *>(sender())){
     m_color=QColorDialog::getColor(Qt::yellow,0,"select color",QColorDialog::DontUseNativeDialog);
    //qDebug()<<&editor;
    }
}

void ColorDelegate::setEditorData(QWidget *editor,const QModelIndex &index) const
{
    QPushButton *myeditor = static_cast<QPushButton*>(editor);
    if(myeditor->text()==""){
        myeditor->click();
        if(!m_color.isValid()){
            QColor color=(QColor)index.model()->data(index,Qt::BackgroundRole).toString();
            myeditor->setStyleSheet(QString("QPushButton {background-color : %1;color : %2;}")
                               .arg(color.name()).arg(color.name()));
            myeditor->setText(color.name());
        }else{
            myeditor->setStyleSheet(QString("QPushButton {background-color : %1;color : %2;}")
                               .arg(m_color.name()).arg(m_color.name()));
            myeditor->setText(m_color.name());
        }
    }
}

void ColorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const
{
    QPushButton *myeditor = static_cast<QPushButton*>(editor);
    if(myeditor->text()!=""){
        QString value=myeditor->text();
        model->setData(index, QVariant(QColor(value)), Qt::BackgroundRole);
    }
}

void ColorDelegate::updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option,
                                         const QModelIndex & /*index*/) const
{
    editor->setGeometry(option.rect);
}
