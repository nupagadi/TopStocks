#include <iostream>

#include "../TopStocks.hpp"
#include "TopStocksHandlerMock.hpp"

namespace top_stocks
{

namespace tests
{

void Add20Stocks(TopStocksHandlerMock& aMock, TopStocks& aTopStocks);

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

    Add20Stocks(mock, topStocks);

    mock.ExpectGainers({{
        {1, 100}, {20, 0}, {19, 0}, {18, 0}, {17, 0}, {16, 0}, {15, 0}, {14, 0}, {13, 0}, {12, 0},
    }});
    mock.ExpectLosers({{
        {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0},
    }});
    topStocks.OnQuote(1, 20);

    mock.ExpectGainers({{
        {1, 100}, {20, 0}, {19, 0}, {18, 0}, {17, 0}, {16, 0}, {15, 0}, {14, 0}, {13, 0}, {12, 0},
    }});
    mock.ExpectLosers({{
        {2, -50}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0},
    }});
    topStocks.OnQuote(2, 10);

    mock.ExpectGainers({{
        {20, 0}, {19, 0}, {18, 0}, {17, 0}, {16, 0}, {15, 0}, {14, 0}, {13, 0}, {12, 0}, {11, 0},
    }});
    mock.ExpectLosers({{
        {1, -50}, {2, -50}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0},
    }});
    topStocks.OnQuote(1, 5);
}

void Add20Stocks(TopStocksHandlerMock& aMock, TopStocks& aTopStocks)
{
    aMock.ExpectGainers({{{1, 0}}});
    aMock.ExpectLosers({{{1, 0}}});
    aTopStocks.OnQuote(1, 10);

    aMock.ExpectGainers({{{2, 0}, {1, 0}}});
    aMock.ExpectLosers({{{1, 0}, {2, 0}}});
    aTopStocks.OnQuote(2, 20);

    aMock.ExpectGainers({{
        {3, 0}, {2, 0}, {1, 0},
    }});
    aMock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0},
    }});
    aTopStocks.OnQuote(3, 30);

    aMock.ExpectGainers({{
        {4, 0}, {3, 0}, {2, 0}, {1, 0},
    }});
    aMock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0}, {4, 0},
    }});
    aTopStocks.OnQuote(4, 40);

    aMock.ExpectGainers({{
        {5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0},
    }});
    aMock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0},
    }});
    aTopStocks.OnQuote(5, 50);

    aMock.ExpectGainers({{
        {6, 0}, {5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0},
    }});
    aMock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0},
    }});
    aTopStocks.OnQuote(6, 60);

    aMock.ExpectGainers({{
        {7, 0}, {6, 0}, {5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0},
    }});
    aMock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0},
    }});
    aTopStocks.OnQuote(7, 70);

    aMock.ExpectGainers({{
        {8, 0}, {7, 0}, {6, 0}, {5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0},
    }});
    aMock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0},
    }});
    aTopStocks.OnQuote(8, 80);

    aMock.ExpectGainers({{
        {9, 0}, {8, 0}, {7, 0}, {6, 0}, {5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0},
    }});
    aMock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0},
    }});
    aTopStocks.OnQuote(9, 90);

    aMock.ExpectGainers({{
        {10, 0}, {9, 0}, {8, 0}, {7, 0}, {6, 0}, {5, 0}, {4, 0}, {3, 0}, {2, 0}, {1, 0},
    }});
    aMock.ExpectLosers({{
        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0},
    }});
    aTopStocks.OnQuote(10, 100);

    for (int i = 11; i <= 20; ++i)
    {
        aMock.ExpectGainersPersist();
        aMock.ExpectLosersPersist();
        aTopStocks.OnQuote(i, i * 10);
    }

//    aMock.ExpectGainers({{
//        {11, 0}, {10, 0}, {9, 0}, {8, 0}, {7, 0}, {6, 0}, {5, 0}, {4, 0}, {3, 0}, {2, 0},
//    }});
//    aMock.ExpectLosers({{
//        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0},
//    }});
//    aTopStocks.OnQuote(11, 110);

//    aMock.ExpectGainers({{
//        {12, 0}, {11, 0}, {10, 0}, {9, 0}, {8, 0}, {7, 0}, {6, 0}, {5, 0}, {4, 0}, {3, 0},
//    }});
//    aMock.ExpectLosers({{
//        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0},
//    }});
//    aTopStocks.OnQuote(12, 120);

//    aMock.ExpectGainers({{
//        {13, 0}, {12, 0}, {11, 0}, {10, 0}, {9, 0}, {8, 0}, {7, 0}, {6, 0}, {5, 0}, {4, 0},
//    }});
//    aMock.ExpectLosers({{
//        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0},
//    }});
//    aTopStocks.OnQuote(13, 130);

//    aMock.ExpectGainers({{
//        {14, 0}, {13, 0}, {12, 0}, {11, 0}, {10, 0}, {9, 0}, {8, 0}, {7, 0}, {6, 0}, {5, 0},
//    }});
//    aMock.ExpectLosers({{
//        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0},
//    }});
//    aTopStocks.OnQuote(14, 140);

//    aMock.ExpectGainers({{
//        {15, 0}, {14, 0}, {13, 0}, {12, 0}, {11, 0}, {10, 0}, {9, 0}, {8, 0}, {7, 0}, {6, 0},
//    }});
//    aMock.ExpectLosers({{
//        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0},
//    }});
//    aTopStocks.OnQuote(15, 150);

//    aMock.ExpectGainers({{
//        {16, 0}, {15, 0}, {14, 0}, {13, 0}, {12, 0}, {11, 0}, {10, 0}, {9, 0}, {8, 0}, {7, 0},
//    }});
//    aMock.ExpectLosers({{
//        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0},
//    }});
//    aTopStocks.OnQuote(16, 160);

//    aMock.ExpectGainers({{
//        {17, 0}, {16, 0}, {15, 0}, {14, 0}, {13, 0}, {12, 0}, {11, 0}, {10, 0}, {9, 0}, {8, 0},
//    }});
//    aMock.ExpectLosers({{
//        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0},
//    }});
//    aTopStocks.OnQuote(17, 170);

//    aMock.ExpectGainers({{
//        {18, 0}, {17, 0}, {16, 0}, {15, 0}, {14, 0}, {13, 0}, {12, 0}, {11, 0}, {10, 0}, {9, 0},
//    }});
//    aMock.ExpectLosers({{
//        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0},
//    }});
//    aTopStocks.OnQuote(18, 180);

//    aMock.ExpectGainers({{
//        {19, 0}, {18, 0}, {17, 0}, {16, 0}, {15, 0}, {14, 0}, {13, 0}, {12, 0}, {11, 0}, {10, 0},
//    }});
//    aMock.ExpectLosers({{
//        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0},
//    }});
//    aTopStocks.OnQuote(19, 190);

//    aMock.ExpectGainers({{
//        {20, 0}, {19, 0}, {18, 0}, {17, 0}, {16, 0}, {15, 0}, {14, 0}, {13, 0}, {12, 0}, {11, 0},
//    }});
//    aMock.ExpectLosers({{
//        {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0},
//    }});
//    aTopStocks.OnQuote(20, 200);
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

