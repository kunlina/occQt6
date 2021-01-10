#include "emptyspacerwidget.h"

emptySpacerWidget::emptySpacerWidget(QWidget *parent) : QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
}
