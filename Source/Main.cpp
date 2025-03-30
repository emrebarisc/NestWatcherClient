#include "Managers/NetworkManager.h"

int main()
{
    NetworkManager networkManager;
    networkManager.Init();
    networkManager.Start();

    return 0;
}