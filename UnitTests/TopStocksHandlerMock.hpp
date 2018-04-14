#pragma once

#include <iostream>
#include <cassert>

#include "../ITopStocks.hpp"

namespace top_stocks
{

namespace tests
{

struct TopStocksHandlerMock : ITopStocksHandler
{
    ~TopStocksHandlerMock()
    {
        assert(mWasGainersExpectationMet);
        assert(mWasLosersExpectationMet);
    }

    void ExpectGainers(const TTopList& aExpect)
    {
        assert(mWasGainersExpectationMet);
        mWasGainersExpectationMet = false;
        mGainers = aExpect;
    }

    void ExpectLosers(const TTopList& aExpect)
    {
        assert(mWasLosersExpectationMet);
        mWasLosersExpectationMet = false;
        mLosers = aExpect;
    }

    void ProcessTopGainersChanged(const TTopList& aChanged) override
    {
        mWasGainersExpectationMet = true;
        assert(AreEqual(mGainers, aChanged) || Print(mGainers, aChanged));
    }

    void ProcessTopLosersChanged(const TTopList& aChanged) override
    {
        mWasLosersExpectationMet = true;
        assert(AreEqual(mLosers, aChanged) || Print(mLosers, aChanged));
    }

private:

    static bool AreEqual(const TTopList& aExpected, const TTopList& aActual)
    {
        for (size_t i = 0; i < aExpected.size(); ++i)
        {
            if (aExpected[i].first != aActual[i].first)
            {
                return false;
            }

            if (std::abs(aExpected[i].second - aActual[i].second) > Epsilon)
            {
                return false;
            }
        }
        return true;
    }

    static bool Print(const TTopList& aExpected, const TTopList& aActual)
    {
        std::cout << "Expected: { " << std::endl;
        for (const auto& e : aExpected)
        {
            std::cout << "{" << e.first << " ; " << e.second << "}" << std::endl;
        }
        std::cout << "}" << std::endl;

        std::cout << "Actual: { " << std::endl;
        for (const auto& e : aActual)
        {
            std::cout << "{" << e.first << " ; " << e.second << "}" << std::endl;
        }
        std::cout << "}" << std::endl;

        return false;
    }

private:

    static const constexpr double Epsilon = 1e-10;

    TTopList mGainers;
    TTopList mLosers;

    bool mWasGainersExpectationMet = true;
    bool mWasLosersExpectationMet = true;
};

}
}
