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

    Q_PROPERTY(QString menu READ menu WRITE setMenu NOTIFY menuChanged)
    Q_PROPERTY(QString appid READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString icon READ icon WRITE setIcon NOTIFY iconChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(qint32 status READ status WRITE setStatus NOTIFY statusChanged)

public:
    DockQuickWindow(QQuickWindow *parent = 0);
    ~DockQuickWindow();

    const QString& menu() { return m_menu; }
    void setMenu(const QString& m) { m_menu = m; Q_EMIT menuChanged(m);}
    Q_SIGNAL void menuChanged(QString);

    const QString& id() { return m_id; }
    void setId(const QString& v) { m_id = v; Q_EMIT idChanged(v);}
    Q_SIGNAL void idChanged(QString);

    const QString& icon() {return m_icon; }
    void setIcon(const QString& v) { m_icon = v; Q_EMIT iconChanged(v);}
    Q_SIGNAL void iconChanged(QString);

    const QString& title() {return m_title; }
    void setTitle(const QString& v) { QQuickWindow::setTitle(v); m_title= v; Q_EMIT titleChanged(v);}
    Q_SIGNAL void titleChanged(QString);

    qint32 status() {return m_status; }
    void setStatus(const qint32 v) { m_status = v; Q_EMIT statusChanged(v);}
    Q_SIGNAL void statusChanged(qint32);

private:
    QString m_menu;
    QString m_id;
    QString m_icon;
    QString m_title;
    qint32 m_status;
    bool firstShow;
    DockApplet* applet;
};

class DockApplet : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "dde.dock.Entry")
    Q_PROPERTY(QString Type READ type)
    Q_PROPERTY(QString Id READ id)
    Q_PROPERTY(quint32 XID READ xid)

    Q_PROPERTY(QString Menu READ menu NOTIFY menuChanged)
    Q_PROPERTY(QString Title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString Icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(qint32 Status READ status NOTIFY statusChanged)

public:
    DockApplet(DockQuickWindow* parent, int xid);

    const QString type() { return "Applet"; }
    const QString id() {return m_parent->id(); }
    const QString menu() { return m_parent->menu(); }
    const QString title() { return m_parent->title(); }
    quint32 xid() { return m_parent->winId(); }
    const QString icon() { return m_parent->icon(); }
    qint32 status() { return m_parent->status(); }
    bool viewable() { return m_parent->isVisible(); }
    QRect allocation() { return QRect(QPoint(m_parent->x(), m_parent->y()), m_parent->size()); }

    Q_SIGNAL void menuChanged(QString);
    Q_SIGNAL void titleChanged(QString);
    Q_SIGNAL void iconChanged(QString);
    Q_SIGNAL void statusChanged(qint32);

private:
    DockQuickWindow* m_parent;
};

#endif // MYITEM_H
