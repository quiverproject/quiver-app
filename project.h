#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QDebug>

class Platform : public QObject {
        Q_OBJECT
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
public:
        explicit Platform(QString name = QString()
                        , bool enabled = false
                        , QObject *parent = 0) : QObject(parent)
              , m_name(name)
              , m_enabled(enabled)
        {
                connect(this, &Platform::enabledChanged, [=]() {
                        qDebug() << m_name << m_enabled;
                });
        }
        QString name() const { return m_name; }
        bool enabled() const { return m_enabled; }
public slots:
        void setName(const QString &arg) { m_name = arg; emit nameChanged(); }
        void setEnabled(const bool &arg) { m_enabled = arg; emit enabledChanged(); }
signals:
        void nameChanged();
        void enabledChanged();
private:
        QString m_name;
        bool m_enabled;
};

class Project : public QObject {
        Q_OBJECT
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(QList<QObject *> platforms READ platforms NOTIFY platformsChanged)
public:
        explicit Project(QString name = QString()
                        , QObject *parent = 0) : QObject(parent)
              , m_name(name)
        {
        }
        QString name() const { return m_name; }
        QList<QObject *> platforms() const { return m_platforms; }
public slots:
        void setName(const QString &arg) { m_name = arg; emit nameChanged(); }
        void addPlatform(Platform *arg) { m_platforms << arg; emit platformsChanged(); }
signals:
        void nameChanged();
        void platformsChanged();
private:
        QString m_name;
        QList<QObject *> m_platforms;
};

#endif // PROJECT_H
