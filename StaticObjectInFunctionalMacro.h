
// 例えば、文字列の関数名から対応する関数を呼ぶ機能を考える。
// 毎回文字列で検索するのは無駄なので、1 度だけ文字列から ID を得て、
// あとはその ID から関数ポインタ配列の要素を得るなりするような実装になると思われる。
//
// そしてユーザー側では return_value = CallFunctionByName(DoSomething, arg1)
// のような書き方ができるようにして、上記のような内部実装を意識せずに済むようにしたい。
// これを実現するため、マクロの中で static 変数を用意して ID を記憶させたい。


// これだと CallFunctionByID の戻り値を返せない。
// 従来はマクロの引数に受け取る変数を指定するしかなかったが…
//#define CallFunctionByName(funcname, ...)\
//    {\
//        static const int id_##funcname = GetFunctionID(#funcname);\
//        CallFunctionByID(id_##funcname, __VA_ARGS__);\
//    }


// lambda を使えば返せる！
// しかしこの lambda は inline 展開されないようで、変数指定方式より効率は落ちる
//#define CallFunctionByName(funcname, ...)\
//    ([&]()->variant{\
//        static const int id_##funcname = GetFunctionID(#funcname);\
//        return CallFunctionByID(id_##funcname, __VA_ARGS__);\
//    })()

