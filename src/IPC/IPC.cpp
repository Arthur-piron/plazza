/*
** EPITECH PROJECT, 2026
** PIZZA_DE_LA_MAMA
** File description:
** IPC
*/

#include "IPC.hpp"
#include "encapsulation/fcntl/fcntl.hpp"
#include "encapsulation/read/read.hpp"
#include "encapsulation/write/write.hpp"
#include "PizzaFactory.hpp"
#include <cstring>
#include <cstdint>


namespace Plazza {
    std::string IPC::numberToBytes(uint32_t value)
    {
        std::string result(4, '\0');
        std::memcpy(result.data(), &value, sizeof(uint32_t));
        return result;
    }

    uint32_t IPC::bytesToNumber(const std::string &data, size_t offset)
    {
        uint32_t value = 0;
        std::memcpy(&value, data.data() + offset, sizeof(uint32_t));
        return value;
    }

    IPC::IPC() : _parentRead(-1), _parentWrite(-1), _childRead(-1), _childWrite(-1),
        _parentStatusRequestWrite(-1), _childStatusRequestRead(-1), _childStatusResponseWrite(-1), _parentStatusResponseRead(-1)
    {
        createPipes();
    }

    IPC::~IPC()
    {
        closeAll();
    }


    std::shared_ptr<IPizza> IPC::unpack(const std::string &serialized)
    {
        if (serialized.length() < 12)
            return nullptr;
        
        PizzaFactory factory;
        uint32_t type = bytesToNumber(serialized, 0);
        uint32_t size = bytesToNumber(serialized, 4);
        uint32_t orderId = bytesToNumber(serialized, 8);

        auto pizza = factory.createPizza((PizzaType)type, (PizzaSize)size);
        pizza->setOrderId(orderId);
        return pizza;
    }

    std::string IPC::pack(const std::shared_ptr<IPizza> &pizza)
    {
        return numberToBytes(pizza->getType()) + 
               numberToBytes(pizza->getSize()) + 
               numberToBytes(pizza->getOrderId());
    }

    void IPC::createPipes()
    {
        int pipeFd1[2];
        int pipeFd2[2];
        int pipeFd3[2];
        int pipeFd4[2];

        if (pipe(pipeFd1) == -1 || pipe(pipeFd2) == -1 || pipe(pipeFd3) == -1 || pipe(pipeFd4) == -1) {
            throw std::runtime_error("Failed to create pipes");
        }

        _parentWrite = pipeFd1[1];
        _childRead = pipeFd1[0];

        _childWrite = pipeFd2[1];
        _parentRead = pipeFd2[0];

        _parentStatusRequestWrite = pipeFd3[1];
        _childStatusRequestRead = pipeFd3[0];

        _childStatusResponseWrite = pipeFd4[1];
        _parentStatusResponseRead = pipeFd4[0];

        Fcntl::set_NonBlocking(_childRead);
        Fcntl::set_NonBlocking(_parentRead);
        Fcntl::set_NonBlocking(_childStatusRequestRead);
        Fcntl::set_NonBlocking(_parentStatusResponseRead);
    }

    void IPC::closeParentWrite()
    {
        if (_parentWrite != -1) {
            close(_parentWrite);
            _parentWrite = -1;
        }
    }

    void IPC::closeParentRead()
    {
        if (_parentRead != -1) {
            close(_parentRead);
            _parentRead = -1;
        }
    }

    void IPC::closeChildRead()
    {
        if (_childRead != -1) {
            close(_childRead);
            _childRead = -1;
        }
    }

    void IPC::closeChildWrite()
    {
        if (_childWrite != -1) {
            close(_childWrite);
            _childWrite = -1;
        }
    }

    void IPC::closeParentStatusRequestWrite()
    {
        if (_parentStatusRequestWrite != -1) {
            close(_parentStatusRequestWrite);
            _parentStatusRequestWrite = -1;
        }
    }

    void IPC::closeParentStatusResponseRead()
    {
        if (_parentStatusResponseRead != -1) {
            close(_parentStatusResponseRead);
            _parentStatusResponseRead = -1;
        }
    }

    void IPC::closeChildStatusRequestRead()
    {
        if (_childStatusRequestRead != -1) {
            close(_childStatusRequestRead);
            _childStatusRequestRead = -1;
        }
    }

    void IPC::closeChildStatusResponseWrite()
    {
        if (_childStatusResponseWrite != -1) {
            close(_childStatusResponseWrite);
            _childStatusResponseWrite = -1;
        }
    }



    void IPC::sendFromParent(const std::shared_ptr<IPizza> &pizza)
    {
        std::string packed = pack(pizza);
        Write::Write_data(_parentWrite, packed);
    }

    std::shared_ptr<IPizza> IPC::receiveInChild(bool &pipeClosed)
    {
        std::string buffer(12, '\0');
        ssize_t bytesRead = Read_data::read_data(_childRead, buffer);

        if (bytesRead == 0) {
            pipeClosed = true;
            return nullptr;
        }
        if (bytesRead < 0) {
            pipeClosed = false;
            return nullptr;
        }

        pipeClosed = false;
        
        return unpack(buffer);
    }

    void IPC::sendFromChild(const std::shared_ptr<IPizza> &pizza)
    {
        std::string packed = pack(pizza);
        Write::Write_data(_childWrite, packed);
    }

    std::shared_ptr<IPizza> IPC::receiveInParent()
    {
        std::string buffer(12, '\0');
        ssize_t bytesRead = Read_data::read_data(_parentRead, buffer);
        
        if (bytesRead <= 0)
            return nullptr;
        
        return unpack(buffer);
    }

    bool IPC::sendStatusRequestFromParent()
    {
        std::string request(1, '\1');
        return Write::Write_data(_parentStatusRequestWrite, request) > 0;
    }

    bool IPC::receiveStatusRequestInChild()
    {
        std::string request(1, '\0');
        ssize_t bytesRead = Read_data::read_data(_childStatusRequestRead, request);

        if (bytesRead <= 0)
            return false;
        return request[0] == 1;
    }

    void IPC::sendStatusFromChild(const KitchenStatusSnapshot &snapshot)
    {
        std::string buffer;
        buffer.reserve(sizeof(int) + snapshot.cookCount + 9 * sizeof(int));
        buffer.append(reinterpret_cast<const char*>(&snapshot.cookCount), sizeof(int));
        for (int i = 0; i < snapshot.cookCount; ++i) {
            char working = snapshot.cookWorking[i];
            buffer.append(&working, sizeof(char));
        }
        buffer.append(reinterpret_cast<const char*>(&snapshot.dough), sizeof(int));
        buffer.append(reinterpret_cast<const char*>(&snapshot.tomato), sizeof(int));
        buffer.append(reinterpret_cast<const char*>(&snapshot.gruyere), sizeof(int));
        buffer.append(reinterpret_cast<const char*>(&snapshot.ham), sizeof(int));
        buffer.append(reinterpret_cast<const char*>(&snapshot.mushroom), sizeof(int));
        buffer.append(reinterpret_cast<const char*>(&snapshot.steak), sizeof(int));
        buffer.append(reinterpret_cast<const char*>(&snapshot.eggplant), sizeof(int));
        buffer.append(reinterpret_cast<const char*>(&snapshot.goatCheese), sizeof(int));
        buffer.append(reinterpret_cast<const char*>(&snapshot.chiefLove), sizeof(int));

        Write::Write_data(_childStatusResponseWrite, buffer);
    }

    bool IPC::receiveStatusInParent(KitchenStatusSnapshot &snapshot)
    {
        int cookCount = 0;
        std::string header(sizeof(int), '\0');
        ssize_t headerRead = Read_data::read_data(_parentStatusResponseRead, header);
        if (headerRead <= 0)
            return false;
        std::memcpy(&cookCount, header.data(), sizeof(int));
        snapshot.cookCount = cookCount;

        size_t remaining = static_cast<size_t>(cookCount) * sizeof(char) + 9 * sizeof(int);
        std::string buffer(remaining, '\0');
        size_t totalRead = 0;

        for (int retry = 0; retry < 100 && totalRead < remaining; ++retry) {
            std::string tmp(remaining - totalRead, '\0');
            ssize_t n = Read_data::read_data(_parentStatusResponseRead, tmp);
            if (n > 0) {
                buffer.replace(totalRead, n, tmp, 0, n);
                totalRead += n;
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        if (totalRead < remaining)
            return false;

        size_t offset = 0;
        snapshot.cookWorking.clear();
        snapshot.cookWorking.resize(cookCount);
        for (int i = 0; i < cookCount; ++i) {
            snapshot.cookWorking[i] = buffer[offset];
            offset += sizeof(char);
        }
        auto readIntFromBuffer = [&](int &out) {
            std::memcpy(&out, buffer.data() + offset, sizeof(int));
            offset += sizeof(int);
        };
        readIntFromBuffer(snapshot.dough);
        readIntFromBuffer(snapshot.tomato);
        readIntFromBuffer(snapshot.gruyere);
        readIntFromBuffer(snapshot.ham);
        readIntFromBuffer(snapshot.mushroom);
        readIntFromBuffer(snapshot.steak);
        readIntFromBuffer(snapshot.eggplant);
        readIntFromBuffer(snapshot.goatCheese);
        readIntFromBuffer(snapshot.chiefLove);

        return true;
    }

    void IPC::closeAll()
    {
        closeParentRead();
        closeParentWrite();
        closeChildRead();
        closeChildWrite();
        closeParentStatusRequestWrite();
        closeChildStatusRequestRead();
        closeChildStatusResponseWrite();
        closeParentStatusResponseRead();
    }

    IPC &operator<<(const std::unique_ptr<IPC> &ipc, const std::shared_ptr<IPizza> &pizza)
    {
        ipc->sendFromParent(pizza);
        return *ipc;
    }

    bool operator>>(const std::unique_ptr<IPC> &ipc, std::shared_ptr<IPizza> &pizza)
    {
        pizza = ipc->receiveInParent();
        return pizza != nullptr;
    }

    IPC &operator<<(const std::unique_ptr<IPC> &ipc, const KitchenStatusSnapshot &snapshot)
    {
        ipc->sendStatusFromChild(snapshot);
        return *ipc;
    }

    bool operator>>(const std::unique_ptr<IPC> &ipc, KitchenStatusSnapshot &snapshot)
    {
        return ipc->receiveStatusInParent(snapshot);
    }
}
