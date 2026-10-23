#include "Shared/src/Runtime/ConfigDirectory.h"
#include "Cached.h"
#include "Shared/src/MemorySub.h"

int main(int argc, char** argv) {
    if (!runtime::InitializeDirectoryArguments(argc, argv)) return 2;

    // Cached.exe completes 31 ordered class-id initializers before WinMain.
    // Portable registration has already populated the actual class count.
    // g_nClassCount = 31
    return CacheDMain(argc, argv);
}
