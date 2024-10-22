/*
 * Copyright (c) Honor Device Co., Ltd. 2013-2019. All rights reserved.
 * ��������������������ͺ����ݽṹ
 * Create: 2013-4-22
 */

#ifndef _TEE_CLIENT_TYPE_H_
#define _TEE_CLIENT_TYPE_H_

#include <stdio.h>
#include <stddef.h>
#include <semaphore.h>
#include "tee_client_list.h"
#include "tee_client_constants.h"

/*
 * @ingroup TEEC_COMMON_DATA
 * �޷������Ͷ���
 */
typedef unsigned int      uint32_t;
/*
 * @ingroup TEEC_COMMON_DATA
 * �з������Ͷ���
 */
typedef signed int        int32_t;
/*
 * @ingroup TEEC_COMMON_DATA
 * �޷��Ŷ����Ͷ���
 */
typedef unsigned short    uint16_t;
/*
 * @ingroup TEEC_COMMON_DATA
 * �з��Ŷ����Ͷ���
 */
typedef signed short      int16_t;
/*
 * @ingroup TEEC_COMMON_DATA
 * �޷����ַ��Ͷ���
 */
typedef unsigned char     uint8_t;
/*
 * @ingroup TEEC_COMMON_DATA
 * �з����ַ��Ͷ���
 */
typedef signed char       int8_t;

/*
 * @ingroup TEEC_COMMON_DATA
 * �������Ͷ���
 */
#ifndef __cplusplus
#ifndef bool
#define bool    uint8_t
#endif
#endif

/*
 * @ingroup TEEC_COMMON_DATA
 * trueֵ�Ķ���
 */
#ifndef true
#define true    1
#endif

/*
 * @ingroup TEEC_COMMON_DATA
 * falseֵ�Ķ���
 */
#ifndef false
#define false   0
#endif

/*
 * @ingroup TEEC_COMMON_DATA
 * NULLֵ�Ķ���
 */
#ifndef NULL
#define NULL 0
#endif

/*
 * @ingroup TEEC_COMMON_DATA
 * ��������ֵ���Ͷ���
 *
 * ���ڱ�ʾ�������ؽ��
 */
typedef enum TEEC_ReturnCode TEEC_Result;

/*
 * @ingroup TEEC_COMMON_DATA
 * UUID���Ͷ���
 *
 * UUID������ѭRFC4122 [2]�����ڱ�ʶ��ȫ����
 */
typedef struct {
    uint32_t timeLow;           /* ʱ����ĵ�4�ֽ� */
    uint16_t timeMid;           /* ʱ�������2�ֽ� */
    uint16_t timeHiAndVersion;  /* ʱ����ĸ�2�ֽ���汾�ŵ���� */
    uint8_t clockSeqAndNode[8]; /* ʱ��������ڵ��ʶ������� */
} TEEC_UUID;

/*
 * @ingroup TEEC_COMMON_DATA
 * TEEC_Context�ṹ�����Ͷ���
 *
 * �����ͻ���Ӧ���밲ȫ����֮�佨�������ӻ���
 */
typedef struct {
    int32_t fd;  /* �ļ������� */
    uint8_t *ta_path;
    struct ListNode session_list;  /* �Ự���� */
    struct ListNode shrd_mem_list; /* �����ڴ����� */
    struct {
        void *buffer;
        sem_t buffer_barrier;
    } share_buffer;    /* share_bufferֻ�и�ͨ��ʹ�� */
} TEEC_Context;

/*
 * @ingroup TEEC_COMMON_DATA
 * TEEC_Session�ṹ�����Ͷ���
 *
 * �����ͻ���Ӧ���밲ȫ����֮�佨���ĻỰ
 */
typedef struct {
    uint32_t session_id;    /* �ỰID���ɰ�ȫ���緵�� */
    TEEC_UUID service_id;   /* ��ȫ�����UUID��ÿ����ȫ����ӵ��Ψһ��UUID */
    uint32_t ops_cnt;       /* �ڻỰ�ڵĲ����� */
    struct ListNode head;   /* �Ự����ͷ */
    TEEC_Context *context;  /* ָ��Ự������TEE���� */
} TEEC_Session;

/*
 * @ingroup TEEC_COMMON_DATA
 * TEEC_SharedMemory�ṹ�����Ͷ���
 *
 * ����һ�鹲���ڴ棬����ע�ᣬҲ���Է���
 */
typedef struct {
    void *buffer;           /* �ڴ�ָ�� */
    uint32_t size;          /* �ڴ��С */
    uint32_t flags;         /* �ڴ��ʶ����������������������ȡֵ��ΧΪ#TEEC_SharedMemCtl */
    uint32_t ops_cnt;       /* �ڴ������ */
    bool is_allocated;      /* �ڴ�����ʾ��������������ע��ģ����Ƿ���� */
    struct ListNode head;   /* �����ڴ�����ͷ */
    TEEC_Context *context;  /* ָ��������TEE���� */
} TEEC_SharedMemory;

/*
 * @ingroup TEEC_COMMON_DATA
 * TEEC_TempMemoryReference�ṹ�����Ͷ���
 *
 * ����һ����ʱ������ָ��\n
 * ��������#TEEC_Parameter�����ͣ��������Ӧ�����Ϳ�����
 * #TEEC_MEMREF_TEMP_INPUT�� #TEEC_MEMREF_TEMP_OUTPUT����#TEEC_MEMREF_TEMP_INOUT
 */
typedef struct {
    void *buffer;   /* ��ʱ������ָ�� */
    uint32_t size;  /* ��ʱ��������С */
} TEEC_TempMemoryReference;

/*
 * @ingroup TEEC_COMMON_DATA
 * TEEC_RegisteredMemoryReference�ṹ�����Ͷ���
 *
 * ���������ڴ�ָ�룬ָ������ע������õĹ����ڴ�\n
 * ��������#TEEC_Parameter�����ͣ��������Ӧ�����Ϳ�����
 * #TEEC_MEMREF_WHOLE�� #TEEC_MEMREF_PARTIAL_INPUT��
 * #TEEC_MEMREF_PARTIAL_OUTPUT���� #TEEC_MEMREF_PARTIAL_INOUT
 */
typedef struct {
    TEEC_SharedMemory *parent;  /* �����ڴ�ָ�� */
    uint32_t size;              /* �����ڴ��ʹ�ô�С */
    uint32_t offset;            /* �����ڴ��ʹ��ƫ�� */
} TEEC_RegisteredMemoryReference;

/*
 * @ingroup TEEC_COMMON_DATA
 * TEEC_Value�ṹ�����Ͷ���
 *
 * ��������������\n
 * ��������#TEEC_Parameter�����ͣ��������Ӧ�����Ϳ�����
 * #TEEC_VALUE_INPUT�� #TEEC_VALUE_OUTPUT����#TEEC_VALUE_INOUT
 */
typedef struct {
    uint32_t a;  /* ��������a */
    uint32_t b;  /* ��������b */
} TEEC_Value;

typedef struct {
    int ion_share_fd;  /* ��������a */
    uint32_t ion_size; /* ��������b */
} TEEC_IonReference;

/*
 * @ingroup TEEC_COMMON_DATA
 * TEEC_Parameter���������Ͷ���
 *
 * ����#TEEC_Operation����Ӧ�Ĳ�������
 */
typedef union {
    TEEC_TempMemoryReference tmpref;        /* ����#TEEC_TempMemoryReference���� */
    TEEC_RegisteredMemoryReference memref;  /* ����#TEEC_RegisteredMemoryReference���� */
    TEEC_Value value;                       /* ����#TEEC_Value���� */
    TEEC_IonReference ionref;
} TEEC_Parameter;

typedef struct {
    uint32_t event_type;    /* Tui event type */
    uint32_t value;         /* return value, is keycode if tui event is getKeycode */
    uint32_t notch;         /* notch size of the screen for tui */
    uint32_t width;         /* width of foldable screen */
    uint32_t height;        /* height of foldable screen */
    uint32_t fold_state;    /* state of foldable screen */
    uint32_t display_state; /* one state of folded state */
    uint32_t phy_width;     /* real width of the mobile */
    uint32_t phy_height;    /* real height of the mobile */
} TEEC_TUI_Parameter;
#define TEEC_TUI_EVENT_TYPE_START_TVM 0x100
/*
 * @ingroup TEEC_COMMON_DATA
 * TEEC_Operation�ṹ�����Ͷ���
 *
 * �򿪻Ự��������ʱ�Ĳ�������ͨ��������������
 * ȡ������Ҳ����ʹ�ô˲���
 */
typedef struct {
    uint32_t started;     /* ��ʶ�Ƿ���ȡ��������0��ʾȡ������ */
    uint32_t paramTypes;  /* ����params�Ĳ�������#TEEC_ParamType����Ҫʹ�ú�#TEEC_PARAM_TYPES��ϲ������� */
    TEEC_Parameter params[TEEC_PARAM_NUM];  /* �������ݣ�����Ϊ#TEEC_Parameter */
    TEEC_Session *session;
    bool cancel_flag;
} TEEC_Operation;

#endif
