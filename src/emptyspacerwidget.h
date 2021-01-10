#ifndef EMPTYSPACERWIDGET_H
#define EMPTYSPACERWIDGET_H

#include <QWidget>

// https://stackoverflow.com/questions/12730591/how-to-add-stretchable-spacer-in-qtoolbar
class emptySpacerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit emptySpacerWidget(QWidget *parent = nullptr);
    ~emptySpacerWidget() = default;
};

#endif // EMPTYSPACERWIDGET_H
