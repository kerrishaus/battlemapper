#include "AppEngine.hpp"
#include "MapDesignerState.hpp"

int main(int argc, char* argv[])
{
    AppSettings settings;
    settings.launchOptions = { argc, argv };

    AppEngine app;
    app.Init(settings);

    app.PushState(new MapDesignerState);

    while (app.isRunning())
    {
        app.HandleEvents();
        app.Update();
        app.Draw();
    }

    return 0;
}