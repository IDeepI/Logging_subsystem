
#pragma region Includes
#include "stdafx.h"

#include "WinDump.h"
#include "LogManager.h"
#pragma endregion
//This entry and dump is for testing and presentation only
int wmain(int argc, wchar_t* argv[])
{
    // Create a dump file whenever the app crashes, only on windows
    SetUnhandledExceptionFilter(unhandled_handler);

    //Test
#define LOAD_TEST_SIZE 10000
    using namespace std::literals;
    auto path1{"/core/execution/orders"sv};
    auto path2{"/core/group/users"sv};
    auto newRoot{"c:/tmp"sv};
    auto msg1{"<message1>"sv};
    auto msg2{"<message2>"sv};
    struct test {};
    getLogManager().log(path1.data(), msg1, msg1, msg1, msg1, msg1, msg1, msg1, msg1, msg1, msg1);

    for (size_t i = 0; i < LOAD_TEST_SIZE; i++) {
        getLogManager().log(path1.data(), i, msg1, msg1);
        getLogManager().log(path2.data(), i, msg2, msg1);
        getLogManager().log(path2.data(), i, msg2, msg2);
        getLogManager().log(path2.data(), i, msg2);
    }
     
    getLogManager().setRoot(newRoot);
  
    return 0;
}

