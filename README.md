# Running hourly VWAP Generation by parsing NASDAQ ITCH 5.0 Tick Data File

## Walkthrough : 

The steps taken - 

- `Source File` : 
    Source File `01302019.NASDAQ_ITCH50.gz` has been fetched from [NASDAQ Official Website](https://emi.nasdaq.com/ITCH/Nasdaq%20ITCH/01302019.NASDAQ_ITCH50.gz) and has been unzipped for ease of usage.

- `Exploration of the tick data file` : 
        The tick data file consists of binanry tick data where the messages can be of different types.
        The zipped file is lazy-loaded and parsed.

 - `Tick Data Parsing` :
    For creation of this project, below messages are only considered - 
   
   1. `Stock Directory Message` : To initiate orders and trades for a particular stock
   2. `Order Add Message (With & Without mpid)` : To handle general orders
   3. `Order Execution Message (With & Without updated Price)` : To populate trades
   4. `Order Cancel Message` : Update book for future manipulation.
   5. `Order Delete Message` : Remove Order from Book
   6. `Order Replace Message` : Update Orders
   7. `Trade Message (Non-Cross)` : Populate non-cross trades
   8. `Trade Message (Cross)` : Populate cross trades
   9. `Broken Trade Message` : Remove broken trades from trades info for accurate VWAP calculation


    Rest messages are ignored. 

    To fasten the parsing file and searching of the required messages, rest message packets are skipped rather than searching the message_type(s) character by character.

- `VWAP Calculation and Final Data Creation` : 

    Non-cross trades are stored into a map object where the running VWAP is generated.

    The running VWAP are stored per ticker per hour if that hour had any trade.
    The result information is stored at `hourly_running_vwap.csv`

- `Codebase Walkthrough` - 
   To prepare the above, we've segregated the codes into 3 header files - 

   `utils.hpp` - Residence of utility functions
   
   `message.hpp` - Residence of all message decoding procedures.
   
   `parser.hpp` - Residence of the parser and running VWAP generation logic
   
    It expects the unzipped source file i.e. `01302019.NASDAQ_ITCH50` to be present at the same directory of `main.cpp`.

    Directory Structure : 

        .
        └── itch-5.0-processing/
            ├── include/
            │   ├── messaeg.hpp
            │   ├── parser.hpp
            |   └── utils.hpp
            ├── main.cpp
            └── 01302019.NASDAQ_ITCH50

    C++ Compiler and Version : g++17

    Required packages - 
    Basic default C++ libraries are used. No additional libraries were required.

    Execution : 

    The code has been executed running `main.cpp`, `time` has been used to clock the processes.

    ```bash
    # Compiling Binary
    g++ --std=c++17 main.cpp -o bin/main

    # Executing Binary
    time bin/main
    ```