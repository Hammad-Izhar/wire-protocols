#include <QApplication>
#include <QStackedLayout>

#include "client/model/session.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    Session::get_instance();
    return app.exec();
}
