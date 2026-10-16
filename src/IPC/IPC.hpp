/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** IPC - Inter-Process Communication
*/

#pragma once

#include <unistd.h>
#include <utility>
#include <fcntl.h>
#include <stdexcept>
#include <memory>
#include <string>
#include <cstdint>
#include <vector>
#include <chrono>
#include <thread>

namespace Plazza {
    class IPizza;

    struct KitchenStatusSnapshot {
        int cookCount;
        std::vector<char> cookWorking;
        int dough;
        int tomato;
        int gruyere;
        int ham;
        int mushroom;
        int steak;
        int eggplant;
        int goatCheese;
        int chiefLove;
    };

    class IPC {
    public:
        IPC();
        ~IPC();

        void closeParentWrite();
        void closeParentRead();
        void closeChildRead();
        void closeChildWrite();
        void closeParentStatusRequestWrite();
        void closeParentStatusResponseRead();
        void closeChildStatusRequestRead();
        void closeChildStatusResponseWrite();


        void sendFromParent(const std::shared_ptr<IPizza> &pizza);
        std::shared_ptr<IPizza> receiveInChild(bool &pipeClosed);
        
        void sendFromChild(const std::shared_ptr<IPizza> &pizza);
        std::shared_ptr<IPizza> receiveInParent();

        bool sendStatusRequestFromParent();
        bool receiveStatusRequestInChild();
        void sendStatusFromChild(const KitchenStatusSnapshot &snapshot);
        bool receiveStatusInParent(KitchenStatusSnapshot &snapshot);

        void closeAll();
        std::string numberToBytes(uint32_t value);
        uint32_t bytesToNumber(const std::string &data, size_t offset);
        

    private:
        int _parentRead;
        int _parentWrite;
        int _childRead;
        int _childWrite;
        int _parentStatusRequestWrite;
        int _childStatusRequestRead;
        int _childStatusResponseWrite;
        int _parentStatusResponseRead;

        std::string pack(const std::shared_ptr<IPizza> &pizza);
        std::shared_ptr<IPizza> unpack(const std::string &serialized);

        void createPipes();
    };

    IPC &operator<<(const std::unique_ptr<IPC> &ipc, const std::shared_ptr<IPizza> &pizza);
    bool operator>>(const std::unique_ptr<IPC> &ipc, std::shared_ptr<IPizza> &pizza);
    IPC &operator<<(const std::unique_ptr<IPC> &ipc, const KitchenStatusSnapshot &snapshot);
    bool operator>>(const std::unique_ptr<IPC> &ipc, KitchenStatusSnapshot &snapshot);
}
