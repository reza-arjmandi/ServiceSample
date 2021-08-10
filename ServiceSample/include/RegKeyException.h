#pragma once

#include <exception>
#include <string>

using namespace std;

class RegKeyException : public exception
{

public:

    RegKeyException(string msg)
        : _msg{ msg }
    {
    }

    const char* what() const noexcept final
    {
        return _msg.c_str();
    }

private:

    string _msg;

};