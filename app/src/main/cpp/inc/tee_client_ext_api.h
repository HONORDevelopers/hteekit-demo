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
 * @par ����:
 * �ǰ�ȫ��ע��agent��listener���Ľӿ�
 *
 * @attention �˲������Ȼ�ӳ�䲢ע�Ṳ���ڴ棨Ŀǰֻ֧��4K��С�Ĺ����ڴ棩��
 * Ȼ����ע��agent
 * @param agentId [IN] �û�����һ��agent_id����TA����ͨ�ŵ�Ψһ��ʶ��
 * ��ˣ�TA������Ϣ��CAʱ������ݸ�agent_id������ͨ��
 * @param devFd [OUT] �û���ȡ����TEE�����豸��������
 * @param buffer [OUT] �û���ȡָ�����ڴ���û�̬��ַ
 * @retval #TEEC_SUCCESS agentע��ɹ�
 * @retval #TEEC_ERROR_GENERIC ������һ�����
 *
 * @par ����:
 * @li libteec���ýӿ������Ĺ����
 * @li tee_client_ext_api.h���ýӿ���������ͷ�ļ�
 * @see ��
 * @since V100R005C00
 */
TEEC_Result TEEC_EXT_RegisterAgent(uint32_t agentId, int *devFd, void **buffer);

/*
 * @ingroup  TEEC_EXT_FUNC
 * @brief wait for event from TA
 *
 * @par ����:
 * �ǰ�ȫ��agent�ȴ���ȫ��TA�¼�
 *
 * @attention �˽ӿڻ������ȴ�����˽������´������߳��������ô˽ӿ�
 * @param agentId [IN] �û�����һ��agent_id����TA����ͨ�ŵ�Ψһ��ʶ��
 * ��ˣ�TA������Ϣ��CAʱ������ݸ�agent_id������ͨ��
 * @param devFd [IN] ����TEE�����豸��������
 * @retval #TEEC_SUCCESS agent�ȴ�TA��Ϣ�¼��ɹ�
 * @retval #TEEC_ERROR_GENERIC ������һ�����
 *
 * @par ����:
 * @li libteec���ýӿ������Ĺ����
 * @li tee_client_ext_api.h���ýӿ���������ͷ�ļ�
 * @see ��
 * @since V100R005C00
 */
TEEC_Result TEEC_EXT_WaitEvent(uint32_t agentId, int devFd);

/*
 * @ingroup  TEEC_EXT_FUNC
 * @brief send response to TA
 *
 * @par ����:
 * �ǰ�ȫ��agent��Ӧ��ȫ��TA���¼�
 *
 * @attention �˽ӿڻỽ�������̼���ִ��
 * @param agentId [IN] �û�����һ��agent_id����TA����ͨ�ŵ�Ψһ��ʶ��
 * ��ˣ�TA������Ϣ��CAʱ������ݸ�agent_id������ͨ��
 * @param devFd [IN] �û�����TEE�����豸��������
 * @retval #TEEC_SUCCESS agent��Ӧ�¼����ͳɹ�
 * @retval #TEEC_ERROR_GENERIC ������һ�����
 *
 * @par ����:
 * @li libteec���ýӿ������Ĺ����
 * @li tee_client_ext_api.h���ýӿ���������ͷ�ļ�
 * @see ��
 * @since V100R005C00
 */
TEEC_Result TEEC_EXT_SendEventResponse(uint32_t agentId, int devFd);


/*
 * @ingroup  TEEC_EXT_FUNC
 * @brief unregister agent API
 *
 * @par ����:
 * �ǰ�ȫ��ȥע��agent��listener���Ľӿ�
 *
 * @attention �˽ӿڻ�֪ͨ�ں�ȥע��agent��ͬʱ�ͷŹ����ڴ棬���ָ�����ڴ��
 * �û�ָ̬�벻���ټ���ʹ����
 * @param agentId [IN] �û�����һ��agent_id����TA����ͨ�ŵ�Ψһ��ʶ��
 * ��ˣ�TA������Ϣ��CAʱ������ݸ�agent_id������ͨ��
 * @param devFd [IN] �û�����TEE�����豸��������
 * @param buffer [IN] �û�ָ�����ڴ���û�̬��ַ����������ʱ�Ὣָ����ΪNULL
 * @retval #TEEC_SUCCESS agentȥע��ɹ�
 * @retval #TEEC_ERROR_GENERIC ������һ�����
 *
 * @par ����:
 * @li libteec���ýӿ������Ĺ����
 * @li tee_client_ext_api.h���ýӿ���������ͷ�ļ�
 * @see ��
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
