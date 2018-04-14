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

    template <typename TMap>
    void Process(TId aStockId, TChange aOldPercent, TChange aNewPercent, const TMap& aMap)
    {
        if (!TComparator<TChange>()(mThreshold, aOldPercent))
        {
            mContainer.erase({aOldPercent, aStockId});
        }

        if (!TComparator<TChange>()(mThreshold, aNewPercent))
        {
            mContainer.emplace(aNewPercent, aStockId);
        }

        if (!TComparator<TChange>()(mTopThreshold, aOldPercent) || !TComparator<TChange>()(mTopThreshold, aNewPercent))
        {
            TTopList topList;

            if (mContainer.size() >= TopSize)
            {
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
            }
            else
            {
                // Assumed be rare or when there are a few stocks.
                std::cout << "Warning: " << "Restoring the top." << std::endl;

                assert(aMap.size() >= TopSize);
                std::array<std::pair<TMap::key_type, TMap::mapped_type>, TopSize> temp;
                auto mapComparator = [](const auto& l, const auto& r)
                {
                    return TComparator<std::pair<TChange, TId>>()(
                        std::make_pair(l.second.second, l.first), std::make_pair(r.second.second, r.first));
                };

                std::partial_sort_copy(aMap.cbegin(), aMap.cend(), temp.begin(), temp.end(), mapComparator);
                std::transform(temp.cbegin(), temp.cend(), topList.begin(),
                    [](const auto& e)
                    {
                        return TQuote{e.first, e.second.second};
                    }
                );
            }

            if (TComparator<TChange>()(aOldPercent, mTopThreshold)
                || TComparator<TChange>()(aNewPercent, mTopThreshold)
                || aOldPercent == mTopThreshold && TComparator<TChange>()(aOldPercent, aNewPercent))
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
            mGainers.Process(aStockId, oldPercent, newPercent, mQuotes);
            mLosers.Process(aStockId, oldPercent, newPercent, mQuotes);
        }
    }

private:

    ITopStocksHandler& mHander;

    std::unordered_map<TId, std::pair<TBase, TChange>> mQuotes;

    TopProcessor<std::greater> mGainers;
    TopProcessor<std::less> mLosers;
};

}
