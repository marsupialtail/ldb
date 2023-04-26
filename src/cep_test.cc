#include "vector"
#include "queue"
#include "cassert"
#include "iostream"
#include "algorithm"
#include "cstdint"
#include "exceptions.h"
#include <stdio.h>
#include <chrono>

#define DIFF 5

void find_tuples(std::vector<std::vector<uint64_t>> lists, std::vector<std::vector<uint64_t>> & results) {
    uint64_t limit = lists.size();
    std::queue<std::pair<std::vector<uint64_t>, std::pair<uint64_t, uint64_t>>> q;
    for (int i = 0; i < lists[0].size(); i++)
    {
      q.push(std::make_pair(std::vector<uint64_t>{lists[0][i]}, std::make_pair(0, lists[0][i])));
    }
    while (!q.empty()) {
        std::vector<uint64_t> curr_tup = q.front().first;
        std::pair<uint64_t, uint64_t> curr_index = q.front().second;
        q.pop();
        uint64_t curr_list = curr_index.first;
        uint64_t curr_value = curr_index.second;
        std::vector<std::pair<uint64_t, uint64_t>> to_add;
        for (uint64_t i = 0; i < lists[curr_list + 1].size(); i++) {
            uint64_t value = lists[curr_list + 1][i];
            if (value >= curr_value && value <= curr_value + DIFF) {
                to_add.push_back(std::make_pair(lists[curr_list + 1][i], value));
            }
        }
        if (curr_list == limit - 2) {
            for (auto tup : to_add) {
                curr_tup.push_back(tup.first);
                results.push_back(curr_tup);
                curr_tup.pop_back();
            }
        } else {
            for (auto tup : to_add) {
                curr_tup.push_back(tup.first);
                q.push(std::make_pair(curr_tup, std::make_pair(curr_list + 1, tup.second)));
                curr_tup.pop_back();
            }
        }
    }
}

void simple(int k, const uint64_t * ts, const std::vector<std::vector<uint64_t>> conditions, std::vector<std::vector<uint64_t>> & ret) {
   
    
    std::vector<uint64_t> fingers(k);
    // ts[conditions[fingers[i]]] must be bigger than or equal to ts[conditions[fingers[i - 1]]]]
    // fingers[i] gets bigger than conditions[i].size() when it reaches the end, quit loop

    uint64_t start_time = ts[conditions[0][0]];
    for(int i = 1; i < k; i++) {
        // walk finger[i] forward enough such that ts[conditions[i][fingers[i]]] >= start_time
        while (fingers[i] < conditions[i].size() && ts[conditions[i][fingers[i]]] < start_time) {
            fingers[i]++;
        }
        if (fingers[i] == conditions[i].size()) {
            // we are done
            return;
        }
        start_time = ts[conditions[i][fingers[i]]];
    }

    std::vector<std::vector<uint64_t>> local_ret(k, std::vector<uint64_t>());

    while (fingers[0] < conditions[0].size()) {

        // check if we are done
        bool done = true;
        for (int i = 1; i < k; i++) {
            if (fingers[i] == conditions[i].size()) {
                return;
            }
        }

        for (int i = 0; i < k; i++) {
            local_ret[i].clear();
        }
        local_ret[0].push_back(conditions[0][fingers[0]]);

        // compute the window now
        uint64_t start_time = ts[conditions[0][fingers[0]]];
        uint64_t end_time = start_time + DIFF;
        for (int i = 1; i < k; i++) {
            // increment fingers[i] until ts[conditions[i][fingers[i]]] >= start_time
            while (ts[conditions[i][fingers[i]]] < start_time && fingers[i] < conditions[i].size() - 1) {
                fingers[i] ++;
            }

            start_time = ts[conditions[i][fingers[i]]];
            uint64_t local_finger = fingers[i];
            while (local_finger < conditions[i].size() && ts[conditions[i][local_finger]] <= end_time ) {
                local_ret[i].push_back(conditions[i][local_finger]);
                local_finger ++;
            }
            
            end_time = ts[conditions[i][local_finger - 1]] + DIFF;
        }

        std::vector<std::vector<uint64_t>> expanded_results;
        find_tuples(local_ret, expanded_results);
        //print out local_ret

        /*for (auto vec : local_ret) {
            for (auto i : vec) {
                std::cout << i << " ";
            }
            std::cout << std::endl;
        }*/

        for (auto vec : expanded_results) {
            for (int i = 0; i < k; i++) {
                ret[i].push_back(vec[i]);
            }
        }
        fingers[0] ++;

        //extend the local_ret into ret
    }    
}

#include "cep_fsm.cc"

int main()
{
    // generate a test case for the function simple
    // test case 1: 3 conditions, 3 events, 1 tuple
    int k = 3;
    uint64_t ts[] = {0, 1, 2, 3, 4, 5, 6};
    std::vector<std::vector<uint64_t>> conditions;
    conditions.push_back(std::vector<uint64_t>{0});
    conditions.push_back(std::vector<uint64_t>{1, 4});
    conditions.push_back(std::vector<uint64_t>{2, 5, 6});
    std::vector<std::vector<uint64_t>> ret(k);
    for (int i = 0; i < k; i++) {
        ret[i] = std::vector<uint64_t>();
        ASSERT_MSG(conditions[i].size() > 0, "condition is empty, should not happen")
    }
    simple(k, ts, conditions, ret);
    for (auto vec : ret) {
        for (auto i : vec) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "---------" << std::endl;
    print_vec_vec(ret);
    std::cout << "---------" << std::endl;
    std::vector<std::vector<uint64_t>> ret2;
    fsm(3, ts, conditions, ret2);
    print_vec_vec(ret2);
    // benchmark
    uint64_t* ts_b = (uint64_t*) malloc(sizeof(uint64_t) * 10000);
    for (int i = 0; i < 10000; i++) {
        ts_b[i] = i;
    }
    std::vector<std::vector<uint64_t>> conditions_b;
    std::vector<uint64_t> conditions_b_0;
    std::vector<uint64_t> conditions_b_1;
    std::vector<uint64_t> conditions_b_2;
    for (int i = 0; i < 10000; i++) {
        if (i % 2 == 0) {
            conditions_b_0.push_back(i);
        }
        if (i % 3 == 0) {
            conditions_b_1.push_back(i);
        }
        if (i % 5 == 0) {
            conditions_b_2.push_back(i);
        }
    }
    conditions_b.push_back(conditions_b_0);
    conditions_b.push_back(conditions_b_1);
    conditions_b.push_back(conditions_b_2);
    std::vector<std::vector<uint64_t>> res_1(k);
    std::vector<std::vector<uint64_t>> res_2;
    for (int i = 0; i < k; i++) {
        res_1[i] = std::vector<uint64_t>();
    }

    benchmark1(3, ts_b, conditions_b, res_1);
    benchmark2(3, ts_b, conditions_b, res_2);
    check_result(res_1, res_2);
    free(ts_b);
}
