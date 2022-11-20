#include <iostream>
#include <array>
#include <algorithm>

#include <time.h>
#include <vector>


#include <stdint.h>

//  Windows
#ifdef _WIN32

#include <intrin.h>
uint64_t rdtsc() {
    return __rdtsc();
}

//  Linux/GCC
#else

uint64_t rdtsc() {
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

#endif


/*
Сортировка подсчетом со словарем в 256 значений.
    begin/end   - Входная оследовательность
    out         - Выходная последовательность
    extract_key - функция экстракта ключа из значения.
                  Должна быть вида uint8_t(*_FwdIt).
*/
template<class _FwdIt, class _Pr>
void counting_sort(_FwdIt begin, _FwdIt end, _FwdIt out,_Pr extract_key) {
    size_t counts[256] = {};
    for (_FwdIt iter = begin; iter < end; iter = std::next(iter)) {
        size_t key = extract_key(*iter);
        counts[key]++;
    }
    size_t total = 0;
    for (size_t& count: counts) {
        size_t old = count;
        count = total;
        total += old;
    }
    for (begin; begin != end; begin = std::next(begin)) {
        _FwdIt where = out;
        std::advance(where, counts[extract_key(*begin)]++);
        *where = std::move(*begin);
    }
}

/*
Сортировка за O(n):
    begin/end                    - Входная последовательность
    buf                          - Буфер (должен быть той же длины, что и последовательность)
    n                            - Размер элемента в байтах
    extract_key_from_n(size_t n) - Функция экстракта ключа из значения n-ного байта. 
                                   Должна быть вида (uint8_t(*_FwdIt))(size_t)
*/
template<class _FwdIt, class _Pr>
void fast_sort(_FwdIt begin, _FwdIt end, _FwdIt buf, size_t n, _Pr extract_key_from_n) {
    size_t m = std::distance(begin, end);
    for (size_t i = 0; i <= n; i++) {
        counting_sort(begin, end, buf, extract_key_from_n(i));
        std::copy_n(begin, n, buf);
    }
}


template<class _T> uint8_t extract_key_from_0(_T a) { return a & 0xff; } // byte(T)
template<class _T> uint8_t extract_key_from_1(_T a) { return (a >> 1) & 0xff; } // byte(T)
template<class _T> uint8_t extract_key_from_2(_T a) { return (a >> 2) & 0xff; } // byte(T)
template<class _T> uint8_t extract_key_from_3(_T a) { return (a >> 3) & 0xff; } // byte(T)
template<class _T> uint8_t extract_key_from_4(_T a) { return (a >> 4) & 0xff; } // byte(T)
template<class _T> uint8_t extract_key_from_5(_T a) { return (a >> 5) & 0xff; } // byte(T)
template<class _T> uint8_t extract_key_from_6(_T a) { return (a >> 6) & 0xff; } // byte(T)
template<class _T> uint8_t extract_key_from_7(_T a) { return (a >> 7) & 0xff; } // byte(T)
template<class _T> uint8_t extract_key_from_8(_T a) { return (a >> 8) & 0xff; } // byte(T)

template<class _T1, class _T2>
auto extract_key_from_n_byte(_T1 n) {
    if (n == 1) return extract_key_from_1<_T2>;
    if (n == 2) return extract_key_from_2<_T2>;
    if (n == 3) return extract_key_from_3<_T2>;
    if (n == 4) return extract_key_from_4<_T2>;
    if (n == 5) return extract_key_from_5<_T2>;
    if (n == 6) return extract_key_from_6<_T2>;
    if (n == 7) return extract_key_from_7<_T2>;
    if (n == 8) return extract_key_from_8<_T2>;
    return extract_key_from_0<_T2>;
}; // byte(*f(T2))(T1)


int main()
{
    std::vector<int> arr(9'000'000);
    std::vector<int> buf(9'000'000);
    
    for (int& elm : arr) elm = rand();
    size_t t = rdtsc();
    fast_sort(
        arr.begin(), arr.end(),
        buf.begin(),
        sizeof(int),
        extract_key_from_n_byte<size_t, int>
    );
    std::cout << rdtsc() - t << '\n';

    for (int& elm : arr) elm = rand();
    t = rdtsc();
    std::sort(arr.begin(), arr.end());
    std::cout << rdtsc() - t << '\n';

    return 0;
}
