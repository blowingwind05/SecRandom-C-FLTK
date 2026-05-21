#include "../include/AppController.h"

int main(int argc, char **argv) {
    AppController app;
    app.initialize();
    return app.run(argc, argv);
}
