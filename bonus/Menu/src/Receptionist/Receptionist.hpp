/*
** EPITECH PROJECT, 2026
** G-CCP-400-LIL-4-1-theplazza-6
** File description:
** Receptionist
*/

#ifndef RECEPTIONIST_HPP_
#define RECEPTIONIST_HPP_

#include "PizzaHandler.hpp"
#include "Utilities.hpp"
#include "Kitchen.hpp"
#include <vector>
#include <set>
#include "Kitchen.hpp"
#include <climits>
#include <map>
#include <memory>
#include <sys/types.h>

#include "IPizza.hpp"
#include "IPC/IPC.hpp"
#include "process.hpp"
namespace Plazza {

    class Receptionist {
    public:
        Receptionist();
        ~Receptionist();
        void sendPizzasToKitchens(std::vector<std::shared_ptr<Plazza::IPizza>> &pizzas, const Plazza::Pizzeria &pizzeria);
        void handleOrder(Plazza::Pizzeria &pizzeria);
        void printGreeting() const;
        int createKitchen(const Plazza::Pizzeria &pizzeria);
        void watchForCompletedOrders();
        int getLowestWorkload();
        void receiveFinishedPizzasFromPipes();
        void stopAllKitchens(bool forceKill);
        void checkDeadKitchens();
        void printReceptionistHelp();
        void printMenu();


    protected:
    private:
        std::map<int, std::vector<std::shared_ptr<Plazza::IPizza>>> _orders;
        std::map<int, size_t> _expectedPizzaCount;
        std::map<int, size_t> _cookedPizzaCount;
        std::mutex _ordersMutex;
        std::thread _watcherThread;
        bool _watcherRunning = false;
        std::mutex _watcherMutex;

        std::mutex _kitchenStateMutex;
        int _nextKitchenId = 1;
        std::set<int> _availableKitchenIds;
        std::map<int, std::unique_ptr<IPC>> _kitchenIPC;
        std::map<int, pid_t> _kitchenPids;
        std::map<int, int> _kitchenOutstandingPizzas;
        std::map<int, int> _kitchenCookCount;

    };
}

#endif /* !RECEPTIONIST_HPP_ */
