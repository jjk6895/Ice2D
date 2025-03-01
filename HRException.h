#pragma once

#include "MinWin.h"
#include <stdexcept>

#define CheckHR(hr) if (FAILED(hr)) throw Ice2D::HRException(hr);

namespace Ice2D
{
    class HRException : public std::runtime_error
    {
    public:
        HRException(HRESULT hr);
        HRESULT GetHRESULT() const;
        const std::wstring& GetErrorMessage() const;
        static void ErrorBox(const std::wstring& msg, const std::wstring& title = L"Error");
        static void ErrorBox(const std::exception& e);
        static void ErrorBox(const HRException& e);
    private:
        HRESULT hr;
        std::wstring message;
        static std::wstring HRESULTToString(HRESULT hr);
    };
}

