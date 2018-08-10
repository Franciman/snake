#include <QApplication>

#include <ui_controllers/main_window.h>

#include <clocale>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    std::setlocale(LC_NUMERIC, "C");

    MainWindow win;
    win.show();
    return app.exec();
}
