#ifndef DRAWHANDLER_H
#define DRAWHANDLER_H

#include "displayareahandler.h"
#include <qobject.h>
#include "drawlist.h"

class QMouseEvent;
class QPixmap;
class QPoint;
class Draw;
class QPainter;

class DrawHandler :public DisplayAreaHandler {
    Q_OBJECT

public:
    DrawHandler( DisplayArea* display );
    virtual bool mousePressEvent ( QMouseEvent* e, const QPoint& unTranslatedPos, double scaleFactor );
    virtual bool mouseReleaseEvent ( QMouseEvent* e, const QPoint& unTranslatedPos, double scaleFactor );
    virtual bool mouseMoveEvent ( QMouseEvent* e, const QPoint& unTranslatedPos, double scaleFactor );
    DrawList drawList() const;
    void setDrawList( const DrawList& );
    bool hasDrawings() const;

    void drawAll( QPainter& );
    void stopDrawing();

public slots:
    void slotLine();
    void slotRectangle();
    void slotCircle();
    void slotSelect();
    void cut();

signals:
    void redraw();
    void active();

protected:
    Draw* createTool();
    Draw* findShape( const QPoint& );
    void setupPainter( QPainter* painter );

private:
    enum Tool { Select, Line, Rectangle, Circle, None};
    Tool _tool;
    Draw* _activeTool;
    DrawList _drawings;


};

#endif /* DRAWHANDLER_H */

