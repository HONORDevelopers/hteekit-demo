/*
* Copyright (c) Honor Technologies Co., Ltd. 2019-2020. All rights reserved.
* Description: The name of the honor libteec so of android S has changed,
* apk should dlopen libteec according to the interfaces in this file.
* * Create: 2019.4.30
*/

#include <dlfcn.h>
#include <pthread.h>

#include "inc/tee_client_api.h"

/*HTEE lib name for APK */
#define HONOR_SYSTEM_LIBTEEC_PATH    "libteec.honor.so"

/* these Function pointers are consistent with those defined in "tee_client_api.h"
 * if you want to call the new interface in "tee_client_api.h",
 * you should add a new definition here
 */
typedef TEEC_Result (*initializeContext_f)(const char *name, TEEC_Context *context);
typedef void (*finalizeContext_f)(TEEC_Context *context);
typedef TEEC_Result (*openSession_f)(TEEC_Context *context, TEEC_Session *session,
    const TEEC_UUID *destination, uint32_t connectionMethod, const void *connectionData,
    TEEC_Operation *operation, uint32_t *returnOrigin);
typedef void (*closeSession_f)(TEEC_Session *session);
typedef TEEC_Result (*invokeCommand_f)(TEEC_Session *session, uint32_t commandID,
    TEEC_Operation *operation, uint32_t *returnOrigin);
typedef TEEC_Result (*registerSharedMemory_f)(TEEC_Context *context,
    TEEC_SharedMemory *sharedMem);
typedef TEEC_Result (*allocateSharedMemory_f)(TEEC_Context *context,
    TEEC_SharedMemory *sharedMem);
typedef void (*releaseSharedMemory_f)(TEEC_SharedMemory *sharedMem);
typedef void (*requestCancellation_f)(TEEC_Operation *operation);
typedef uint32_t (*getTEEVersion_f)();
typedef TEEC_Result (*sendSecfile_f)(const char *path, TEEC_Session *session);
typedef TEEC_Result (*tui_send_event_f)(TEEC_TUI_Parameter *tuiParam);

/* As above, if you want to call the new interface in "tee_client_api.h",
 * you should add a new member to TeecApiTable
 */
typedef struct {
    initializeContext_f     initializeContextFn;
    finalizeContext_f       finalizeContextFn;
    openSession_f           openSessionFn;
    closeSession_f          closeSessionFn;
    invokeCommand_f         invokeCommandFn;
    registerSharedMemory_f  registerSharedMemoryFn;
    allocateSharedMemory_f  allocateSharedMemoryFn;
    releaseSharedMemory_f   releaseSharedMemoryFn;
    requestCancellation_f   requestCancellationFn;
    getTEEVersion_f         getTEEVersionFn;
    sendSecfile_f           sendSecfileFn;
    tui_send_event_f        tui_send_eventFn;
} TeecApiTable;

static TeecApiTable g_teecApiTable;
static void *g_libTeecHandle = NULL;
static pthread_mutex_t g_libTeecHandleLock = PTHREAD_MUTEX_INITIALIZER;

static TEEC_Result GetBasicApiSymbol(TeecApiTable *teecApiTable)
{
    teecApiTable->initializeContextFn =
        (initializeContext_f)(dlsym(g_libTeecHandle, "TEEC_InitializeContext"));
    if (teecApiTable->initializeContextFn == NULL) {
        TEEC_Error("get symbol TEEC_InitializeContext failed\n");
        return TEEC_ERROR_GENERIC;
    }

    teecApiTable->finalizeContextFn =
        (finalizeContext_f)(dlsym(g_libTeecHandle, "TEEC_FinalizeContext"));
    if (teecApiTable->finalizeContextFn == NULL) {
        TEEC_Error("get symbol TEEC_FinalizeContext failed\n");
        return TEEC_ERROR_GENERIC;
    }

    teecApiTable->openSessionFn =
        (openSession_f)(dlsym(g_libTeecHandle, "TEEC_OpenSession"));
    if (teecApiTable->openSessionFn == NULL) {
        TEEC_Error("get symbol TEEC_OpenSession failed\n");
        return TEEC_ERROR_GENERIC;
    }

    teecApiTable->closeSessionFn =
        (closeSession_f)(dlsym(g_libTeecHandle, "TEEC_CloseSession"));
    if (teecApiTable->closeSessionFn == NULL) {
        TEEC_Error("get symbol TEEC_CloseSession failed\n");
        return TEEC_ERROR_GENERIC;
    }

    teecApiTable->invokeCommandFn =
        (invokeCommand_f)(dlsym(g_libTeecHandle, "TEEC_InvokeCommand"));
    if (teecApiTable->invokeCommandFn == NULL) {
        TEEC_Error("get symbol TEEC_InvokeCommand failed\n");
        return TEEC_ERROR_GENERIC;
    }

    return TEEC_SUCCESS;
}

static TEEC_Result GetMemApiSymbol(TeecApiTable *teecApiTable)
{
    teecApiTable->registerSharedMemoryFn =
        (registerSharedMemory_f)(dlsym(g_libTeecHandle, "TEEC_RegisterSharedMemory"));
    if (teecApiTable->registerSharedMemoryFn == NULL) {
        TEEC_Error("get symbol TEEC_RegisterSharedMemory failed\n");
        return TEEC_ERROR_GENERIC;
    }

    teecApiTable->allocateSharedMemoryFn =
        (allocateSharedMemory_f)(dlsym(g_libTeecHandle, "TEEC_AllocateSharedMemory"));
    if (teecApiTable->allocateSharedMemoryFn == NULL) {
        TEEC_Error("get symbol TEEC_AllocateSharedMemory failed\n");
        return TEEC_ERROR_GENERIC;
    }

    teecApiTable->releaseSharedMemoryFn =
        (releaseSharedMemory_f)(dlsym(g_libTeecHandle, "TEEC_ReleaseSharedMemory"));
    if (teecApiTable->releaseSharedMemoryFn == NULL) {
        TEEC_Error("get symbol TEEC_ReleaseSharedMemory failed\n");
        return TEEC_ERROR_GENERIC;
    }

    teecApiTable->requestCancellationFn =
        (requestCancellation_f)(dlsym(g_libTeecHandle, "TEEC_RequestCancellation"));
    if (teecApiTable->requestCancellationFn == NULL) {
        TEEC_Error("get symbol TEEC_RequestCancellation failed\n");
        return TEEC_ERROR_GENERIC;
    }

    return TEEC_SUCCESS;
}

/* APK should dlopen so lib in the order shown by this function,
 * make sure the APK is compatible with all versions.
 */
static TEEC_Result GetTeecApiSymbol()
{
    TeecApiTable teecApiTable = {0};
    (void)pthread_mutex_lock(&g_libTeecHandleLock);

    if (g_libTeecHandle != NULL) {
        (void)pthread_mutex_unlock(&g_libTeecHandleLock);
        return TEEC_SUCCESS;
    }

    /* for Andorid system APK */
    g_libTeecHandle = dlopen(HONOR_SYSTEM_LIBTEEC_PATH, RTLD_LAZY);
    if (g_libTeecHandle == NULL) {
        TEEC_Error("open libteec.honor.so failed\n");
        return TEEC_ERROR_GENERIC;
    }


    if (GetBasicApiSymbol(&teecApiTable) != TEEC_SUCCESS || GetMemApiSymbol(&teecApiTable) != TEEC_SUCCESS) {
        TEEC_Error("get symbol failed\n");
        goto ERROR;
    }

    /* the following interfaces are not defined by GP,
     * they are not included in old libteec so version.
     * we may get null ptr from the old so version, it's ok, do not return error.
     */
    teecApiTable.getTEEVersionFn =
        (getTEEVersion_f)(dlsym(g_libTeecHandle, "TEEC_GetTEEVersion"));
    if (teecApiTable.getTEEVersionFn == NULL) {
        TEEC_Error("get symbol TEEC_GetTEEVersion failed\n");
        return TEEC_ERROR_GENERIC;
    }
    teecApiTable.sendSecfileFn =
        (sendSecfile_f)(dlsym(g_libTeecHandle, "TEEC_SendSecfile"));
    if (teecApiTable.sendSecfileFn == NULL) {
        TEEC_Error("get symbol TEEC_SendSecfile failed\n");
        return TEEC_ERROR_GENERIC;
    } 
    teecApiTable.tui_send_eventFn =
            (tui_send_event_f)(dlsym(g_libTeecHandle, "TEEC_EXT_Tui_send_event"));
    if (teecApiTable.tui_send_eventFn == NULL) {
        TEEC_Error("get symbol TEEC_EXT_Tui_send_event failed\n");
        return TEEC_ERROR_GENERIC;
    }
    g_teecApiTable = teecApiTable;
    (void)pthread_mutex_unlock(&g_libTeecHandleLock);
    return TEEC_SUCCESS;

ERROR:
    dlclose(g_libTeecHandle);
    g_libTeecHandle = NULL;
    (void)pthread_mutex_unlock(&g_libTeecHandleLock);
    return TEEC_ERROR_GENERIC;
}

/* The following are Interfaces for APK,
 * in these interfaces, you should make sure the function pointer in g_teecApiTable is not NULL
 */
TEEC_Result TEEC_InitializeContext(const char *name, TEEC_Context *context)
{
    TEEC_Result ret = GetTeecApiSymbol();
    if (ret != TEEC_SUCCESS) {
        TEEC_Error("get teec api symbol failed!\n");
        return TEEC_ERROR_GENERIC;
    }

    return g_teecApiTable.initializeContextFn(name, context);
}

void TEEC_FinalizeContext(TEEC_Context *context)
{
    if (g_teecApiTable.finalizeContextFn == NULL) {
        TEEC_Error("finalizeContextFn is null!\n");
        return;
    }

    g_teecApiTable.finalizeContextFn(context);
}

TEEC_Result TEEC_OpenSession(TEEC_Context *context, TEEC_Session *session,
    const TEEC_UUID *destination, uint32_t connectionMethod,
    const void *connectionData, TEEC_Operation *operation,
    uint32_t *returnOrigin)
{
    if (g_teecApiTable.openSessionFn == NULL) {
        TEEC_Error("openSessionFn is null!\n");
        return TEEC_ERROR_GENERIC;
    }

    return g_teecApiTable.openSessionFn(context, session, destination,
        connectionMethod, connectionData, operation, returnOrigin);
}

void TEEC_CloseSession(TEEC_Session *session)
{
    if (g_teecApiTable.closeSessionFn == NULL) {
        TEEC_Error("closeSessionFn is null!\n");
        return;
    }

    g_teecApiTable.closeSessionFn(session);
}

TEEC_Result TEEC_InvokeCommand(TEEC_Session *session, uint32_t commandID,
    TEEC_Operation *operation, uint32_t *returnOrigin)
{
    if (g_teecApiTable.invokeCommandFn == NULL) {
        TEEC_Error("invokeCommandFn is null!\n");
        return TEEC_ERROR_GENERIC;
    }

    return g_teecApiTable.invokeCommandFn(session, commandID, operation, returnOrigin);
}

TEEC_Result TEEC_RegisterSharedMemory(TEEC_Context *context,
    TEEC_SharedMemory *sharedMem)
{
    if (g_teecApiTable.registerSharedMemoryFn == NULL) {
        TEEC_Error("registerSharedMemoryFn is null!\n");
        return TEEC_ERROR_GENERIC;
    }

    return g_teecApiTable.registerSharedMemoryFn(context, sharedMem);
}

TEEC_Result TEEC_AllocateSharedMemory(TEEC_Context *context,
    TEEC_SharedMemory *sharedMem)
{
    if (g_teecApiTable.allocateSharedMemoryFn == NULL) {
        TEEC_Error("allocateSharedMemoryFn is null!\n");
        return TEEC_ERROR_GENERIC;
    }

    return g_teecApiTable.allocateSharedMemoryFn(context, sharedMem);
}

void TEEC_ReleaseSharedMemory(TEEC_SharedMemory *sharedMem)
{
    if (g_teecApiTable.releaseSharedMemoryFn == NULL) {
        TEEC_Error("releaseSharedMemoryFn is null!\n");
        return;
    }

    g_teecApiTable.releaseSharedMemoryFn(sharedMem);
}

void TEEC_RequestCancellation(TEEC_Operation *operation)
{
    if (g_teecApiTable.requestCancellationFn == NULL) {
        TEEC_Error("requestCancellationFn is null!\n");
        return;
    }

    g_teecApiTable.requestCancellationFn(operation);
}

uint32_t TEEC_GetTEEVersion()
{
    TEEC_Result ret = GetTeecApiSymbol();
    if (ret != TEEC_SUCCESS) {
        TEEC_Error("get teec api symbol failed!\n");
        return TEEC_FAIL;
    }

    if (g_teecApiTable.getTEEVersionFn == NULL) {
        TEEC_Error("getTEEVersionFn is null!\n");
        return TEEC_FAIL;
    }

    return g_teecApiTable.getTEEVersionFn();
}

TEEC_Result TEEC_SendSecfile(const char *path, TEEC_Session *session)
{
    if (g_teecApiTable.sendSecfileFn == NULL) {
        TEEC_Error("sendSecfileFn is null!\n");
        return TEEC_ERROR_GENERIC;
    }

    return g_teecApiTable.sendSecfileFn(path, session);
}
TEEC_Result TEEC_EXT_Tui_send_event(TEEC_TUI_Parameter *tuiParam)
{
    TEEC_Result ret = GetTeecApiSymbol();
    if (g_teecApiTable.tui_send_eventFn == NULL) {
        TEEC_Error("tui_send_eventFn is null!\n");
        return TEEC_ERROR_GENERIC;
    }

    return g_teecApiTable.tui_send_eventFn(tuiParam);
}