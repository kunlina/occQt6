#ifndef OCCVIEWCONTEXTMENU_H
#define OCCVIEWCONTEXTMENU_H

#include <QMenu>

#include "occviewenums.h"

class occViewContextMenu : public QMenu
{
    Q_OBJECT
public:
    explicit occViewContextMenu(QWidget *parent,
                                occViewEnums::drawStyle drawStyle);
    ~occViewContextMenu() = default;

signals:
    void fitAll();
    void fitArea();

    // view actions
    void axo();
    void front();
    void back();
    void left();
    void right();
    void top();
    void bottom();

    // draw styles
    void points();
    void wireframe();
    void hlrOn();
    void shaded();
    void shadedWithEdges();

private:

    int _iconHeight;
    occViewEnums::drawStyle _curDrawStyle;
    void addViewActions(QMenu &viewMenu);
    void addDrawStyles(QMenu &drawMenu);
};

#endif // OCCVIEWCONTEXTMENU_H
