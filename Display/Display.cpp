#include <ctime>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <thread>

#include "../TopStocks.hpp"

struct Display : top_stocks::ITopStocksHandler
{
    Display()
        : mGainers{"Top Gainers:"}
        , mLosers{"Top Losers:"}
    {
        mGainers.resize(11);
        mLosers.resize(11);
    }

    void ProcessTopGainersChanged(const top_stocks::TTopList& aContainer) override
    {
        for (size_t i = 0; i < aContainer.size(); ++i)
        {
            mGainers[i + 1] =
                "#" + std::to_string(i + 1) + " "
                + std::to_string(aContainer[i].first) + " "
                + std::to_string(aContainer[i].second);
        }

        Print();
    }

    void ProcessTopLosersChanged(const top_stocks::TTopList& aContainer) override
    {
        for (size_t i = 0; i < aContainer.size(); ++i)
        {
            mLosers[i + 1] =
                "#" + std::to_string(i + 1) + " "
                + std::to_string(aContainer[i].first) + " "
                + std::to_string(aContainer[i].second);
        }

        Print();
    }

private:

    void Print() const
    {
        for (const auto& e : mGainers)
        {
            std::cout << e << std::endl;
        }
        std::cout << std::endl;
        for (const auto& e : mLosers)
        {
            std::cout << e << std::endl;
        }
        std::cout << std::endl;
    }

    std::vector<std::string> mGainers;
    std::vector<std::string> mLosers;
};

int main(int argc, char *argv[])
{
    std::cout << "Welcome to Top Stocks Display!" << std::endl;

    Display display;
    top_stocks::TopStocks topStocks(display);

    for (size_t i = 0; i < 10000; ++i)
    {
        topStocks.OnQuote(i, i * 10);
    }

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    while (true)
    {
        int id = std::rand() % 10000 + 1;
        double percent = id * 10 + (std::rand() % 1500 - 500) * id * 10 / 100;
        percent = percent < 0 ? percent / 5 : percent;
        topStocks.OnQuote(id , percent);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return 0;
}
