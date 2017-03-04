#ifndef WINRT_IN_APP_CALLBACKS_H
#define WINRT_IN_APP_CALLBACKS_H


#ifdef __cplusplus
extern "C" {
#endif

	void WinRTInAppPurchase_AddValidProductCallback(const char* id, const char* name, const char* desc, const char* price, void* data);

	void WinRTInAppPurchase_AddInvalidProductCallback(const char* id, void* data);

	void WinRTInAppPurchase_ReceiveProductsStartCallback(void* data);

	void WinRTInAppPurchase_ReceiveProductsEndCallback(void* data);

	void WinRTInAppPurchase_AddTransactionCallback(const char* id, const char* productId, const char* state, void* data);

	void WinRTInAppPurchase_ReceiveTransactionsStartCallback(void* data);

	void WinRTInAppPurchase_ReceiveTransactionsEndCallback(void* data);

	void WinRTInAppPurchase_RestoreFinishedCallback(void* data, int error);

#ifdef __cplusplus
}
#endif


#endif // WINRT_IN_APP_CALLBACKS_H
