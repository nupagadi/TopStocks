#pragma once

#include <array>
#include <utility>

namespace top_stocks
{

using TId = int;
using TBase = double;
using TChange = double;

using TQuote = std::pair<TId, TChange>;
using TTopList = std::array<TQuote, 10>;

struct ITopStocks
{
    virtual ~ITopStocks() = default;

    virtual void OnQuote(int aStockId, double aPrice) = 0;

};

struct ITopStocksHandler
{
    virtual ~ITopStocksHandler() = default;

    virtual void ProcessTopGainersChanged(const TTopList&) = 0;

    virtual void ProcessTopLosersChanged(const TTopList&) = 0;
};

}
