/************************************************************************/
/* author: xy                                                           */
/* date: 20110918                                                       */
/************************************************************************/
#ifndef __MRU_CACHE_H_256FCF72_8663_41DC_B98A_B822F6007912__
#define __MRU_CACHE_H_256FCF72_8663_41DC_B98A_B822F6007912__

#include <atlcoll.h>
#include <utility>

////////////////////////////////////////////////////////////////////////////////////////////////////

// XyAtlMap: a moded CAtlMap

template <typename K, typename V, class KTraits = CElementTraits<K>, class VTraits = CElementTraits<V>>
class XyAtlMap
{
public:
    typedef typename KTraits::INARGTYPE KINARGTYPE;
    typedef typename KTraits::OUTARGTYPE KOUTARGTYPE;
    typedef typename VTraits::INARGTYPE VINARGTYPE;
    typedef typename VTraits::OUTARGTYPE VOUTARGTYPE;

    class CPair : public __POSITION
    {
    protected:
        explicit CPair(/* _In_ */ KINARGTYPE key) : m_key(key)
        {
        }

    public:
        const K m_key;
        V m_value;
    };

private:
    class CNode : public CPair
    {
    public:
        CNode(
            /* _In_ */ KINARGTYPE key,
            _In_ UINT nHash) : CPair(key),
                               m_nHash(nHash)
        {
        }

    public:
        UINT GetHash() const throw()
        {
            return (m_nHash);
        }

    public:
        CNode* m_pNext;
        UINT m_nHash;
    };

public:
    explicit XyAtlMap(
        _In_ UINT nBins = 17,
        _In_ float fOptimalLoad = 0.75f,
        _In_ float fLoThreshold = 0.25f,
        _In_ float fHiThreshold = 2.25f,
        _In_ UINT nBlockSize = 10) throw();

    [[nodiscard]] std::size_t GetCount() const throw();
    [[nodiscard]] bool IsEmpty() const throw();

    [[nodiscard]] bool Lookup(
        /* _In_ */ KINARGTYPE key,
        _Out_ VOUTARGTYPE value) const;
    [[nodiscard]] const CPair* Lookup(/* _In_ */ KINARGTYPE key) const throw();
    [[nodiscard]] CPair* Lookup(/* _In_ */ KINARGTYPE key) throw();
    V& operator[](/* _In_ */ KINARGTYPE key) throw(...);

    [[nodiscard]] POSITION SetAtIfNotExists(
        /* _In_ */ KINARGTYPE key,
        /* _In_ */ VINARGTYPE value,
        bool* new_item_added);

    [[nodiscard]] POSITION SetAt(
        /* _In_ */ KINARGTYPE key,
        /* _In_ */ VINARGTYPE value);
    void SetValueAt(
        _In_ POSITION pos,
        /* _In_ */ VINARGTYPE value);

    [[nodiscard]] bool RemoveKey(/* _In_ */ KINARGTYPE key) throw();
    void RemoveAll();
    void RemoveAtPos(_In_ POSITION pos) throw();

    [[nodiscard]] POSITION GetStartPosition() const throw();
    void GetNextAssoc(
        _Inout_ POSITION& pos,
        _Out_ KOUTARGTYPE key,
        _Out_ VOUTARGTYPE value) const;
    [[nodiscard]] const CPair* GetNext(_Inout_ POSITION& pos) const throw();
    [[nodiscard]] CPair* GetNext(_Inout_ POSITION& pos) throw();
    [[nodiscard]] const K& GetNextKey(_Inout_ POSITION& pos) const;
    [[nodiscard]] const V& GetNextValue(_Inout_ POSITION& pos) const;
    [[nodiscard]] V& GetNextValue(_Inout_ POSITION& pos);
    void GetAt(
        _In_ POSITION pos,
        _Out_ KOUTARGTYPE key,
        _Out_ VOUTARGTYPE value) const;
    [[nodiscard]] CPair* GetAt(_In_ POSITION pos) throw();
    [[nodiscard]] const CPair* GetAt(_In_ POSITION pos) const throw();
    [[nodiscard]] const K& GetKeyAt(_In_ POSITION pos) const;
    [[nodiscard]] const V& GetValueAt(_In_ POSITION pos) const;
    [[nodiscard]] V& GetValueAt(_In_ POSITION pos);

    [[nodiscard]] UINT GetHashTableSize() const throw();
    [[nodiscard]] bool InitHashTable(
        _In_ UINT nBins,
        _In_ bool bAllocNow = true);
    void EnableAutoRehash() throw();
    void DisableAutoRehash() throw();
    void Rehash(_In_ UINT nBins = 0);
    void SetOptimalLoad(
        _In_ float fOptimalLoad,
        _In_ float fLoThreshold,
        _In_ float fHiThreshold,
        _In_ bool bRehashNow = false);

#ifdef _DEBUG
    void AssertValid() const;
#endif // _DEBUG

    // Implementation
private:
    CNode** m_ppBins;
    std::size_t m_nElements;
    UINT m_nBins;
    float m_fOptimalLoad;
    float m_fLoThreshold;
    float m_fHiThreshold;
    std::size_t m_nHiRehashThreshold;
    std::size_t m_nLoRehashThreshold;
    ULONG m_nLockCount;
    UINT m_nBlockSize;
    CAtlPlex* m_pBlocks;
    CNode* m_pFree;

private:
    bool IsLocked() const throw();
    [[nodiscard]] UINT PickSize(_In_ std::size_t nElements) const throw();
    [[nodiscard]] CNode* NewNode(
        /* _In_ */ KINARGTYPE key,
        _In_ UINT iBin,
        _In_ UINT nHash);
    void FreeNode(_Inout_ CNode* pNode);
    void FreePlexes() throw();
    [[nodiscard]] CNode* GetNode(
        /* _In_ */ KINARGTYPE key,
        _Out_ UINT& iBin,
        _Out_ UINT& nHash,
        _Deref_out_opt_ CNode*& pPrev) const throw();
    [[nodiscard]] CNode* CreateNode(
        /* _In_ */ KINARGTYPE key,
        _In_ UINT iBin,
        _In_ UINT nHash) throw(...);
    void RemoveNode(
        _In_ CNode* pNode,
        _In_opt_ CNode* pPrev) throw();
    [[nodiscard]] CNode* FindNextNode(_In_ CNode* pNode) const throw();
    void UpdateRehashThresholds() throw();

public:
    ~XyAtlMap() throw();

private:
    // Private to prevent use
    explicit XyAtlMap(_In_ const XyAtlMap&) throw();
    XyAtlMap& operator=(_In_ const XyAtlMap&) throw();
};


template <typename K, typename V, class KTraits, class VTraits>
inline std::size_t XyAtlMap<K, V, KTraits, VTraits>::GetCount() const throw()
{
    return (m_nElements);
}

template <typename K, typename V, class KTraits, class VTraits>
inline bool XyAtlMap<K, V, KTraits, VTraits>::IsEmpty() const throw()
{
    return (m_nElements == 0);
}

template <typename K, typename V, class KTraits, class VTraits>
inline V& XyAtlMap<K, V, KTraits, VTraits>::operator[](/* _In_ */ KINARGTYPE key) throw(...)
{
    CNode* pNode;
    UINT iBin;
    UINT nHash;
    CNode* pPrev;

    pNode = GetNode(key, iBin, nHash, pPrev);
    if (pNode == nullptr)
    {
        pNode = CreateNode(key, iBin, nHash);
    }

    return (pNode->m_value);
}

template <typename K, typename V, class KTraits, class VTraits>
inline UINT XyAtlMap<K, V, KTraits, VTraits>::GetHashTableSize() const throw()
{
    return (m_nBins);
}

template <typename K, typename V, class KTraits, class VTraits>
inline void XyAtlMap<K, V, KTraits, VTraits>::GetAt(
    _In_ POSITION pos,
    _Out_ KOUTARGTYPE key,
    _Out_ VOUTARGTYPE value) const
{
    ATLENSURE(pos != nullptr);

    CNode* pNode{ static_cast<CNode*>(pos) };

    key = pNode->m_key;
    value = pNode->m_value;
}

template <typename K, typename V, class KTraits, class VTraits>
inline typename XyAtlMap<K, V, KTraits, VTraits>::CPair* XyAtlMap<K, V, KTraits, VTraits>::GetAt(
    _In_ POSITION pos) throw()
{
    ATLASSERT(pos != nullptr);

    return (static_cast<CPair*>(pos));
}

template <typename K, typename V, class KTraits, class VTraits>
inline const typename XyAtlMap<K, V, KTraits, VTraits>::CPair* XyAtlMap<K, V, KTraits, VTraits>::GetAt(
    _In_ POSITION pos) const throw()
{
    ATLASSERT(pos != nullptr);

    return (static_cast<const CPair*>(pos));
}

template <typename K, typename V, class KTraits, class VTraits>
inline const K& XyAtlMap<K, V, KTraits, VTraits>::GetKeyAt(_In_ POSITION pos) const
{
    ATLENSURE(pos != nullptr);

    CNode* pNode{ (CNode*)pos };

    return (pNode->m_key);
}

template <typename K, typename V, class KTraits, class VTraits>
inline const V& XyAtlMap<K, V, KTraits, VTraits>::GetValueAt(_In_ POSITION pos) const
{
    ATLENSURE(pos != nullptr);

    CNode* pNode{ (CNode*)pos };

    return (pNode->m_value);
}

template <typename K, typename V, class KTraits, class VTraits>
inline V& XyAtlMap<K, V, KTraits, VTraits>::GetValueAt(_In_ POSITION pos)
{
    ATLENSURE(pos != nullptr);

    CNode* pNode{ (CNode*)pos };

    return (pNode->m_value);
}

template <typename K, typename V, class KTraits, class VTraits>
inline void XyAtlMap<K, V, KTraits, VTraits>::DisableAutoRehash() throw()
{
    ++m_nLockCount;
}

template <typename K, typename V, class KTraits, class VTraits>
inline void XyAtlMap<K, V, KTraits, VTraits>::EnableAutoRehash() throw()
{
    ATLASSUME(m_nLockCount > 0);
    --m_nLockCount;
}

template <typename K, typename V, class KTraits, class VTraits>
inline bool XyAtlMap<K, V, KTraits, VTraits>::IsLocked() const throw()
{
    return (m_nLockCount != 0);
}

template <typename K, typename V, class KTraits, class VTraits>
UINT XyAtlMap<K, V, KTraits, VTraits>::PickSize(_In_ std::size_t nElements) const throw()
{
    // List of primes such that bin_primes[i] is the smallest prime greater than 2^(5+i/3)
    static const UINT bin_primes[] = {
        17, 23, 29, 37, 41, 53, 67, 83, 103, 131, 163, 211, 257, 331, 409, 521, 647, 821,
        1031, 1291, 1627, 2053, 2591, 3251, 4099, 5167, 6521, 8209, 10331,
        13007, 16411, 20663, 26017, 32771, 41299, 52021, 65537, 82571, 104033,
        131101, 165161, 208067, 262147, 330287, 416147, 524309, 660563,
        832291, 1048583, 1321139, 1664543, 2097169, 2642257, 3329023, 4194319,
        5284493, 6658049, 8388617, 10568993, 13316089, UINT_MAX
    };

    std::size_t nBins{ (std::size_t)(nElements / m_fOptimalLoad) };
    UINT nBinsEstimate = UINT(UINT_MAX < nBins ? UINT_MAX : nBins);

    // Find the smallest prime greater than our estimate
    int iPrime{ 0 };
    while (nBinsEstimate > bin_primes[iPrime])
    {
        ++iPrime;
    }

    if (bin_primes[iPrime] == UINT_MAX)
    {
        return (nBinsEstimate);
    }
    else
    {
        return (bin_primes[iPrime]);
    }
}

template <typename K, typename V, class KTraits, class VTraits>
typename XyAtlMap<K, V, KTraits, VTraits>::CNode* XyAtlMap<K, V, KTraits, VTraits>::CreateNode(
    /* _In_ */ KINARGTYPE key,
    _In_ UINT iBin,
    _In_ UINT nHash) throw(...)
{
    CNode* pNode;

    if (m_ppBins == nullptr)
    {
        bool bSuccess;

        bSuccess = InitHashTable(m_nBins);
        if (!bSuccess)
        {
            AtlThrow(E_OUTOFMEMORY);
        }
    }

    pNode = NewNode(key, iBin, nHash);

    return (pNode);
}

template <typename K, typename V, class KTraits, class VTraits>
POSITION XyAtlMap<K, V, KTraits, VTraits>::GetStartPosition() const throw()
{
    if (IsEmpty())
    {
        return (nullptr);
    }

    for (UINT iBin{ 0 }; iBin < m_nBins; ++iBin)
    {
        if (m_ppBins[iBin] != nullptr)
        {
            return (POSITION(m_ppBins[iBin]));
        }
    }
    ATLASSERT(false);

    return (nullptr);
}

template <typename K, typename V, class KTraits, class VTraits>
POSITION XyAtlMap<K, V, KTraits, VTraits>::SetAtIfNotExists(/* _In_ */ KINARGTYPE key, /* _In_ */ VINARGTYPE value,
                                                            bool* new_item_added)
{
    CNode* pNode;
    UINT iBin;
    UINT nHash;
    CNode* pPrev;

    if (new_item_added)
    {
        *new_item_added = false;
    }

    pNode = GetNode(key, iBin, nHash, pPrev);
    if (pNode == nullptr)
    {
        pNode = CreateNode(key, iBin, nHash);
        _ATLTRY
        {
            pNode->m_value = value;
        }
        _ATLCATCHALL()
        {
            RemoveAtPos(POSITION(pNode));
            _ATLRETHROW;
        }
        if (new_item_added)
        {
            *new_item_added = true;
        }
    }

    return (POSITION(pNode));
}

template <typename K, typename V, class KTraits, class VTraits>
POSITION XyAtlMap<K, V, KTraits, VTraits>::SetAt(
    /* _In_ */ KINARGTYPE key,
    /* _In_ */ VINARGTYPE value)
{
    CNode* pNode;
    UINT iBin;
    UINT nHash;
    CNode* pPrev;

    pNode = GetNode(key, iBin, nHash, pPrev);
    if (pNode == nullptr)
    {
        pNode = CreateNode(key, iBin, nHash);
        _ATLTRY
        {
            pNode->m_value = value;
        }
        _ATLCATCHALL()
        {
            RemoveAtPos(POSITION(pNode));
            _ATLRETHROW;
        }
    }
    else
    {
        pNode->m_value = value;
    }

    return (POSITION(pNode));
}

template <typename K, typename V, class KTraits, class VTraits>
void XyAtlMap<K, V, KTraits, VTraits>::SetValueAt(
    _In_ POSITION pos,
    /* _In_ */ VINARGTYPE value)
{
    ATLASSUME(pos != nullptr);

    CNode* pNode{ static_cast<CNode*>(pos) };

    pNode->m_value = value;
}

template <typename K, typename V, class KTraits, class VTraits>
XyAtlMap<K, V, KTraits, VTraits>::XyAtlMap(
    _In_ UINT nBins,
    _In_ float fOptimalLoad,
    _In_ float fLoThreshold,
    _In_ float fHiThreshold,
    _In_ UINT nBlockSize) throw() : m_ppBins(nullptr),
                                    m_nElements(0),
                                    m_nBins(nBins),
                                    m_fOptimalLoad(fOptimalLoad),
                                    m_fLoThreshold(fLoThreshold),
                                    m_fHiThreshold(fHiThreshold),
                                    m_nHiRehashThreshold(UINT_MAX),
                                    m_nLoRehashThreshold(0),
                                    m_nLockCount(0), // Start unlocked
                                    m_nBlockSize(nBlockSize),
                                    m_pBlocks(nullptr),
                                    m_pFree(nullptr)
{
    ATLASSERT(nBins > 0);
    ATLASSERT(nBlockSize > 0);

    SetOptimalLoad(fOptimalLoad, fLoThreshold, fHiThreshold, false);
}

template <typename K, typename V, class KTraits, class VTraits>
void XyAtlMap<K, V, KTraits, VTraits>::SetOptimalLoad(
    _In_ float fOptimalLoad,
    _In_ float fLoThreshold,
    _In_ float fHiThreshold,
    _In_ bool bRehashNow)
{
    ATLASSERT(fOptimalLoad > 0);
    ATLASSERT(((fLoThreshold >= 0) && (fLoThreshold < fOptimalLoad)));
    ATLASSERT(fHiThreshold > fOptimalLoad);

    m_fOptimalLoad = fOptimalLoad;
    m_fLoThreshold = fLoThreshold;
    m_fHiThreshold = fHiThreshold;

    UpdateRehashThresholds();

    if (bRehashNow && ((m_nElements > m_nHiRehashThreshold) ||
                       (m_nElements < m_nLoRehashThreshold)))
    {
        Rehash(PickSize(m_nElements));
    }
}

template <typename K, typename V, class KTraits, class VTraits>
void XyAtlMap<K, V, KTraits, VTraits>::UpdateRehashThresholds() throw()
{
    m_nHiRehashThreshold = std::size_t(m_fHiThreshold * m_nBins);
    m_nLoRehashThreshold = std::size_t(m_fLoThreshold * m_nBins);
    if (m_nLoRehashThreshold < 17)
    {
        m_nLoRehashThreshold = 0;
    }
}

template <typename K, typename V, class KTraits, class VTraits>
bool XyAtlMap<K, V, KTraits, VTraits>::InitHashTable(_In_ UINT nBins, _In_ bool bAllocNow)
{
    ATLASSUME(m_nElements == 0);
    ATLASSERT(nBins > 0);

    if (m_ppBins != nullptr)
    {
        delete[] m_ppBins;
        m_ppBins = nullptr;
    }

    if (bAllocNow)
    {
        ATLTRY(m_ppBins = new CNode*[nBins]);
        if (m_ppBins == nullptr)
        {
            return false;
        }

        ATLENSURE(UINT_MAX / sizeof(CNode*) >= nBins);
        std::memset(m_ppBins, 0, sizeof(CNode*) * nBins);
    }
    m_nBins = nBins;

    UpdateRehashThresholds();

    return true;
}

template <typename K, typename V, class KTraits, class VTraits>
void XyAtlMap<K, V, KTraits, VTraits>::RemoveAll()
{
    DisableAutoRehash();
    if (m_ppBins != nullptr)
    {
        for (UINT iBin{ 0 }; iBin < m_nBins; ++iBin)
        {
            CNode* pNext;

            pNext = m_ppBins[iBin];
            while (pNext != nullptr)
            {
                CNode* pKill;

                pKill = pNext;
                pNext = pNext->m_pNext;
                FreeNode(pKill);
            }
        }
    }

    delete[] m_ppBins;
    m_ppBins = nullptr;
    m_nElements = 0;

    if (!IsLocked())
    {
        (void)InitHashTable(PickSize(m_nElements), false);
    }

    FreePlexes();
    EnableAutoRehash();
}

template <typename K, typename V, class KTraits, class VTraits>
XyAtlMap<K, V, KTraits, VTraits>::~XyAtlMap() throw()
{
    _ATLTRY
    {
        RemoveAll();
    }
    _ATLCATCHALL()
    {
        ATLASSERT(false);
    }
}

#pragma push_macro("new")
#undef new

template <typename K, typename V, class KTraits, class VTraits>
typename XyAtlMap<K, V, KTraits, VTraits>::CNode* XyAtlMap<K, V, KTraits, VTraits>::NewNode(
    /* _In_ */ KINARGTYPE key,
    _In_ UINT iBin,
    _In_ UINT nHash)
{
    CNode* pNewNode;

    if (m_pFree == nullptr)
    {
        CAtlPlex* pPlex;
        CNode* pNode;

        pPlex = CAtlPlex::Create(m_pBlocks, m_nBlockSize, sizeof(CNode));
        if (pPlex == nullptr)
        {
            AtlThrow(E_OUTOFMEMORY);
        }
        pNode = (CNode*)pPlex->data();
        pNode += m_nBlockSize - 1;
        for (int iBlock = m_nBlockSize - 1; iBlock >= 0; --iBlock)
        {
            pNode->m_pNext = m_pFree;
            m_pFree = pNode;
            --pNode;
        }
    }
    ATLENSURE(m_pFree != nullptr);
    pNewNode = m_pFree;
    m_pFree = pNewNode->m_pNext;

    _ATLTRY
    {
        ::new (pNewNode) CNode(key, nHash);
    }
    _ATLCATCHALL()
    {
        pNewNode->m_pNext = m_pFree;
        m_pFree = pNewNode;

        _ATLRETHROW;
    }
    ++m_nElements;

    pNewNode->m_pNext = m_ppBins[iBin];
    m_ppBins[iBin] = pNewNode;

    if ((m_nElements > m_nHiRehashThreshold) && !IsLocked())
    {
        Rehash(PickSize(m_nElements));
    }

    return (pNewNode);
}

#pragma pop_macro("new")

template <typename K, typename V, class KTraits, class VTraits>
void XyAtlMap<K, V, KTraits, VTraits>::FreeNode(_Inout_ CNode* pNode)
{
    ATLENSURE(pNode != nullptr);

    pNode->~CNode();
    pNode->m_pNext = m_pFree;
    m_pFree = pNode;

    ATLASSUME(m_nElements > 0);
    --m_nElements;

    if ((m_nElements < m_nLoRehashThreshold) && !IsLocked())
    {
        Rehash(PickSize(m_nElements));
    }

    if (m_nElements == 0)
    {
        FreePlexes();
    }
}

template <typename K, typename V, class KTraits, class VTraits>
void XyAtlMap<K, V, KTraits, VTraits>::FreePlexes() throw()
{
    m_pFree = nullptr;
    if (m_pBlocks != nullptr)
    {
        m_pBlocks->FreeDataChain();
        m_pBlocks = nullptr;
    }
}

template <typename K, typename V, class KTraits, class VTraits>
typename XyAtlMap<K, V, KTraits, VTraits>::CNode* XyAtlMap<K, V, KTraits, VTraits>::GetNode(
    /* _In_ */ KINARGTYPE key,
    _Out_ UINT& iBin,
    _Out_ UINT& nHash,
    _Deref_out_opt_ CNode*& pPrev) const throw()
{
    CNode* pFollow;

    nHash = KTraits::Hash(key);
    iBin = nHash % m_nBins;

    if (m_ppBins == nullptr)
    {
        return (nullptr);
    }

    pFollow = nullptr;
    pPrev = nullptr;
    for (CNode* pNode{ m_ppBins[iBin] }; pNode != nullptr; pNode = pNode->m_pNext)
    {
        if ((pNode->GetHash() == nHash) && KTraits::CompareElements(pNode->m_key, key))
        {
            pPrev = pFollow;
            return (pNode);
        }
        pFollow = pNode;
    }

    return (nullptr);
}

template <typename K, typename V, class KTraits, class VTraits>
bool XyAtlMap<K, V, KTraits, VTraits>::Lookup(
    /* _In_ */ KINARGTYPE key,
    _Out_ VOUTARGTYPE value) const
{
    UINT iBin;
    UINT nHash;
    CNode* pNode;
    CNode* pPrev;

    pNode = GetNode(key, iBin, nHash, pPrev);
    if (pNode == nullptr)
    {
        return (false);
    }

    value = pNode->m_value;

    return (true);
}

template <typename K, typename V, class KTraits, class VTraits>
const typename XyAtlMap<K, V, KTraits, VTraits>::CPair* XyAtlMap<K, V, KTraits, VTraits>::Lookup(
    /* _In_ */ KINARGTYPE key) const throw()
{
    UINT iBin;
    UINT nHash;
    CNode* pNode;
    CNode* pPrev;

    pNode = GetNode(key, iBin, nHash, pPrev);

    return (pNode);
}

template <typename K, typename V, class KTraits, class VTraits>
typename XyAtlMap<K, V, KTraits, VTraits>::CPair* XyAtlMap<K, V, KTraits, VTraits>::Lookup(
    /* _In_ */ KINARGTYPE key) throw()
{
    UINT iBin;
    UINT nHash;
    CNode* pNode;
    CNode* pPrev;

    pNode = GetNode(key, iBin, nHash, pPrev);

    return (pNode);
}

template <typename K, typename V, class KTraits, class VTraits>
bool XyAtlMap<K, V, KTraits, VTraits>::RemoveKey(/* _In_ */ KINARGTYPE key) throw()
{
    CNode* pNode;
    UINT iBin;
    UINT nHash;
    CNode* pPrev;

    pPrev = nullptr;
    pNode = GetNode(key, iBin, nHash, pPrev);
    if (pNode == nullptr)
    {
        return (false);
    }

    RemoveNode(pNode, pPrev);

    return (true);
}

template <typename K, typename V, class KTraits, class VTraits>
void XyAtlMap<K, V, KTraits, VTraits>::RemoveNode(
    _In_ CNode* pNode,
    _In_opt_ CNode* pPrev) throw()
{
    ATLENSURE(pNode != nullptr);

    UINT iBin{ pNode->GetHash() % m_nBins };

    if (pPrev == nullptr)
    {
        ATLASSUME(m_ppBins[iBin] == pNode);
        m_ppBins[iBin] = pNode->m_pNext;
    }
    else
    {
        ATLASSERT(pPrev->m_pNext == pNode);
        pPrev->m_pNext = pNode->m_pNext;
    }
    FreeNode(pNode);
}

template <typename K, typename V, class KTraits, class VTraits>
void XyAtlMap<K, V, KTraits, VTraits>::RemoveAtPos(_In_ POSITION pos) throw()
{
    ATLENSURE(pos != nullptr);

    CNode* pNode{ static_cast<CNode*>(pos) };
    CNode* pPrev{ nullptr };
    UINT iBin{ pNode->GetHash() % m_nBins };

    ATLASSUME(m_ppBins[iBin] != nullptr);
    if (pNode == m_ppBins[iBin])
    {
        pPrev = nullptr;
    }
    else
    {
        pPrev = m_ppBins[iBin];
        while (pPrev->m_pNext != pNode)
        {
            pPrev = pPrev->m_pNext;
            ATLASSERT(pPrev != nullptr);
        }
    }
    RemoveNode(pNode, pPrev);
}

template <typename K, typename V, class KTraits, class VTraits>
void XyAtlMap<K, V, KTraits, VTraits>::Rehash(_In_ UINT nBins)
{
    CNode** ppBins{ nullptr };

    if (nBins == 0)
    {
        nBins = PickSize(m_nElements);
    }

    if (nBins == m_nBins)
    {
        return;
    }

    ATLTRACE(atlTraceMap, 2, _T("Rehash: %u bins\n"), nBins);

    if (m_ppBins == nullptr)
    {
        // Just set the new number of bins
        (void)InitHashTable(nBins, false);
        return;
    }

    ATLTRY(ppBins = new CNode*[nBins]);
    if (ppBins == nullptr)
    {
        AtlThrow(E_OUTOFMEMORY);
    }

    ATLENSURE(UINT_MAX / sizeof(CNode*) >= nBins);
    std::memset(ppBins, 0, nBins * sizeof(CNode*));

    // Nothing gets copied.  We just rewire the old nodes
    // into the new bins.
    for (UINT iSrcBin{ 0 }; iSrcBin < m_nBins; ++iSrcBin)
    {
        CNode* pNode;

        pNode = m_ppBins[iSrcBin];
        while (pNode != nullptr)
        {
            CNode* pNext;
            UINT iDestBin;

            pNext = pNode->m_pNext; // Save so we don't trash it
            iDestBin = pNode->GetHash() % nBins;
            pNode->m_pNext = ppBins[iDestBin];
            ppBins[iDestBin] = pNode;

            pNode = pNext;
        }
    }

    delete[] m_ppBins;
    m_ppBins = ppBins;
    m_nBins = nBins;

    UpdateRehashThresholds();
}

template <typename K, typename V, class KTraits, class VTraits>
void XyAtlMap<K, V, KTraits, VTraits>::GetNextAssoc(
    _Inout_ POSITION& pos,
    _Out_ KOUTARGTYPE key,
    _Out_ VOUTARGTYPE value) const
{
    CNode* pNode;
    CNode* pNext;

    ATLASSUME(m_ppBins != nullptr);
    ATLENSURE(pos != nullptr);

    pNode = (CNode*)pos;
    pNext = FindNextNode(pNode);

    pos = POSITION(pNext);
    key = pNode->m_key;
    value = pNode->m_value;
}

template <typename K, typename V, class KTraits, class VTraits>
const typename XyAtlMap<K, V, KTraits, VTraits>::CPair* XyAtlMap<K, V, KTraits, VTraits>::GetNext(
    _Inout_ POSITION& pos) const throw()
{
    CNode* pNode;
    CNode* pNext;

    ATLASSUME(m_ppBins != nullptr);
    ATLASSERT(pos != nullptr);

    pNode = (CNode*)pos;
    pNext = FindNextNode(pNode);

    pos = POSITION(pNext);

    return (pNode);
}

template <typename K, typename V, class KTraits, class VTraits>
typename XyAtlMap<K, V, KTraits, VTraits>::CPair* XyAtlMap<K, V, KTraits, VTraits>::GetNext(
    _Inout_ POSITION& pos) throw()
{
    ATLASSUME(m_ppBins != nullptr);
    ATLASSERT(pos != nullptr);

    CNode* pNode{ static_cast<CNode*>(pos) };
    CNode* pNext{ FindNextNode(pNode) };

    pos = POSITION(pNext);

    return (pNode);
}

template <typename K, typename V, class KTraits, class VTraits>
const K& XyAtlMap<K, V, KTraits, VTraits>::GetNextKey(
    _Inout_ POSITION& pos) const
{
    CNode* pNode;
    CNode* pNext;

    ATLASSUME(m_ppBins != nullptr);
    ATLENSURE(pos != nullptr);

    pNode = (CNode*)pos;
    pNext = FindNextNode(pNode);

    pos = POSITION(pNext);

    return (pNode->m_key);
}

template <typename K, typename V, class KTraits, class VTraits>
const V& XyAtlMap<K, V, KTraits, VTraits>::GetNextValue(
    _Inout_ POSITION& pos) const
{
    CNode* pNode;
    CNode* pNext;

    ATLASSUME(m_ppBins != nullptr);
    ATLENSURE(pos != nullptr);

    pNode = (CNode*)pos;
    pNext = FindNextNode(pNode);

    pos = POSITION(pNext);

    return (pNode->m_value);
}

template <typename K, typename V, class KTraits, class VTraits>
V& XyAtlMap<K, V, KTraits, VTraits>::GetNextValue(
    _Inout_ POSITION& pos)
{
    CNode* pNode;
    CNode* pNext;

    ATLASSUME(m_ppBins != nullptr);
    ATLENSURE(pos != nullptr);

    pNode = (CNode*)pos;
    pNext = FindNextNode(pNode);

    pos = POSITION(pNext);

    return (pNode->m_value);
}

template <typename K, typename V, class KTraits, class VTraits>
typename XyAtlMap<K, V, KTraits, VTraits>::CNode* XyAtlMap<K, V, KTraits, VTraits>::FindNextNode(
    _In_ CNode* pNode) const throw()
{
    CNode* pNext;

    if (pNode == nullptr)
    {
        ATLASSERT(FALSE);
        return nullptr;
    }

    if (pNode->m_pNext != nullptr)
    {
        pNext = pNode->m_pNext;
    }
    else
    {
        UINT iBin;

        pNext = nullptr;
        iBin = (pNode->GetHash() % m_nBins) + 1;
        while ((pNext == nullptr) && (iBin < m_nBins))
        {
            if (m_ppBins[iBin] != nullptr)
            {
                pNext = m_ppBins[iBin];
            }

            ++iBin;
        }
    }

    return (pNext);
}

#ifdef _DEBUG
template <typename K, typename V, class KTraits, class VTraits>
void XyAtlMap<K, V, KTraits, VTraits>::AssertValid() const
{
    ATLASSUME(m_nBins > 0);
    // non-empty map should have hash table
    ATLASSERT((IsEmpty() || (m_ppBins != nullptr)));
}
#endif

#pragma push_macro("new")
#undef new

////////////////////////////////////////////////////////////////////////////////////////////////////

template <
    typename K,
    typename V,
    class KTraits = CElementTraits<K>>
class XyMru
{
public:
    explicit XyMru(std::size_t max_item_num) : _max_item_num(max_item_num) {}

    [[nodiscard]] inline POSITION UpdateCache(POSITION pos)
    {
        _list.MoveToHead(pos);
        return pos;
    }
    [[nodiscard]] inline POSITION UpdateCache(POSITION pos, const V& value)
    {
        _list.GetAt(pos).second = value;
        _list.MoveToHead(pos);
        return pos;
    }
    [[nodiscard]] inline POSITION UpdateCache(const K& key, const V& value)
    {
        POSITION pos;
        POSITION pos_hash_value{ nullptr };
        bool new_item_added{ false };
        pos = _hash.SetAtIfNotExists(key, (POSITION) nullptr, &new_item_added);
        if (new_item_added)
        {
            pos_hash_value = _list.AddHead(ListItem(pos, value));
            _hash.SetValueAt(pos, pos_hash_value);
        }
        else
        {
            pos_hash_value = _hash.GetValueAt(pos);
            _list.GetAt(pos_hash_value).second = value;
            _list.MoveToHead(pos_hash_value);
        }
        if (_list.GetCount() > _max_item_num)
        {
            _hash.RemoveAtPos(_list.GetTail().first);
            _list.RemoveTail();
        }
        return pos_hash_value;
    }
    [[nodiscard]] inline POSITION AddHeadIfNotExists(const K& key, const V& value, bool* new_item_added)
    {
        POSITION pos;
        POSITION pos_hash_value{ nullptr };
        bool new_hash_item_added{ false };
        pos = _hash.SetAtIfNotExists(key, (POSITION) nullptr, &new_hash_item_added);
        if (new_hash_item_added)
        {
            pos_hash_value = _list.AddHead(ListItem(pos, value));
            _hash.SetValueAt(pos, pos_hash_value);
            if (new_item_added)
            {
                *new_item_added = true;
            }
        }
        else
        {
            pos_hash_value = _hash.GetValueAt(pos);
            if (new_item_added)
            {
                *new_item_added = false;
            }
        }
        if (_list.GetCount() > _max_item_num)
        {
            _hash.RemoveAtPos(_list.GetTail().first);
            _list.RemoveTail();
        }
        return pos_hash_value;
    }
    inline void RemoveAll()
    {
        _hash.RemoveAll();
        _list.RemoveAll();
    }
    inline void RemoveTail()
    {
        if (!_list.IsEmpty())
        {
            _hash.RemoveAtPos(_list.GetTail().first);
            _list.RemoveTail();
        }
    }

    [[nodiscard]] inline POSITION Lookup(const K& key) const
    {
        POSITION pos;
        if (_hash.Lookup(key, pos))
        {
            return pos;
        }
        else
        {
            return nullptr;
        }
    }
    [[nodiscard]] inline V& GetAt(POSITION pos)
    {
        return _list.GetAt(pos).second;
    }
    [[nodiscard]] inline const V& GetAt(POSITION pos) const
    {
        return _list.GetAt(pos).second;
    }
    [[nodiscard]] inline const K& GetKeyAt(POSITION pos) const
    {
        return _hash.GetKeyAt(_list.GetAt(pos).first);
    }
    [[nodiscard]] inline std::size_t SetMaxItemNum(const std::size_t max_item_num)
    {
        _max_item_num = max_item_num;
        while (_list.GetCount() > _max_item_num)
        {
            _hash.RemoveAtPos(_list.GetTail().first);
            _list.RemoveTail();
        }
        return _max_item_num;
    }
    [[nodiscard]] inline std::size_t GetMaxItemNum() const { return _max_item_num; }
    [[nodiscard]] inline std::size_t GetCurItemNum() const { return _list.GetCount(); }

protected:
    typedef std::pair<POSITION, V> ListItem;
    CAtlList<ListItem> _list;
    XyAtlMap<K, POSITION, KTraits> _hash;

    std::size_t _max_item_num;
};

template <
    typename K,
    typename V,
    class KTraits = CElementTraits<K>>
class EnhancedXyMru : public XyMru<K, V, KTraits>
{
public:
    explicit EnhancedXyMru(std::size_t max_item_num) : XyMru<K, V, KTraits>(max_item_num), _cache_hit(0), _query_count(0) {}

    [[nodiscard]] std::size_t SetMaxItemNum(const std::size_t max_item_num, const bool clear_statistic_info = false)
    {
        if (clear_statistic_info)
        {
            _cache_hit = 0;
            _query_count = 0;
        }
        return __super::SetMaxItemNum(max_item_num);
    }
    void RemoveAll(const bool clear_statistic_info = false)
    {
        if (clear_statistic_info)
        {
            _cache_hit = 0;
            _query_count = 0;
        }
        __super::RemoveAll();
    }

    [[nodiscard]] inline POSITION Lookup(const K& key)
    {
        ++_query_count;
        POSITION pos{ __super::Lookup(key) };
        _cache_hit += (pos != nullptr);
        return pos;
    }
    [[nodiscard]] inline POSITION AddHeadIfNotExists(const K& key, const V& value, bool* new_item_added)
    {
        ++_query_count;
        bool tmp{ false };
        POSITION pos{ __super::AddHeadIfNotExists(key, value, &tmp) };
        _cache_hit += (tmp == false);
        if (new_item_added)
        {
            *new_item_added = tmp;
        }
        return pos;
    }

    [[nodiscard]] inline std::size_t GetCacheHitCount() const { return _cache_hit; }
    [[nodiscard]] inline std::size_t GetQueryCount() const { return _query_count; }

protected:
    std::size_t _cache_hit;
    std::size_t _query_count;
};

#endif // end of __MRU_CACHE_H_256FCF72_8663_41DC_B98A_B822F6007912__
