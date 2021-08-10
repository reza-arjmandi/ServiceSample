#pragma once

#include <exception>
#include <string>

using namespace std;

class SynchronizerException : public exception
{

public:

    SynchronizerException(string msg)
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