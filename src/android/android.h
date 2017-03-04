#ifndef __ANDROID_H__
#define __ANDROID_H__

/* This is for C++ and does no harm in C */
#ifdef __cplusplus
extern "C" {
#endif

#include <jni.h>

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_nativeInit(JNIEnv* env, jobject o, jobject assetMgr);

void android_getLogFileDirectory(char *buffer, int length);

void android_getPrivateFilesPath(char *buffer, int length);

void android_getDeviceTypeHint(char *buffer, int length);

void android_getGamePackagePath(char *buffer, int length);

void android_getGamePackagePatchPath(char *buffer, int length);

void android_getGameFilePath(char *buffer, int length);

void android_getFontPath(char *buffer, int length);

void android_getLocalSettingsPath(char *buffer, int length);

void android_getEncodedString(char *inputString, char *encoding, char *buffer, int *length);

void android_getUTFString(char *inputString, char *encoding, char *buffer, int *length);

void android_showURLInBrowser(char *URL);

int android_advertisementPrepare(char *key, int number);

int android_advertisementShow(char *key, int number);

void android_getPlatformIdentifierString(char *buffer, int length);

void android_getAppVersionString(char *buffer, int length);

void androidiap_SetCallbackObject(void *obj);

void androidiap_SetEventsEnabledDummy(int enabled);

void androidiap_VerifyInAppProductsList(const char* prodIdList);

void androidiap_StartPurchase(const char* productId);

int androidiap_ConfirmPurchaseDummy(const char* transId);

void androidiap_RefreshPreviousPurchases();

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_IAPAddValidProductCallback(JNIEnv* env, jobject o, jstring id, jstring name, jstring desc, jstring price);

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_IAPAddInvalidProductCallback(JNIEnv* env, jobject o, jstring id);

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_IAPReceiveProductsStartCallback(JNIEnv* env, jobject o);

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_IAPReceiveProductsEndCallback(JNIEnv* env, jobject o);

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_IAPAddTransactionCallback(JNIEnv* env, jobject o, jstring transId, jstring productId, jstring state);

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_IAPReceiveTransactionsStartCallback(JNIEnv* env, jobject o);

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_IAPReceiveTransactionsEndCallback(JNIEnv* env, jobject o);

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_IAPRestoreFinishedCallback(JNIEnv* env, jobject o, jint error);

#ifdef __cplusplus
}
#endif

#endif // __ANDROID_H__
