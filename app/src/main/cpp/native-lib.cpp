#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <jni.h>
#include <string>
#include "inc/tee_client_type.h"
#include "inc/tee_client_api.h"

#define CMD_GET_TA_VERSION     3
#define OUT_BUFFER_LEN   460800
#define OUT_BUFFER_INDEX 1
#define START_FLAG       1

static const TEEC_UUID Hello_World_uuid = {
        0x25aaa1b6, 0x190a, 0x3396,
        { 0x8a, 0x5c, 0x6d, 0x8b, 0xa3, 0x64, 0x17, 0xa8 }
};

TEEC_Context g_test_context = {0};
TEEC_Session g_test_session = {0};
static int g_test_inited = 0;

//调用TUI能力，需要触发TVM 启动
void threadFunction() {
    TEEC_TUI_Parameter *tuiCmd = NULL;
    tuiCmd = (TEEC_TUI_Parameter *)malloc(sizeof(TEEC_TUI_Parameter));
    if (tuiCmd == NULL) {
        TEEC_Error("tuiCmd malloc failed\n");
    }

    tuiCmd->event_type = TEEC_TUI_EVENT_TYPE_START_TVM;
    TEEC_ReturnCode result = TEEC_EXT_Tui_send_event(tuiCmd);
    if(result != TEEC_SUCCESS) {
        TEEC_Error("TEEC_EXT_Tui_send_event failed");
    }
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_hihonor_hteedemo_MainActivity_stringFromTA(JNIEnv *env, jobject instance,
                                                         jstring path) {
    (void)instance;
    const char* ptr = NULL;
    char test_buf[256] = {0};
    unsigned int buf_len = sizeof(test_buf);
    if (path != NULL)
        ptr = (env)->GetStringUTFChars(path, false);
    else
        return env->NewStringUTF("invalid parameter of TA path");

    threadFunction();

    TEEC_Context context = {0};
    TEEC_Session session = {0};
    TEEC_Result result;
    TEEC_Operation operation = {0};
    uint32_t origin = 0;

    result = TEEC_InitializeContext(NULL, &context);
    if(result != TEEC_SUCCESS) {
        TEEC_Error("teec initial failed\n");
        return env->NewStringUTF("error, initial failed to get result from TEE");
    }

    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(
            TEEC_NONE,
            TEEC_NONE,
            TEEC_MEMREF_TEMP_INPUT,
            TEEC_MEMREF_TEMP_INPUT);
    context.ta_path = (uint8_t *)ptr;

    result = TEEC_OpenSession(&context, &session, &Hello_World_uuid,
                              TEEC_LOGIN_IDENTIFY, NULL, &operation, NULL);
    if(result != TEEC_SUCCESS) {
        TEEC_Error("teec open session failed, ret is 0x%x\n", result);
        TEEC_FinalizeContext(&context);
        return env->NewStringUTF("error, opensession failed to get result from TEE");
    }

    operation.started = START_FLAG;
    operation.paramTypes = TEEC_PARAM_TYPES(
            TEEC_NONE,
            TEEC_NONE,
            TEEC_NONE,
            TEEC_MEMREF_TEMP_OUTPUT);
    operation.params[3].tmpref.buffer = test_buf;
    operation.params[3].tmpref.size = buf_len;

    result = TEEC_InvokeCommand(&session, CMD_GET_TA_VERSION, &operation, &origin);
    TEEC_CloseSession(&session);
    TEEC_FinalizeContext(&context);
    if (result != TEEC_SUCCESS) {
        TEEC_Error("invoke failed, codes=0x%x, origin=0x%x\n", result, origin);
        return env->NewStringUTF("error, get this device id failed");
    } else {
        return env->NewStringUTF(test_buf);
    }
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_hihonor_hteedemo_MainActivity_openTestTA(JNIEnv *env, jobject thiz, jstring path) {
    const char* ptr = NULL;
    char test_buf[OUT_BUFFER_LEN] = {0};
    if (path != NULL)
        ptr = (env)->GetStringUTFChars(path, false);
    else
        return env->NewStringUTF("invalid parameter of TA path");

    TEEC_Result result;
    TEEC_Operation operation = {0};

    TEEC_Error(" TEEC_InitializeContext enter\n");
    result = TEEC_InitializeContext(NULL, &g_test_context);
    if(result != TEEC_SUCCESS) {
        TEEC_Error("teec initial failed\n");
        return env->NewStringUTF("failed to get result from TEE");
    }
    TEEC_Error(" TEEC_InitializeContext ok\n");
    operation.started = 1;
    operation.paramTypes = TEEC_PARAM_TYPES(
            TEEC_NONE,
            TEEC_NONE,
            TEEC_MEMREF_TEMP_INPUT,
            TEEC_MEMREF_TEMP_INPUT);
    g_test_context.ta_path = (uint8_t *)ptr;

    TEEC_Error(" TEEC_OpenSession enter\n");
    result = TEEC_OpenSession(&g_test_context, &g_test_session, &Hello_World_uuid,
                              TEEC_LOGIN_IDENTIFY, NULL, &operation, NULL);
    if(result != TEEC_SUCCESS) {
        TEEC_Error("teec open session failed, ret is 0x%x\n", result);
        TEEC_FinalizeContext(&g_test_context);
        return env->NewStringUTF("failed to get result from TEE");
    }
    TEEC_Error(" TEEC_OpenSession ok\n");

    g_test_inited = 1;
    return env->NewStringUTF(test_buf);
}

extern "C"
JNIEXPORT jbyteArray JNICALL
Java_com_hihonor_hteedemo_MainActivity_sendCmdToTestTA(JNIEnv *env, jobject thiz, jint cmd,
                                                       jint data) {
    char test_buf[OUT_BUFFER_LEN] = {0};
    unsigned int buf_len = sizeof(test_buf);
    jbyteArray image_buffer = NULL;
    unsigned int image_buffer_len = 0;


    TEEC_Result result;
    TEEC_Operation operation = {0};
    uint32_t origin = 0;

    operation.started = START_FLAG;
    operation.paramTypes = TEEC_PARAM_TYPES(
            TEEC_VALUE_INPUT,
            TEEC_MEMREF_TEMP_OUTPUT,
            TEEC_NONE,
            TEEC_NONE);
    TEEC_Error(" params[0].value.a = %d\n", data);
    operation.params[0].value.a = data;     // a for input data
    operation.params[OUT_BUFFER_INDEX].tmpref.buffer = test_buf;
    operation.params[OUT_BUFFER_INDEX].tmpref.size = buf_len;

    TEEC_Error(" TEEC_InvokeCommand：%d enter\n", cmd);
    result = TEEC_InvokeCommand(&g_test_session, cmd, &operation, &origin);
    if (result != TEEC_SUCCESS) {
        TEEC_Error("invoke failed, codes=0x%x, origin=0x%x\n", result, origin);
        return image_buffer;
    }
    image_buffer_len = operation.params[OUT_BUFFER_INDEX].tmpref.size;
    TEEC_Error(" image_buffer_len：%d\n", image_buffer_len);
    image_buffer = env->NewByteArray(image_buffer_len);
    env->SetByteArrayRegion(image_buffer, 0, image_buffer_len, (jbyte *)test_buf);
    TEEC_Error(" TEEC_InvokeCommand ok\n");
    return image_buffer;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_hihonor_hteedemo_MainActivity_closeTestTA(JNIEnv *env, jobject thiz) {

    if (g_test_inited) {
        TEEC_Error(" TEEC_CloseSession enter\n");
        TEEC_CloseSession(&g_test_session);
        TEEC_Error(" TEEC_CloseSession ok\n");
        TEEC_Error(" TEEC_FinalizeContext enter\n");
        TEEC_FinalizeContext(&g_test_context);
        TEEC_Error(" TEEC_FinalizeContext ok\n");
        g_test_inited = 0;
    } else {
        TEEC_Error(" multi close\n");
    }
    return env->NewStringUTF(" closeTestTA ok\n");
}