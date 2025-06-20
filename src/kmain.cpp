// extern "C": Using C-style linking keeps the function name unmodified;
// otherwise C++ extends the name and we can't call it from assembly.
extern "C" int kernel_main()
{
    while(true);
    return 0;
}
