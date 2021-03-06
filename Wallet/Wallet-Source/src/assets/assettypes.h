//
// Created by Jeremy Anderson on 5/15/18.
//

#ifndef RAVENCOIN_NEWASSET_H
#define RAVENCOIN_NEWASSET_H

#include <amount.h>
#include <string>
#include <unordered_map>
#include "serialize.h"

#define MAX_UNIT 8

class CAssetsCache;

class CNewAsset {
public:
    std::string strName; // MAX 31 Bytes
    CAmount nAmount;     // 8 Bytes
    int8_t units;        // 1 Byte
    int8_t nReissuable;  // 1 Byte
    int8_t nHasIPFS;     // 1 Byte
    std::string strIPFSHash; // MAX 40 Bytes

    CNewAsset()
    {
        SetNull();
    }

    CNewAsset(const std::string& strName, const CAmount& nAmount, const int& units, const int& nReissuable, const int& nHasIPFS, const std::string& strIPFSHash);

    CNewAsset(const CNewAsset& asset);
    CNewAsset& operator=(const CNewAsset& asset);

    void SetNull()
    {
        strName= "";
        nAmount = 0;
        units = int8_t(MAX_UNIT);
        nReissuable = int8_t(0);
        nHasIPFS = int8_t(0);
        strIPFSHash = "";
    }

    bool IsNull() const;

    bool IsValid(std::string& strError, CAssetsCache& assetCache, bool fCheckMempool = false, bool fCheckDuplicateInputs = true) const;

    std::string ToString();

    void ConstructTransaction(CScript& script) const;
    void ConstructOwnerTransaction(CScript& script) const;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(strName);
        READWRITE(nAmount);
        READWRITE(units);
        READWRITE(nReissuable);
        READWRITE(nHasIPFS);
        READWRITE(strIPFSHash);
    }
};

class AssetComparator
{
public:
    bool operator()(const CNewAsset& s1, const CNewAsset& s2) const
    {
        return s1.strName < s2.strName;
    }
};

class CAssetTransfer {
public:
    std::string strName;
    CAmount nAmount;


    CAssetTransfer()
    {
        SetNull();
    }

    void SetNull()
    {
        nAmount = 0;
        strName = "";
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(strName);
        READWRITE(nAmount);
    }

    CAssetTransfer(const std::string& strAssetName, const CAmount& nAmount);
    bool IsValid(std::string& strError) const;
    void ConstructTransaction(CScript& script) const;
};

class CReissueAsset {
public:
    std::string strName;
    CAmount nAmount;
    int8_t nReissuable;
    std::string strIPFSHash;

    CReissueAsset()
    {
        SetNull();
    }

    void SetNull()
    {
        nAmount = 0;
        strName = "";
        nReissuable = 1;
        strIPFSHash = "";
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(strName);
        READWRITE(nAmount);
        READWRITE(nReissuable);
        READWRITE(strIPFSHash);
    }

    CReissueAsset(const std::string& strAssetName, const CAmount& nAmount, const int& nReissuable, const std::string& strIPFSHash);
    bool IsValid(std::string& strError, CAssetsCache& assetCache) const;
    void ConstructTransaction(CScript& script) const;
    bool IsNull() const;
};


/** THESE ARE ONLY TO BE USED WHEN ADDING THINGS TO THE CACHE DURING CONNECT AND DISCONNECT BLOCK */
struct CAssetCacheNewAsset
{
    CNewAsset asset;
    std::string address;

    CAssetCacheNewAsset(const CNewAsset& asset, const std::string& address)
    {
        this->asset = asset;
        this->address = address;
    }

    bool operator<(const CAssetCacheNewAsset& rhs) const {
        return asset.strName < rhs.asset.strName;
    }
};

struct CAssetCacheReissueAsset
{
    CReissueAsset reissue;
    std::string address;
    COutPoint out;

    CAssetCacheReissueAsset(const CReissueAsset& reissue, const std::string& address, const COutPoint& out)
    {
        this->reissue = reissue;
        this->address = address;
        this->out = out;
    }

    bool operator<(const CAssetCacheReissueAsset& rhs) const {
        return out < rhs.out;
    }

};

struct CAssetCacheNewTransfer
{
    CAssetTransfer transfer;
    std::string address;
    COutPoint out;

    CAssetCacheNewTransfer(const CAssetTransfer& transfer, const std::string& address, const COutPoint& out)
    {
        this->transfer = transfer;
        this->address = address;
        this->out = out;
    }

    bool operator<(const CAssetCacheNewTransfer& rhs ) const
    {

        return out < rhs.out;
//        if (transfer.strName < rhs.transfer.strName)
//            return true;
//
//        if (rhs.transfer.strName == transfer.strName && address == rhs.address && transfer.nAmount == rhs.transfer.nAmount && out == rhs.out) {
//            return false;
//        } else if (rhs.transfer.strName == transfer.strName && (address != rhs.address || transfer.nAmount != rhs.transfer.nAmount || out == rhs.out))
//            return true;
//
//        return false;
    }
};

struct CAssetCacheNewOwner
{
    std::string assetName;
    std::string address;

    CAssetCacheNewOwner(const std::string& assetName, const std::string& address)
    {
        this->assetName = assetName;
        this->address = address;
    }

    bool operator<(const CAssetCacheNewOwner& rhs) const {

        return assetName < rhs.assetName;
//        if (assetName < rhs.assetName)
//            return true;
    }
};

struct CAssetCacheUndoAssetAmount
{
    std::string assetName;
    std::string address;
    CAmount nAmount;

    CAssetCacheUndoAssetAmount(const std::string& assetName, const std::string& address, const CAmount& nAmount)
    {
        this->assetName = assetName;
        this->address = address;
        this->nAmount = nAmount;
    }
};

struct CAssetCacheSpendAsset
{
    std::string assetName;
    std::string address;
    CAmount nAmount;

    CAssetCacheSpendAsset(const std::string& assetName, const std::string& address, const CAmount& nAmount)
    {
        this->assetName = assetName;
        this->address = address;
        this->nAmount = nAmount;
    }
};

struct CAssetCachePossibleMine
{
    std::string assetName;
    COutPoint out;
    CTxOut txOut;

    CAssetCachePossibleMine(const std::string& assetName, const COutPoint& out, const CTxOut txOut)
    {
        this->assetName = assetName;
        this->out = out;
        this->txOut = txOut;
    }

    bool operator<(const CAssetCachePossibleMine &other) const
    {
        return out < other.out;
    }
};

// Least Recently Used Cache
template<typename cache_key_t, typename cache_value_t>
class CLRUCache {
public:
    typedef typename std::pair<cache_key_t, cache_value_t> key_value_pair_t;
    typedef typename std::list<key_value_pair_t>::iterator list_iterator_t;

    CLRUCache(size_t max_size) : maxSize(max_size) {
    }
    CLRUCache()
    {
        SetNull();
    }

    void Put(const cache_key_t& key, const cache_value_t& value)
    {
        auto it = cacheItemsMap.find(key);
        cacheItemsList.push_front(key_value_pair_t(key, value));
        if (it != cacheItemsMap.end()) {
            cacheItemsList.erase(it->second);
            cacheItemsMap.erase(it);
        }
        cacheItemsMap[key] = cacheItemsList.begin();

        if (cacheItemsMap.size() > maxSize) {
            auto last = cacheItemsList.end();
            last--;
            cacheItemsMap.erase(last->first);
            cacheItemsList.pop_back();
        }
    }

    void Erase(const cache_key_t& key)
    {
        auto it = cacheItemsMap.find(key);
        if (it != cacheItemsMap.end()) {
            cacheItemsList.erase(it->second);
            cacheItemsMap.erase(it);
        }
    }

    const cache_value_t& Get(const cache_key_t& key)
    {
        auto it = cacheItemsMap.find(key);
        if (it == cacheItemsMap.end()) {
            throw std::range_error("There is no such key in cache");
        } else {
            cacheItemsList.splice(cacheItemsList.begin(), cacheItemsList, it->second);
            return it->second->second;
        }
    }

    bool Exists(const cache_key_t& key) const
    {
        return cacheItemsMap.find(key) != cacheItemsMap.end();
    }

    size_t Size() const
    {
        return cacheItemsMap.size();
    }


    void Clear()
    {
        cacheItemsMap.clear();
        cacheItemsList.clear();
    }

    void SetNull()
    {
        maxSize = 0;
        Clear();
    }


    void SetSize(const size_t size)
    {
        maxSize = size;
    }

   const std::unordered_map<cache_key_t, list_iterator_t>& GetItemsMap()
    {
        return cacheItemsMap;
    };


    CLRUCache(const CLRUCache& cache)
    {
        this->cacheItemsList = cache.cacheItemsList;
        this->cacheItemsMap = cache.cacheItemsMap;
        this->maxSize = cache.maxSize;
    }

private:
    std::list<key_value_pair_t> cacheItemsList;
    std::unordered_map<cache_key_t, list_iterator_t> cacheItemsMap;
    size_t maxSize;
};

#endif //RAVENCOIN_NEWASSET_H
