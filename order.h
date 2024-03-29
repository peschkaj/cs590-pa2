//Copyright (C) 2019 Jeremiah Peschka and Joel Lilja
#pragma once

#ifndef ORDER_H_
#define ORDER_H_

// Re-ordering array for DCT/ICT pre-computed
const int order[64][2] = {
    {0, 0}, {0, 1}, {1, 0}, {2, 0}, {1, 1}, {0, 2}, {0, 3}, {2, 1},  //  7
    {1, 2}, {3, 0}, {4, 0}, {3, 1}, {2, 2}, {1, 3}, {0, 4}, {0, 5},  // 15
    {1, 4}, {2, 3}, {3, 2}, {4, 1}, {5, 0}, {6, 0}, {5, 1}, {4, 2},  // 23
    {3, 3}, {2, 4}, {1, 5}, {0, 6}, {0, 7}, {1, 6}, {2, 5}, {3, 4},  // 31
    {4, 3}, {5, 2}, {6, 1}, {7, 0}, {1, 7}, {2, 6}, {3, 5}, {4, 4},  // 39
    {5, 3}, {6, 2}, {7, 1}, {2, 7}, {3, 6}, {4, 5}, {5, 4}, {6, 3},  // 47
    {7, 2}, {7, 3}, {6, 4}, {5, 5}, {4, 6}, {3, 7}, {4, 7}, {5, 6},  // 55
    {6, 5}, {7, 4}, {7, 5}, {6, 6}, {5, 7}, {6, 7}, {7, 6}, {7, 7}   // 63
};

#endif
