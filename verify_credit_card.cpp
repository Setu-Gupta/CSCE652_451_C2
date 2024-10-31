//#include <cstdio>

bool verify_credit_card(int* digits) {
    int tot = 0;
    for (int i = 0; i < 16; i++) {
        int num = digits[15 - i];
        if (i % 2 == 1) {
            num *= 2;
            if (num > 9) {
                num -= 9;  
            }
        }
        tot += num;
    }
    return (tot % 10 == 0);
}

/*
int main(){
    //int my_card[16] = {4,4,8,5,3,4,5,1,9,4,4,9,3,8,8,3};
    int my_card[16] = {4,4,8,5,3,4,5,1,8,4,4,9,3,8,8,3};
    printf("%d\n", verify_credit_card(my_card));
}*/