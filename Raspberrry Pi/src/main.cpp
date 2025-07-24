// main.cpp
#include "app/app.h"

int main() {
    App app;
    app.init();
    app.run();
    app.cleanup();
    return 0;
}