#pragma once

#include <filesystem>
#include <fstream>
#include <vector>
#include <sstream>

#include "UserInformation.h"

using namespace std;

class UserInformationParser
{

public:

    UserInformationParser(const filesystem::path& file_path)
    {
        parse(file_path);
    }

    vector<UserInformation> get_user_informations()
    {
        return _user_informations;
    }

private:

    void parse(const filesystem::path& file_path)
    {
        ifstream file{ file_path };
        string line;
        while (getline(file, line))
        {
            istringstream sstr{ line };
            UserInformation info;
            sstr >> info.user_name >> info.password >> info.domain;
            _user_informations.push_back(info);
        }
    }

    vector<UserInformation> _user_informations;

};