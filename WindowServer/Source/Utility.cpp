#include "MainPCH.h"
#include "Header/Utility.h"



std::string Utility::ToUTF8(const std::string& strAnsi)
{
    if (strAnsi.empty()) return "";

    int nLen = MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), -1, NULL, 0);
    if (nLen <= 0) return "";

    std::wstring wstr(nLen, 0);
    MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), -1, wstr.data(), nLen);

    int nLenUTF8 = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (nLenUTF8 <= 0) return "";

    std::string strUtf8(nLenUTF8, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, strUtf8.data(), nLenUTF8, NULL, NULL);

    if (!strUtf8.empty() && strUtf8.back() == '\0') strUtf8.pop_back();

    return strUtf8;
}


std::string Utility::ToANSI(const std::string& strUtf8)
{
    if (strUtf8.empty()) return "";

    int nLen = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, NULL, 0);
    if (nLen <= 0) return "";

    std::wstring wstr(nLen, 0);
    MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, wstr.data(), nLen);

    int nLenAnsi = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (nLenAnsi <= 0) return "";

    std::string strAnsi(nLenAnsi, 0);
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, strAnsi.data(), nLenAnsi, NULL, NULL);

    if (!strAnsi.empty() && strAnsi.back() == '\0') strAnsi.pop_back();

    return strAnsi;
}