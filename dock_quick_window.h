#ifndef MYITEM_H
#define MYITEM_H

#include <QQuickItem>
#include <QQuickWindow>
#include <QtDBus/QDBusAbstractAdaptor>
#include <QRect>

class DockApplet;
class DockQuickWindow: public QQuickWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(DockQuickWindow)

    Q_PROPERTY(QString menu READ menu WRITE setMenu)

public:
    const QString& menu() {
        return m_menu;
    }
    void setMenu(const QString& m) {
        m_menu = m;
    }

    DockQuickWindow(QQuickWindow *parent = 0);
    ~DockQuickWindow();
protected:
    void resizeEvent(QResizeEvent *);
private:
    QString m_menu;
    bool firstShow;
    DockApplet* applet;
};
struct Rectangle {
    int16_t X;
    int16_t Y;
    u_int16_t Width;
    u_int16_t Height;
    Rectangle(int16_t x, int16_t y, u_int16_t w, u_int16_t h):
        X(x), Y(y), Width(w), Height(h)
    {
    }
};

class DockApplet : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "dde.dock.Entry")
    Q_PROPERTY(QString Type READ type)
    Q_PROPERTY(QString Id READ id)
    Q_PROPERTY(QString Menu READ menu)
    Q_PROPERTY(QString Tooltip READ title)
    Q_PROPERTY(quint32 XID READ xid)
    Q_PROPERTY(QString Icon READ icon)
    Q_PROPERTY(qint32 Status READ status)
    Q_PROPERTY(bool QuickWindowViewable READ viewable)
    Q_PROPERTY(QRect Allocation READ allocation)

public:
    DockApplet(DockQuickWindow* parent, int xid);

    const QString type() { return "Applet"; }
    const QString id() {return "Applet" + QString::number(xid()); }
    const QString menu() { return m_parent->menu(); }
    const QString title() { return m_parent->title(); }
    quint32 xid() { return m_parent->winId(); }
    const QString icon() { return "dss"; }
    qint32 status() { return 1; }
    bool viewable() { return m_parent->isVisible(); }
    QRect allocation() { return QRect(QPoint(m_parent->x(), m_parent->y()), m_parent->size()); }

private:
    DockQuickWindow* m_parent;
signals:
    void hide();
};

#endif // MYITEM_H

