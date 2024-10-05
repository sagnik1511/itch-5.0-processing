#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include "message.hpp"
#define NANOSECONDS_PER_HOUR 3600 * 1e9


int main() {

    std::ifstream binFile("01302019.NASDAQ_ITCH50", std::ios::binary);

    if(!binFile){
        std::cerr << "Error loading the binary file" << std::endl;
        return 1;
    }
    else{
        std::cout << NANOSECONDS_PER_HOUR << std::endl;
    }
    bool flag = true;
    char messageType;
    baseMessage *msg = nullptr;

    while(binFile.read(&messageType, 1)) {
        auto it = packet_sizes.find(messageType);
        if (it != packet_sizes.end()){
            if (messageType == 'S') {
                msg = new SystemEvent();
            } 
            else if (messageType == 'R') {
                msg = new StockDirectory();
            }
            else if (messageType == 'H') {
                msg = new StockTradingAction();
            }
            else if (messageType == 'Y') {
                msg = new RegSHOShortSalePriceTestIndicator();
            } 
            else if (messageType == 'L') {
                msg = new MarketParticipationPos();
            }
            else if (messageType == 'V') {
                msg = new MWCBDecline();
            } 
            else if (messageType == 'W') {
                msg = new MWCBStatus();
            } 
            else if (messageType == 'K') {
                msg = new QuotingPeriodUpdate();
            } 
            else if (messageType == 'J') {
                msg = new LULDAuctionCollar();
            } 
            else if (messageType == 'h') {
                msg = new OpeartionalHalt();
            } 
            else if (messageType == 'A') {
                msg = new AddOrderNoMPID();
            } 
            else if (messageType == 'F') {
                msg = new AddOrderWithMPID();
            } 
            else if (messageType == 'E') {
                msg = new OrderExecuted();
            } 
            else if (messageType == 'C') {
                msg = new OrderExecutedWithPrice();
            } 
            else if (messageType == 'X') {
                msg = new OrderCancel();
            } 
            else if (messageType == 'D') {
                msg = new OrderDelete();
            } 
            else if (messageType == 'U') {
                msg = new OrderReplace();
            } 
            else if (messageType == 'P') {
                msg = new NonCrossTrade();
            } 
            else if (messageType == 'Q') {
                msg = new CrossTrade();
            } 
            else if (messageType == 'B') {
                msg = new BrokenTrade();
            } 
            else if (messageType == 'I') {
                msg = new NetOrderImbalance();
            }
            else{
                char packet[it->second];
                binFile.read(packet, it->second);
                continue;
            }

            if(flag){
                msg->load(binFile);
                msg->show();
                break;
            }
        }
    }

    binFile.close();

    return 0;
}