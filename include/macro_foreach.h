#ifndef MACRO_FOREACH_H
#define MACRO_FOREACH_H

#define PP_ARG_N( \
        _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
        _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
        _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
        _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
        _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
        _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
        _61,_62,_63,N,...) N

#define PP_RSEQ_N() \
    63,62,61,60, \
    59,58,57,56,55,54,53,52,51,50, \
    49,48,47,46,45,44,43,42,41,40, \
    39,38,37,36,35,34,33,32,31,30, \
    29,28,27,26,25,24,23,22,21,20, \
    19,18,17,16,15,14,13,12,11,10, \
    9,8,7,6,5,4,3,2,1,0

#define _fe_0(_call, ...)
#define _fe_1(_call, x) _call(x)
#define _fe_2(_call, x, ...) _call(x) _fe_1(_call, __VA_ARGS__)
#define _fe_3(_call, x, ...) _call(x) _fe_2(_call, __VA_ARGS__)
#define _fe_4(_call, x, ...) _call(x) _fe_3(_call, __VA_ARGS__)
#define _fe_5(_call, x, ...) _call(x) _fe_4(_call, __VA_ARGS__)
#define _fe_6(_call, x, ...) _call(x) _fe_5(_call, __VA_ARGS__)
#define _fe_7(_call, x, ...) _call(x) _fe_6(_call, __VA_ARGS__)
#define _fe_8(_call, x, ...) _call(x) _fe_7(_call, __VA_ARGS__)
#define _fe_9(_call, x, ...) _call(x) _fe_8(_call, __VA_ARGS__)
#define _fe_10(_call, x, ...) _call(x) _fe_9(_call, __VA_ARGS__)
#define _fe_11(_call, x, ...) _call(x) _fe_10(_call, __VA_ARGS__)
#define _fe_12(_call, x, ...) _call(x) _fe_11(_call, __VA_ARGS__)
#define _fe_13(_call, x, ...) _call(x) _fe_12(_call, __VA_ARGS__)
#define _fe_14(_call, x, ...) _call(x) _fe_13(_call, __VA_ARGS__)
#define _fe_15(_call, x, ...) _call(x) _fe_14(_call, __VA_ARGS__)
#define _fe_16(_call, x, ...) _call(x) _fe_15(_call, __VA_ARGS__)
#define _fe_17(_call, x, ...) _call(x) _fe_16(_call, __VA_ARGS__)
#define _fe_18(_call, x, ...) _call(x) _fe_17(_call, __VA_ARGS__)
#define _fe_19(_call, x, ...) _call(x) _fe_18(_call, __VA_ARGS__)
#define _fe_20(_call, x, ...) _call(x) _fe_19(_call, __VA_ARGS__)
#define _fe_21(_call, x, ...) _call(x) _fe_20(_call, __VA_ARGS__)
#define _fe_22(_call, x, ...) _call(x) _fe_21(_call, __VA_ARGS__)
#define _fe_23(_call, x, ...) _call(x) _fe_22(_call, __VA_ARGS__)
#define _fe_24(_call, x, ...) _call(x) _fe_23(_call, __VA_ARGS__)
#define _fe_25(_call, x, ...) _call(x) _fe_24(_call, __VA_ARGS__)
#define _fe_26(_call, x, ...) _call(x) _fe_25(_call, __VA_ARGS__)
#define _fe_27(_call, x, ...) _call(x) _fe_26(_call, __VA_ARGS__)
#define _fe_28(_call, x, ...) _call(x) _fe_27(_call, __VA_ARGS__)
#define _fe_29(_call, x, ...) _call(x) _fe_28(_call, __VA_ARGS__)
#define _fe_30(_call, x, ...) _call(x) _fe_29(_call, __VA_ARGS__)
#define _fe_31(_call, x, ...) _call(x) _fe_30(_call, __VA_ARGS__)
#define _fe_32(_call, x, ...) _call(x) _fe_31(_call, __VA_ARGS__)
#define _fe_33(_call, x, ...) _call(x) _fe_32(_call, __VA_ARGS__)
#define _fe_34(_call, x, ...) _call(x) _fe_33(_call, __VA_ARGS__)
#define _fe_35(_call, x, ...) _call(x) _fe_34(_call, __VA_ARGS__)
#define _fe_36(_call, x, ...) _call(x) _fe_35(_call, __VA_ARGS__)
#define _fe_37(_call, x, ...) _call(x) _fe_36(_call, __VA_ARGS__)
#define _fe_38(_call, x, ...) _call(x) _fe_37(_call, __VA_ARGS__)
#define _fe_39(_call, x, ...) _call(x) _fe_38(_call, __VA_ARGS__)
#define _fe_40(_call, x, ...) _call(x) _fe_39(_call, __VA_ARGS__)
#define _fe_41(_call, x, ...) _call(x) _fe_40(_call, __VA_ARGS__)
#define _fe_42(_call, x, ...) _call(x) _fe_41(_call, __VA_ARGS__)
#define _fe_43(_call, x, ...) _call(x) _fe_42(_call, __VA_ARGS__)
#define _fe_44(_call, x, ...) _call(x) _fe_43(_call, __VA_ARGS__)
#define _fe_45(_call, x, ...) _call(x) _fe_44(_call, __VA_ARGS__)
#define _fe_46(_call, x, ...) _call(x) _fe_45(_call, __VA_ARGS__)
#define _fe_47(_call, x, ...) _call(x) _fe_46(_call, __VA_ARGS__)
#define _fe_48(_call, x, ...) _call(x) _fe_47(_call, __VA_ARGS__)
#define _fe_49(_call, x, ...) _call(x) _fe_48(_call, __VA_ARGS__)
#define _fe_50(_call, x, ...) _call(x) _fe_49(_call, __VA_ARGS__)
#define _fe_51(_call, x, ...) _call(x) _fe_50(_call, __VA_ARGS__)
#define _fe_52(_call, x, ...) _call(x) _fe_51(_call, __VA_ARGS__)
#define _fe_53(_call, x, ...) _call(x) _fe_52(_call, __VA_ARGS__)
#define _fe_54(_call, x, ...) _call(x) _fe_53(_call, __VA_ARGS__)
#define _fe_55(_call, x, ...) _call(x) _fe_54(_call, __VA_ARGS__)
#define _fe_56(_call, x, ...) _call(x) _fe_55(_call, __VA_ARGS__)
#define _fe_57(_call, x, ...) _call(x) _fe_56(_call, __VA_ARGS__)
#define _fe_58(_call, x, ...) _call(x) _fe_57(_call, __VA_ARGS__)
#define _fe_59(_call, x, ...) _call(x) _fe_58(_call, __VA_ARGS__)
#define _fe_60(_call, x, ...) _call(x) _fe_59(_call, __VA_ARGS__)
#define _fe_61(_call, x, ...) _call(x) _fe_60(_call, __VA_ARGS__)
#define _fe_62(_call, x, ...) _call(x) _fe_61(_call, __VA_ARGS__)

#define CALL_MACRO_X_FOR_EACH(x, ...) \
        PP_ARG_N("ignored", ##__VA_ARGS__, \
                    _fe_62, _fe_61, _fe_60, _fe_59,\
                _fe_58, _fe_57, _fe_56, _fe_55,\
                _fe_54, _fe_53, _fe_52, _fe_51,\
                _fe_50, _fe_49, _fe_48, _fe_47,\
                _fe_46, _fe_45, _fe_44, _fe_43,\
                _fe_42, _fe_41, _fe_40, _fe_39,\
                _fe_38, _fe_37, _fe_36, _fe_35,\
                _fe_34, _fe_33, _fe_32, _fe_31,\
                _fe_30, _fe_29, _fe_28, _fe_27,\
                _fe_26, _fe_25, _fe_24, _fe_23,\
                _fe_22, _fe_21, _fe_20, _fe_19,\
                _fe_18, _fe_17, _fe_16, _fe_15,\
                _fe_14, _fe_13, _fe_12, _fe_11,\
                _fe_10, _fe_9, _fe_8, _fe_7,\
                _fe_6, _fe_5, _fe_4, _fe_3,\
                _fe_2, _fe_1, _fe_0)(x, ##__VA_ARGS__)

// the same but without '##'
#define CALL_MACRO_X_FOR_EACH2(x, ...) \
        PP_ARG_N("ignored", __VA_ARGS__, \
                    _fe_62, _fe_61, _fe_60, _fe_59,\
                _fe_58, _fe_57, _fe_56, _fe_55,\
                _fe_54, _fe_53, _fe_52, _fe_51,\
                _fe_50, _fe_49, _fe_48, _fe_47,\
                _fe_46, _fe_45, _fe_44, _fe_43,\
                _fe_42, _fe_41, _fe_40, _fe_39,\
                _fe_38, _fe_37, _fe_36, _fe_35,\
                _fe_34, _fe_33, _fe_32, _fe_31,\
                _fe_30, _fe_29, _fe_28, _fe_27,\
                _fe_26, _fe_25, _fe_24, _fe_23,\
                _fe_22, _fe_21, _fe_20, _fe_19,\
                _fe_18, _fe_17, _fe_16, _fe_15,\
                _fe_14, _fe_13, _fe_12, _fe_11,\
                _fe_10, _fe_9, _fe_8, _fe_7,\
                _fe_6, _fe_5, _fe_4, _fe_3,\
                _fe_2, _fe_1, _fe_0)(x, __VA_ARGS__)

#define GET_CALL_CHAIN(x, ...) \
    PP_ARG_N("ignored", ##__VA_ARGS__, \
                _fe_62, _fe_61, _fe_60, _fe_59,\
                _fe_58, _fe_57, _fe_56, _fe_55,\
                _fe_54, _fe_53, _fe_52, _fe_51,\
                _fe_50, _fe_49, _fe_48, _fe_47,\
                _fe_46, _fe_45, _fe_44, _fe_43,\
                _fe_42, _fe_41, _fe_40, _fe_39,\
                _fe_38, _fe_37, _fe_36, _fe_35,\
                _fe_34, _fe_33, _fe_32, _fe_31,\
                _fe_30, _fe_29, _fe_28, _fe_27,\
                _fe_26, _fe_25, _fe_24, _fe_23,\
                _fe_22, _fe_21, _fe_20, _fe_19,\
                _fe_18, _fe_17, _fe_16, _fe_15,\
                _fe_14, _fe_13, _fe_12, _fe_11,\
                _fe_10, _fe_9, _fe_8, _fe_7,\
                _fe_6, _fe_5, _fe_4, _fe_3,\
                _fe_2, _fe_1, _fe_0)

#define REP0(X)
#define REP1(X) X
#define REP2(X) REP1(X) X
#define REP3(X) REP2(X) X
#define REP4(X) REP3(X) X
#define REP5(X) REP4(X) X
#define REP6(X) REP5(X) X
#define REP7(X) REP6(X) X
#define REP8(X) REP7(X) X
#define REP9(X) REP8(X) X
#define REP10(X) REP9(X) X

#define REP(HUNDREDS,TENS,ONES,X) \
  REP##HUNDREDS(REP10(REP10(X))) \
  REP##TENS(REP10(X)) \
  REP##ONES(X)

#define ARGS1 0
#define ARGS2 0, 1
#define ARGS3 0, 1, 2
#define ARGS4 0, 1, 2, 3
#define ARGS5 0, 1, 2, 3, 4
#define ARGS6 0, 1, 2, 3, 4, 5
#define ARGS7 0, 1, 2, 3, 4, 5, 6
#define ARGS8 0, 1, 2, 3, 4, 5, 6, 7
#define ARGS9 0, 1, 2, 3, 4, 5, 6, 7, 8
#define ARGS10 0, 1, 2, 3, 4, 5, 6, 7, 8, 9


#endif // MACRO_FOREACH_H
