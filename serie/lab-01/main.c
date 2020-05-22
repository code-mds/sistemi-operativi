int callee(int x, int y) {
    return x+y;
}

int main(){
    int a,b;
    a = 13;
    b = 17;
    callee(a,b);
}