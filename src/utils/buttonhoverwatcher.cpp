#include "headers/utils/buttonhoverwatcher.h"

ButtonHoverWatcher::ButtonHoverWatcher(QObject *parent) : QObject(parent)
{}

bool ButtonHoverWatcher::eventFilter(QObject *watched, QEvent *event)
{
    QPushButton *button = qobject_cast<QPushButton*>(watched);
    if (!button) {
        return false;
    }

    if (event->type() == QEvent::Enter) {
        button->setIcon(QIcon(":/images/clear-hover.png"));
        return true;
    }

    if (event->type() == QEvent::Leave){
        button->setIcon(QIcon(":/images/clear.png"));
        return true;
    }

    return false;
}
