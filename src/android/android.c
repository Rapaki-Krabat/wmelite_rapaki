
#include "android.h"
#include "StoreKit_functions.h"
#include "StoreKit_callbacks.h"

#include "SDL_android.h"

#include "DisplayConfig.h"
#include "LicenseCheck.h"

#include <android/log.h>
#include <android/asset_manager_jni.h>

const char* className_wmeliteFunctions = "org/deadcode/wmelite";

static jobject callbackObject;

AAssetManager *assetManager;

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_nativeInit(JNIEnv* env, jobject o, jobject assetMgr)
{

	callbackObject = (*env)->NewGlobalRef(env, o);

	assetManager = AAssetManager_fromJava(env, assetMgr);

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "Global ref to WMELITE=%s", (callbackObject == NULL) ? "NULL" : "OK");
	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "Asset manager ref=%s", (assetManager == NULL) ? "NULL" : "OK");

	// localEnv = env;
}

void android_getLogFileDirectory(char *buffer, int length)
{
	const char *tmp;

	// get the proper jni env from SDL
	// JNIEnv *env = Android_JNI_GetEnv();
	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getLogFileDirectory", "()Ljava/lang/String;");
	jstring str = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	tmp = (*env)->GetStringUTFChars(env, str, NULL);

	if (strnlen(tmp, length) < length) {
		strncpy(buffer, tmp, length);
	} else {
		buffer[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getLogFileDirectory() returns %s", buffer);

	(*env)->ReleaseStringUTFChars(env, str, tmp);
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, str);
}

void android_getPrivateFilesPath(char *buffer, int length)
{
	const char *tmp;

	// get the proper jni env from SDL
	// JNIEnv *env = Android_JNI_GetEnv();
	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getPrivateFilesPath", "()Ljava/lang/String;");
	jstring str = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	tmp = (*env)->GetStringUTFChars(env, str, NULL);

	if (strnlen(tmp, length) < length) {
		strncpy(buffer, tmp, length);
	} else {
		buffer[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getPrivateFilesPath() returns %s", buffer);

	(*env)->ReleaseStringUTFChars(env, str, tmp);
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, str);
}

void android_getDeviceTypeHint(char *buffer, int length)
{
	const char *tmp;

	// get the proper jni env from SDL
	// JNIEnv *env = Android_JNI_GetEnv();
	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getDeviceTypeHint", "()Ljava/lang/String;");
	jstring str = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	tmp = (*env)->GetStringUTFChars(env, str, NULL);

	if (strnlen(tmp, length) < length) {
		strncpy(buffer, tmp, length);
	} else {
		buffer[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getDeviceTypeHint() returns %s", buffer);

	(*env)->ReleaseStringUTFChars(env, str, tmp);
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, str);
}

void android_getGamePackagePath(char *buffer, int length)
{
	const char *tmp;

	// get the proper jni env from SDL
	// JNIEnv *env = Android_JNI_GetEnv();
	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);

	// __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getGamePackagePath() env=%s", (env == NULL) ? "NULL" : "OK");

	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getGamePackagePath", "()Ljava/lang/String;");

	// __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getGamePackagePath() callbackid=%d", callbackID);

	jstring str = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	// __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getGamePackagePath() jstring=%s", (str == NULL) ? "NULL" : "OK");

	tmp = (*env)->GetStringUTFChars(env, str, NULL);

	if (strnlen(tmp, length) < length) {
		strncpy(buffer, tmp, length);
	} else {
		buffer[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getGamePackagePath() returns %s", buffer);

	(*env)->ReleaseStringUTFChars(env, str, tmp);
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, str);
}

void android_getGamePackagePatchPath(char *buffer, int length)
{
	const char *tmp;

	// get the proper jni env from SDL
	// JNIEnv *env = Android_JNI_GetEnv();
	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);

	// __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getGamePackagePath() env=%s", (env == NULL) ? "NULL" : "OK");

	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getGamePackagePatchPath", "()Ljava/lang/String;");

	// __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getGamePackagePath() callbackid=%d", callbackID);

	jstring str = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	// __android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getGamePackagePath() jstring=%s", (str == NULL) ? "NULL" : "OK");

	tmp = (*env)->GetStringUTFChars(env, str, NULL);

	if (strnlen(tmp, length) < length) {
		strncpy(buffer, tmp, length);
	} else {
		buffer[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getGamePackagePatchPath() returns %s", buffer);

	(*env)->ReleaseStringUTFChars(env, str, tmp);
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, str);
}

void android_getGameFilePath(char *buffer, int length)
{
	const char *tmp;

	// get the proper jni env from SDL
	// JNIEnv *env = Android_JNI_GetEnv();
	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getGameFilePath", "()Ljava/lang/String;");
	jstring str = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	tmp = (*env)->GetStringUTFChars(env, str, NULL);

	if (strnlen(tmp, length) < length) {
		strncpy(buffer, tmp, length);
	} else {
		buffer[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getGameFilePath() returns %s", buffer);

	(*env)->ReleaseStringUTFChars(env, str, tmp);
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, str);
}

void android_getFontPath(char *buffer, int length)
{
	const char *tmp;

	// get the proper jni env from SDL
	// JNIEnv *env = Android_JNI_GetEnv();
	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getFontPath", "()Ljava/lang/String;");
	jstring str = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	tmp = (*env)->GetStringUTFChars(env, str, NULL);

	if (strnlen(tmp, length) < length) {
		strncpy(buffer, tmp, length);
	} else {
		buffer[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getFontPath() returns %s", buffer);

	(*env)->ReleaseStringUTFChars(env, str, tmp);
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, str);
}

void android_getLocalSettingsPath(char *buffer, int length)
{
	const char *tmp;

	// get the proper jni env from SDL
	// JNIEnv *env = Android_JNI_GetEnv();
	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getLocalSettingsPath", "()Ljava/lang/String;");
	jstring str = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	tmp = (*env)->GetStringUTFChars(env, str, NULL);

	if (strnlen(tmp, length) < length) {
		strncpy(buffer, tmp, length);
	} else {
		buffer[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getLocalSettingsPath() returns %s", buffer);

	(*env)->ReleaseStringUTFChars(env, str, tmp);
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, str);
}

void android_getEncodedString(char *inputString, char *encoding, char *buffer, int *length)
{
	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getEncodedString", "(Ljava/lang/String;Ljava/lang/String;)[B");
	jstring input = (*env)->NewStringUTF(env, inputString);
	jstring enc = NULL;
	if (encoding != NULL) {
		enc = (*env)->NewStringUTF(env, encoding);
	}

	jbyteArray result = (jbyteArray) (*env)->CallObjectMethod(env, callbackObject, callbackID, input, enc);

	// for debugging
	// (*env)->ExceptionDescribe(env);
	// (*env)->ExceptionClear(env);

	if (result != NULL) {
		jsize retLen = (*env)->GetArrayLength(env, result);
		jbyte *retBuffer = (*env)->GetByteArrayElements(env, result, NULL);

		if (retBuffer != NULL) {
			if (retLen < (*length)) {
				memcpy(buffer, retBuffer, retLen);
				buffer[retLen] = 0;
				*length = retLen;
			} else {
				__android_log_print(ANDROID_LOG_ERROR, "org.libsdl.app", "android_getEncodedString() length %d out of range!", retLen);
			}
			(*env)->ReleaseByteArrayElements(env, result, retBuffer, 0);
		} else {
			__android_log_print(ANDROID_LOG_ERROR, "org.libsdl.app", "android_getEncodedString() buffer not accessible!");
		}
	} else {
		__android_log_print(ANDROID_LOG_ERROR, "org.libsdl.app", "android_getEncodedString() returns NULL!");
	}

	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, input);
	if (encoding != NULL) {
		(*env)->DeleteLocalRef(env, enc);
	}
	if (result != NULL) {
		(*env)->DeleteLocalRef(env, result);
	}
}

void android_getUTFString(char *inputString, char *encoding, char *buffer, int *length)
{
	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getUTFString", "([BLjava/lang/String;)Ljava/lang/String;");

	size_t inLen = strnlen(inputString, 32768);
	jbyteArray input = (*env)->NewByteArray(env, inLen);
	jbyte *inputBuffer = (*env)->GetByteArrayElements(env, input, NULL);
	memcpy(inputBuffer, inputString, inLen);
	(*env)->ReleaseByteArrayElements(env, input, inputBuffer, 0);

	jstring enc = NULL;
	if (encoding != NULL) {
		enc = (*env)->NewStringUTF(env, encoding);
	}

	jstring result = (jstring) (*env)->CallObjectMethod(env, callbackObject, callbackID, input, enc);

	// for debugging
	// (*env)->ExceptionDescribe(env);
	// (*env)->ExceptionClear(env);

	if (result != NULL) {
		jsize retLen = (*env)->GetStringUTFLength(env, result);
		jbyte *retBuffer = (*env)->GetStringUTFChars(env, result, NULL);

		if (retBuffer != NULL) {
			if (retLen < (*length)) {
				memcpy(buffer, retBuffer, retLen);
				buffer[retLen] = 0;
				*length = retLen;
			} else {
				__android_log_print(ANDROID_LOG_ERROR, "org.libsdl.app", "android_getUTFString() length %d out of range!", retLen);
			}
			(*env)->ReleaseStringUTFChars(env, result, retBuffer);
		} else {
			__android_log_print(ANDROID_LOG_ERROR, "org.libsdl.app", "android_getUTFString() buffer not accessible!");
		}
	} else {
		__android_log_print(ANDROID_LOG_ERROR, "org.libsdl.app", "android_getUTFString() returns NULL!");
	}

	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, input);
	if (encoding != NULL) {
		(*env)->DeleteLocalRef(env, enc);
	}
	if (result != NULL) {
		(*env)->DeleteLocalRef(env, result);
	}
}

void android_showURLInBrowser(char *URL)
{
  JNIEnv *env = Android_JNI_GetEnv();
  jclass cls = (*env)->GetObjectClass(env, callbackObject);
  jmethodID callbackID = (*env)->GetMethodID(env, cls, "showURLInBrowser", "(Ljava/lang/String;)V");

  jstring urlString = (*env)->NewStringUTF(env,URL);

  (*env)->CallVoidMethod(env, callbackObject, callbackID, urlString);

  (*env)->DeleteLocalRef(env, urlString);
  (*env)->DeleteLocalRef(env, cls);
}

int android_advertisementPrepare(char *key, int number)
{
	  JNIEnv *env = Android_JNI_GetEnv();
	  jclass cls = (*env)->GetObjectClass(env, callbackObject);
	  jmethodID callbackID = (*env)->GetMethodID(env, cls, "advertisementPrepare", "(Ljava/lang/String;I)I");

	  jstring keyString = (*env)->NewStringUTF(env, key);
	  jint numberVal = (jint) number;
	  jint ret;

	  ret = (*env)->CallIntMethod(env, callbackObject, callbackID, keyString, numberVal);

	  (*env)->DeleteLocalRef(env, keyString);

	  return (int) ret;
}

int android_advertisementShow(char *key, int number)
{
	  JNIEnv *env = Android_JNI_GetEnv();
	  jclass cls = (*env)->GetObjectClass(env, callbackObject);
	  jmethodID callbackID = (*env)->GetMethodID(env, cls, "advertisementShow", "(Ljava/lang/String;I)I");

	  jstring keyString = (*env)->NewStringUTF(env, key);
	  jint numberVal = (jint) number;
	  jint ret;

	  ret = (*env)->CallIntMethod(env, callbackObject, callbackID, keyString, numberVal);

	  (*env)->DeleteLocalRef(env, keyString);

	  return (int) ret;
}

void android_getPlatformIdentifierString(char *buffer, int length)
{
	const char *tmp;

	// get the proper jni env from SDL
	// JNIEnv *env = Android_JNI_GetEnv();
	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getPlatformIdentifierString", "()Ljava/lang/String;");
	jstring str = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	tmp = (*env)->GetStringUTFChars(env, str, NULL);

	if (strnlen(tmp, length) < length) {
		strncpy(buffer, tmp, length);
	} else {
		buffer[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getPlatformIdentifierString() returns %s", buffer);

	(*env)->ReleaseStringUTFChars(env, str, tmp);
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, str);
}

void android_getAppVersionString(char *buffer, int length)
{
	const char *tmp;

	// get the proper jni env from SDL
	// JNIEnv *env = Android_JNI_GetEnv();
	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "getAppVersionString", "()Ljava/lang/String;");
	jstring str = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	tmp = (*env)->GetStringUTFChars(env, str, NULL);

	if (strnlen(tmp, length) < length) {
		strncpy(buffer, tmp, length);
	} else {
		buffer[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "android_getAppVersionString() returns %s", buffer);

	(*env)->ReleaseStringUTFChars(env, str, tmp);
	(*env)->DeleteLocalRef(env, cls);
	(*env)->DeleteLocalRef(env, str);
}

// already supported by SDL itself
void SetKeepDisplayOn(int keepOn)
{
  Android_JNI_SuspendScreenSaver((keepOn == 0) ? SDL_FALSE : SDL_TRUE);
}

static int lastLicenseCheckStartTime = 0;

void LicenseCheckStartAsync(int startTime)
{
	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "licenseCheckStart", "()V");

	(*env)->CallVoidMethod(env, callbackObject, callbackID);
	
	lastLicenseCheckStartTime = startTime;

	(*env)->DeleteLocalRef(env, cls);
}

int LicenseCheckStartTime()
{
	return lastLicenseCheckStartTime;
}

int LicenseCheckPollStatus()
{
	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "licenseCheckPoll", "()I");
	jint ret;
	
	ret = (*env)->CallIntMethod(env, callbackObject, callbackID);
	
	(*env)->DeleteLocalRef(env, cls);

	return (int) ret;
}

void LicenseCheckAbort()
{
	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "licenseCheckAbort", "()V");

	(*env)->CallVoidMethod(env, callbackObject, callbackID);
	
	lastLicenseCheckStartTime = 0;

	(*env)->DeleteLocalRef(env, cls);
}

int LicenseCheckResultStatus()
{
	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "licenseCheckResultStatus", "()I");
	jint ret;
	
	ret = (*env)->CallIntMethod(env, callbackObject, callbackID);
	
	(*env)->DeleteLocalRef(env, cls);

	return (int) ret;
}

void LicenseCheckResultDetail(char *str, int maxlen)
{
	const char *tmp;

	JNIEnv *env = Android_JNI_GetEnv();
	jclass cls = (*env)->GetObjectClass(env, callbackObject);
	jmethodID callbackID = (*env)->GetMethodID(env, cls, "licenseCheckResultDetail", "()Ljava/lang/String;");
	jstring retStr = (*env)->CallObjectMethod(env, callbackObject, callbackID);

	tmp = (*env)->GetStringUTFChars(env, retStr, NULL);

	if (strnlen(tmp, maxlen) < maxlen) {
		strncpy(str, tmp, maxlen);
	} else {
		str[0] = 0;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, "org.libsdl.app", "licenseCheckResultDetail() returns %s", str);

	(*env)->ReleaseStringUTFChars(env, retStr, tmp);
	(*env)->DeleteLocalRef(env, cls);
	
	// not a local reference (?)
	// (*env)->DeleteLocalRef(env, retStr);
}

static void *callbackObj;

void StoreKit_SetExternalData(void *data)
{
	callbackObj = data;
}

int StoreKit_IsStoreAvailable()
{
    // Google Play is always available
    return 1;
}

void StoreKit_EnableEvents()
{
    // dummy
}

void StoreKit_DisableEvents()
{
    // dummy
}

void StoreKit_ValidateProducts(const char* ids, TTextEncoding encoding)
{
  JNIEnv *env = Android_JNI_GetEnv();
  jclass cls = (*env)->GetObjectClass(env, callbackObject);
  jmethodID callbackID = (*env)->GetMethodID(env, cls, "iapVerifyProductsList", "(Ljava/lang/String;)V");

  jstring javaString = (*env)->NewStringUTF(env, ids);

  (*env)->CallVoidMethod(env, callbackObject, callbackID, javaString);

  (*env)->DeleteLocalRef(env, javaString);
  (*env)->DeleteLocalRef(env, cls);
}

void StoreKit_Purchase(const char* prodId, TTextEncoding encoding)
{
  JNIEnv *env = Android_JNI_GetEnv();
  jclass cls = (*env)->GetObjectClass(env, callbackObject);
  jmethodID callbackID = (*env)->GetMethodID(env, cls, "iapStartPurchase", "(Ljava/lang/String;)V");

  jstring javaString = (*env)->NewStringUTF(env, prodId);

  (*env)->CallVoidMethod(env, callbackObject, callbackID, javaString);

  (*env)->DeleteLocalRef(env, javaString);
  (*env)->DeleteLocalRef(env, cls);
}

void StoreKit_RestoreTransactions(TTextEncoding encoding)
{
  JNIEnv *env = Android_JNI_GetEnv();
  jclass cls = (*env)->GetObjectClass(env, callbackObject);
  jmethodID callbackID = (*env)->GetMethodID(env, cls, "iapRefreshPreviousPurchases", "(Z)V");
  jboolean restoreFlag = JNI_TRUE;

  (*env)->CallVoidMethod(env, callbackObject, callbackID, restoreFlag);

  lastLicenseCheckStartTime = 0;

  (*env)->DeleteLocalRef(env, cls);
}

int StoreKit_FinishTransaction(const char* transId, TTextEncoding encoding)
{
    // dummy
	return 1;
}

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_IAPAddValidProductCallback(JNIEnv* env, jobject o, jstring id, jstring name, jstring desc, jstring price)
{
	const char *tmp;

	char idCString[4096];
	char nameCString[4096];
	char descCString[4096];
	char priceCString[4096];

	tmp = (*env)->GetStringUTFChars(env, id, NULL);
	if (strnlen(tmp, 4096) < 4096) {
		strncpy(idCString, tmp, 4096);
	} else {
		idCString[0] = 0;
	}
	(*env)->ReleaseStringUTFChars(env, id, tmp);

	tmp = (*env)->GetStringUTFChars(env, name, NULL);
	if (strnlen(tmp, 4096) < 4096) {
		strncpy(nameCString, tmp, 4096);
	} else {
		nameCString[0] = 0;
	}
	(*env)->ReleaseStringUTFChars(env, name, tmp);

	tmp = (*env)->GetStringUTFChars(env, desc, NULL);
	if (strnlen(tmp, 4096) < 4096) {
		strncpy(descCString, tmp, 4096);
	} else {
		descCString[0] = 0;
	}
	(*env)->ReleaseStringUTFChars(env, desc, tmp);

	tmp = (*env)->GetStringUTFChars(env, price, NULL);
	if (strnlen(tmp, 4096) < 4096) {
		strncpy(priceCString, tmp, 4096);
	} else {
		priceCString[0] = 0;
	}
	(*env)->ReleaseStringUTFChars(env, price, tmp);


	StoreKit_AddValidProductCallback(idCString, nameCString, descCString, priceCString, callbackObj);
}

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_IAPAddInvalidProductCallback(JNIEnv* env, jobject o, jstring id)
{
	const char *tmp;

	char idCString[4096];
	tmp = (*env)->GetStringUTFChars(env, id, NULL);

	if (strnlen(tmp, 4096) < 4096) {
		strncpy(idCString, tmp, 4096);
	} else {
		idCString[0] = 0;
	}
	(*env)->ReleaseStringUTFChars(env, id, tmp);

	StoreKit_AddInvalidProductCallback(idCString, callbackObj);
}

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_IAPReceiveProductsStartCallback(JNIEnv* env, jobject o)
{
	StoreKit_ReceiveProductsStartCallback(callbackObj);
}

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_IAPReceiveProductsEndCallback(JNIEnv* env, jobject o)
{
	StoreKit_ReceiveProductsEndCallback(callbackObj);
}

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_IAPAddTransactionCallback(JNIEnv* env, jobject o, jstring transId, jstring productId, jstring state)
{
	const char *tmp;

	char transIdCString[4096];
	char productIdCString[4096];
	char stateCString[4096];

	tmp = (*env)->GetStringUTFChars(env, transId, NULL);
	if (strnlen(tmp, 4096) < 4096) {
		strncpy(transIdCString, tmp, 4096);
	} else {
		transIdCString[0] = 0;
	}
	(*env)->ReleaseStringUTFChars(env, transId, tmp);

	tmp = (*env)->GetStringUTFChars(env, productId, NULL);
	if (strnlen(tmp, 4096) < 4096) {
		strncpy(productIdCString, tmp, 4096);
	} else {
		productIdCString[0] = 0;
	}
	(*env)->ReleaseStringUTFChars(env, productId, tmp);

	tmp = (*env)->GetStringUTFChars(env, state, NULL);
	if (strnlen(tmp, 4096) < 4096) {
		strncpy(stateCString, tmp, 4096);
	} else {
		stateCString[0] = 0;
	}
	(*env)->ReleaseStringUTFChars(env, state, tmp);

	StoreKit_AddTransactionCallback(transIdCString, productIdCString, stateCString, callbackObj);
}

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_IAPReceiveTransactionsStartCallback(JNIEnv* env, jobject o)
{
	StoreKit_ReceiveTransactionsStartCallback(callbackObj);
}

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_IAPReceiveTransactionsEndCallback(JNIEnv* env, jobject o)
{
	StoreKit_ReceiveTransactionsEndCallback(callbackObj);
}

JNIEXPORT void JNICALL Java_org_deadcode_wmelite_WMELiteFunctions_IAPRestoreFinishedCallback(JNIEnv* env, jobject o, jint error)
{
	StoreKit_RestoreFinishedCallback(callbackObj, (int) error);
}
