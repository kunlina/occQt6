#ifndef OCCVIEWCONTEXTMENU_H
#define OCCVIEWCONTEXTMENU_H

#include <QMenu>

class occViewContextMenu : public QMenu
{
    Q_OBJECT
public:
    explicit occViewContextMenu(QWidget *parent = nullptr);
    ~occViewContextMenu() = default;

signals:
    void fitAll();
    void fitArea();

    // view actions
    void axo();
    void front();
    void back();

private:



    int _iconHeight;
    void addViewActions(QMenu &viewMenu);
};

#endif // OCCVIEWCONTEXTMENU_H
