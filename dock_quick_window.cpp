#include "dock_quick_window.h"
#include <QDBusConnection>

DockQuickWindow::DockQuickWindow(QQuickWindow *parent):
    QQuickWindow(parent), firstShow(true),
    applet(new DockApplet(this, winId()))
{
    // By default, QQuickItem does not draw anything. If you subclass
    // QQuickItem to create a visual item, you will need to uncomment the
    // following line and re-implement updatePaintNode()

    // setFlag(ItemHasContents, true);
    qDebug() << "hehe:" <<  this->winId();
    this->applet->setProperty("menu", "123");

}

void DockQuickWindow::resizeEvent(QResizeEvent *) {
    if (firstShow) {
        firstShow=false;
        this->hide();
    }
}

DockQuickWindow::~DockQuickWindow()
{
}



DockApplet::DockApplet(DockQuickWindow* parent, int xid) :
    QDBusAbstractAdaptor(parent),
    m_parent(parent)
{
    QString id  = QString::number(xid);
    QDBusConnection::sessionBus().registerService("dde.dock.entry.Applet" + id);
    qDebug() << "Register:" << QDBusConnection::sessionBus().registerObject("/dde/dock/entry/Applet" + id, parent);

    qDebug() << "CreateProxyer for:" <<  xid;
}
