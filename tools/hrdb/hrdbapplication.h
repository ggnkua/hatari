#ifndef HRDBAPPLICATION_H
#define HRDBAPPLICATION_H

#include <QApplication>
#include "models/session.h"

class HrdbApplication : public QApplication
{
public:
    HrdbApplication(int &argc, char **argv);

    // The application stores the session, so it can be used by command-line
    Session                     m_session;
};

#endif // HRDBAPPLICATION_H
