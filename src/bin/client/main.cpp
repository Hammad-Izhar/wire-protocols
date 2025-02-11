#include <QApplication>
#include <QPushButton>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "client/model/session.hpp"
#include "client/gui/connection_window.hpp"
#include "client/gui/authentication_window.hpp"

class WorkerThread : public QThread
{
public:
    void run() override
    {
        Session &session = Session::getInstance();
        QMutexLocker lock(&session.session_mutex);

        while (!session.socket_fd.has_value())
        {
            while (!session.connection_address.has_value())
                session.worker_cond_var.wait(&session.session_mutex);

            if (!session.connection_address.has_value())
            {
                continue;
            }

            auto [hostname, port] = session.connection_address.value();
            QThread::sleep(2);

            if (port == 1)
            {
                session.socket_fd = 1;
            }
            else
            {
                session.error = "Connection failed :(";
                session.connection_address.reset();
            }
            session.gui_cond_var.wakeAll();
        }
    }
};

int gui_main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ConnectionWindow window;
    window.show();

    return app.exec();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Start the worker thread
    WorkerThread worker;
    worker.start();

    ConnectionWindow window;
    window.show();

    int result = app.exec();

    // Wait for the worker thread to finish before exiting
    worker.quit();
    worker.wait();

    return result;
}
