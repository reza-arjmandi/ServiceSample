#pragma once

#include <string>
#include <memory>

using namespace std;

#include "RegKey.h"
#include "Synchronizer.h"
#include "RegKeyWatcherException.h"

template<typename IOContextType>
class RegKeyWatcher
{

public:

    RegKeyWatcher(
        IOContextType& io_ctx, 
        shared_ptr<RegKey> reg_key)
        : _io_ctx{ io_ctx },
        _reg_key{ reg_key }
    {
    }

    void watch_change()
    {
        _sync = make_unique<Synchronizer>();
        _reg_key->open();
        notify_if_reg_key_is_changed(
            _reg_key->native_handler(), 
            _sync->native_handler());
        _sync->wait();
        _reg_key->close();
    }
    
    template<typename HandlerType>
    void async_watch_change(HandlerType&& handler)
    {
        _io_ctx.send([&, 
            _handler{ forward<HandlerType>(handler) }]() {
            watch_change();
            _handler();
        });
    }

    void release_wait()
    {
        if (_sync)
        {
            _sync->release_wait();
        }
    }

private:

    void notify_if_reg_key_is_changed(HKEY reg_key, HANDLE notify_handler)
    {
        DWORD filter = REG_NOTIFY_CHANGE_NAME |
            REG_NOTIFY_CHANGE_ATTRIBUTES |
            REG_NOTIFY_CHANGE_LAST_SET |
            REG_NOTIFY_CHANGE_SECURITY;
        LONG err_code;
        // Watch the registry key for a change of value.
        err_code = RegNotifyChangeKeyValue(reg_key,
            TRUE,
            filter,
            notify_handler,
            TRUE);
        if (err_code != ERROR_SUCCESS)
        {
            throw RegKeyWatcherException{
                "Error in RegNotifyChangeKeyValue."
            };
            return;
        }
    }

    unique_ptr<Synchronizer> _sync{ nullptr };

    shared_ptr<RegKey> _reg_key;
    IOContextType& _io_ctx;

};