#ifndef PARSER_HPP
#define PARSER_HPP
#endif

#pragma once


#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <map>
#include <variant>
#include "message.hpp"
#define NANOSECONDS_PER_HOUR 3600 * 1e9


using Data = std::variant<char, uint16_t, uint32_t, uint64_t, double>;


class Parser{
    std::string fp;
    std::string rawTradesFilePath = "/workspaces/itch-5.0-processing/raw/raw_trades.csv";
    std::string openOrdersFilePath = "/workspaces/itch-5.0-processing/raw/open_orders.csv";
    std::map<uint16_t, std::string>stock_map;
    std::map<uint16_t, std::map<uint64_t, std::vector<Data>>> orders, trades;

    void writeRawInfo(){

        std::ofstream rawTrades;
        std::ofstream openOrders;
        std::string name;
        rawTrades.open(rawTradesFilePath);
        openOrders.open(openOrdersFilePath);

        // Printing Header
        rawTrades << "name,ts,vol,price,\n";
        openOrders << "name,ts,vol,price,\n";

        for(auto& [stockLocate, stockTrades] : trades){
            name = stock_map[stockLocate];
            for(auto& [matchNumber, trade] : stockTrades){
                rawTrades << name << ",";
                for(auto& tradeEl : trade){
                    std::visit([&rawTrades](auto&& value) {
                        rawTrades << value << ",";  // Write each value followed by a comma
                    }, tradeEl);
                }
                rawTrades << "\n";

                //rawTrades << name << "," << (trade)[0] << "," << (trade)[1] << "," << (trade)[3] << ",\n";
            }
        }

        for(auto& [stockLocate, stockOrders] : orders){
            name = stock_map[stockLocate];
            for(auto& [orderRefNumber, order] : stockOrders){
                openOrders << name << ",";
                for(auto& orderEl : order){
                    std::visit([&openOrders](auto&& value) {
                        openOrders << value << ",";  // Write each value followed by a comma
                    }, orderEl);
                }
                openOrders << "\n";

                //rawTrades << name << "," << (trade)[0] << "," << (trade)[1] << "," << (trade)[3] << ",\n";
            }
        }
    }

    public:
    Parser(std::string fp) : fp(fp) {
        stock_map.clear();
        orders.clear();
        trades.clear();
    };

    void parse(){
        std::ifstream binFile(fp, std::ios::binary);

        if(!binFile){
            std::cerr << "Error loading the binary file" << std::endl;
        }
        char messageType;
        std::vector<Data> order, newOrder, trade;

        while(binFile.read(&messageType, 1)) {
            auto it = packet_sizes.find(messageType);
            if (it != packet_sizes.end()){
                // if (messageType == 'S') {
                //     SystemEvent msg;
                //     msg.load(binFile);
                // } 
                if (messageType == 'R') {
                    StockDirectory msg;
                    msg.load(binFile);
                    stock_map[msg.stockLocate] = msg.stock;
                }
                // else if (messageType == 'H') {
                //     StockTradingAction msg;
                //     msg.load(binFile);
                // }
                // else if (messageType == 'Y') {
                //     RegSHOShortSalePriceTestIndicator msg;
                //     msg.load(binFile);
                // } 
                // else if (messageType == 'L') {
                //     MarketParticipationPos msg;
                //     msg.load(binFile);
                // }
                // else if (messageType == 'V') {
                //     MWCBDecline msg;
                //     msg.load(binFile);
                // } 
                // else if (messageType == 'W') {
                //     MWCBStatus msg;
                //     msg.load(binFile);
                // } 
                // else if (messageType == 'K') {
                //     QuotingPeriodUpdate msg;
                //     msg.load(binFile);
                // } 
                // else if (messageType == 'J') {
                //     LULDAuctionCollar msg;
                //     msg.load(binFile);
                // } 
                // else if (messageType == 'h') {
                //     OpeartionalHalt msg;
                //     msg.load(binFile);
                // } 
                else if (messageType == 'A') {
                    AddOrderNoMPID msg;
                    msg.load(binFile);
                    if(msg.buySellIndicator == 'B'){

                        // If Order not found earlier
                        if(orders[msg.stockLocate].find(msg.orderRefNumber) == orders[msg.stockLocate].end()){
                            order = {msg.timestamp, msg.shares, msg.price};
                            orders[msg.stockLocate][msg.orderRefNumber] = order;
                        }
                        else {
                            std::cerr << "[AddOrderNoMPID] Order Ref " << msg.orderRefNumber << " was already in queue" << std::endl;
                        }
                    }
                } 
                else if (messageType == 'F') {
                    AddOrderWithMPID msg;
                    msg.load(binFile);

                    if(msg.buySellIndicator == 'B'){

                        // If Order not found earlier
                        if(orders[msg.stockLocate].find(msg.orderRefNumber) != orders[msg.stockLocate].end()){
                            std::cerr << "[AddOrderWithMPID] Order Ref " << msg.orderRefNumber << " already exists!" << std::endl;
                        }
                        else {
                            order = {msg.timestamp, msg.shares, msg.price};
                            orders[msg.stockLocate][msg.orderRefNumber] = order;
                        }
                    }
                } 
                else if (messageType == 'E') {
                    OrderExecuted msg;
                    msg.load(binFile);
                    if(orders[msg.stockLocate].find(msg.orderRefNumber) == orders[msg.stockLocate].end()){
                        // std::cerr << "[OrderExecuted] Order Ref " << msg.orderRefNumber << " not found!" << std::endl;
                    }
                    else {
                        order = orders[msg.stockLocate][msg.orderRefNumber];
                        uint32_t dVol = std::get<uint32_t>(order[1]) - msg.executedShares;
                        if(dVol >= 0){
                            trades[msg.stockLocate][msg.matchNumber] = {msg.timestamp, msg.executedShares, std::get<double>(order[2])};
                            if(dVol > 0){
                                order[1] = dVol;
                                orders[msg.stockLocate][msg.orderRefNumber] = order;
                            }
                            else{
                                orders[msg.stockLocate].erase(msg.orderRefNumber);
                            }
                        }
                    }
                } 
                else if (messageType == 'C') {
                    OrderExecutedWithPrice msg;
                    msg.load(binFile);
                    if(orders[msg.stockLocate].find(msg.orderRefNumber) == orders[msg.stockLocate].end()){
                        // std::cerr << "[OrderExecutedWithPrice] Order Ref " << msg.orderRefNumber << " not found!" << std::endl;
                    }
                    else {
                        order = orders[msg.stockLocate][msg.orderRefNumber];
                        uint32_t dVol = std::get<uint32_t>(order[1]) - msg.executedShares;
                        if(dVol >= 0){
                            if(msg.printable == 'Y'){
                                trades[msg.stockLocate][msg.matchNumber] = {msg.timestamp, msg.executedShares, msg.executionPrice};
                            }
                            if(dVol > 0){
                                order[1] = dVol;
                                orders[msg.stockLocate][msg.orderRefNumber] = order;
                            }
                            else{
                                orders[msg.stockLocate].erase(msg.orderRefNumber);
                            }
                        }
                    }

                } 
                else if (messageType == 'X') {
                    OrderCancel msg;
                    msg.load(binFile);
                    if(orders[msg.stockLocate].find(msg.orderRefNumber) == orders[msg.stockLocate].end()){
                        // std::cerr << "[OrderCancel] Order Ref " << msg.orderRefNumber << " not found!" << std::endl;
                    }
                    else {
                        order = orders[msg.stockLocate][msg.orderRefNumber];
                        uint32_t dVol = std::get<uint32_t>(order[1]) - msg.cancelledShares;
                        if(dVol >= 0){
                            if(dVol > 0){
                                order[1] = dVol;
                                orders[msg.stockLocate][msg.orderRefNumber] = order;
                            }
                            else{
                                orders[msg.stockLocate].erase(msg.orderRefNumber);
                            }
                        }
                    } 
                }
                else if (messageType == 'D') {
                    OrderDelete msg;
                    msg.load(binFile);
                    if(orders[msg.stockLocate].find(msg.orderRefNumber) == orders[msg.stockLocate].end()){
                        // std::cerr << "[OrderDelete] Order Ref " << msg.orderRefNumber << " not found!" << std::endl;
                    }
                    else{
                        orders[msg.stockLocate].erase(msg.orderRefNumber);
                    }
                } 
                else if (messageType == 'U') {
                    OrderReplace msg;
                    msg.load(binFile);
                    if(orders[msg.stockLocate].find(msg.originalOrderRefNumber) == orders[msg.stockLocate].end()){
                        // std::cerr << "[OrderReplace] Order Ref " << msg.originalOrderRefNumber << " not found!" << std::endl;
                    }
                    else{
                        orders[msg.stockLocate].erase(msg.originalOrderRefNumber);
                        newOrder = {msg.timestamp, msg.shares, msg.price};
                        orders[msg.stockLocate][msg.newOrderRefNumber] = newOrder;
                    }
                } 
                else if (messageType == 'P') {
                    NonCrossTrade msg;
                    msg.load(binFile);
                    if(trades[msg.stockLocate].find(msg.matchNumber) != trades[msg.stockLocate].end()){
                        std::cerr << "[NonCrossTrade] Match Number " << msg.matchNumber << " already exists!" << std::endl;
                    }
                    else if(msg.buySellIndicator == 'B'){
                        trade = {msg.timestamp, msg.shares, msg.price};
                        trades[msg.stockLocate][msg.matchNumber] = trade;
                    }
                } 
                else if (messageType == 'Q') {
                    CrossTrade msg;
                    msg.load(binFile);
                    if(trades[msg.stockLocate].find(msg.matchNumber) != trades[msg.stockLocate].end()){
                        std::cerr << "[CrossTrade] Match Number " << msg.matchNumber << " already exists!" << std::endl;
                    }
                    else {
                        trade = {msg.timestamp, msg.shares, msg.crossPrice};
                        trades[msg.stockLocate][msg.matchNumber] = trade;
                    }

                } 
                else if (messageType == 'B') {
                    BrokenTrade msg;
                    msg.load(binFile);
                    if(trades[msg.stockLocate].find(msg.matchNumber) == trades[msg.stockLocate].end()){
                        // std::cerr << "[BrokenTrade] Match Number " << msg.matchNumber << " not found!" << std::endl;
                    }
                    else{
                        trades[msg.stockLocate].erase(msg.matchNumber);
                    }
                } 
                // else if (messageType == 'I') {
                //     NetOrderImbalance msg;
                //     msg.load(binFile);
                // }
                else{
                    char packet[it->second];
                    binFile.read(packet, it->second);
                    continue;
                }
            }
        }
        binFile.close();

        // Write Raw Data
        writeRawInfo();

    }

};