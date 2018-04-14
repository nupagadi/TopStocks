#pragma once

#include <cassert>
#include <iterator>
#include <functional>
#include <set>
#include <unordered_map>

#include "ITopStocks.hpp"

namespace top_stocks
{

template <template <typename> typename TComparator>
struct TopProcessor
{
    using TCallback = std::function<void(const TTopList&)>;

    TopProcessor(TChange aInitialThreshold, TCallback aCallback)
        : mThreshold(aInitialThreshold)
        , mTopThreshold(aInitialThreshold)
        , mCallback(aCallback)
    {

    }

    void Process(TId aStockId, TChange aOldPercent, TChange aNewPercent)
    {
        if (!mContainer.key_comp()({mThreshold, 0}, {aOldPercent, 0}))
        {
            mContainer.erase({aOldPercent, aStockId});
        }

        if (!mContainer.key_comp()({mThreshold, 0}, {aNewPercent, 0}))
        {
            mContainer.emplace(aNewPercent, aStockId);
        }

        if (!mContainer.key_comp()({mTopThreshold, 0}, {aOldPercent, 0})
            || !mContainer.key_comp()({mTopThreshold, 0}, {aNewPercent, 0}))
        {
            TTopList topList;

            assert(mContainer.size() >= TopSize);
            size_t i = 0;
            auto it = mContainer.cbegin();
            for (; i < topList.size(); ++i, ++it)
            {
                topList[i] = {it->second, it->first};
            }

            if (TopMaxCapacity < mContainer.size())
            {
                std::advance(it, TopMaxCapacity - i - 1);
                mThreshold = it->first;
                mContainer.erase(++it, mContainer.end());
            }
            mTopThreshold = topList.back().second;

            if (mContainer.key_comp()({aOldPercent, 0}, {mTopThreshold, 0})
                || mContainer.key_comp()({aNewPercent, 0}, {mTopThreshold, 0})
                || aOldPercent == mTopThreshold && mContainer.key_comp()({aOldPercent, 0}, {aNewPercent, 0}))
            {
                mCallback(topList);
            }
        }

        assert(mContainer.size() <= TopMaxCapacity);
    }

    template <typename TMap>
    void Copy(const TMap& aMap)
    {
        mContainer.clear();
        for (const auto& e : aMap)
        {
            mContainer.emplace(e.second.second, e.first);
            mTopThreshold = mThreshold = e.second.second;
        }
        assert(mContainer.size() <= TopSize);

        TTopList topList;
        std::transform(mContainer.cbegin(), mContainer.cend(), topList.begin(),
            [](const auto& e)
            {
                return TQuote{e.second, e.first};
            }
        );

        mCallback(topList);
    }


private:

    using TTopElement = std::pair<TChange, TId>;

    std::set<TTopElement, TComparator<TTopElement>> mContainer;

    TChange mThreshold {};
    TChange mTopThreshold {};

    std::function<void(const TTopList&)> mCallback;

    static const constexpr size_t TopMaxCapacity = 16;
};

struct TopStocks : ITopStocks
{
    TopStocks(ITopStocksHandler& aHandler)
        : mHander(aHandler)
        , mGainers(std::numeric_limits<TChange>::min(),
            std::bind(&ITopStocksHandler::ProcessTopGainersChanged, std::ref(mHander), std::placeholders::_1))
        , mLosers(std::numeric_limits<TChange>::max(),
            std::bind(&ITopStocksHandler::ProcessTopLosersChanged, std::ref(mHander), std::placeholders::_1))
    {

    }

    void OnQuote(int aStockId, double aPrice) override
    {
        if (aStockId <= 0)
        {
            return;
        }

        TChange oldPercent = 0, newPercent = 0;

        auto quoteIterator = mQuotes.find(aStockId);
        if (quoteIterator == mQuotes.cend())
        {
            if (aPrice <= 0)
            {
                return;
            }

            quoteIterator = mQuotes.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(aStockId),
                std::forward_as_tuple(aPrice, 0.)).first;
        }
        else
        {
            auto& quote = quoteIterator->second;

            if (aPrice <= 0)
            {
                quote.first = 0;
            }

            if (quote.first)
            {
                oldPercent = std::exchange(quote.second, (aPrice - quote.first) / quote.first * 100);
            }
            else
            {
                oldPercent = std::exchange(quote.second, 0);
            }
            newPercent = quote.second;
        }

        if (mQuotes.size() <= TopSize)
        {
            mGainers.Copy(mQuotes);
            mLosers.Copy(mQuotes);
        }
        else
        {
            mGainers.Process(aStockId, oldPercent, newPercent);
            mLosers.Process(aStockId, oldPercent, newPercent);
        }
    }

private:

    ITopStocksHandler& mHander;

    std::unordered_map<TId, std::pair<TBase, TChange>> mQuotes;

    TopProcessor<std::greater> mGainers;
    TopProcessor<std::less> mLosers;
};

}
