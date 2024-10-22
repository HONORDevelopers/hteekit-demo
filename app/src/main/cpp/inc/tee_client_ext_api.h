/*
 * Copyright (c) Honor Device Co., Ltd. 2020-2020. All rights reserved.
 * Description: tee client api only for honor ca, which cannot be opened to third-party companies
 * Create: 2020-01-16
 */

#ifndef _TEE_CLIENT_EXT_API_H_
#define _TEE_CLIENT_EXT_API_H_

#include "tee_client_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @ingroup  TEEC_EXT_FUNC
 * @brief register agent API
 *
 * @par 描述:
 * 非安全侧注册agent（listener）的接口
 *
 * @attention 此操作首先会映射并注册共享内存（目前只支持4K大小的共享内存），
 * 然后再注册agent
 * @param agentId [IN] 用户传入一个agent_id，与TA进行通信的唯一标识，
 * 因此，TA发送消息给CA时，会根据该agent_id来发起通信
 * @param devFd [OUT] 用户获取访问TEE驱动设备的描述符
 * @param buffer [OUT] 用户获取指向共享内存的用户态地址
 * @retval #TEEC_SUCCESS agent注册成功
 * @retval #TEEC_ERROR_GENERIC 其他的一般错误
 *
 * @par 依赖:
 * @li libteec：该接口所属的共享库
 * @li tee_client_ext_api.h：该接口声明所在头文件
 * @see 无
 * @since V100R005C00
 */
TEEC_Result TEEC_EXT_RegisterAgent(uint32_t agentId, int *devFd, void **buffer);

/*
 * @ingroup  TEEC_EXT_FUNC
 * @brief wait for event from TA
 *
 * @par 描述:
 * 非安全侧agent等待安全侧TA事件
 *
 * @attention 此接口会阻塞等待，因此建议在新创建的线程中来调用此接口
 * @param agentId [IN] 用户传入一个agent_id，与TA进行通信的唯一标识，
 * 因此，TA发送消息给CA时，会根据该agent_id来发起通信
 * @param devFd [IN] 访问TEE驱动设备的描述符
 * @retval #TEEC_SUCCESS agent等待TA消息事件成功
 * @retval #TEEC_ERROR_GENERIC 其他的一般错误
 *
 * @par 依赖:
 * @li libteec：该接口所属的共享库
 * @li tee_client_ext_api.h：该接口声明所在头文件
 * @see 无
 * @since V100R005C00
 */
TEEC_Result TEEC_EXT_WaitEvent(uint32_t agentId, int devFd);

/*
 * @ingroup  TEEC_EXT_FUNC
 * @brief send response to TA
 *
 * @par 描述:
 * 非安全侧agent响应安全侧TA的事件
 *
 * @attention 此接口会唤醒主进程继续执行
 * @param agentId [IN] 用户传入一个agent_id，与TA进行通信的唯一标识，
 * 因此，TA发送消息给CA时，会根据该agent_id来发起通信
 * @param devFd [IN] 用户访问TEE驱动设备的描述符
 * @retval #TEEC_SUCCESS agent响应事件发送成功
 * @retval #TEEC_ERROR_GENERIC 其他的一般错误
 *
 * @par 依赖:
 * @li libteec：该接口所属的共享库
 * @li tee_client_ext_api.h：该接口声明所在头文件
 * @see 无
 * @since V100R005C00
 */
TEEC_Result TEEC_EXT_SendEventResponse(uint32_t agentId, int devFd);


/*
 * @ingroup  TEEC_EXT_FUNC
 * @brief unregister agent API
 *
 * @par 描述:
 * 非安全侧去注册agent（listener）的接口
 *
 * @attention 此接口会通知内核去注册agent，同时释放共享内存，因此指向共享内存的
 * 用户态指针不能再继续使用了
 * @param agentId [IN] 用户传入一个agent_id，与TA进行通信的唯一标识，
 * 因此，TA发送消息给CA时，会根据该agent_id来发起通信
 * @param devFd [IN] 用户访问TEE驱动设备的描述符
 * @param buffer [IN] 用户指向共享内存的用户态地址，函数返回时会将指针置为NULL
 * @retval #TEEC_SUCCESS agent去注册成功
 * @retval #TEEC_ERROR_GENERIC 其他的一般错误
 *
 * @par 依赖:
 * @li libteec：该接口所属的共享库
 * @li tee_client_ext_api.h：该接口声明所在头文件
 * @see 无
 * @since V100R005C00
 */
TEEC_Result TEEC_EXT_UnregisterAgent(uint32_t agentId, int devFd, void **buffer);

/*
 * Function:    TEEC_Send_Secfile
 * Description: This function sends a secfile to TEE and saves it for use
 * Parameters:  path: path of secfile
                session: current session
 * Return:      TEEC_SUCCESS: success
 *              OTHERS: fail
 */
TEEC_Result TEEC_SendSecfile(const char *path, TEEC_Session *session);

#ifdef __cplusplus
}
#endif

#endif
