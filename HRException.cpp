#include "pch.h"

#include "HRException.h"

namespace Ice2D
{
    HRException::HRException(HRESULT hr) : std::runtime_error("HRESULT Exception"), hr(hr)
    {
        message = HRESULTToString(hr);
    }

    HRESULT HRException::GetHRESULT() const
    {
        return hr;
    }

    const std::wstring& HRException::GetErrorMessage() const
    {
        return message;
    }

    void HRException::ErrorBox(const std::wstring& msg, const std::wstring& title)
    {
        MessageBox(NULL, msg.c_str(), title.c_str(), MB_ICONERROR | MB_OK);
    }

    void HRException::ErrorBox(const std::exception& e)
    {
        ErrorBox(std::wstring(e.what(), e.what() + strlen(e.what())), L"Exception Thrown");
    }

    void HRException::ErrorBox(const HRException& e)
    {
        ErrorBox(e.GetErrorMessage(), L"HR Exception Thrown");
    }

    std::wstring HRException::HRESULTToString(HRESULT hr)
    {
        wchar_t* errorMsg = nullptr;

        DWORD dwFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
        FormatMessage(dwFlags, nullptr, hr, 0, (wchar_t*)&errorMsg, 0, nullptr);

        std::wstring strMsg = errorMsg ? errorMsg : L"Unknown error";

        if (errorMsg)
        {
            LocalFree(errorMsg);
        }

        return strMsg;
    }
}