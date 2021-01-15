#include "occwindow.h"

occWindow::occWindow(QWidget *parent) : Aspect_Window(), _myWidget(parent)
{
    _xLeft = _myWidget->rect().left();
    _yTop = _myWidget->rect().top();
    _xRight = _myWidget->rect().right();
    _yBottom = _myWidget->rect().bottom();
}


void occWindow::Destroy()
{
    _myWidget = nullptr;
}


Aspect_Drawable occWindow::NativeParentHandle() const
{
    QWidget* aParentWidget = _myWidget->parentWidget();
    if (aParentWidget)
      return (Aspect_Drawable)aParentWidget->winId();
    else
      return 0;
}


Aspect_Drawable occWindow::NativeHandle() const
{
    return (Aspect_Drawable)_myWidget->winId();
}


Standard_Boolean occWindow::IsMapped() const
{
    return !( _myWidget->isMinimized() || _myWidget->isHidden() );
}


void occWindow::Map() const
{
    _myWidget->show();
    _myWidget->update();
}


void occWindow::Unmap() const
{
    _myWidget->hide();
    _myWidget->update();
}


Aspect_TypeOfResize occWindow::DoResize()
{
    int aMask {0};
    auto aMode = Aspect_TOR_UNKNOWN;

    if (!_myWidget->isMinimized()) {
        if ( Abs(_myWidget->rect().left()   - _xLeft   ) > 2 ) aMask |= 1;
        if ( Abs(_myWidget->rect().right()  - _xRight  ) > 2 ) aMask |= 2;
        if ( Abs(_myWidget->rect().top()    - _yTop    ) > 2 ) aMask |= 4;
        if ( Abs(_myWidget->rect().bottom() - _yBottom ) > 2 ) aMask |= 8;

        switch ( aMask )
        {
          case 0:
            aMode = Aspect_TOR_NO_BORDER;
            break;
          case 1:
            aMode = Aspect_TOR_LEFT_BORDER;
            break;
          case 2:
            aMode = Aspect_TOR_RIGHT_BORDER;
            break;
          case 4:
            aMode = Aspect_TOR_TOP_BORDER;
            break;
          case 5:
            aMode = Aspect_TOR_LEFT_AND_TOP_BORDER;
            break;
          case 6:
            aMode = Aspect_TOR_TOP_AND_RIGHT_BORDER;
            break;
          case 8:
            aMode = Aspect_TOR_BOTTOM_BORDER;
            break;
          case 9:
            aMode = Aspect_TOR_BOTTOM_AND_LEFT_BORDER;
            break;
          case 10:
            aMode = Aspect_TOR_RIGHT_AND_BOTTOM_BORDER;
            break;
          default:
            break;
        }  // end switch

        _xLeft  = static_cast<Standard_Integer>(_myWidget->rect().left());
        _xRight = static_cast<Standard_Integer>(_myWidget->rect().right());
        _yTop   = static_cast<Standard_Integer>(_myWidget->rect().top());
        _yBottom = static_cast<Standard_Integer>(_myWidget->rect().bottom());
      } // end if

      return aMode;
}

Standard_Real occWindow::Ratio() const
{
    auto aRect = _myWidget->rect();
    return Standard_Real(aRect.right() - aRect.left()) / Standard_Real(aRect.bottom() - aRect.top());
}

void occWindow::Size(Standard_Integer &theWidth, Standard_Integer &theHeight) const
{
    theWidth = _myWidget->rect().right();
    theHeight = _myWidget->rect().bottom();
}


void occWindow::Position(Standard_Integer &theX1,
                          Standard_Integer &theY1,
                          Standard_Integer &theX2,
                          Standard_Integer &theY2) const
{
    theX1 = _myWidget->rect().left();
    theX2 = _myWidget->rect().right();
    theY1 = _myWidget->rect().top();
    theY2 = _myWidget->rect().bottom();
}

IMPLEMENT_STANDARD_RTTIEXT(occWindow, AspectWindow)
