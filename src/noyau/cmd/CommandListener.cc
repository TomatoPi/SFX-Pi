/**********************************************************************
 * @file Listener.h
 * @author TomatoPi
 * @version 1.2
 *
 * Command Listener
 * Create a new thread and wait for console inputs
 **********************************************************************/
#include "CommandListener.h"

#define NAME ("CommandListerner")

CommandListener::CommandListener() : _buffer(), _thread()
{
    sfx::log(NAME, "Start Listener ... ");
    _thread = std::thread(CommandListener::threadRun, this);
}

CommandListener::~CommandListener()
{
    sfx::log(NAME, "Wait For Command Thread to Stop ... ");
    // Wait thread to Stop before destroy Listener
    _thread.join();
    sfx::sfxStream << "Closed\n";
}

std::string CommandListener::getBuffer()
{
    return _buffer;
}

void CommandListener::clearBuffer()
{
    _buffer.clear();
    _buffer = "";
}

int CommandListener::threadRun(CommandListener* listener)
{
    return listener->run();
}
int CommandListener::run()
{
    sfx::sfxStream << "Started\n";
    while (!sfx::Global_NeedToExit)
    {
        // Wait for input
        std::string buffer = "";
        std::getline(std::cin, buffer);

        // If received exit command tell Programm to start exit procedure
        if ( buffer == "exit" )
        {
            sfx::log(NAME, "Terminate command recived\n");
            sfx::Global_NeedToExit = true;
        }
        
        // Assign m_buffer value after getline to ensure the entire line
        // to be copied
        _buffer = buffer;
    }
    return 0;
}
