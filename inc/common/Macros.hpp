#define ENABLE_LOG 0

#if ENABLE_LOG
  #define LOG(x) x;
#else
  #define LOG(x) do {} while(0);
#endif