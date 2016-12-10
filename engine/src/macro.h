#if !defined(MACRO_H)

#define FOREVER for (;;)
#define FOR(p, q, z) for (size_t z = p; z < q; ++z)
#define DFOR(p, q, z) for (size_t z = q; z > p; --z)
#define PRINT(p) std::cout << p << std::endl
#define STRING(s) #s
#define SCALL(x, c) if (x != nullptr) c
#define ZeroMemory(f,q) for(unsigned Idx = 0; Idx < q; Idx++) f[Idx] = 0	

#define MACRO_H
#endif