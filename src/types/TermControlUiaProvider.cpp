// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "precomp.h"
#include "TermControlUiaProvider.hpp"

using namespace Microsoft::Terminal;
using namespace Microsoft::Console::Types;
using namespace Microsoft::WRL;

HRESULT TermControlUiaProvider::RuntimeClassInitialize(_In_ ::Microsoft::Console::Types::IUiaData* const uiaData,
                                                       _In_ ::Microsoft::Console::Types::IControlInfo* controlInfo)
{
    RETURN_HR_IF_NULL(E_INVALIDARG, uiaData);
    RETURN_IF_FAILED(ScreenInfoUiaProviderBase::RuntimeClassInitialize(uiaData));

    _controlInfo = controlInfo;

    // TODO GitHub #1914: Re-attach Tracing to UIA Tree
    //Tracing::s_TraceUia(nullptr, ApiCall::Constructor, nullptr);
    return S_OK;
}

IFACEMETHODIMP TermControlUiaProvider::Navigate(_In_ NavigateDirection direction,
                                                _COM_Outptr_result_maybenull_ IRawElementProviderFragment** ppProvider)
{
    // TODO GitHub #1914: Re-attach Tracing to UIA Tree
    /*ApiMsgNavigate apiMsg;
    apiMsg.Direction = direction;
    Tracing::s_TraceUia(this, ApiCall::Navigate, &apiMsg);*/
    *ppProvider = nullptr;

    if (direction == NavigateDirection_Parent)
    {
        try
        {
            // TODO GitHub #2102: UIA Tree Navigation
            //_pUiaParent->QueryInterface(IID_PPV_ARGS(ppProvider));
        }
        catch (...)
        {
            *ppProvider = nullptr;
            return wil::ResultFromCaughtException();
        }
        RETURN_IF_NULL_ALLOC(*ppProvider);
    }

    // For the other directions the default of nullptr is correct
    return S_OK;
}

IFACEMETHODIMP TermControlUiaProvider::get_BoundingRectangle(_Out_ UiaRect* pRect)
{
    // TODO GitHub #1914: Re-attach Tracing to UIA Tree
    //Tracing::s_TraceUia(this, ApiCall::GetBoundingRectangle, nullptr);

    RECT rc = _controlInfo->GetBounds();

    pRect->left = rc.left;
    pRect->top = rc.top;
    pRect->width = rc.right - rc.left;
    pRect->height = rc.bottom - rc.top;

    return S_OK;
}

IFACEMETHODIMP TermControlUiaProvider::get_HostRawElementProvider(_COM_Outptr_result_maybenull_ IRawElementProviderSimple** ppProvider) noexcept
{
    try
    {
        return _controlInfo->GetHostUiaProvider(ppProvider);
    }
    CATCH_RETURN();
}

IFACEMETHODIMP TermControlUiaProvider::get_FragmentRoot(_COM_Outptr_result_maybenull_ IRawElementProviderFragmentRoot** ppProvider)
{
    // TODO GitHub #1914: Re-attach Tracing to UIA Tree
    //Tracing::s_TraceUia(this, ApiCall::GetFragmentRoot, nullptr);
    try
    {
        // TODO GitHub #2102: UIA Tree Navigation - the special fragments that knows about all of its descendants is called a fragment root
        //_pUiaParent->QueryInterface(IID_PPV_ARGS(ppProvider));
        *ppProvider = nullptr;
    }
    catch (...)
    {
        *ppProvider = nullptr;
        return wil::ResultFromCaughtException();
    }
    RETURN_IF_NULL_ALLOC(*ppProvider);
    return S_OK;
}

const COORD TermControlUiaProvider::GetFontSize() const
{
    return _controlInfo->GetFontSize();
}

const RECT TermControlUiaProvider::GetPadding() const
{
    return _controlInfo->GetPadding();
}

const double TermControlUiaProvider::GetScaleFactor() const
{
    return _controlInfo->GetScaleFactor();
}

void TermControlUiaProvider::ChangeViewport(const SMALL_RECT NewWindow)
{
    _controlInfo->ChangeViewport(NewWindow);
}

HRESULT TermControlUiaProvider::GetSelectionRanges(_In_ IRawElementProviderSimple* pProvider, const std::wstring_view wordDelimiters, _Out_ std::deque<ComPtr<UiaTextRangeBase>>& result)
{
    try
    {
        result.clear();
        typename std::remove_reference<decltype(result)>::type temporaryResult;

        std::deque<ComPtr<TermControlUiaTextRange>> ranges;
        RETURN_IF_FAILED(TermControlUiaTextRange::GetSelectionRanges(_pData, pProvider, wordDelimiters, ranges));

        while (!ranges.empty())
        {
            temporaryResult.emplace_back(std::move(ranges.back()));
            ranges.pop_back();
        }

        std::swap(result, temporaryResult);
        return S_OK;
    }
    CATCH_RETURN();
}

HRESULT TermControlUiaProvider::CreateTextRange(_In_ IRawElementProviderSimple* const pProvider, const std::wstring_view wordDelimiters, _COM_Outptr_result_maybenull_ UiaTextRangeBase** ppUtr)
{
    RETURN_HR_IF_NULL(E_INVALIDARG, ppUtr);
    *ppUtr = nullptr;
    TermControlUiaTextRange* result = nullptr;
    RETURN_IF_FAILED(MakeAndInitialize<TermControlUiaTextRange>(&result, _pData, pProvider, wordDelimiters));
    *ppUtr = result;
    return S_OK;
}

HRESULT TermControlUiaProvider::CreateTextRange(_In_ IRawElementProviderSimple* const pProvider,
                                                const Cursor& cursor,
                                                const std::wstring_view wordDelimiters,
                                                _COM_Outptr_result_maybenull_ UiaTextRangeBase** ppUtr)
{
    RETURN_HR_IF_NULL(E_INVALIDARG, ppUtr);
    *ppUtr = nullptr;
    TermControlUiaTextRange* result = nullptr;
    RETURN_IF_FAILED(MakeAndInitialize<TermControlUiaTextRange>(&result, _pData, pProvider, cursor, wordDelimiters));
    *ppUtr = result;
    return S_OK;
}

HRESULT TermControlUiaProvider::CreateTextRange(_In_ IRawElementProviderSimple* const pProvider,
                                                const COORD start,
                                                const COORD end,
                                                const std::wstring_view wordDelimiters,
                                                _COM_Outptr_result_maybenull_ UiaTextRangeBase** ppUtr)
{
    RETURN_HR_IF_NULL(E_INVALIDARG, ppUtr);
    *ppUtr = nullptr;
    TermControlUiaTextRange* result = nullptr;
    RETURN_IF_FAILED(MakeAndInitialize<TermControlUiaTextRange>(&result, _pData, pProvider, start, end, wordDelimiters));
    *ppUtr = result;
    return S_OK;
}

HRESULT TermControlUiaProvider::CreateTextRange(_In_ IRawElementProviderSimple* const pProvider,
                                                const UiaPoint point,
                                                const std::wstring_view wordDelimiters,
                                                _COM_Outptr_result_maybenull_ UiaTextRangeBase** ppUtr)
{
    RETURN_HR_IF_NULL(E_INVALIDARG, ppUtr);
    *ppUtr = nullptr;
    TermControlUiaTextRange* result = nullptr;
    RETURN_IF_FAILED(MakeAndInitialize<TermControlUiaTextRange>(&result, _pData, pProvider, point, wordDelimiters));
    *ppUtr = result;
    return S_OK;
}