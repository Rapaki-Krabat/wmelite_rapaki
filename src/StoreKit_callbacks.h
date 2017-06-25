#ifndef __StoreKit_callbacks_H__
#define __StoreKit_callbacks_H__


#ifdef __cplusplus
extern "C" {
#endif

    void StoreKit_AddValidProductCallback(const char* id, const char* name, const char* desc, const char* price, void* data);

    void StoreKit_AddInvalidProductCallback(const char* id, void* data);
    
    void StoreKit_ReceiveProductsStartCallback(void* data);
    
    void StoreKit_ReceiveProductsEndCallback(void* data);

    void StoreKit_AddTransactionCallback(const char* id, const char* productId, const char* state, void* data);
    
    void StoreKit_ReceiveTransactionsStartCallback(void* data);
    
    void StoreKit_ReceiveTransactionsEndCallback(void* data);

    void StoreKit_RestoreFinishedCallback(void* data, int error);
    
#ifdef __cplusplus
}
#endif


#endif // __StoreKit_callbacks_H__
