#pragma once
#include <windows.h>

#include <string>
#include <exception> 

#include "RegKeyException.h"

using namespace std;

class RegKey
{

public:

    using NativeKeyType = HKEY;

    RegKey(wstring main_key, wstring sub_key)
    {
        _reg_key = open_reg_key(str_2_main_key(main_key), sub_key);
    }

    ~RegKey()
    {
        close_reg_key(_reg_key);
    }

    NativeKeyType native_handler()
    {
        return _reg_key;
    }

private:

    void close_reg_key(NativeKeyType key)
    {
        // Close the key.
        auto err_code{ RegCloseKey(key) };
        if (err_code != ERROR_SUCCESS)
        {
            throw RegKeyException{
                "Error in RegCloseKey."
            };
        }
    }

    NativeKeyType str_2_main_key(wstring main_key_str)
    {
        // Convert parameters to appropriate handles.
        if (main_key_str == wstring{ L"HKLM" }) return HKEY_LOCAL_MACHINE;
        else if (main_key_str == wstring{ L"HKU" }) return HKEY_USERS;
        else if (main_key_str == wstring{ L"HKCU" }) return HKEY_CURRENT_USER;
        else if (main_key_str == wstring{ L"HKCR" }) return HKEY_CLASSES_ROOT;
        else if (main_key_str == wstring{ L"HCC" }) return HKEY_CURRENT_CONFIG;
        else
        {
            throw RegKeyException{
                "main key should be one of "
                "[HKLM | HKU | HKCU | HKCR | HCC]"
            };
        }
    }

    NativeKeyType open_reg_key(NativeKeyType main_key, wstring sub_key)
    {
        NativeKeyType reg_key;
        LONG err_code;
        // Convert parameters to appropriate handles.

        err_code = RegOpenKeyEx(
            main_key, sub_key.data(), 0, KEY_NOTIFY, &reg_key);
        if (err_code != ERROR_SUCCESS)
        {
            throw RegKeyException{
                "Error in RegOpenKeyEx."
            };
        }
        return reg_key;
    }

    NativeKeyType _reg_key;

};