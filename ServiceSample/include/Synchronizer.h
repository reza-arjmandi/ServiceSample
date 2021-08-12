#pragma once

#include <windows.h>

#include "SynchronizerException.h"

class Synchronizer
{

public:

    using NativeHandlerType = HANDLE;

    Synchronizer()
    {
        _wait_handler = create_wait_handler();
    }

    ~Synchronizer()
    {
        close_wait_handler(_wait_handler);
    }

    NativeHandlerType native_handler()
    {
        return _wait_handler;
    }

    void wait()
    {
        while(!_is_stopped) 
        {
            DWORD milli{ 500 };
            auto res{ WaitForSingleObject(_wait_handler, milli) };
            if (res == WAIT_FAILED)
            {
                throw SynchronizerException{
                    "Error in WaitForSingleObject."
                };
                return;
            }
            if (res == WAIT_OBJECT_0)
            {
                break;
            }
        }
        
    }

    void release_wait()
    {
        _is_stopped = true;
    }

private:

    NativeHandlerType create_wait_handler()
    {
        // Create an event.
        auto event = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (event == NULL)
        {
            throw SynchronizerException{
                "Error in CreateEvent."
            };
        }
        return event;
    }

    void close_wait_handler(NativeHandlerType wait_handler)
    {
        if (!CloseHandle(wait_handler))
        {
            throw SynchronizerException{
                "Error in CloseHandle."
            };
            return;
        }
    }

    bool _is_stopped{ false };
    NativeHandlerType _wait_handler;

};