#define ENABLE_LOG 1 

#if ENABLE_LOG
  #define LOG(x) x;
#else
  #define LOG(x) do {} while(0);
#endif