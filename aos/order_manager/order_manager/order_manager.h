#pragma once
#include <unordered_map>

#include "aos/order_manager/order_manager/i_order_manager.h"
#include "aos/order_manager/order_status_provider/i_order_status_provider.h"
#include "aos/order_manager/order_submitter/i_order_submitter.h"
#include "aos/uid/i_uid_manager.h"
#include "aot/Logger.h"
#include "aot/common/mem_pool.h"
#include "aot/strategy/market_order.h"

namespace aos {
namespace impl {

template <typename Price, typename Qty, typename Uid>
class OrderManager
    : public IOrderSubmitterNoPool<Price, Qty, Uid>,
      public IOrderSubmiCancelNoPool<Uid>,
      public IOrderManager<Price, Qty, common::MemoryPoolNotThreadSafety, Uid>,
      public IOrderStatusProvider {
    boost::intrusive_ptr<
        IOrderSubmitter<Price, Qty, common::MemoryPoolNotThreadSafety, Uid>>
        order_submitter_;
    boost::intrusive_ptr<IUIDManager<Uid, common::MemoryPoolNotThreadSafety>>
        uid_manager_;
    std::unordered_map<Uid, OrderData<Price, Qty>> orders_;

  public:
    explicit OrderManager(
        boost::intrusive_ptr<
            IOrderSubmitter<Price, Qty, common::MemoryPoolNotThreadSafety, Uid>>
            order_submitter,
        boost::intrusive_ptr<
            IUIDManager<Uid, common::MemoryPoolNotThreadSafety>>
            uid_manager)
        : order_submitter_(order_submitter), uid_manager_(uid_manager) {};
    ~OrderManager() override { logi("{}", "OrderManager dtor"); };

    bool PlaceOrder(common::ExchangeId exchange_id,
                    common::TradingPair trading_pair, Price price, Qty qty,
                    common::Side side, TimeInForce tif,
                    OrderRouting routing) override {
        auto id = uid_manager_->GetUniqueID();
        if (orders_.contains(id)) {
            logw("order with uid:{} is exist", id);
            return false;
        }
        orders_[id] = {exchange_id, trading_pair,         price, qty, side, tif,
                       routing,     aos::OrderStatus::New};
        logi("PlaceOrder uid:{} orders_count:{}", id, orders_.size());
        SubmitOrder(exchange_id, trading_pair, price, qty, side, tif, routing,
                    id);
        return true;
    };

    bool CancelOrder(common::ExchangeId exchange_id, OrderRouting routing,
                     Uid order_id) override {
        if (!orders_.contains(order_id)) {
            logw("not found order with uid:{}", order_id);
            return false;
        }
        orders_[order_id].status = OrderStatus::CanceledByUser;
        logi("CancelOrder uid:{} is success. orders_count:{}", order_id,
             orders_.size());
        SubmitCancelOrder(exchange_id, routing, order_id);
        return true;
    };

    void UpdateStatusOrder(Uid order_id, OrderStatus order_status) override {
        if (!orders_.contains(order_id)) {
            logw("not found order with uid:{}", order_id);
            return;
        }
        logi(
            "UpdateStatusOrder uid:{} new:{} old:{} "
            "orders_count:{}",
            order_id, order_status, orders_[order_id].status, orders_.size());
        orders_[order_id].status = order_status;
    };

    std::pair<bool, OrderStatus> GetStatus(Uid order_id) override {
        if (!orders_.contains(order_id)) {
            logw("not found order with uid:{}", order_id);
            return std::make_pair(true, aos::OrderStatus::Expired);
        }
        return std::make_pair(true, orders_[order_id].status);
    };

  private:
    void SubmitOrder(common::ExchangeId exchange_id,
                     common::TradingPair trading_pair, Price price, Qty qty,
                     common::Side side, TimeInForce tif, OrderRouting routing,
                     Uid uid) override {
        order_submitter_->SubmitOrder(exchange_id, trading_pair, price, qty,
                                      side, tif, routing, uid);
    };
    void SubmitCancelOrder(common::ExchangeId exchange_id, OrderRouting routing,
                           Uid uid) override {
        order_submitter_->SubmitCancelOrder(exchange_id, routing, uid);
    }
};
};  // namespace impl
};  // namespace aos