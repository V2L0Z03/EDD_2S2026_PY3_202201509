#include <QApplication>
#include "../include/ui/LoginWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Forzamos un estilo nativo fusion que queda mejor con dark mode
    QApplication::setStyle("Fusion");
    
    LoginWindow login;
    login.show();
    
    return app.exec();
}