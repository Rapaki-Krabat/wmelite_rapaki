/*
 * androidCallbacks.h
 *
 *  Created on: 04.03.2017
 *      Author: daniel
 */

#ifndef ANDROIDCALLBACKS_H_
#define ANDROIDCALLBACKS_H_


#ifdef __cplusplus
extern "C" {
#endif

	void android_AddValidProductCallback(const char* id, const char* name, const char* desc, const char* price, void* data);

	void android_AddInvalidProductCallback(const char* id, void* data);

	void android_ReceiveProductsStartCallback(void* data);

	void android_ReceiveProductsEndCallback(void* data);

	void android_AddTransactionCallback(const char* id, const char* productId, const char* state, void* data);

	void android_ReceiveTransactionsStartCallback(void* data);

	void android_ReceiveTransactionsEndCallback(void* data);

	void android_RestoreFinishedCallback(void* data, int error);

#ifdef __cplusplus
}
#endif



#endif /* ANDROIDCALLBACKS_H_ */
