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
        if (mContainer.key_comp()({aOldPercent, 0}, {mThreshold, 0}))
        {
            auto gr = mContainer.erase({aOldPercent, aStockId});
            assert(gr == 1);
        }

        if (mContainer.key_comp()({aNewPercent, 0}, {mThreshold, 0}))
        {
            mContainer.emplace(aNewPercent, aStockId);
        }

        if (mContainer.key_comp()({aOldPercent, 0}, {mTopThreshold, 0})
            || mContainer.key_comp()({aNewPercent, 0}, {mTopThreshold, 0}))
        {
            auto getTop = [](auto& from, auto& to)
            {
                auto it = from.cbegin();
                size_t i = 0;
                for (; i < to.size() && it != from.cend(); ++i, ++it)
                {
                    to[i] = {it->second, it->first};
                }

                if (TopMaxCapacity < from.size())
                {
                    std::advance(it, TopMaxCapacity - i);
                    from.erase(it, from.end());
                }
            };

            TTopList topList;
            getTop(mContainer, topList);
            mTopThreshold = topList.back().second;
            mCallback(topList);
        }

        assert(mContainer.size() <= TopMaxCapacity);
    }

    template <typename TMap>
    void Copy(const TMap& aMap)
    {
        for (const auto& e : aMap)
        {
            mContainer.emplace(e.second.second, e.first);
        }
        assert(mContainer.size() <= TopSize);
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

        if (mQuotes.size() < TopSize)
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
