#include <stdio.h>
int func4(int x, int y, int z){
    int eax = z;
    unsigned int ecx = eax;
    ecx >>= 31;
    eax += ecx;
    eax >>= 1
    if(ecx <= x){
        eax = 0;
        if(ecx >= x){
            return 0;
        }else{
            x = ecx + 1;
            eax = func(x, y, z);
            eax *= 2;
            ++eax;
        }
    }
    return eax;
}
int main(){
    return 0;
}