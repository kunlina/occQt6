#ifndef HUDWIDGET_H
#define HUDWIDGET_H

#include <QObject>
#include <QWidget>

// heads-up display widget, similar to Heads-Up View in Solidworks
class hudWidget : public QWidget
{
    Q_OBJECT
public:
    explicit hudWidget(QWidget *parent = nullptr);

signals:

};

#endif // HUDWIDGET_H
