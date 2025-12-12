#pragma once

class Utility
{

public:
    static std::string ToUTF8(const std::string& strAnsi);
    static std::string ToANSI(const std::string& strUtf8);
};