//
// Created by yawaflua on 13/11/2024.
//
#pragma once

#ifndef OREFTOTZEVAADOM_CPP
#define OREFTOTZEVAADOM_CPP

namespace tzeva_adom {
    inline int oref_threat_to_tzeva_adom(int threat) {
        switch (threat) {
            case 1: return 0;
            case 2: return 5;
            case 3: return 7;
            case 4: return 8;
            case 6:
            case 7: return 3;
            case 8: return 0;
            case 9: return 5;
            case 10: return 4;
            case 11: return 6;
            default: return 9;
        }
    }
}
#endif