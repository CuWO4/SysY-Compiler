int main() {
    int x = 10;
    {
        int y = 20;
        {
            int z = 30;
        }
    }
    {
        int p = 40;
    }
    return x;
}
