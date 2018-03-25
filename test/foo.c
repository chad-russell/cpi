int fib(int n) {
    int nn = n;
    if (nn == 0 || nn == 1) {
        return 1;
    }
    return fib(nn - 1) + fib(nn - 2);
}

struct foo {
    int x;
    int y;
};

int main(int argc, char **argv)
{
    struct foo a;
    a.x = 13;
    a.y = 18;

    int b = fib(a.x) + 3;

    int c = 91;

    return a.x + a.y + b + c;
}
