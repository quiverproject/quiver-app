#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>

class Platform : public QObject
{
        Q_OBJECT
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
public:
        explicit Platform(QString name = QString(), QObject *parent = 0) {
                m_name = name;
        }
        QString name() const { return m_name; }
signals:
        void nameChanged();
public slots:
        void setName(const QString &arg) {
                m_name = arg;
        }
private:
        QString m_name;
};

class Project : public QObject
{
        Q_OBJECT
        Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
        Q_PROPERTY(QList<QObject *> platforms READ platforms NOTIFY platformsChanged)
public:
        explicit Project(QObject *parent = 0);
        QString name() const { return m_name; }
        QList<QObject *> platforms() const { return m_platforms; }
signals:
        void nameChanged();
        void platformsChanged();
public slots:
        void setName(const QString &arg);
private:
        QString m_name;
        QList<QObject *> m_platforms;
};

#endif // PROJECT_H
