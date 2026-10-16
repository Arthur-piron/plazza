/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** Receptionist
*/

#include "Receptionist.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <fcntl.h>
#include <signal.h>
#include <thread>
#include <sstream>
#include <chrono>
#include <cerrno>
#include "PizzaFactory.hpp"

namespace Plazza {

Receptionist::Receptionist()
{
}

Receptionist::~Receptionist()
{
    {
        std::lock_guard<std::mutex> lock(_watcherMutex);
        _watcherRunning = false;
    }
    if (_watcherThread.joinable())
        _watcherThread.join();

    stopAllKitchens(true);
}

void Receptionist::checkDeadKitchens()
{
    int status = 0;
    pid_t pid;

    while ((pid = Plazza::Process::waitpid(-1, status, WNOHANG)) > 0) {
        std::lock_guard<std::mutex> lock(_kitchenStateMutex);

        for (auto iterator = _kitchenPids.begin(); iterator != _kitchenPids.end(); ++iterator) {
            if (iterator->second == pid) {
                int kitchenId = iterator->first;

                _kitchenIPC.erase(kitchenId);
                _kitchenOutstandingPizzas.erase(kitchenId);
                _kitchenCookCount.erase(kitchenId);

                _availableKitchenIds.insert(kitchenId);

                _kitchenPids.erase(iterator);

                break;
            }
        }
    }
}

void Receptionist::stopAllKitchens(bool forceKill)
{
    std::vector<pid_t> childPids;

    {
        std::lock_guard<std::mutex> kitchenLock(_kitchenStateMutex);
        for (auto &[_, ipc] : _kitchenIPC) {
            if (ipc)
                ipc->closeAll();
        }
        for (const auto &[_, pid] : _kitchenPids) {
            if (pid > 0)
                childPids.push_back(pid);
        }
        _kitchenIPC.clear();
        _kitchenPids.clear();
        _kitchenOutstandingPizzas.clear();
        _kitchenCookCount.clear();
        _availableKitchenIds.clear();
    }

    if (forceKill) {
        for (pid_t pid : childPids)
            Plazza::Process::kill(pid, SIGTERM);
    }

    for (pid_t pid : childPids) {
        int status = 0;
        try {
            Plazza::Process::waitpid(pid, status, 0);
        } catch (const std::exception &e) {
            std::cerr << "waitpid failed: "
                    << e.what()
                    << std::endl;
        }
    }
}

int Receptionist::getLowestWorkload()
{
    std::lock_guard<std::mutex> kitchenLock(_kitchenStateMutex);
    int lowestWorkload = INT_MAX;
    int selectedKitchenId = -1;

    for (const auto &[kitchenId, outstanding] : _kitchenOutstandingPizzas) {
        if (outstanding < lowestWorkload) {
            lowestWorkload = outstanding;
            selectedKitchenId = kitchenId;
        }
    }
    return selectedKitchenId;
}

int Receptionist::createKitchen(const Plazza::Pizzeria &pizzeria)
{
    int kitchenId = 0;
    {
        std::lock_guard<std::mutex> kitchenLock(_kitchenStateMutex);
        if (!_availableKitchenIds.empty()) {
            kitchenId = *_availableKitchenIds.begin();
            _availableKitchenIds.erase(_availableKitchenIds.begin());
        } else {
            kitchenId = _nextKitchenId++;
        }
    }

    auto ipc = std::make_unique<IPC>();

    pid_t pid = Plazza::Process::fork();

    if (pid < 0) {
        {
            std::lock_guard<std::mutex> kitchenLock(_kitchenStateMutex);
            _availableKitchenIds.insert(kitchenId);
        }
        std::cerr << "Fork failed!\n";
        Plazza::Process::exit(84);
    }
    else if (pid == 0) {
        ipc->closeParentRead();
        ipc->closeParentWrite();
        ipc->closeParentStatusRequestWrite();
        ipc->closeParentStatusResponseRead();

        auto idleTimeout = std::chrono::seconds(5);
        auto lastActiveAt = std::chrono::steady_clock::now();

        Kitchen childKitchen(
            pizzeria.timeMultiplier, 
            pizzeria.cookNbPerKitchen, 
            pizzeria.timeToRefill, 
            kitchenId
        );

        bool pipeOpen = true;

        while (true) {
            bool didWork = false;
            bool parentPipeClosed = false;
            auto pizza = ipc->receiveInChild(parentPipeClosed);

            if (pizza != nullptr) {
                childKitchen.addPizzaToQueue(pizza);
                lastActiveAt = std::chrono::steady_clock::now();
                didWork = true;
            }
            if (parentPipeClosed) {
                pipeOpen = false;
                didWork = true;
            }

            while (ipc->receiveStatusRequestInChild()) {
                KitchenStatusSnapshot snapshot;
                snapshot.cookCount = childKitchen.getCookCount();
                snapshot.cookWorking.assign(snapshot.cookCount, 0);
                for (int i = 0; i < snapshot.cookCount; ++i) {
                    snapshot.cookWorking[i] = childKitchen.getThreadPool().isCooking(i) ? 1 : 0;
                }
                auto ingredients = childKitchen.getCurrentIngredients();
                snapshot.dough = ingredients.dough;
                snapshot.tomato = ingredients.tomato;
                snapshot.gruyere = ingredients.gruyere;
                snapshot.ham = ingredients.ham;
                snapshot.mushroom = ingredients.mushroom;
                snapshot.steak = ingredients.steak;
                snapshot.eggplant = ingredients.eggplant;
                snapshot.goatCheese = ingredients.goatCheese;
                snapshot.chiefLove = ingredients.chiefLove;
                ipc << snapshot;
            }

            std::vector<std::shared_ptr<Plazza::IPizza>> finishedPizzas;

            {
                std::lock_guard<std::mutex> lock(childKitchen.getThreadPool().getDoneMutex());

                auto &doneQueue = childKitchen.getThreadPool().getDoneQueue();

                while (!doneQueue.empty()) {
                    finishedPizzas.push_back(doneQueue.front());
                    doneQueue.pop();
                }
            }
            for (const auto &pizza : finishedPizzas) {
                ipc->sendFromChild(pizza); // on envoie la pizza finie au parent
                didWork = true;
            }
            const int pendingPizzas = childKitchen.getThreadPool().getPendingPizzaCount();
            if (pendingPizzas > 0)
                lastActiveAt = std::chrono::steady_clock::now();

            if (!pipeOpen && pendingPizzas == 0 && finishedPizzas.empty())
                break;

            if (pendingPizzas == 0 && std::chrono::steady_clock::now() - lastActiveAt >= idleTimeout)
                break;

            if (!didWork)
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        ipc->closeAll();
        return 0;
    }
    else {
        ipc->closeChildRead();
        ipc->closeChildWrite();
        ipc->closeChildStatusRequestRead();
        ipc->closeChildStatusResponseWrite();

        {
            std::lock_guard<std::mutex> kitchenLock(_kitchenStateMutex);
            _kitchenIPC[kitchenId] = std::move(ipc);
            _kitchenPids[kitchenId] = pid;
            _kitchenOutstandingPizzas[kitchenId] = 0;
            _kitchenCookCount[kitchenId] = pizzeria.cookNbPerKitchen;
        }
        
        return kitchenId;
    }
    return -1;
}

void Receptionist::sendPizzasToKitchens(std::vector<std::shared_ptr<Plazza::IPizza>> &pizzas, const Plazza::Pizzeria &pizzeria)
{
    {
        std::lock_guard<std::mutex> lock(_ordersMutex);
        for (const auto &pizza : pizzas) {
            _orders[pizza->getOrderId()].push_back(pizza);
            _expectedPizzaCount[pizza->getOrderId()] += 1;
        }
    }

    for (const auto &pizza : pizzas) {
        int selectedKitchenId = -1;

        bool noKitchens = false;
        {
            std::lock_guard<std::mutex> kitchenLock(_kitchenStateMutex);
            noKitchens = _kitchenIPC.empty();
        }
        if (noKitchens) {
            selectedKitchenId = createKitchen(pizzeria);
        } else {
            selectedKitchenId = getLowestWorkload();
            int outstanding = 0;
            int cookCount = pizzeria.cookNbPerKitchen;
            {
                std::lock_guard<std::mutex> kitchenLock(_kitchenStateMutex);
                outstanding = _kitchenOutstandingPizzas[selectedKitchenId];
                cookCount = _kitchenCookCount[selectedKitchenId];
            }
            if (outstanding >= 2 * cookCount) {
                selectedKitchenId = createKitchen(pizzeria);
            }
        }

        {
            std::lock_guard<std::mutex> kitchenLock(_kitchenStateMutex);
            _kitchenIPC[selectedKitchenId] << pizza; // on envoie la pizza à la cuisine sélectionnée
            _kitchenOutstandingPizzas[selectedKitchenId]++;
        }
    }
}

void Receptionist::printGreeting() const
{
    int randomAnswer = rand() % 6;

    switch (randomAnswer) {
        case 0:
            std::cout << "\nWelcome to Stracotto Pizza! What can we make for you?" << std::endl << std::endl;
            break;
        case 1:
            std::cout << "\nWelcome to Stracotto Pizza! Mamma mia, what delicious pizza(s) can we make for you today?" << std::endl << std::endl;
            break;
        case 2:
            std::cout << "\nWelcome to Stracotto Pizza! The ovens are hot and ready, what pizza(s) are you craving?" << std::endl << std::endl;
            break;
        case 3:
            std::cout << "\nWelcome to Stracotto Pizza! Extra cheese? Double steak? What's on your mind?" << std::endl << std::endl;
            break;
        case 4:
            std::cout << "\nWelcome to Stracotto Pizza, where every slice is a masterpiece! What do you wish for?" << std::endl << std::endl;
            break;
        case 5:
            std::cout << "\nWelcome to Stracotto Pizza! Pizza time! What tasty creation(s) would you like today?" << std::endl << std::endl;
            break;
    }
}

void Receptionist::handleOrder(Plazza::Pizzeria &pizzeria)
{
    std::string orderString;
    int orderID = 0;

    {
        std::lock_guard<std::mutex> lock(_watcherMutex);
        _watcherRunning = true;
    }
    _watcherThread = std::thread(&Receptionist::watchForCompletedOrders, this);

    while (true) {
        printGreeting();

        std::getline(std::cin, orderString);

        if (orderString.empty())
            continue;
        
        if (orderString == "exit") {
            {
                std::lock_guard<std::mutex> lock(_watcherMutex);
                _watcherRunning = false;
            }
            Plazza::Thread::join(_watcherThread);
            stopAllKitchens(true);
            break;
        }
        else if (orderString == "status") {
            std::vector<int> kitchenIds;
            {
                std::lock_guard<std::mutex> kitchenLock(_kitchenStateMutex);
                for (const auto &[kitchenId, ipc] : _kitchenIPC)
                    kitchenIds.push_back(kitchenId);
            }

            if (kitchenIds.empty()) {
                std::cout << "\nNo kitchens are currently open.\n";
                continue;
            }

           {
            std::lock_guard<std::mutex> kitchenLock(_kitchenStateMutex);
            for (auto iterator = kitchenIds.begin(); iterator != kitchenIds.end(); ) {
                const int kitchenId = *iterator;
                auto kitchen = _kitchenIPC.find(kitchenId);
                if (kitchen == _kitchenIPC.end()) {
                    iterator = kitchenIds.erase(iterator);
                    continue;
                }
                if (!kitchen->second->sendStatusRequestFromParent()) {
                    iterator = kitchenIds.erase(iterator);
                    continue;
                }
                ++iterator;
                }
            }

            if (kitchenIds.empty()) {
                std::cout << "\nNo kitchens are currently open.\n";
                continue;
            }

            std::cout << "\nCurrent kitchens status:\n";
            for (auto iterator = kitchenIds.begin(); iterator != kitchenIds.end(); ) {
                const int kitchenId = *iterator;
                int pendingPizzas = 0;
                int cookCount = 0;
                {
                    std::lock_guard<std::mutex> kitchenLock(_kitchenStateMutex);
                    auto iterator = _kitchenOutstandingPizzas.find(kitchenId);
                    if (iterator != _kitchenOutstandingPizzas.end())
                        pendingPizzas = iterator->second;
                    cookCount = _kitchenCookCount[kitchenId];
                }

                KitchenStatusSnapshot snapshot{};
                bool hasStatus = false;
                for (int retry = 0; retry < 50 && !hasStatus; ++retry) {
                {
                    std::lock_guard<std::mutex> kitchenLock(_kitchenStateMutex);
                    auto iterator = _kitchenIPC.find(kitchenId);
                    if (iterator == _kitchenIPC.end())
                        break;
                    hasStatus = (iterator->second >> snapshot);
                }
                if (!hasStatus)
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
                if (!hasStatus) {
                    ++iterator;
                    continue;
                }
                std::cout << "\n[KITCHEN #" << kitchenId << "]" << std::endl << std::endl;
                std::cout << pendingPizzas << " pizza(s) pending" << std::endl;
                std::cout << "Cooks: "<< std::endl;
                for (size_t j = 0; j < static_cast<size_t>(cookCount); j++) {
                    char working = 0;
                    if (hasStatus && j < snapshot.cookWorking.size())
                        working = snapshot.cookWorking[j];
                    std::cout << "    Cook #" << (j + 1) << ": " << (working ? "Working..." : "Sleeping...") << std::endl;
                }
                std::cout << "Ingredients:" << std::endl
                            << "    Dough:           " << (hasStatus ? snapshot.dough : -1) << std::endl
                            << "    Tomato:          " << (hasStatus ? snapshot.tomato : -1) << std::endl
                            << "    Gruyere:         " << (hasStatus ? snapshot.gruyere : -1) << std::endl
                            << "    Ham:             " << (hasStatus ? snapshot.ham : -1) << std::endl
                            << "    Mushroom:        " << (hasStatus ? snapshot.mushroom : -1) << std::endl
                            << "    Steak:           " << (hasStatus ? snapshot.steak : -1) << std::endl
                            << "    Eggplant:        " << (hasStatus ? snapshot.eggplant : -1) << std::endl
                            << "    Goat Cheese:     " << (hasStatus ? snapshot.goatCheese : -1) << std::endl
                            << "    Chief's Love:    " << (hasStatus ? snapshot.chiefLove : -1) << std::endl;
                ++iterator;
            }
            continue;
        }

        std::vector<std::shared_ptr<Plazza::IPizza>> pizzasInOrder;

        Plazza::PizzaHandler parser;

        pizzasInOrder = parser.checkPizzaOrder(orderString);

        if (pizzasInOrder.empty())
            continue;
        orderID++;
        if (orderID >= 1000)
            orderID = 1;
        for (auto &pizza : pizzasInOrder)
            pizza->setOrderId(orderID);

        std::cout << "You are order [#" << orderID << "], I'll call you when your pizza(s) are ready!" << std::endl;
        Plazza::Utilities::writeOutput("New order received: [#" + std::to_string(orderID) + "] " + orderString);
        sendPizzasToKitchens(pizzasInOrder, pizzeria);
        
    }
}

void Receptionist::watchForCompletedOrders()
{
    while (true) {
        {
            std::lock_guard<std::mutex> lock(_watcherMutex);
            if (!_watcherRunning)
                break;
        }

        checkDeadKitchens();
        receiveFinishedPizzasFromPipes();

        {
            std::lock_guard<std::mutex> lock(_ordersMutex);

            for (auto iterator = _expectedPizzaCount.begin(); iterator != _expectedPizzaCount.end(); ) {
                const int orderId = iterator->first;
                const size_t expectedCount = iterator->second;
                const size_t cookedCount = _cookedPizzaCount[orderId];

                if (cookedCount >= expectedCount) {
                    std::cout << "[ORDER #" << orderId << " COMPLETE] All pizzas are ready!\n";
                    Plazza::Utilities::writeOutput("Order [#" + std::to_string(orderId) + "] is complete.");
                    _orders.erase(orderId);
                    _cookedPizzaCount.erase(orderId);
                    iterator = _expectedPizzaCount.erase(iterator);
                } else {
                    ++iterator;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Receptionist::receiveFinishedPizzasFromPipes()
{
    std::vector<int> kitchenIds;
    {
        std::lock_guard<std::mutex> kitchenLock(_kitchenStateMutex);
        if (_kitchenIPC.empty())
            return;
        kitchenIds.reserve(_kitchenIPC.size());
        for (const auto &[kitchenId, ipc] : _kitchenIPC)
            kitchenIds.push_back(kitchenId);
    }

    for (const int kitchenId : kitchenIds) {
        while (true) {
            std::shared_ptr<Plazza::IPizza> finishedPizza;
            {
                std::lock_guard<std::mutex> kitchenLock(_kitchenStateMutex);
                auto iterator = _kitchenIPC.find(kitchenId);
                if (iterator == _kitchenIPC.end())
                    break;
                if (!(iterator->second >> finishedPizza))
                    break;
            }

            if (finishedPizza == nullptr)
                break;

            {
                std::lock_guard<std::mutex> lock(_ordersMutex);
                _cookedPizzaCount[finishedPizza->getOrderId()] += 1;
            }
            {
                std::lock_guard<std::mutex> kitchenLock(_kitchenStateMutex);
                auto iterator = _kitchenOutstandingPizzas.find(kitchenId);
                if (iterator != _kitchenOutstandingPizzas.end() && iterator->second > 0)
                    iterator->second--;
            }
        }
    }
}
}
