## random thoughts
### march
-march=native 
https://stackoverflow.com/questions/52653025/why-is-march-native-used-so-rarely

### reverse_copy

use [reverse_copy](https://cplusplus.com/reference/algorithm/reverse_copy/) instead of 
```c
vector<int> invpermvals=vector<int>();
for(auto p : permVal){
    invpermvals.push_back(invperm[p]);
}
```

### using uint_fast8_t

[c++ types](https://en.cppreference.com/w/cpp/types/integer)

### cashing behavior

there might be value in cashing some intermediates 

## bitdomain reimplementation
### old

[godbolt implementation](https://godbolt.org/#z:OYLghAFBqd5QCxAYwPYBMCmBRdBLAF1QCcAaPECAMzwBtMA7AQwFtMQByARg9KtQYEAysib0QXACx8BBAKoBnTAAUAHpwAMvAFYTStJg1DIApACYAQuYukl9ZATwDKjdAGFUtAK4sGe1wAyeAyYAHI%2BAEaYxCAAHACcpAAOqAqETgwe3r56KWmOAkEh4SxRMQm2mPYFDEIETMQEWT5%2BXJXVGXUNBEVhkdFxiQr1jc05bcPdvSVlgwCUtqhexMjsHOYAzMHI3lgA1CYbbgBumA4kh9gmGgCC1zdeaUZ7zGwKSUyre8Poh1a39wIAE8kpgsFRvgRiF4HHsIoR0KgWExggB9AgmADs/xuezxe1O52IhzcEVQnkue0RLD%2B93xe2CBG%2BeAAXphabd6Yy9jRiMMqhzcfjuQwfAAVaHsjY4%2BnwgjUlEMdEQOaC2UIpGK5Ui0hkzwMhiEVXSun446oPDob6YAgQEXGmVmi1W4iYJS2%2B1qp2W622h2mvHmn2u90qr14vW0F4CTBhk2cs1nIjEo6MymoJI1BRxx2B517JLERk5%2B5YgAicI1yLRGPjd1ucoVNZAIEbmprYexpuphzLhOTJMjlxLCbSbN7Gi9vP5tF7AFouF7RSwJV4pWXJ3Xy6WG1WtQQW23q0qPYIXrryVHgkasY6exs%2B0mLkchxtsBAGORDQR/aPWeuGC9PAqDtb9VS7BM8WnAgBQfTdcxecVJV7McpUdcs9iqJQDggoUoLwPkYNnB8F3DRCV2QuCvW3AFMTLHcbiDK0j33FtQ09XC8QAei4wtGRA8wzGgqo4UwfhXV9RxnnMABWdAQAOMw5JMGS3EAswzFIT9hNoX8hWAiAexkixAJkstVUgvZXQIZZAK3BMjJMlT6IfKE1yXJD3MsXtFzrPEDJ03t5y4HC3CxNwXhJHSLLwnkCJnXs7MdHi%2BMEASNJ0vYmCoGDiEk4JgEUuSFNk35VPUzTtPioi9Jo%2BtGPzFjmxAEMbVApkGHA5LeKLNKIEEzKonEzArLdG0pMK0qSqUsq1MErS%2BGqqo9P8kCwDARzTPMgNRps4gkoYvFNuc3sqDEJQPIotc5x8oCQLslyy2i28dvE9qGUS6xfIsPASVQv5fu8ywup21bDKRFSfuckHLPpQKH1%2Bvz6XxCJXSYABrMi8Tq%2BkceFED4cezrQbiwjYLLUikbxlLeoIdKhKWqNsty0b3Qmor5I5lS5o0hbnq3OiGKYyt5XbE82LamGhXvR8iUHS9h1Q0g3MwFbSYSqikeXVd13%2BgX6IBW5haa8WWrGv0XociGH37Z9SQVt8ICVs7aCUNXCcpx1tcojdBTq%2B5IxFptTYYGNO0dazbMheSWsMYA3RbGgGHQVEDOpAA6KJgGCFVSCpJF09cXODmM5yIED1Qusj/a9lUWk6LmA4H17fOWEL5ORzuQXDfq%2B5bZTNw0zfIOxfRFsMyzcPTX7kkh%2BwTDXabisZ9TQRh3dkh3u0Cc/j2bejj1iw9%2Bsawpa5EDHO3syz%2BRrD06SR4EFRCJPnRiBtDV7Hu6FPHq4YBeFB%2B0FtKDgCxaCcBkrwPw3BeCoE4GFIGR8FBLBWCNTYPBlaaFAQsdGIANgbHTvgohxCSEADZ9CcEkFArBpA4EcF4AoEAGhMEcC0AsOAsAkBoBYEkOg0RyCUG4bw%2BgMRgBSDaDQWguVGHlxofCZgxAgScAwfIhoQIADyERtBJmUbwbhbBBDqIYLQJRrDeBYAiF4YAbgxCu10aQLAyIjDiDMQ4giSY8CnEYa4zAqgzheBgvYxkVQaG0DwKjNRHgsA0KhHgFg9jTjEDJEoMsmAnHADCUYLBCwqAGGAAoAAangTAAB3dRoJoEYP4IIEQYh2BSBkIIRQKh1CuN0G0AwWTTCIP0OExhkAFgTwyN4ucyIVgIBQujIEBh0YjTnOojYew5w/F7MAUUJ9LBmC0LQxJRYsD9NznYDxzgICuDGK0UggRgh9FKAMNoeR0gCHObkVIjyGDTH6DECYVRjm1BGE0TwLQ9BHIcJ0f5HzblfNsP855ExwXXJmHchYyDlirAkGAiB1DXF0NrrEUhc5SGSD2MAZAyA9hSHTiFCAuBCAkEUhsLgcxeAHjMXMHBeCCEkK5UQ8h4COBUK0jQuhDCmEsLYaQDhiAQBLAIA/AgAiIBCL4cQUIrA1jYDLAAcT2KQ9OpC9gQE1aEOQewWAYBGiSslXANhMsufgZMlo9DVOEKIcQDTnXNLUDQ9ppASnECYEkXRGKOCQMFdizg6iAlyr2KgCEqg8UEqJZa8lkhKUGo8Dw5V9LGXMuyey/BhDuVcoofyrF2zhW2FFSythwazBltgZwXNrKFiJLSM4SQQA)
```c
typedef struct bitdomain_t{
    vector<bool> dom; //make this an int
    int size; //useless (maybe just set it it might not hurt)
    int firstel; // cached behavior
    int numTrue; // make this a function with cached behavior
    bitdomain_t();
    bitdomain_t(int n,bool init);
    void set(int n); // 
    void reset(int n);
    void set();
    void reset();
    bool none();
    vector<int> options();
    void print();
} bitdomain_t;
```

### new 

[godbolt implementation](https://godbolt.org/#z:OYLghAFBqd5QCxAYwPYBMCmBRdBLAF1QCcAaPECAMzwBtMA7AQwFtMQByARg9KtQYEAysib0QXACx8BBAKoBnTAAUAHpwAMvAFYTStJg1DIApACYAQuYukl9ZATwDKjdAGFUtAK4sGIaa4AMngMmAByPgBGmMQgAKxmpAAOqAqETgwe3r7%2ByanpAsGhESzRsQm2mPaOAkIETMQEWT5%2B0naYDhl1DQRF4VEx8YkK9Y3NOW2jvSH9pYMJAJS2qF7EyOwc5gDMIcjeWADUJltuyCP4gsfYBzcHAPR3B%2BcgICwhJhoAgh/fXz8EAE8kpgsFQngRiF4HAdIoR0KgWEwQgB9AgmADsVi%2BtwOIQITzwAC9MMcsZ8cV48VszKiDvCWAA1MSkn44gBuqDw6CemAIEDxBwYCxZ2NuHK5PL5wq2ZPZnO5xEwSj5AqFIvJYvlB0Vyog0tlt0iqE8goEmD16pxRpNeAUAHkkjUGPzBIL9azbniAFQHVCOjIKC0yn4YgAiMLhCKRDFR6pDX3F3NhBHp0dRL11quFmI9N3pTNoRy24fzYiO6LcB0pgmpscxXDDRbcx0rDDj6ND8c%2BiYjKajKIILx1vJd%2BLVOdFeYRBaLJenZfMADYDgA/CDVgi1tH1xstltu9udv4JrXJ1MDjMj7MGnFTxkL4sHCBcJv7tLEhYHAC0Bwbwcn5ZHr83anpGiIXiAw5ShiN50vOhbHOGGiHl2PzWoWZ79jGg4gAwZoWhOGo3IqBCrAwBzIsikSUrQjgxikSRoF4ggQKWtDun8HZduhvbnthLy2g6TqjgehE4iRZFPmxRbYFcbpHGYy4bluMENlx/7fFxx6fN6vFYemIB%2Bk6gbXrmuKul6VQKLOgqYAA7uZaJxBYlHUXQdHIgxTEsWxwpxJ2GkUniAActJ4DZyGBbcyk0vikQ2Wxlq3PwxDriFtLaIhkUWAcmUnO%2BJIyrl1jWKQ0mIXB94ISc%2B5cKZAE4ngVCsfBClKVSsWqWG9VEbeOJWSYzl4IN4YVXlsF9Tcw2WCVUV9WGHoLQBEnEORA0aQtMocEstCcHEvB%2BNwvCoJwzYzZYTwrGsmAKVsPCkIOHBaAsSwANYgFsWwAHSfb9f3/Yu%2BicJIB2aMdnC8AoIAaA9YNLHAsBIGgLBJHQMTkJQyOo/QsTAFIXB8O5MRQxAkRg6QsLMMQAKcPdlMNACdqRNoHSPfdyNsIIdoMLQNNPbwWDUcAbhiLQUNHWVmCIkY4j82VeCKp0bJKuTmCqB0XgEBs914lU5O0HgkTEAzHhYOTEJ4CwtO8MrxBGkooZS4YwAG0YcN8AYwAKAyeD2Q6jDWzIggiGI7BSEH8hKGo5O6ATBhu6Y502AbkRQ5ASxGQGnBfoiawIIhCivQCBivTdX52ls37nIhwAMF4JWWGYWikKgtvEFyKvp5U1QZC4DDuJ4LR6EEMwlGUegpGkTrjH4BOTwUDB9GPgwE%2B0nS1FMM96GvTrdI0S8DLEq%2Bb4POTHz0B9zEfSwKFd6wSDte2g3LJ0cAcqjBYuX6LpIBzAMgyBfySC%2Bi%2BCAuBCAkFunVXgj1npvQ%2Bt9f6SDfqA12hwEGpBDrN1fpDaGsN%2BbwxgIgQymskiawxhALGaNiBhFYBsbAoYADiBxFxfWXBARhYQ5AHBYBgG6/9AFcC2AsXgIIIHt3QHofgwdRDiHDtIyOKh1By1jqQOyxskjW0fhwfamDyavztKQzWvowQfy/j/P%2BACgEgKfB4FG1CoEiPwXA0g71Po/WQUgoG6Dn7YIhrYPBsDtraLML48GHAYFwyWLbNIzhJBAA%3D%3D%3D)

```c
typedef struct bitdomain_t{
    int size;
    uint domVal;
    bitdomain_t();                // not implemented
    bitdomain_t(int n,bool init); // not implemented 
    void set(int n);
    void reset(int n);
    void set();
    void reset();
    bool none();
    bool isOption(int n);
    uint* options();
    void print();                 // not implemented 
} bitdomain_t;
```

### questions we can ask the struct:
1. do you have posible numbers
2. what are all the numbers you can be
3. is n an posible number
4. smalest number you can be

### changes we can make:
1. set a number as a posibility
    - changes 1, 2, 3, 4
2. remove a number as a posibility
    - changes 1, 2, 3, 4
3. set all numbers as posibility
    - changes 1, 2, 3, 4
4. remove all number as a posibility
    - changes 1, 2, 3, 4

### resolve questions:
1. do you have posible number

    using __builtin_popcount()
    ```c
    bool bitdomain_t::none(){
        return __builtin_popcount(domVal);
    }
    ```
2. what are all the numbers you can be

    why are we using vector that dynamicaly alocates
    ```c
    uint* bitdomain_t::options(){
    uint *els = new uint[__builtin_popcount(domVal)];
    uint i = 0;
    uint b = domVal;
    for(uint j=0; j<size; j++){
        if(domVal & uint{1}){
            els[i] = j;
            i++;
        }
    }
    return els;
    };
    ```
3. is n an posible number

    [stackoverflow origin](https://stackoverflow.com/questions/47981/how-to-set-clear-and-toggle-a-single-bit) and [godbolt analysation](https://godbolt.org/z/Wfzh8xsjW)

    ```c
    bool bitdomain_t::isOption(int n){
        return (domVal >> n) & uint{1};
    }
    ```
4. smalest number you can be
    
    using __builtin_clz(n)
    this question is currently not asked but could be usefull, largest is asked at the moment -> migth be worth it

### resolve changes:
1. set a number as a posibility

    ```c
    void bitdomain_t::set(int n){
        domVal = domVal | uint{1} << n;
    }
    ```
2. remove a number as a posibility

    ```c
    void bitdomain_t::reset(int n){
        domVal = domVal & ~(uint{1} << n);
    }
    ```

3. set all numbers as posibility

    since it is a number we can just max it out NOTE: need to chop of unneeded ones at the front, the reason it looks so wierd is to resolve the note
    ```c
    void bitdomain_t::set(){
        domVal = (1 << matrixSize) - 1;
    }
    ```

4. remove all number as a posibility

    since it is a number we can just zero it out
    ```c
    void bitdomain_t::reset(int n){
        domVal = -1;
    }
    ```