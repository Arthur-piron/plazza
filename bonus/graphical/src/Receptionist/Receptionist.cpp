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
#include "Renderer.hpp"

namespace Plazza {

static std::string buildOrderRecap(const std::vector<std::shared_ptr<Plazza::IPizza>> &pizzas)
{
    std::vector<std::pair<Plazza::PizzaSize, Plazza::PizzaType>> order;
    std::map<std::pair<Plazza::PizzaSize, Plazza::PizzaType>, int> counts;

    for (const auto &pizza : pizzas) {
        auto key = std::make_pair(pizza->getSize(), pizza->getType());

        if (counts.find(key) == counts.end())
            order.push_back(key);
        counts[key] += 1;
    }

    Plazza::PizzaHandler parser;
    std::ostringstream summary;

    for (const auto &key : order) {
        summary << parser.pizzaTypeToString(key.second)
            << " " << parser.pizzaSizeToString(key.first)
            << " x" << counts[key] << '\n';
    }

    return summary.str();
}

Receptionist::Receptionist()
{
}

Receptionist::~Receptionist()
{
    stopWatcher();
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
                snapshot.cookStates.assign(snapshot.cookCount, 0);
                snapshot.cookChiefKissTokens.assign(snapshot.cookCount, 0);
                for (int i = 0; i < snapshot.cookCount; ++i) {
                    snapshot.cookWorking[i] = childKitchen.getThreadPool().isCooking(i) ? 1 : 0;
                    snapshot.cookStates[i] = static_cast<char>(childKitchen.getThreadPool().getCookState(i));
                    snapshot.cookChiefKissTokens[i] = childKitchen.getThreadPool().getCookChiefKissToken(i);
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

void Receptionist::watchForCompletedOrders(Renderer &renderer)
{
    int pollTick = 0;

    while (true) {
        {
            std::lock_guard<std::mutex> lock(_watcherMutex);
            if (!_watcherRunning)
                break;
        }

        checkDeadKitchens();
        receiveFinishedPizzasFromPipes();

        pollTick++;
        if (pollTick % 20 == 0)
            pollKitchenSnapshots();

        {
            std::lock_guard<std::mutex> lock(_ordersMutex);
            for (auto it = _expectedPizzaCount.begin(); it != _expectedPizzaCount.end(); ) {
                const int orderId = it->first;
                if (_cookedPizzaCount[orderId] >= it->second) {
                    std::cout << "[ORDER #" << orderId << " COMPLETE] All pizzas are ready!\n";
                    Plazza::Utilities::writeOutput("Order [#" + std::to_string(orderId) + "] is complete.");
                    renderer.markOrderFinished(orderId);
                    _orders.erase(orderId);
                    _cookedPizzaCount.erase(orderId);
                    it = _expectedPizzaCount.erase(it);
                } else
                    it++;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void Receptionist::pollKitchenSnapshots()
{
    std::vector<int> kitchenIds;
    {
        std::lock_guard<std::mutex> lock(_kitchenStateMutex);

        for (const auto &[id, _] : _kitchenIPC)
            kitchenIds.push_back(id);
    }

    for (int id : kitchenIds) {
        {
            std::lock_guard<std::mutex> lock(_kitchenStateMutex);

            auto it = _kitchenIPC.find(id);
            if (it == _kitchenIPC.end())
                continue;
            it->second->sendStatusRequestFromParent();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        KitchenStatusSnapshot snap{};
        bool valid = false;

        {
            std::lock_guard<std::mutex> lock(_kitchenStateMutex);
            auto it = _kitchenIPC.find(id);
            if (it != _kitchenIPC.end())
                valid = (it->second >> snap);
        }

        if (valid) {
            std::lock_guard<std::mutex> lock(_snapshotMutex);
            _lastSnapshots[id] = snap;
        }
    }

    std::lock_guard<std::mutex> lock(_snapshotMutex);
    for (auto it = _lastSnapshots.begin(); it != _lastSnapshots.end(); ) {
        std::lock_guard<std::mutex> klock(_kitchenStateMutex);

        if (_kitchenIPC.find(it->first) == _kitchenIPC.end())
            it = _lastSnapshots.erase(it);
        else
            it++;
    }
}

void Receptionist::startWatcher(Renderer &renderer)
{
    {
        std::lock_guard<std::mutex> lock(_watcherMutex);
        _watcherRunning = true;
    }
    _watcherThread = std::thread(&Receptionist::watchForCompletedOrders, this, std::ref(renderer));
}

void Receptionist::stopWatcher()
{
    {
        std::lock_guard<std::mutex> lock(_watcherMutex);
        _watcherRunning = false;
    }

    if (_watcherThread.joinable())
        _watcherThread.join();
}

void Receptionist::processOrder(const std::string &orderString,
    Plazza::Pizzeria &pizzeria, Renderer &renderer)
{
    if (orderString == "exit") {
        Plazza::Utilities::writeOutput("Exiting...");
        _exitRequested = true;
        return;
    }

    if (orderString == "status") {
        std::lock_guard<std::mutex> lock(_snapshotMutex);
        if (_lastSnapshots.empty())
            Plazza::Utilities::writeOutput("No kitchens active.");
        else {
            Plazza::Utilities::writeOutput("=== KITCHEN STATUS ===");

            for (const auto &[kitchenId, snap] : _lastSnapshots) {
                Plazza::Utilities::writeOutput("Kitchen #" + std::to_string(kitchenId) + ":");

                for (int i = 0; i < snap.cookCount; ++i) {
                    std::string state;
                    if (snap.cookStates[i] == 0)
                        state = "FREE";
                    else if (snap.cookStates[i] == 1)
                        state = "COOKING";
                    else if (snap.cookStates[i] == 2)
                        state = "WAITING_FOR_REFILL";
                    else
                        state = "UNKNOWN";
                    Plazza::Utilities::writeOutput("  Cook " + std::to_string(i + 1) + ": " + state);
                }
                Plazza::Utilities::writeOutput("  Ingredients: dough=" + std::to_string(snap.dough)
                    + " tomato=" + std::to_string(snap.tomato)
                    + " gruyere=" + std::to_string(snap.gruyere)
                    + " ham=" + std::to_string(snap.ham)
                    + " mushroom=" + std::to_string(snap.mushroom)
                    + " steak=" + std::to_string(snap.steak)
                    + " eggplant=" + std::to_string(snap.eggplant)
                    + " goatCheese=" + std::to_string(snap.goatCheese)
                    + " chiefLove=" + std::to_string(snap.chiefLove));
            }
            Plazza::Utilities::writeOutput("======================");
        }
        return;
    }

    Plazza::PizzaHandler parser;
    std::vector<std::shared_ptr<Plazza::IPizza>> pizzasInOrder = parser.checkPizzaOrder(orderString);

    if (pizzasInOrder.empty()) {
        renderer.showTransientMessage("We don't make that here.", 2.f);
        return;
    }
    if (_orderID >= 1000)
        _orderID = 1;
    for (auto &pizza : pizzasInOrder)
        pizza->setOrderId(_orderID);

    renderer.spawnClient(buildOrderRecap(pizzasInOrder), _orderID);
    Plazza::Utilities::writeOutput("New order received: [#" + std::to_string(_orderID) + "] " + orderString);
    sendPizzasToKitchens(pizzasInOrder, pizzeria);
    _orderID++;
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

std::map<int, KitchenStatusSnapshot> Receptionist::getSnapshots()
{
    std::lock_guard<std::mutex> lock(_snapshotMutex);

    return _lastSnapshots;
}

int Receptionist::getKitchenCount()
{
    std::lock_guard<std::mutex> lock(_kitchenStateMutex);

    return static_cast<int>(_kitchenIPC.size());
}

}
