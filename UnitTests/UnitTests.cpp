#include <iostream>

#include "../TopStocks.hpp"
#include "TopStocksHandlerMock.hpp"

namespace top_stocks
{

namespace tests
{

void Add20Stocks(TopStocksHandlerMock& aMock, TopStocks& aTopStocks);
void AddYetAnother20Stocks(TopStocksHandlerMock& aMock, TopStocks& aTopStocks);

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

void ShouldOperateMoreThan20()
{
    TopStocksHandlerMock mock;
    TopStocks topStocks(mock);

    Add20Stocks(mock, topStocks);

    AddYetAnother20Stocks(mock, topStocks);

    mock.ExpectGainers({{
        {1, 900}, {40, 0}, {39, 0}, {38, 0}, {37, 0}, {36, 0}, {35, 0}, {34, 0}, {33, 0}, {32, 0},
    }});
    mock.ExpectLosers({{
        {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0},
    }});
    topStocks.OnQuote(1, 100);

    mock.ExpectGainers({{
        {1, 900}, {2, 400}, {40, 0}, {39, 0}, {38, 0}, {37, 0}, {36, 0}, {35, 0}, {34, 0}, {33, 0},
    }});
    mock.ExpectLosers({{
        {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0}, {12, 0},
    }});
    topStocks.OnQuote(2, 100);

    mock.ExpectGainers({{
        {1, 900}, {2, 400}, {3, (100-30)/0.3}, {40, 0}, {39, 0}, {38, 0}, {37, 0}, {36, 0}, {35, 0}, {34, 0},
    }});
    mock.ExpectLosers({{
        {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0}, {12, 0}, {13, 0},
    }});
    topStocks.OnQuote(3, 100);

    mock.ExpectGainers({{
        {1, 900}, {2, 400}, {40, 0}, {39, 0}, {38, 0}, {37, 0}, {36, 0}, {35, 0}, {34, 0}, {33, 0},
    }});
    mock.ExpectLosers({{
        {3, -200/3.}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0}, {12, 0},
    }});
    topStocks.OnQuote(3, 10);

    mock.ExpectGainers({{
        {1, 900}, {2, 400}, {4, 150}, {40, 0}, {39, 0}, {38, 0}, {37, 0}, {36, 0}, {35, 0}, {34, 0},
    }});
    mock.ExpectLosers({{
        {3, -200/3.}, {5, 0}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0}, {12, 0}, {13, 0},
    }});
    topStocks.OnQuote(4, 100);

    mock.ExpectGainers({{
        {1, 900}, {2, 400}, {4, 150}, {5, 100}, {40, 0}, {39, 0}, {38, 0}, {37, 0}, {36, 0}, {35, 0},
    }});
    mock.ExpectLosers({{
        {3, -200/3.}, {6, 0}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0}, {12, 0}, {13, 0}, {14, 0},
    }});
    topStocks.OnQuote(5, 100);

    mock.ExpectGainers({{
        {1, 900}, {2, 400}, {4, 150}, {5, 100}, {40, 0}, {39, 0}, {38, 0}, {37, 0}, {36, 0}, {35, 0},
    }});
    mock.ExpectLosers({{
        {6, -90}, {3, -200/3.}, {7, 0}, {8, 0}, {9, 0}, {10, 0}, {11, 0}, {12, 0}, {13, 0}, {14, 0},
    }});
    topStocks.OnQuote(6, 6);

    mock.ExpectGainers({{
        {1, 900}, {2, 400}, {4, 150}, {5, 100}, {40, 0}, {39, 0}, {38, 0}, {37, 0}, {36, 0}, {35, 0},
    }});
    mock.ExpectLosers({{
        {6, -90}, {3, -200/3.}, {7, -10}, {8, 0}, {9, 0}, {10, 0}, {11, 0}, {12, 0}, {13, 0}, {14, 0},
    }});
    topStocks.OnQuote(7, 63);

    mock.ExpectGainers({{
        {1, 900}, {2, 400}, {4, 150}, {5, 100}, {40, 0}, {39, 0}, {38, 0}, {37, 0}, {36, 0}, {35, 0},
    }});
    mock.ExpectLosers({{
        {6, -90}, {8, -87.5}, {3, -200/3.}, {7, -10}, {9, 0}, {10, 0}, {11, 0}, {12, 0}, {13, 0}, {14, 0},
    }});
    topStocks.OnQuote(8, 10);

    mock.ExpectGainers({{
        {1, 900}, {2, 400}, {9, 200}, {4, 150}, {5, 100}, {40, 0}, {39, 0}, {38, 0}, {37, 0}, {36, 0},
    }});
    mock.ExpectLosers({{
        {6, -90}, {8, -87.5}, {3, -200/3.}, {7, -10}, {10, 0}, {11, 0}, {12, 0}, {13, 0}, {14, 0}, {15, 0},
    }});
    topStocks.OnQuote(9, 270);

    mock.ExpectGainers({{
        {1, 900}, {2, 400}, {9, 200}, {4, 150}, {5, 100}, {40, 0}, {39, 0}, {38, 0}, {37, 0}, {36, 0},
    }});
    mock.ExpectLosers({{
        {10, -98}, {6, -90}, {8, -87.5}, {3, -200/3.}, {7, -10}, {11, 0}, {12, 0}, {13, 0}, {14, 0}, {15, 0},
    }});
    topStocks.OnQuote(10, 2);

    mock.ExpectGainers({{
        {1, 900}, {11, 800}, {2, 400}, {9, 200}, {4, 150}, {5, 100}, {40, 0}, {39, 0}, {38, 0}, {37, 0},
    }});
    mock.ExpectLosers({{
        {10, -98}, {6, -90}, {8, -87.5}, {3, -200/3.}, {7, -10}, {12, 0}, {13, 0}, {14, 0}, {15, 0}, {16, 0},
    }});
    topStocks.OnQuote(11, 990);

    mock.ExpectGainers({{
        {1, 900}, {11, 800}, {2, 400}, {9, 200}, {4, 150}, {5, 100}, {40, 0}, {39, 0}, {38, 0}, {37, 0},
    }});
    mock.ExpectLosers({{
        {10, -98}, {6, -90}, {8, -87.5}, {12, -85}, {3, -200/3.}, {7, -10}, {13, 0}, {14, 0}, {15, 0}, {16, 0},
    }});
    topStocks.OnQuote(12, 18);

    mock.ExpectGainers({{
        {1, 900}, {11, 800}, {13, 500}, {2, 400}, {9, 200}, {4, 150}, {5, 100}, {40, 0}, {39, 0}, {38, 0},
    }});
    mock.ExpectLosers({{
        {10, -98}, {6, -90}, {8, -87.5}, {12, -85}, {3, -200/3.}, {7, -10}, {14, 0}, {15, 0}, {16, 0}, {17, 0},
    }});
    topStocks.OnQuote(13, 780);

    mock.ExpectGainers({{
        {1, 900}, {11, 800}, {13, 500}, {2, 400}, {9, 200}, {4, 150}, {5, 100}, {40, 0}, {39, 0}, {38, 0},
    }});
    mock.ExpectLosers({{
        {10, -98}, {6, -90}, {8, -87.5}, {12, -85}, {30, -80}, {3, -200/3.}, {7, -10}, {14, 0}, {15, 0}, {16, 0},
    }});
    topStocks.OnQuote(30, 60);

    mock.ExpectGainers({{
        {1, 900}, {11, 800}, {13, 500}, {2, 400}, {31, 300}, {9, 200}, {4, 150}, {5, 100}, {40, 0}, {39, 0},
    }});
    mock.ExpectLosers({{
        {10, -98}, {6, -90}, {8, -87.5}, {12, -85}, {30, -80}, {3, -200/3.}, {7, -10}, {14, 0}, {15, 0}, {16, 0},
    }});
    topStocks.OnQuote(31, 1240);

    mock.ExpectGainers({{
        {1, 900}, {11, 800}, {29, 600}, {13, 500}, {2, 400}, {31, 300}, {9, 200}, {4, 150}, {5, 100}, {40, 0},
    }});
    mock.ExpectLosers({{
        {10, -98}, {6, -90}, {8, -87.5}, {12, -85}, {30, -80}, {3, -200/3.}, {7, -10}, {14, 0}, {15, 0}, {16, 0},
    }});
    topStocks.OnQuote(29, 2030);

    mock.ExpectGainers({{
        {15, 1900}, {1, 900}, {11, 800}, {29, 600}, {13, 500}, {2, 400}, {31, 300}, {9, 200}, {4, 150}, {5, 100},
    }});
    mock.ExpectLosers({{
        {10, -98}, {6, -90}, {8, -87.5}, {12, -85}, {30, -80}, {3, -200/3.}, {7, -10}, {14, 0}, {16, 0}, {17, 0},
    }});
    topStocks.OnQuote(15, 3000);

    mock.ExpectGainersPersist();
    mock.ExpectLosers({{
        {10, -98}, {20, -95}, {6, -90}, {8, -87.5}, {12, -85}, {30, -80}, {3, -200/3.}, {7, -10}, {14, 0}, {16, 0},
    }});
    topStocks.OnQuote(20, 10);

    mock.ExpectGainersPersist();
    mock.ExpectLosers({{
        {10, -98}, {20, -95}, {6, -90}, {8, -87.5}, {12, -85}, {30, -80}, {3, -200/3.}, {7, -10}, {14, 0}, {17, 0},
    }});
    topStocks.OnQuote(16, 240);

    mock.ExpectGainersPersist();
    mock.ExpectLosersPersist();
    topStocks.OnQuote(16, 200);

    mock.ExpectGainersPersist();
    mock.ExpectLosers({{
        {10, -98}, {20, -95}, {32, -93.75}, {6, -90}, {8, -87.5}, {12, -85}, {30, -80}, {3, -200/3.}, {7, -10}, {14, 0},
    }});
    topStocks.OnQuote(32, 20);

    mock.ExpectGainersPersist();
    mock.ExpectLosers({{
        {10, -98}, {40, -97.5}, {20, -95}, {32, -93.75}, {6, -90}, {8, -87.5}, {12, -85}, {30, -80}, {3, -200/3.}, {7, -10},
    }});
    topStocks.OnQuote(40, 10);

    mock.ExpectGainersPersist();
    mock.ExpectLosersPersist();
    topStocks.OnQuote(39, 380);

    mock.ExpectGainersPersist();
    mock.ExpectLosersPersist();
    topStocks.OnQuote(39, 400);

    mock.ExpectGainersPersist();
    mock.ExpectLosersPersist();
    topStocks.OnQuote(38, 370);

    mock.ExpectGainers({{
        {1, 900}, {11, 800}, {29, 600}, {13, 500}, {2, 400}, {31, 300}, {9, 200}, {4, 150}, {5, 100}, {16, 25},
    }});
    mock.ExpectLosers({{
        {10, -98}, {40, -97.5}, {20, -95}, {32, -93.75}, {15, -90 - 10/3.}, {6, -90}, {8, -87.5}, {12, -85}, {30, -80}, {3, -200/3.},
    }});
    topStocks.OnQuote(15, 10);

    mock.ExpectGainers({{
        {11, 800}, {29, 600}, {13, 500}, {2, 400}, {31, 300}, {9, 200}, {4, 150}, {5, 100}, {16, 25}, {39, 2.5641025641},
    }});
    mock.ExpectLosers({{
        {10, -98}, {40, -97.5}, {20, -95}, {32, -93.75}, {15, -90 - 10/3.}, {6, -90}, {8, -87.5}, {1, -86}, {12, -85}, {30, -80},
    }});
    topStocks.OnQuote(1, 1.4);

    mock.ExpectGainers({{
        {29, 600}, {13, 500}, {2, 400}, {31, 300}, {9, 200}, {4, 150}, {5, 100}, {16, 25}, {39, 2.5641025641}, {37, 0},
    }});
    mock.ExpectLosers({{
        {11, -99.9}, {10, -98}, {40, -97.5}, {20, -95}, {32, -93.75}, {15, -90 - 10/3.}, {6, -90}, {8, -87.5}, {1, -86}, {12, -85},
    }});
    topStocks.OnQuote(11, 0.11);

    mock.ExpectGainers({{
        {13, 500}, {2, 400}, {31, 300}, {9, 200}, {4, 150}, {5, 100}, {16, 25}, {39, 2.5641025641}, {37, 0}, {36, 0},
    }});
    mock.ExpectLosers({{
        {29, -99.95}, {11, -99.9}, {10, -98}, {40, -97.5}, {20, -95}, {32, -93.75}, {15, -90 - 10/3.}, {6, -90}, {8, -87.5}, {1, -86},
    }});
    topStocks.OnQuote(29, 0.145);

    mock.ExpectGainers({{
        {13, 500}, {31, 300}, {9, 200}, {4, 150}, {5, 100}, {16, 25}, {39, 2.5641025641}, {37, 0}, {36, 0}, {35, 0},
    }});
    mock.ExpectLosers({{
        {2, -99.995}, {29, -99.95}, {11, -99.9}, {10, -98}, {40, -97.5}, {20, -95}, {32, -93.75}, {15, -90 - 10/3.}, {6, -90}, {8, -87.5},
    }});
    topStocks.OnQuote(2, 0.001);

    mock.ExpectGainers({{
        {13, 500}, {31, 300}, {9, 200}, {5, 100}, {16, 25}, {39, 2.5641025641}, {37, 0}, {36, 0}, {35, 0}, {34, 0},
    }});
    mock.ExpectLosers({{
        {4, -99.99975}, {2, -99.995}, {29, -99.95}, {11, -99.9}, {10, -98}, {40, -97.5}, {20, -95}, {32, -93.75}, {15, -90 - 10/3.}, {6, -90},
    }});
    topStocks.OnQuote(4, 0.0001);

    mock.ExpectGainers({{
        {13, 500}, {31, 300}, {9, 200}, {4, 150}, {5, 100}, {16, 25}, {39, 2.5641025641}, {37, 0}, {36, 0}, {35, 0},
    }});
    mock.ExpectLosers({{
        {2, -99.995}, {29, -99.95}, {11, -99.9}, {10, -98}, {40, -97.5}, {20, -95}, {32, -93.75}, {15, -90 - 10/3.}, {6, -90}, {8, -87.5},
    }});
    topStocks.OnQuote(4, 100);
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

void AddYetAnother20Stocks(TopStocksHandlerMock& aMock, TopStocks& aTopStocks)
{
    for (int i = 21; i <= 40; ++i)
    {
        aMock.ExpectGainersPersist();
        aMock.ExpectLosersPersist();
        aTopStocks.OnQuote(i, i * 10);
    }
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
    ShouldOperateMoreThan20();

    std::cout << "All tests passed." << std::endl;
    return 0;
}

