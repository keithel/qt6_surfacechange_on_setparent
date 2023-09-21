#ifndef EVENTLURKER_H
#define EVENTLURKER_H

#include <QObject>

class EventLurker : public QObject
{
    Q_OBJECT
public:
    explicit EventLurker(QObject *parent = nullptr);

    bool eventFilter(QObject *watched, QEvent *event);
};

#endif // EVENTLURKER_H
