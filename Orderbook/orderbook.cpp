#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <deque>
#include <memory>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

struct Order {
    uint64_t order_id;
    bool is_buy;
    double price;
    uint64_t quantity;
    uint64_t timestamp_ns;

    Order() = default;
    Order(uint64_t id, bool buy, double p, uint64_t qty, uint64_t ts)
        : order_id(id), is_buy(buy), price(p), quantity(qty), timestamp_ns(ts) {}
};

struct PriceLevel {
    double price;
    uint64_t total_quantity;

    PriceLevel() : price(0.0), total_quantity(0) {}
    PriceLevel(double p, uint64_t qty) : price(p), total_quantity(qty) {}
};

struct OrderTracker {
    Order data;
    OrderTracker(const Order& ord) : data(ord) {}
};

class LevelManager {
private:
    double price_point;
    deque<uint64_t> order_queue;
    uint64_t aggregate_volume;

public:
    LevelManager(double price) : price_point(price), aggregate_volume(0) {}

    void push_order(uint64_t order_id, uint64_t qty) {
        order_queue.push_back(order_id);
        aggregate_volume += qty;
    }

    bool remove_order(uint64_t order_id, uint64_t qty) {
        auto iter = find(order_queue.begin(), order_queue.end(), order_id);
        if (iter != order_queue.end()) {
            order_queue.erase(iter);
            aggregate_volume -= qty;
            return true;
        }
        return false;
    }

    void update_volume(uint64_t old_qty, uint64_t new_qty) {
        aggregate_volume = aggregate_volume - old_qty + new_qty;
    }

    uint64_t get_volume() const { return aggregate_volume; }
    double get_price() const { return price_point; }
    bool is_empty() const { return order_queue.empty(); }
    const deque<uint64_t>& get_orders() const { return order_queue; }
};

class OrderBook {
private:
    map<double, unique_ptr<LevelManager>> bid_levels;
    map<double, unique_ptr<LevelManager>> ask_levels;
    unordered_map<uint64_t, unique_ptr<OrderTracker>> active_orders;

    map<double, unique_ptr<LevelManager>>& fetch_side(bool buy_side) {
        return buy_side ? bid_levels : ask_levels;
    }

    const map<double, unique_ptr<LevelManager>>& fetch_side(bool buy_side) const {
        return buy_side ? bid_levels : ask_levels;
    }

    LevelManager* get_or_create_level(bool buy_side, double price) {
        auto& side = fetch_side(buy_side);
        auto iter = side.find(price);
        if (iter == side.end()) {
            auto new_level = make_unique<LevelManager>(price);
            auto* ptr = new_level.get();
            side[price] = move(new_level);
            return ptr;
        }
        return iter->second.get();
    }

    void cleanup_level(bool buy_side, double price) {
        auto& side = fetch_side(buy_side);
        auto iter = side.find(price);
        if (iter != side.end() && iter->second->is_empty()) {
            side.erase(iter);
        }
    }

    void execute_matching() {
        while (!bid_levels.empty() && !ask_levels.empty()) {
            auto bid_iter = bid_levels.rbegin();
            auto ask_iter = ask_levels.begin();

            double best_bid_price = bid_iter->second->get_price();
            double best_ask_price = ask_iter->second->get_price();

            if (best_bid_price < best_ask_price) break;

            const auto& bid_queue = bid_iter->second->get_orders();
            const auto& ask_queue = ask_iter->second->get_orders();

            if (bid_queue.empty() || ask_queue.empty()) break;

            uint64_t bid_id = bid_queue.front();
            uint64_t ask_id = ask_queue.front();

            process_match(bid_id, ask_id);
        }
    }

    void process_match(uint64_t bid_id, uint64_t ask_id) {
        auto bid_iter = active_orders.find(bid_id);
        auto ask_iter = active_orders.find(ask_id);

        if (bid_iter == active_orders.end() || ask_iter == active_orders.end()) return;

        Order& bid_order = bid_iter->second->data;
        Order& ask_order = ask_iter->second->data;

        uint64_t match_qty = min(bid_order.quantity, ask_order.quantity);

        cout << "Executed Bid Order of Id: " << bid_id
             << " and price: " << bid_order.price
             << " for Ask order of Id: " << ask_id
             << " and price: " << ask_order.price << "\n";

        bid_order.quantity -= match_qty;
        ask_order.quantity -= match_qty;

        auto bid_level = bid_levels.find(bid_order.price);
        if (bid_level != bid_levels.end()) {
            bid_level->second->update_volume(match_qty, 0);
        }

        auto ask_level = ask_levels.find(ask_order.price);
        if (ask_level != ask_levels.end()) {
            ask_level->second->update_volume(match_qty, 0);
        }

        if (bid_order.quantity == 0) cancel_order(bid_id);
        if (ask_order.quantity == 0) cancel_order(ask_id);
    }

public:
    void add_order(const Order& order) {
        if (active_orders.count(order.order_id) > 0) {
            return;
        }

        auto tracker = make_unique<OrderTracker>(order);
        LevelManager* level = get_or_create_level(order.is_buy, order.price);
        level->push_order(order.order_id, order.quantity);
        active_orders[order.order_id] = move(tracker);
        execute_matching();
    }

    bool cancel_order(uint64_t order_id) {
        auto iter = active_orders.find(order_id);
        if (iter == active_orders.end()) return false;

        OrderTracker* tracker = iter->second.get();
        Order& ord = tracker->data;

        auto& side = fetch_side(ord.is_buy);
        auto level_iter = side.find(ord.price);

        if (level_iter != side.end()) {
            level_iter->second->remove_order(order_id, ord.quantity);
            cleanup_level(ord.is_buy, ord.price);
        }

        active_orders.erase(iter);
        return true;
    }

    bool amend_order(uint64_t order_id, double new_price, uint64_t new_quantity) {
        auto iter = active_orders.find(order_id);
        if (iter == active_orders.end()) return false;

        OrderTracker* tracker = iter->second.get();
        Order& ord = tracker->data;

        if (ord.price != new_price) {
            Order modified_order = ord;
            modified_order.price = new_price;
            modified_order.quantity = new_quantity;
            cancel_order(order_id);
            add_order(modified_order);
            return true;
        }

        auto& side = fetch_side(ord.is_buy);
        auto level_iter = side.find(ord.price);

        if (level_iter != side.end()) {
            level_iter->second->update_volume(ord.quantity, new_quantity);
            ord.quantity = new_quantity;
        }
        return true;
    }

    void get_snapshot(size_t depth, vector<PriceLevel>& bids, vector<PriceLevel>& asks) const {
        bids.clear();
        asks.clear();

        for (auto iter = bid_levels.rbegin();
             iter != bid_levels.rend() && bids.size() < depth; ++iter) {
            bids.emplace_back(iter->second->get_price(), iter->second->get_volume());
        }

        for (auto iter = ask_levels.begin();
             iter != ask_levels.end() && asks.size() < depth; ++iter) {
            asks.emplace_back(iter->second->get_price(), iter->second->get_volume());
        }
    }

    void print_book(size_t depth = 10) const {
        vector<PriceLevel> bids, asks;
        get_snapshot(depth, bids, asks);

        size_t max_items = max(bids.size(), asks.size());
        size_t counter = 0;

        for (size_t i = 0; i < max_items && counter < depth; ++i) {
            if (i < bids.size()) {
                auto level_iter = bid_levels.find(bids[i].price);
                if (level_iter != bid_levels.end()) {
                    const auto& orders = level_iter->second->get_orders();
                    for (uint64_t order_id : orders) {
                        auto ord_iter = active_orders.find(order_id);
                        if (ord_iter != active_orders.end()) {
                            const Order& ord = ord_iter->second->data;
                            cout << "Bid orderId: " << ord.order_id << "\n";
                            cout << "Price: " << fixed << setprecision(2)
                                 << ord.price << " , Quantity: " << ord.quantity << "\n\n";
                            counter++;
                            if (counter >= depth) break;
                        }
                    }
                }
            }

            if (i < asks.size() && counter < depth) {
                auto level_iter = ask_levels.find(asks[i].price);
                if (level_iter != ask_levels.end()) {
                    const auto& orders = level_iter->second->get_orders();
                    for (uint64_t order_id : orders) {
                        auto ord_iter = active_orders.find(order_id);
                        if (ord_iter != active_orders.end()) {
                            const Order& ord = ord_iter->second->data;
                            cout << "\nAsk orderId: " << ord.order_id << "\n";
                            cout << "Price: " << fixed << setprecision(2)
                                 << ord.price << " , Quantity: " << ord.quantity << "\n";
                            counter++;
                            if (counter >= depth) break;
                        }
                    }
                }
            }

            if (counter < depth) {
                cout << "--------------------\n";
            }
        }
    }

    void print_order(uint64_t order_id) const {
        auto iter = active_orders.find(order_id);
        if (iter == active_orders.end()) {
            cout << "Order ID " << order_id << " not found in the order book.\n";
            return;
        }

        const Order& ord = iter->second->data;
        cout << "Order ID: " << order_id << "\n";
        cout << "Buy " << (ord.is_buy ? "Yes" : "No")
             << ", Price: " << ord.price
             << ", Quantity: " << ord.quantity
             << ", Timestamp: " << ord.timestamp_ns << "\n";
    }
};

int main() {
    OrderBook orderBook;

    cout << "Testing Order Book\n\n";

    cout << "Adding buy orders\n";
    Order buy1(1001, true, 50.25, 100, 1000000000);
    Order buy4(1011, true, 50.25, 200, 1000000010);
    Order buy2(1002, true, 50.50, 200, 1000000001);
    Order buy3(1003, true, 50.00, 150, 1000000002);

    orderBook.add_order(buy1);
    orderBook.add_order(buy2);
    orderBook.add_order(buy3);
    orderBook.add_order(buy4);

    cout << "\nBook state after adding buy orders:\n\n";
    orderBook.print_book(5);
    cout << endl;

    cout << "Adding sell orders\n";
    Order sell1(2001, false, 51.00, 80, 1000000003);
    Order sell2(2002, false, 51.25, 120, 1000000004);
    Order sell3(2003, false, 50.75, 90, 1000000005);
    Order sell4(2004, false, 50.95, 190, 1000000015);

    orderBook.add_order(sell1);
    orderBook.add_order(sell2);
    orderBook.add_order(sell3);
    orderBook.add_order(sell4);

    cout << "\nBook state after adding sell orders:\n\n";
    orderBook.print_book(10);
    cout << endl;

    cout << "Getting snapshot\n";
    vector<PriceLevel> bids, asks;
    orderBook.get_snapshot(4, bids, asks);

    cout << "\nTop 4 Bids:\n";
    for (const auto& bid : bids) {
        cout << "  Price: " << bid.price << ", Quantity: " << bid.total_quantity << endl;
    }

    cout << "\nTop 4 Asks:\n";
    for (const auto& ask : asks) {
        cout << "  Price: " << ask.price << ", Quantity: " << ask.total_quantity << endl;
    }
    cout << endl;

    cout << "Adding sell order that matches\n";
    Order sell_match(2005, false, 50.25, 50, 1000000006);
    orderBook.add_order(sell_match);

    cout << "\nBook state after matching:\n\n";
    orderBook.print_book(10);
    cout << endl;

    cout << "Testing cancellation\n";
    cout << "Canceling order 1001\n";
    bool cancelled = orderBook.cancel_order(1001);
    cout << (cancelled ? "Success" : "Failed") << endl;

    cout << "Canceling order 9999\n";
    cancelled = orderBook.cancel_order(9999);
    cout << (cancelled ? "Success" : "Failed") << endl;

    cout << "\nBook state after cancellation:\n\n";
    orderBook.print_book(10);
    cout << endl;

    cout << "Testing amendment\n";
    cout << "Amending order 1002\n";
    bool amended = orderBook.amend_order(1002, 49.75, 300);
    cout << (amended ? "Success" : "Failed") << endl;

    cout << "\nBook state after amendment:\n\n";
    orderBook.print_book(10);
    cout << endl;

    cout << "Adding aggressive orders\n";
    Order aggressive_buy(3001, true, 52.00, 200, 1000000007);
    Order aggressive_sell(3002, false, 49.00, 100, 1000000008);

    orderBook.add_order(aggressive_buy);
    orderBook.add_order(aggressive_sell);

    cout << "\nFinal book state:\n\n";
    orderBook.print_book(10);

    cout << "\nTesting complete\n";

    return 0;
}