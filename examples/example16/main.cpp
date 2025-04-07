#include "aos/aos.h"

int main() {
    {
        try {
            aos::types::NetComponents components;
            auto [realized_pnl_storage, unrealized_pnl_storage, unreal_pnl_calc,
                  position_strategy, position_storage] = components.Build();
        } catch (...) {
            loge("can't init components");
        }
    }
    {
        try {
            aos::types::HedgedComponents components;
            auto [realized_pnl_storage, unrealized_pnl_storage, unreal_pnl_calc,
                  position_strategy, position_storage] = components.Build();
        } catch (...) {
            loge("can't init components");
        }
    }
    fmtlog::poll();
    return 0;
}