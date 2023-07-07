
#pragma region Includes
#include "stdafx.h"

#include "WinDump.h"
#include "LogManager.h"
#pragma endregion

int wmain(int argc, wchar_t* argv[])
{
    // Create a dump file whenever the gateway crashes only on windows
    SetUnhandledExceptionFilter(unhandled_handler);

    //Test
#ifdef _DEBUG
//     STARTUPINFO si = { sizeof(si) }; PROCESS_INFORMATION pi = {};
//     std::wstring path = L"\"C:\\Program Files\\ConEmu\\ConEmu\\ConEmuC.exe\" /AUTOATTACH";
//     if (CreateProcess(NULL,
//         &path[0],
//         NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
//     {
//         CloseHandle(pi.hProcess); CloseHandle(pi.hThread);
//     }


    //lg::LogManager::instance():: ?? Нужен ли мне singleton?

    using namespace std::literals;
    auto path{"/core/execution/orders"sv};
    auto msg1{"<message1>"sv};
    auto msg2{"<message2>"sv};
    auto msg3{"<message3>"sv};
    lg::LogManager::Log(path, msg1);
    lg::LogManager::Log(path, msg1, msg2);

  
    return 0;

#endif // DEBUG
}

