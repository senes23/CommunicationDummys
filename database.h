#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QObject>

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);
    void Connect();
    void Disconnect();
    void sendQuery();

signals:

private:
    QSqlDatabase _db;
};

#endif // DATABASE_H
