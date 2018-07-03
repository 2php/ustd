#pragma once

#include "ustd/core/builtin.h"

namespace ustd
{

template<class ...T>
struct Tuple;

#define T class
template<T T0 >                                                       /**/ struct Tuple <T0>                                     /**/ { constexpr static let $size =  1; T0 $0;                                                                /**/ };
template<T T0, T T1 >                                                 /**/ struct Tuple <T0, T1>                                 /**/ { constexpr static let $size =  2; T0 $0; T1 $1;                                                         /**/ };
template<T T0, T T1, T T2 >                                           /**/ struct Tuple <T0, T1, T2>                             /**/ { constexpr static let $size =  3; T0 $0; T1 $1; T2 $2;                                                  /**/ };
template<T T0, T T1, T T2, T T3 >                                     /**/ struct Tuple <T0, T1, T2, T3>                         /**/ { constexpr static let $size =  4; T0 $0; T1 $1; T2 $2; T3 $3;                                           /**/ };
template<T T0, T T1, T T2, T T3, T T4 >                               /**/ struct Tuple <T0, T1, T2, T3, T4>                     /**/ { constexpr static let $size =  5; T0 $0; T1 $1; T2 $2; T3 $3; T4 $4;                                    /**/ };
template<T T0, T T1, T T2, T T3, T T4, T T5 >                         /**/ struct Tuple <T0, T1, T2, T3, T4, T5>                 /**/ { constexpr static let $size =  6; T0 $0; T1 $1; T2 $2; T3 $3; T4 $4; T5 $5;                             /**/ };
template<T T0, T T1, T T2, T T3, T T4, T T5, T T6 >                   /**/ struct Tuple <T0, T1, T2, T3, T4, T5, T6>             /**/ { constexpr static let $size =  7; T0 $0; T1 $1; T2 $2; T3 $3; T4 $4; T5 $5; T6 $6;                      /**/ };
template<T T0, T T1, T T2, T T3, T T4, T T5, T T6, T T7 >             /**/ struct Tuple <T0, T1, T2, T3, T4, T5, T6, T7>         /**/ { constexpr static let $size =  8; T0 $0; T1 $1; T2 $2; T3 $3; T4 $4; T5 $5; T6 $6; T7 $7;               /**/ };
template<T T0, T T1, T T2, T T3, T T4, T T5, T T6, T T7, T T8 >       /**/ struct Tuple <T0, T1, T2, T3, T4, T5, T6, T7, T8>     /**/ { constexpr static let $size =  9; T0 $0; T1 $1; T2 $2; T3 $3; T4 $4; T5 $5; T6 $6; T7 $7; T8 $8;        /**/ };
template<T T0, T T1, T T2, T T3, T T4, T T5, T T6, T T7, T T8, T T9 > /**/ struct Tuple <T0, T1, T2, T3, T4, T5, T6, T7, T8, T9> /**/ { constexpr static let $size = 10; T0 $0; T1 $1; T2 $2; T3 $3; T4 $4; T5 $5; T6 $6; T7 $7; T8 $8; T9 $9; /**/ };
#undef T

}
