#pragma once

#include <atlsync.h>
#include "DirectVobSub.h"
#include "SubRenderIntf.h"
#include "SubRenderOptionsImpl.h"

class CDirectVobSubFilter;

// Expose this interface if you do NOT want to work with XySubFilter in a graph
interface __declspec(uuid("8871ac83-89c8-44e9-913f-a9a2a322440a")) IXySubFilterGraphMutex : public IUnknown
{

};

class __declspec(uuid("2dfcb782-ec20-4a7c-b530-4577adb33f21")) XySubFilter
    : public CBaseFilter
    , public CDVS4XySubFilter
    , public ISpecifyPropertyPages
    , public IAMStreamSelect
    , public CAMThread
    , public SubRenderOptionsImpl
    , public ISubRenderProvider
    , public IXySubFilterGraphMutex
{
public:
    friend class SubtitleInputPin2;

    XySubFilter(LPUNKNOWN punk, HRESULT* phr, const GUID& clsid = __uuidof(XySubFilter));
    virtual ~XySubFilter();

    DECLARE_IUNKNOWN
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    DECLARE_ISUBRENDEROPTIONS

    //CBaseFilter
    [[nodiscard]] CBasePin* GetPin(int n);
    [[nodiscard]] int GetPinCount();
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP JoinFilterGraph(IFilterGraph* pGraph, LPCWSTR pName);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP QueryFilterInfo(FILTER_INFO* pInfo);

    DECLSPEC_NOTHROW STDMETHODIMP Pause();

    // XyOptionsImpl
    [[nodiscard]] virtual HRESULT OnOptionChanged(unsigned field);
    [[nodiscard]] virtual HRESULT DoGetField(unsigned field, void *value);

    // IXyOptions
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XyGetString   (unsigned field, LPWSTR   *value, int *chars);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP XySetBool     (unsigned field, bool      value);

    // DirectVobSubImpl
    [[nodiscard]] HRESULT GetCurStyles(SubStyle sub_style[], const int count);
    [[nodiscard]] HRESULT SetCurStyles(const SubStyle sub_style[], const int count);

    // IDirectVobSub
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP get_LanguageName(int iLanguage, WCHAR** ppName);

    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP get_CachesInfo(CachesInfo* caches_info);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP get_XyFlyWeightInfo(XyFlyWeightInfo* xy_fw_info);

    // IDirectVobSub2
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP put_TextSettings(STSStyle* pDefStyle);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP put_AspectRatioSettings(CSimpleTextSubtitle::EPARCompensationType* ePARCompensationType);

    // ISpecifyPropertyPages
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP GetPages(CAUUID* pPages);

    // IAMStreamSelect
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Count(DWORD* pcStreams); 
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Enable(long lIndex, DWORD dwFlags); 
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Info(long lIndex, AM_MEDIA_TYPE** ppmt, DWORD* pdwFlags, LCID* plcid, DWORD* pdwGroup, WCHAR** ppszName, IUnknown** ppObject, IUnknown** ppUnk);

    //ISubRenderProvider
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP RequestFrame(REFERENCE_TIME start, REFERENCE_TIME stop, LPVOID context);
    [[nodiscard]] DECLSPEC_NOTHROW STDMETHODIMP Disconnect(void);
private:
    void SetYuvMatrix();
    void SetRgbOutputLevel();
    [[nodiscard]] bool Open();

    void UpdateSubtitle(bool fApplyDefStyle = true);
    void SetSubtitle(ISubStream* pSubStream, bool fApplyDefStyle = true);
    void InvalidateSubtitle(REFERENCE_TIME rtInvalidate = -1, DWORD_PTR nSubtitleId = -1);

    [[nodiscard]] HRESULT UpdateParamFromConsumer(bool getNameAndVersion=false);

    [[nodiscard]] int FindPreferedLanguage(bool fHideToo = true);
    void UpdatePreferedLanguages(CString l);

    // the text input pin is using these
    void AddSubStream(ISubStream* pSubStream);
    void RemoveSubStream(ISubStream* pSubStream);
    [[nodiscard]] HRESULT CheckInputType(const CMediaType* pmt);

    [[nodiscard]] HRESULT GetIsEmbeddedSubStream(int iSelected, bool *fIsEmbedded);

    [[nodiscard]] bool LoadExternalSubtitle(IFilterGraph* pGraph);

    HRESULT GetRidOfVSFilter();
    HRESULT StartStreaming();

    [[nodiscard]] HRESULT FindAndConnectConsumer(IFilterGraph* pGraph);

    void UpdateLanguageCount();

    CStringW DumpProviderInfo();
    CStringW DumpConsumerInfo();
private:
    class CFileReloaderData
    {
    public:
        ATL::CEvent ThreadStartedEvent, EndThreadEvent, RefreshEvent;
        CAtlList<CString> files;
        CAtlArray<CTime> mtime;
    } m_frd;

    void SetupFRD(CStringArray& paths, CAtlArray<HANDLE>& handles);
    DWORD ThreadProc();
private:
    ColorConvTable::YuvMatrixType m_video_yuv_matrix_decided_by_sub;
    ColorConvTable::YuvRangeType m_video_yuv_range_decided_by_sub;

    ISubStream *m_curSubStream;
    CInterfaceList<ISubStream> m_pSubStreams;
    CAtlList<bool> m_fIsSubStreamEmbeded;

    // critical section protecting all
    CCritSec m_csFilter;

    CCritSec m_csProviderFields;// critical section protecting fields to be read by consumer
                                // it is needed because otherwise consumer has to hold m_csSubLock which may be holded 
                                // by renderring thread for a long time

    CCritSec m_csConsumer;      // critical section protecting m_consumer

    CComPtr<IXySubRenderProvider> m_sub_provider;

    CAtlArray<SubtitleInputPin2*> m_pSubtitleInputPin;

    // don't set the "hide subtitles" stream until we are finished with loading
    bool m_fLoading;

    bool m_not_first_pause;

    SystrayIconData m_tbid;
    HANDLE m_hSystrayThread;

    CComPtr<ISubRenderConsumer> m_consumer;
    ULONGLONG m_context_id;

    REFERENCE_TIME m_last_requested;

    bool m_workaround_mpc_hc;//enable workaround for MPC-HC to prevent be removed from the graph

    bool m_disconnect_entered;

    CStringW m_filter_info_string;

#ifdef SUBTITLE_FRAME_DUMP_FILE
    int   m_dump_subtitle_frame_start_rt;
    int   m_dump_subtitle_frame_end_rt;
    bool  m_dump_subtitle_frame_to_txt;
    bool  m_dump_subtitle_frame_alpha_channel;
    bool  m_dump_subtitle_frame_rgb_bmp;
    DWORD m_dump_subtitle_frame_background_color;
#endif
};
