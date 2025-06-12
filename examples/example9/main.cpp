#include <iostream>
#include <queue>
#include <thread>
#include <vector>

#include "aos/uid/number_pool.h"
#include "aos/uid/uid_manager.h"
#include "aos/uid/unique_id_generator.h"
#include "fmtlog.h"
#include "aos/common/mem_pool.h"

int main() {
    aos::impl::UIDManagerContainer uid_manager_container(1);
    auto uid_manager = uid_manager_container.Build();
    for (int i = 0; i < 100; i++) {
        auto it = uid_manager->GetUniqueID();
        if (i % 2 == 0) {
            uid_manager->ReturnIDToPool(it);
        }
        logi("uid:{}", it);
    }
    return 0;
}