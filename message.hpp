#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#endif

#pragma once
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include "utils.hpp"


const std::map<char, int> packet_sizes = {
    {'S', 11},  // System Event Message
    {'R', 38},  // Stock Directory Message
    {'H', 24},  // Stock Trading Action
    {'Y', 19},  // Reg SHO Short Sale Price Test RestrictedIndicator Message
    {'L', 25},  // Market Participant Position Message
    {'V', 34},  // MWCB Decline Level Message
    {'W', 11},  // MWCB Status Message
    {'K', 27},  // Quoting Period Update
    {'J', 34},  // Limit Up – Limit Down (LULD) Auction Collar Message
    {'h', 20},  // Operational Halt Message
    {'A', 35},  // Order Message (Add)
    {'F', 39},  // Order Message (Add with mpid)
    {'E', 30},  // Order Message (Execution)
    {'C', 35},  // Order Message (Execution with updated price)
    {'X', 22},  // Order Message (Cancel)
    {'D', 18},  // Order Message (Delete)
    {'U', 34},  // Order Message (Replace)
    {'P', 43},  // Trade Message (Non-Cross)
    {'Q', 39},  // Trade Message (Cross)
    {'B', 18},  // Trade Message (Broken)
    {'I', 49},  // Net Order Imbalance Indicator (NOII) Message
    {'O', 47},  // Direct Listing with Capital Raise Price Discovery Message
    {'N', 19}   // Retail Price Improvement Indicator(RPII)
};

#pragma pack(push, 1)

struct baseMessage{

    public :
    virtual void load(std::ifstream &file) = 0;
    virtual void show() = 0;
};

struct SystemEvent : public baseMessage {
    public:
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    char eventCode;

    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        file.read(&eventCode, sizeof(eventCode));
    }

    void show() {
        std::cout << "Stock Locate -> " << stockLocate << std::endl;
        std::cout << "Tracking Number -> " << trackingNumber << std::endl;
        std::cout << "Timestamp -> " << timestamp << std::endl;
        std::cout << "Event Code -> " << eventCode << std::endl;
    }   
};


// At the start of each tradingday, Nasdaq disseminates stock directory messages for allactive symbolsin the Nasdaq execution system.
// Market data redistributors should process this message to populate the FinancialStatusIndicator (required display
// field) and the Market Category (recommended display field) for Nasdaq listed issues
struct StockDirectory : public baseMessage {
    public:
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    std::string stock;
    char marketCategory;
    char finStatus;
    uint32_t roundLotSize;
    char roundLotsOnly;
    char issueClassification;
    std::string issueSubType;
    char authenticity;
    char shortSaleThreshIndicator;
    char ipoFlag;
    char LULDRefPriceTier;
    char etpFlag;
    uint32_t etpLeverageFactor;
    char invIndicator;

    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        stock = readStock(file);
        file.read(&marketCategory, sizeof(marketCategory));
        file.read(&finStatus, sizeof(finStatus));
        roundLotSize = readBigEndianInteger(file, 4);
        file.read(&roundLotsOnly, sizeof(roundLotsOnly));
        file.read(&issueClassification, sizeof(issueClassification));
        issueSubType = readString(file, 2);
        file.read(&authenticity, sizeof(authenticity));
        file.read(&shortSaleThreshIndicator, sizeof(shortSaleThreshIndicator));
        file.read(&ipoFlag, sizeof(ipoFlag));
        file.read(&LULDRefPriceTier, sizeof(LULDRefPriceTier));
        file.read(&etpFlag, sizeof(etpFlag));
        etpLeverageFactor = readBigEndianInteger(file, 4);
        file.read(&invIndicator, sizeof(invIndicator));
    }
    void show()  {
        std::cout << "Stock Locate -> " << stockLocate << std::endl;
        std::cout << "Tracking Number -> " << trackingNumber << std::endl;
        std::cout << "Timestamp -> " << timestamp << std::endl;
        std::cout << "Stock -> " << stock << std::endl;
        std::cout << "Market Category -> " << marketCategory << std::endl;
        std::cout << "Financial Status -> " << finStatus << std::endl;
        std::cout << "Round Lot Size -> " << roundLotSize << std::endl;
        std::cout << "Round Lots Only -> " << roundLotsOnly << std::endl;
        std::cout << "Issue Classification -> " << issueClassification << std::endl;
        std::cout << "Issue Sub Type -> " << issueSubType << std::endl;
        std::cout << "Authenticity -> " << authenticity << std::endl;
        std::cout << "Short Sale Threshold Indicator -> " << shortSaleThreshIndicator << std::endl;
        std::cout << "IPO Flag -> " << ipoFlag << std::endl;
        std::cout << "LULD Reference Price Tier -> " << LULDRefPriceTier << std::endl;
        std::cout << "ETP Flag -> " << etpFlag << std::endl;
        std::cout << "ETP Leverage Factor -> " << etpLeverageFactor << std::endl;
        std::cout << "Investment Indicator -> " << invIndicator << std::endl;
    }
};


// Nasdaq uses this administrative message to indicate the current trading status of a security to the trading community.
// Prior to the start of system hours, Nasdaq will send out a Trading Action spin. In the spin, Nasdaq will send out a
// Stock Trading Action message with the “T” (Trading Resumption) for all Nasdaq- and other exchange-listed
// securities that are eligible for trading at the start of the system hours. If a security is absent from the pre-
// opening Trading Action spin, firms should assume that the security is being treated as halted in the Nasdaq
// platform at the start of the system hours. Please note that securities may be halted in the Nasdaq system for
// regulatory or operational reasons.
// Afterthe start of system hours, Nasdaq will use the TradingAction message to relay changes in trading status for an
// individual security. Messages will be sent when a stock is:
// • Halted
// • Paused (* The paused status will be disseminated for NASDAQ-listed securities only. Trading pauses on non-NASDAQ listed securities will be treated simply as a halt.)
// • Released for quotation
// • Released for trading
struct StockTradingAction: public baseMessage{
    public:
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    std::string stock;
    char tradingState;
    char reversed;
    std::string reason;


    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        stock = readStock(file);
        file.read(&tradingState, sizeof(tradingState));
        file.read(&reversed, sizeof(reversed));
        reason = readString(file, 4);
    }
    void show() {
        std::cout << "Stock Locate -> " << stockLocate << std::endl;
        std::cout << "Tracking Number -> " << trackingNumber << std::endl;
        std::cout << "Timestamp -> " << timestamp << std::endl;
        std::cout << "Stock -> " << stock << std::endl;
        std::cout << "Trading State -> " << tradingState << std::endl;
        std::cout << "Reversed -> " << reversed << std::endl;
        std::cout << "Reason -> " << reason << std::endl;
    }

};


// In February 2011, the Securities and Exchange Commission (SEC) implemented changes to Rule 201 of the
// Regulation SHO (Reg SHO). For details, please refer to SEC Release Number 34-61595. In association with
// the Reg SHO rule change, Nasdaq will introduce the following Reg SHO Short Sale Price Test Restricted
// Indicator message format.
// For Nasdaq-listed issues, Nasdaq supports a full pre-opening spin of Reg SHO Short Sale Price Test Restricted
// Indicator messages indicating the Rule 201 status for all active issues. Nasdaq also sends the Reg SHO
// Short Sale Price Test Restricted Indicator message in the event of an intraday status change.
// For other exchange-•-listed issues, Nasdaq relays the Reg SHO Short Sale Price Test Restricted Indicator
// message when it receives an update from the primary listing exchange.
// Nasdaq processes orders based on the most Reg SHO Restriction status value
struct RegSHOShortSalePriceTestIndicator : public baseMessage {
    public:
    uint16_t locateCode;
    uint16_t trackingNumber;
    uint64_t timestamp;
    std::string stock;
    char regSHOAction;

    void load(std::ifstream &file){
        locateCode = readBigEndianInteger(file, 2);
        //file.read(reinterpret_cast<char*>(&locateCode), sizeof(locateCode));
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        stock = readStock(file);
        file.read(&regSHOAction, sizeof(regSHOAction));
    }

    void show() {
        std::cout << "Locate Code -> " << locateCode << std::endl;
        std::cout << "Tracking Number -> " << trackingNumber << std::endl;
        std::cout << "Timestamp -> " << timestamp << std::endl;
        std::cout << "Stock -> " << stock << std::endl;
        std::cout << "Reg SHO Action -> " << regSHOAction << std::endl;
    }

};


// At the start of each trading day, Nasdaq disseminates a spin of market participant position messages. The
// message provides the PrimaryMarketMakerstatus, MarketMakermode and MarketParticipant state for
// each Nasdaq market participant firm registered in an issue. Market participant firms may use these fields to
// comply with certain market place rules.
// Through out the day,Nasdaq will send out this message only if Nasdaq Operations changes the status of a
// marketparticipant firm in an issue.
struct MarketParticipationPos : public baseMessage {
    public:
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    std::string mpid;
    std::string stock;
    char primaryMarketMaker;
    char marketMakerMode;
    char marketParticipantState;

    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        mpid = readString(file, 4);
        stock = readStock(file);
        file.read(&primaryMarketMaker, sizeof(primaryMarketMaker));
        file.read(&marketMakerMode, sizeof(marketMakerMode));
        file.read(&marketParticipantState, sizeof(marketParticipantState));
    }

    void show() {
        std::cout << "Stock Locate -> " << stockLocate << std::endl;
        std::cout << "Tracking Number -> " << trackingNumber << std::endl;
        std::cout << "Timestamp -> " << timestamp << std::endl;
        std::cout << "MPID -> " << mpid << std::endl;
        std::cout << "Stock -> " << stock << std::endl;
        std::cout << "Primary Market Maker -> " << primaryMarketMaker << std::endl;
        std::cout << "Market Maker Mode -> " << marketMakerMode << std::endl;
        std::cout << "Market Participant State -> " << marketParticipantState << std::endl;
    }

};


// Informs data recipients what the daily MWCB breach points are set to for the current trading day.
struct MWCBDecline : public baseMessage {
    public:
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    uint64_t level1Raw, level2Raw, level3Raw;
    double level1;
    double level2;
    double level3;

    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        level1Raw = static_cast<uint64_t>(readBigEndianInteger(file, 8));
        level2Raw = static_cast<uint64_t>(readBigEndianInteger(file, 8));
        level3Raw = static_cast<uint64_t>(readBigEndianInteger(file, 8));
        level1 = level1Raw / double(1e8);
        level2 = level2Raw / double(1e8);
        level3 = level3Raw / double(1e8);
    }

    void show() {
            std::cout << "Stock Locate -> " << stockLocate << std::endl;
            std::cout << "Tracking Number -> " << trackingNumber << std::endl;
            std::cout << "Timestamp -> " << timestamp << std::endl;
            std::cout << "Level 1 -> " << level1 << std::endl;
            std::cout << "Level 2 -> " << level2 << std::endl;
            std::cout << "Level 3 -> " << level3 << std::endl;
    }

};


// Informs data recipients when a MWCB has breached one of the established levels
struct MWCBStatus : public baseMessage {
    public:
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    char breachedLevel;

    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        file.read(&breachedLevel, sizeof(breachedLevel));
    }
    void show() {
        std::cout << "Stock Locate -> " << stockLocate << std::endl;
        std::cout << "Tracking Number -> " << trackingNumber << std::endl;
        std::cout << "Timestamp -> " << timestamp << std::endl;
        std::cout << "Breached Level -> " << breachedLevel << std::endl;
    }
};


// Indicates the anticipated IPO quotation release time of a security.
struct QuotingPeriodUpdate : public baseMessage {
    public:
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    std::string stock;
    uint32_t ipoQuotationReleaseTime;
    char ipoQuotationreleaseQualifier;
    uint32_t ipoPriceRaw;
    double ipoPrice;

    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        stock = readStock(file);
        ipoQuotationReleaseTime = readBigEndianInteger(file, 4);
        file.read(&ipoQuotationreleaseQualifier, sizeof(ipoQuotationreleaseQualifier));
        ipoPriceRaw = static_cast<uint32_t>(readBigEndianInteger(file, 4));
        ipoPrice = ipoPriceRaw / 10000.0;
    }

    void show() {
        std::cout << "Stock Locate -> " << stockLocate << std::endl;
        std::cout << "Tracking Number -> " << trackingNumber << std::endl;
        std::cout << "Timestamp -> " << timestamp << std::endl;
        std::cout << "Stock -> " << stock << std::endl;
        std::cout << "IPO Quotation Release Time -> " << ipoQuotationReleaseTime << std::endl;
        std::cout << "IPO Quotation Release Qualifier -> " << ipoQuotationreleaseQualifier << std::endl;
        std::cout << "IPO Price -> " << ipoPrice << std::endl;
    }
};


//Indicates the auction collar thresholds within which a paused security can reopen following a LULD Trading Pause.
struct LULDAuctionCollar : public baseMessage{
    public:
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    std::string stock;
    uint32_t auctionCollarRefPriceRaw, upperAuctionCollarPriceRaw, lowerAuctionCollarPriceRaw;
    double auctionCollarRefPrice;
    double upperAuctionCollarPrice;
    double lowerAuctionCollarPrice;
    uint32_t auctionCollarExtension;

    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        stock = readStock(file);
        auctionCollarRefPriceRaw = static_cast<uint32_t>(readBigEndianInteger(file, 4));
        upperAuctionCollarPriceRaw = static_cast<uint32_t>(readBigEndianInteger(file, 4));
        lowerAuctionCollarPriceRaw = static_cast<uint32_t>(readBigEndianInteger(file, 4));
        auctionCollarRefPrice = auctionCollarRefPriceRaw / 10000.0;
        upperAuctionCollarPrice = upperAuctionCollarPriceRaw / 10000.0;
        lowerAuctionCollarPrice = lowerAuctionCollarPriceRaw / 10000.0; 
        file.read(reinterpret_cast<char*>(&auctionCollarExtension), sizeof(auctionCollarExtension));
    }

    void show() {
        std::cout << "Stock Locate -> " << stockLocate << std::endl;
        std::cout << "Tracking Number -> " << trackingNumber << std::endl;
        std::cout << "Timestamp -> " << timestamp << std::endl;
        std::cout << "Stock -> " << stock << std::endl;
        std::cout << "Auction Collar Reference Price -> " << auctionCollarRefPrice << std::endl;
        std::cout << "Upper Auction Collar Price -> " << upperAuctionCollarPrice << std::endl;
        std::cout << "Lower Auction Collar Price -> " << lowerAuctionCollarPrice << std::endl;
        std::cout << "Auction Collar Extension -> " << auctionCollarExtension << std::endl;
    }

};

// The Exchange uses this message to indicate the current Operational Status of a security to the trading
// community. An Operational Halt means that there has been an interruption of service on the identified
// security impacting only the designated Market Center. These Halts differ from the “Stock Trading
// Action” message types since an Operational Halt is specific to the exchange for which it is declared, and
// does not interrupt the ability of the trading community to trade the identified instrument on any other
// marketplace.
// Nasdaq uses this administrative message to indicate the current trading status of the three market centers
// operated by Nasdaq.
struct OpeartionalHalt: public baseMessage{
    public:
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    std::string stock;
    char marketCode;
    char opeartionalHaltAction;

    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        stock = readStock(file);
        file.read(&marketCode, sizeof(marketCode));
        file.read(&opeartionalHaltAction, sizeof(opeartionalHaltAction));
    }

    void show() {
        std::cout << "Stock Locate -> " << stockLocate << std::endl;
        std::cout << "Tracking Number -> " << trackingNumber << std::endl;
        std::cout << "Timestamp -> " << timestamp << std::endl;
        std::cout << "Stock -> " << stock << std::endl;
        std::cout << "Market Code -> " << marketCode << std::endl;
        std::cout << "Operational Halt Action -> " << opeartionalHaltAction << std::endl;
    }

};


// This message will be generated for unattributed orders accepted by the Nasdaq system. (Note: If a firm wants to
// display a MPID for unattributed orders, Nasdaq recommends that it use the MPID of “NSDQ”.)
struct AddOrderNoMPID : public baseMessage {
    public:
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    uint64_t orderRefNumber;
    char buySellIndicator;
    uint32_t shares;
    std::string stock;
    uint32_t priceRaw;
    double price;

    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        orderRefNumber = readBigEndianInteger(file, 8);
        file.read(&buySellIndicator, sizeof(buySellIndicator));
        file.read(reinterpret_cast<char*>(&shares), sizeof(shares));
        stock = readStock(file);
        priceRaw = static_cast<uint32_t>(readBigEndianInteger(file, 4));
        price = priceRaw / 10000.0;
    }

    void show() {
        std::cout << "Stock Locate -> " << stockLocate << std::endl;
        std::cout << "Tracking Number -> " << trackingNumber << std::endl;
        std::cout << "Timestamp -> " << timestamp << std::endl;
        std::cout << "Order Reference Number -> " << orderRefNumber << std::endl;
        std::cout << "Buy/Sell Indicator -> " << buySellIndicator << std::endl;
        std::cout << "Shares -> " << shares << std::endl;
        std::cout << "Stock -> " << stock << std::endl;
        std::cout << "Price -> " << price << std::endl;
    }
};


// This message will be generated for attributed orders and quotations accepted by the Nasdaq system.
struct AddOrderWithMPID : public AddOrderNoMPID {
    std::string attribution;
    void load(std::ifstream &file){
        AddOrderNoMPID::load(file);
        attribution = readString(file, 4);
    }

    void show() override {
        AddOrderNoMPID::show();
        std::cout << "Attribution -> " << attribution << std::endl;
    }

};

// This message is sent whenever an orderon the book is executed in whole or in part. It is possible to receive several
// Order Executed Messages for the same order reference number if that order is executed in several parts. The
// multiple Order Executed Messages on the same order are cumulative.
struct OrderExecuted : public baseMessage {
    public:
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    uint64_t orderRefNumber;
    uint32_t executedShares;
    uint64_t matchNumber;

    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        orderRefNumber = readBigEndianInteger(file, 8);
        file.read(reinterpret_cast<char*>(&executedShares), sizeof(executedShares));
        matchNumber = readBigEndianInteger(file, 8);
    }

    void show()  {
        std::cout << "Stock Locate -> " << stockLocate << std::endl;
        std::cout << "Tracking Number -> " << trackingNumber << std::endl;
        std::cout << "Timestamp -> " << timestamp << std::endl;
        std::cout << "Order Reference Number -> " << orderRefNumber << std::endl;
        std::cout << "Executed Shares -> " << executedShares << std::endl;
        std::cout << "Match Number -> " << matchNumber << std::endl;
    }

};

// This message issent whenever an order on the book is executed in whole or in part at a price different from the
// initial display price. Since the execution price is different than the display price of the original Add Order,Nasdaq
// includes a pricefieldwithin this executionmessage.
// It is possible to receive multiple Order Executed and Order Executed With Price messages for the same order if that
// order is executed in several parts.The multiple Order Executed messages on the same orderare cumulative.
// These executions may be marked as non-•-printable. If the execution is marked as non-•-printed, it means that the
// shares will be included into a later bulk print (e.g., in the case of cross executions). If a firm is looking to use the data
// in time-•-and-•-sales displays or volume calculations, Nasdaq recommends that firms ignore messages marked as non-
// -- printable to prevent double counting.
struct OrderExecutedWithPrice : public OrderExecuted {
    public:
    char printable;
    uint32_t executionPriceRaw;
    double executionPrice;

    void load(std::ifstream &file){
        OrderExecuted::load(file);
        file.read(&printable, sizeof(printable));
        executionPriceRaw = static_cast<uint32_t>(readBigEndianInteger(file, 4));
        executionPrice = executionPriceRaw / 10000.0;
    }

    void show() override {
        OrderExecuted::show();
        std::cout << "Printable -> " << printable << std::endl;
        std::cout << "Execution Price -> " << executionPrice << std::endl;
    }
};


// This message is sent whenever an order on the book is modified as a result of a partial cancellation.
struct OrderCancel : public baseMessage {
    public: 
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    uint64_t orderRefNumber;
    uint32_t cancelledShares;

    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        orderRefNumber = readBigEndianInteger(file, 8);
        cancelledShares = readBigEndianInteger(file, 4);
    }

    void show() override {
        std::cout << "Stock Locate -> " << stockLocate << std::endl;
        std::cout << "Tracking Number -> " << trackingNumber << std::endl;
        std::cout << "Timestamp -> " << timestamp << std::endl;
        std::cout << "Order Reference Number -> " << orderRefNumber << std::endl;
        std::cout << "Cancelled Shares -> " << cancelledShares << std::endl;
    }
};


// This message is sent whenever an order on the book is being cancelled. All remaining shares are no longer
// accessible so the order must be removed from the book.
struct OrderDelete : public baseMessage {
    public:
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    uint64_t orderRefNumber;

    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        orderRefNumber = readBigEndianInteger(file, 8);
    }

    void show()  {
    std::cout << "Stock Locate -> " << stockLocate << std::endl;
    std::cout << "Tracking Number -> " << trackingNumber << std::endl;
    std::cout << "Timestamp -> " << timestamp << std::endl;
    std::cout << "Order Reference Number -> " << orderRefNumber << std::endl;
    }
};


// This message is sent whenever an order on the book has been cancel-•-replaced. All remaining shares from the
// original order are no longer accessible, and must be removed. The new order details are provided for the
// replacement, along with a new order reference number which will be used henceforth. Since the side, stock
// symbol and attribution(if any) cannot be changed by an OrderReplace event,these fields are not included in the
// message. Firms should retain the side, stock symbol and MPID from the original Add Order message.
struct OrderReplace : public baseMessage {
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    uint64_t originalOrderRefNumber;
    uint64_t newOrderRefNumber;
    uint32_t shares;
    uint32_t priceRaw;
    double price;

    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        originalOrderRefNumber = readBigEndianInteger(file, 8);
        newOrderRefNumber = readBigEndianInteger(file, 8);
        shares = readBigEndianInteger(file, 4);
        priceRaw = static_cast<uint32_t>(readBigEndianInteger(file, 4));
        price = priceRaw / 10000.0;
    }

    void show() {
        
        std::cout << "Stock Locate: " << stockLocate << std::endl;
        std::cout << "Tracking Number: " << trackingNumber << std::endl;
        std::cout << "Timestamp: " << timestamp << std::endl;
        std::cout << "Original Order Reference Number: " << originalOrderRefNumber << std::endl;
        std::cout << "New Order Reference Number: " << newOrderRefNumber << std::endl;
        std::cout << "Shares: " << shares << std::endl;
        std::cout << "Price Raw: " << priceRaw << std::endl;
        std::cout << "Price: " << price << std::endl;
    }

};


// The Trade Message is designed to provide execution details for normal match events involving non-•-displayable
// order types. (Note: There is a separate message for Nasdaq cross events.)
// Since no Add Order Message is generated when a non-•-displayed order is initially received, Nasdaq cannot use the
// Order Executed messages for all matches. Therefore this message indicates when a match occurs between non---
// displayable order types. A Trade Message is transmitted each time a non-•-displayable order is executed in whole or
// in part. It is possible to receive multiple Trade Messages for the same order if that order is executed in several parts.
// Trade Messages for the same order are cumulative.
// Trade Messages should be included in Nasdaq time-•-and-•-sales displays as well as volume and other market
// statistics. Since Trade Messages do not affect the book, however, they may be ignored by firms just looking to build
// and track the Nasdaq execution system display.
struct NonCrossTrade : public baseMessage{
    
    public:
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    uint64_t orderRefNumber;
    char buySellIndicator;
    uint32_t shares;
    std::string stock;
    uint32_t priceRaw;
    uint32_t price;
    uint64_t matchNumber;

    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        orderRefNumber = readBigEndianInteger(file, 8);
        file.read(&buySellIndicator, sizeof(buySellIndicator));
        shares = readBigEndianInteger(file, 4);
        stock = readStock(file);
        priceRaw = static_cast<uint32_t>(readBigEndianInteger(file, 4));
        price = priceRaw / 10000.0;
        matchNumber = readBigEndianInteger(file, 8);
    }

    void show(){
        std::cout << "Stock Locator -> " << stockLocate << std::endl;
        std::cout << "Tracking Number -> " << trackingNumber << std::endl;
        std::cout << "Timestamp -> " << timestamp << std::endl;
        std::cout << "Order Reference Number -> " << orderRefNumber << std::endl;
        std::cout << "Buy/Sell Indicator -> " << buySellIndicator << std::endl;
        std::cout << "Shares -> " << shares << std::endl;
        std::cout << "Stock Name -> " << stock << std::endl;
        std::cout << "Price -> " << price << std::endl;
        std::cout << "Match Number -> " << matchNumber << std::endl;
    }
};


// Cross Trade message indicates that Nasdaq has completed its cross process for a specific security. Nasdaq sends out
// a Cross Trade message for all active issues in the system following the Opening, Closing and EMC cross events. Firms
// may use the Cross Trade message to determine when the cross for each security has been completed. (Note: For
// the halted / paused securities, firms should use the Trading Action message to determine when an issue has been
// released for trading.)
// For most issues, the Cross Trade message will indicate the bulk volume associated with the cross event. If the order
// interest is insufficient to conduct a cross in a particular issue, however, the Cross Trade message may show the
// shares as zero.
// To avoid double counting of cross volume, firms should not include transactions marked as non-•-printable in time---
// and-•-sales displays or market statistic calculations.
struct CrossTrade : public baseMessage {

    public:
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    uint64_t shares;
    std::string stock;
    uint32_t crossPriceRaw;
    float crossPrice;
    uint64_t matchNumber;
    char crossType;

    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        file.read(reinterpret_cast<char*>(&shares), sizeof(shares));
        stock = readStock(file);
        crossPriceRaw = static_cast<uint32_t>(readBigEndianInteger(file, 4));
        crossPrice = crossPriceRaw / 10000.0;
        matchNumber = readBigEndianInteger(file, 8);
        file.read(&crossType, sizeof(crossType));
    }
    void show()  {
        std::cout << "Stock Locate -> " << stockLocate << std::endl;
        std::cout << "Tracking Number -> " << trackingNumber << std::endl;
        std::cout << "Timestamp -> " << timestamp << std::endl;
        std::cout << "Shares -> " << shares << std::endl;
        std::cout << "Stock -> " << stock << std::endl;
        std::cout << "Cross Price -> " << crossPrice << std::endl;
        std::cout << "Match Number -> " << matchNumber << std::endl;
        std::cout << "Cross Type -> " << crossType << std::endl;
    }
};


// The Broken Trade Message issent whenever an execution on Nasdaq is broken. An execution may be broken if it is
// found to be “clearly erroneous” pursuant to Nasdaq’s Clearly Erroneous Policy.A trade break is final; once a trade is
// broken, it cannot be reinstated.
// Firms that use the ITCH feed to create time---and---sales displays or calculate market statistics should be prepared
// to process the broken trade message. If a firm is only using the ITCH feed to build a book, however, it may ignore
// these messages as they have no impact on the current book.
struct BrokenTrade : baseMessage {

    public:
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    uint64_t matchNumber;

    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        matchNumber = readBigEndianInteger(file, 8);
    }

    void show() {
    std::cout << "Stock Locate -> " << stockLocate << std::endl;
    std::cout << "Tracking Number -> " << trackingNumber << std::endl;
    std::cout << "Timestamp -> " << timestamp << std::endl;
    std::cout << "Match Number -> " << matchNumber << std::endl;
}
};

struct NetOrderImbalance : public baseMessage {

    uint32_t farPriceRaw, nearPriceRaw, currRefPrceRaw;

    public:
    uint16_t stockLocate;
    uint16_t trackingNumber;
    uint64_t timestamp;
    uint64_t pairedShares;
    uint64_t imbalanceShares;
    char imbalanceDirection;
    std::string stock;
    float farPrice;
    float nearPrice;
    float currRefPrice;
    char crossType;
    char priceVariationIndicator;


    void load(std::ifstream &file){
        stockLocate = readBigEndianInteger(file, 2);
        trackingNumber = readBigEndianInteger(file, 2);
        timestamp = readBigEndianInteger(file, 6);
        pairedShares = readBigEndianInteger(file, 8);
        imbalanceShares = readBigEndianInteger(file, 8);
        file.read(&imbalanceDirection, sizeof(imbalanceDirection));
        stock = readStock(file);
        farPriceRaw = static_cast<uint32_t>(readBigEndianInteger(file, 4));
        nearPriceRaw = static_cast<uint32_t>(readBigEndianInteger(file, 4));
        currRefPrceRaw = static_cast<uint32_t>(readBigEndianInteger(file, 4));
        farPrice = farPriceRaw / 10000.0;
        nearPrice = nearPrice / 10000.0;
        currRefPrice = currRefPrceRaw / 10000.0;
        file.read(&crossType, sizeof(crossType));
        file.read(&priceVariationIndicator, sizeof(priceVariationIndicator));
    }

    void show()  {
        std::cout << "Stock Locate -> " << stockLocate << std::endl;
        std::cout << "Tracking Number -> " << trackingNumber << std::endl;
        std::cout << "Timestamp -> " << timestamp << std::endl;
        std::cout << "Paired Shares -> " << pairedShares << std::endl;
        std::cout << "Imbalance Shares -> " << imbalanceShares << std::endl;
        std::cout << "Imbalance Direction -> " << imbalanceDirection << std::endl;
        std::cout << "Stock -> " << stock << std::endl;
        std::cout << "Far Price -> " << farPrice << std::endl;
        std::cout << "Near Price -> " << nearPrice << std::endl;
        std::cout << "Current Reference Price -> " << currRefPrice << std::endl;
        std::cout << "Cross Type -> " << crossType << std::endl;
        std::cout << "Price Variation Indicator -> " << priceVariationIndicator << std::endl;
    }
};

                    