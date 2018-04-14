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



    std::cout << "All tests passed." << std::endl;
    return 0;
}

