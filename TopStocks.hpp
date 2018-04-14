#pragma once

#include <cassert>
#include <functional>
#include <set>
#include <unordered_map>

#include "ITopStocks.hpp"

namespace top_stocks
{

struct TopStocks : ITopStocks
{
    TopStocks(ITopStocksHandler& aHandler)
        : mHander(aHandler)
    {

    }

    void OnQuote(int aStockId, double aPrice) override
    {
        if (aStockId <= 0)
        {
            return;
        }

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

            TChange old;
            if (quote.first)
            {
                old = std::exchange(quote.second, (aPrice - quote.first) / quote.first * 100);
            }
            else
            {
                old = std::exchange(quote.second, 0);
            }

            auto gr = mGainers.erase({old, aStockId});
            assert(gr == 1);
            auto lr = mLosers.erase({old, aStockId});
            assert(lr == 1);
        }

        mGainers.emplace(quoteIterator->second.second, aStockId);
        mLosers.emplace(quoteIterator->second.second, aStockId);

        TTopList gainers;
        TTopList losers;
        auto getTop = [](const auto& from, auto& to)
        {
            auto it = from.cbegin();
            for (size_t i = 0; i < to.size() && it != from.cend(); ++i, ++it)
            {
                to[i] = {it->second, it->first};
            }
        };

        getTop(mGainers, gainers);
        getTop(mLosers, losers);

        mHander.ProcessTopGainersChanged(gainers);
        mHander.ProcessTopLosersChanged(losers);
    }

private:

    ITopStocksHandler& mHander;

    std::unordered_map<TId, std::pair<TBase, TChange>> mQuotes;

    using TTopElement = std::pair<TChange, TId>;
    template <template <typename> typename TComparator>
    using TTop = std::set<TTopElement, TComparator<TTopElement>>;
    TTop<std::greater> mGainers;
    TTop<std::less> mLosers;
};

}
