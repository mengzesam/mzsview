#ifndef DELEGATE_H
#define DELEGATE_H
#include <QStyledItemDelegate>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>

class ColorDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    ColorDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                               const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const override;

    void updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void getColor();

private:
    QColor m_color;
};

#endif // DELEGATE_H
