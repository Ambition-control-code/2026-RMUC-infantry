#ifndef VOFA_H
#define VOFA_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 发送 VOFA JustFloat 数据
 */
void vofa_transmit(float v1, float v2, float v3, float v4);

#ifdef __cplusplus
}
#endif

#endif // VOFA_H
