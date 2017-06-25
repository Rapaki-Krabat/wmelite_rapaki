#ifndef __StoreKit_functions_H__
#define __StoreKit_functions_H__


#ifdef __cplusplus
extern "C" {
#endif

    void StoreKit_SetExternalData(void* data);
    
    int StoreKit_IsStoreAvailable();

    void StoreKit_EnableEvents();
    
    void StoreKit_DisableEvents();
    
    void StoreKit_ValidateProducts(const char* ids);
    
    void StoreKit_Purchase(const char* prodId);
    
    void StoreKit_RestoreTransactions();
    
    int StoreKit_FinishTransaction(const char* transId);
    
#ifdef __cplusplus
}
#endif


#endif // __StoreKit_functions_H__
