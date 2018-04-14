#include <iostream>

#include "../TopStocks.hpp"
#include "TopStocksHandlerMock.hpp"

namespace top_stocks
{

namespace tests
{

void ShouldCalcPropperPercent()
{
    TopStocksHandlerMock mock;
    TopStocks topStocks(mock);

    mock.ExpectGainers({{{42, 0}}});
    mock.ExpectLosers({{{42, 0}}});

    topStocks.OnQuote(42, 100);

    mock.ExpectGainers({{{42, 12.3}}});
    mock.ExpectLosers({{{42, 12.3}}});

    topStocks.OnQuote(42, 112.3);
}

void ShouldReturnInProperOrder()
{
    TopStocksHandlerMock mock;
    TopStocks topStocks(mock);

    mock.ExpectGainers({{{42, 0}}});
    mock.ExpectLosers({{{42, 0}}});
    topStocks.OnQuote(42, 100);

    mock.ExpectGainers({{{42, 12.3}}});
    mock.ExpectLosers({{{42, 12.3}}});
    topStocks.OnQuote(42, 112.3);

    mock.ExpectGainers({{{42, 12.3}, {41, 0}}});
    mock.ExpectLosers({{{41, 0}, {42, 12.3}}});
    topStocks.OnQuote(41, 100);

    mock.ExpectGainers({{{42, 12.3}, {41, 12}}});
    mock.ExpectLosers({{{41, 12}, {42, 12.3}}});
    topStocks.OnQuote(41, 112);
}

void ShouldIgnoreIncorrectId()
{
    TopStocksHandlerMock mock;
    TopStocks topStocks(mock);

    // Expect nothing.

    topStocks.OnQuote(-42, 100);
    topStocks.OnQuote(-42, 123);
    topStocks.OnQuote(0, 112.3);
    topStocks.OnQuote(0, 11);
}

void ShouldResetIncorrectPrices()
{
    TopStocksHandlerMock mock;
    TopStocks topStocks(mock);

    mock.ExpectGainers({{{42, 0}}});
    mock.ExpectLosers({{{42, 0}}});
    topStocks.OnQuote(42, 100);

    mock.ExpectGainers({{{42, 12.3}}});
    mock.ExpectLosers({{{42, 12.3}}});
    topStocks.OnQuote(42, 112.3);

    mock.ExpectGainers({{{42, 0}}});
    mock.ExpectLosers({{{42, 0}}});
    topStocks.OnQuote(42, -112.3);
}

void ShouldNotConfuseStocksWithEqualPercents()
{
    TopStocksHandlerMock mock;
    TopStocks topStocks(mock);

    mock.ExpectGainers({{{42, 0}}});
    mock.ExpectLosers({{{42, 0}}});
    topStocks.OnQuote(42, 100);

    mock.ExpectGainers({{{42, 0}, {41, 0}}});
    mock.ExpectLosers({{{41, 0}, {42, 0}}});
    topStocks.OnQuote(41, 1000);

    mock.ExpectGainers({{{42, 12.3}, {41, 0}}});
    mock.ExpectLosers({{{41, 0}, {42, 12.3}}});
    topStocks.OnQuote(42, 112.3);
}

void ShouldRemoveOldPercents()
{
    TopStocksHandlerMock mock;
    TopStocks topStocks(mock);

    mock.ExpectGainers({{{42, 0}}});
    mock.ExpectLosers({{{42, 0}}});
    topStocks.OnQuote(42, 100);

    mock.ExpectGainers({{{42, 12.3}}});
    mock.ExpectLosers({{{42, 12.3}}});
    topStocks.OnQuote(42, 112.3);

    mock.ExpectGainers({{{42, 32.1}}});
    mock.ExpectLosers({{{42, 32.1}}});
    topStocks.OnQuote(42, 132.1);
}

void ShouldReturnTopTen()
{
    TopStocksHandlerMock mock;
    TopStocks topStocks(mock);

    mock.ExpectGainers({{{1, 0}}});
    mock.ExpectLosers({{{1, 0}}});
    topStocks.OnQuote(1, 10);

    mock.ExpectGainers({{{2, 0}, {1, 0}}});
    mock.ExpectLosers({{{1, 0}, {2, 0}}});
    topStocks.OnQuote(2, 20);

    mock.ExpectGainers({{
        {3, 0}, {2, 0}, {1, 0}
    }});
    mock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0}
    }});
    topStocks.OnQuote(3, 30);

    mock.ExpectGainers({{
        {4, 0}, {3, 0}, {2, 0}, {1, 0}
    }});
    mock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0}, {4, 0}
    }});
    topStocks.OnQuote(4, 40);

    mock.ExpectGainers({{
        {5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0}
    }});
    mock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}
    }});
    topStocks.OnQuote(5, 50);

    mock.ExpectGainers({{
        {6, 0}, {5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0}
    }});
    mock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}
    }});
    topStocks.OnQuote(6, 60);

    mock.ExpectGainers({{
        {7, 0}, {6, 0}, {5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0}
    }});
    mock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}
    }});
    topStocks.OnQuote(7, 70);

    mock.ExpectGainers({{
        {8, 0}, {7, 0}, {6, 0}, {5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0}
    }});
    mock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}
    }});
    topStocks.OnQuote(8, 80);

    mock.ExpectGainers({{
        {9, 0}, {8, 0}, {7, 0}, {6, 0}, {5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0}
    }});
    mock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}
    }});
    topStocks.OnQuote(9, 90);

    mock.ExpectGainers({{
        {10, 0}, {9, 0}, {8, 0}, {7, 0}, {6, 0}, {5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0}
    }});
    mock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}
    }});
    topStocks.OnQuote(10, 100);

    mock.ExpectGainers({{
        {11, 0}, {10, 0}, {9, 0}, {8, 0}, {7, 0}, {6, 0}, {5, 0}, {4, 0}, {3, 0}, {2, 0}
    }});
    mock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}
    }});
    topStocks.OnQuote(11, 110);

}

}
}

int main(int argc, char *argv[])
{
    using namespace top_stocks::tests;

    ShouldCalcPropperPercent();
    ShouldReturnInProperOrder();
    ShouldIgnoreIncorrectId();
    ShouldResetIncorrectPrices();
    ShouldNotConfuseStocksWithEqualPercents();
    ShouldRemoveOldPercents();
    ShouldReturnTopTen();



    std::cout << "All tests passed." << std::endl;
    return 0;
}

