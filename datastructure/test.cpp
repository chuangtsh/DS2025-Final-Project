#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cassert>
#include "PairingHeap.h"

// 假設你的 PairingHeap 類別已經定義好
// #include "PairingHeap.h" 

void runTest() {
    std::cout << "開始測試 deleteNode..." << std::endl;

    PairingHeap<int> ph;
    
    // 用來對照正確答案的容器
    // 儲存 pair: <數值, 指向 Heap 節點的指標>
    std::vector<std::pair<int, PairingNode<int>*>> tracker;
    
    // 亂數產生器
    std::mt19937 rng(std::random_device{}());
    
    // 1. 隨機插入數據 (例如 10000 筆)
    std::cout << "正在插入數據..." << std::endl;
    for (int i = 0; i < 10000; ++i) {
        int val = rng() % 100000;
        // 假設你的 insert 會回傳節點指標
        PairingNode<int>* node = ph.insert(val); 
        tracker.push_back({val, node});
    }

    // 2. 隨機刪除節點 (例如刪除 10000 筆)
    std::cout << "正在隨機刪除節點..." << std::endl;
    for (int i = 0; i < 10000; ++i) {
        if (tracker.empty()) break;

        // 隨機選一個 index 刪除
        std::uniform_int_distribution<int> dist(0, tracker.size() - 1);
        int idx = dist(rng);

        // 從 Heap 中刪除
        ph.deleteNode(tracker[idx].second);

        // 從對照組中刪除
        tracker.erase(tracker.begin() + idx);
        
        // (可選) 每次刪除後檢查 min 是否正確
        if (!tracker.empty()) {
            int expectedMin = 100000;
            for(auto& p : tracker) expectedMin = std::min(expectedMin, p.first);
            assert(ph.findMin() == expectedMin);
        }
    }

    // 3. 最終驗證：把 Heap 全部 pop 出來，跟對照組排序後比對
    std::cout << "正在驗證剩餘結構..." << std::endl;
    std::vector<int> heapResult;
    while (!ph.isEmpty()) {
        heapResult.push_back(ph.findMin());
        ph.deleteMin();
    }

    std::vector<int> trackerResult;
    for (auto& p : tracker) {
        trackerResult.push_back(p.first);
    }
    std::sort(trackerResult.begin(), trackerResult.end());

    // 比對大小和內容
    assert(heapResult.size() == trackerResult.size());
    for (size_t i = 0; i < heapResult.size(); ++i) {
        if (heapResult[i] != trackerResult[i]) {
            std::cerr << "測試失敗！Index " << i 
                      << " Heap: " << heapResult[i] 
                      << " Expected: " << trackerResult[i] << std::endl;
            return;
        }
    }

    std::cout << "恭喜！deleteNode 測試通過！" << std::endl;
}

int main() {
    runTest();
    return 0;
}


/*/usr/bin/g++ -fsanitize=address -g /home/latuuu/dit/DS2025-Final-Project/datastructure/test.cpp -o /home/latuuu/dit/DS2025-Final-Project/datastructure/test_sanitized*/