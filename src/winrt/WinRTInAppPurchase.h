#ifndef WINRT_IN_APP_PURCHASE_H
#define WINRT_IN_APP_PURCHASE_H

#ifdef __cplusplus
extern "C" {
#endif

	void wrtiap_SetCallbackObject(void *obj);

	void wrtiap_SetEventsEnabledDummy(bool enabled);

	void wrtiap_VerifyInAppProductsList(const char* prodIdList);

	void wrtiap_StartPurchase(const char* productId);

	int wrtiap_ConfirmPurchaseDummy(const char* transId);

	void wrtiap_RefreshPreviousPurchases();

#ifdef __cplusplus
}
#endif


#endif // WINRT_IN_APP_PURCHASE_H

