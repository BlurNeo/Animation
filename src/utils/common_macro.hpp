#pragma once

#include <chrono>
#if MOTION_EST_TIME
#  define __TIC__(name)
// #define __TIC__(name) \
//     auto t_start_##name = std::chrono::high_resolution_clock::now(); \
//     static double t_total_##name = 0; \
//     static int count_##name = 0;
#  define __TOC__(name)
// #define __TOC__(name) {\
//     auto t_end_##name = std::chrono::high_resolution_clock::now(); \
//     auto during_##name = std::chrono::duration<double, std::milli>(t_end_##name - t_start_##name).count(); \
//     MOTION_LOGD(#name" time cost:%lf ms.\n" , during_##name); \
//     t_total_##name += during_##name; count_##name++; \
//     if (count_##name == 100) { \
//         auto t_average_##name = t_total_##name / 100.0; \
//         MOTION_LOGI(#name" average time cost: %lf ms.\n" , t_average_##name); count_##name = 0; t_total_##name = 0;\
//     }\
// }
#else
#  define __TIC__(name)
#  define __TOC__(name)
#endif

#define ANIM_LOGI(fmt, ...) \
  printf("%s [File %s][Line %d] " fmt, __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__);
#define ANIM_LOGE(fmt, ...) \
  printf("%s [File %s][Line %d] " fmt, __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__);
#define ANIM_LOGD(fmt, ...) \
  printf("%s [File %s][Line %d] " fmt, __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__);

#define GL_CHECK_RET(ret) \
  { \
    int gl = glGetError(); \
    if (gl != GL_NO_ERROR) {             \
      ANIM_LOGE("Check Errorr: %d\n", gl);       \
      return ret;                          \
    } \
  }

#define GL_CHECK() \
  { \
    int gl = glGetError(); \
    if (gl != GL_NO_ERROR) {             \
      ANIM_LOGE("Check Errorr: %d\n", gl);       \
      return;                          \
    } \
  }

#define CHECK_EQ_RET(val, expected, ret) \
  if ((val) != (expected)) {             \
    ANIM_LOGE("Check Errorr\n");       \
    return ret;                          \
  }
#define CHECK_NE_RET(val, expected, ret) \
  if ((val) == (expected)) {             \
    ANIM_LOGE("Check Error\n");        \
    return ret;                          \
  }
#define CHECK_NE_RET_WITH_LOG(val, expected, ret, log) \
  if ((val) == (expected)) {                           \
    ANIM_LOGE(log);                                  \
    return ret;                                        \
  }

#define CHECK_EQ_RET_WITH_LOG(val, expected, ret, log) \
  if ((val) != (expected)) {                           \
    ANIM_LOGE(log);                                  \
    return ret;                                        \
  }

#define CHECK_EQ(val, expected)   \
  if ((val) != (expected)) {      \
    ANIM_LOGE("Check Error\n"); \
    return;                       \
  }
#define CHECK_NE(val, expected)   \
  if ((val) == (expected)) {      \
    ANIM_LOGE("Check Error\n"); \
    return;                       \
  }

#define CHECK_EQ_WITH_LOG(val, expected, log) \
  if ((val) != (expected)) {                  \
    ANIM_LOGE(log);                         \
    return;                                   \
  }
#define CHECK_NE_WITH_LOG(val, expected, log) \
  if ((val) == (expected)) {                  \
    ANIM_LOGE(log);                         \
    return;                                   \
  }