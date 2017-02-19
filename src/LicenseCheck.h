#ifndef LICENSE_CHECK_H
#define LICENSE_CHECK_H

#ifdef __cplusplus
extern "C" {
#endif

	void LicenseCheckStartAsync();

	// 0 --> not pending, != 0 --> pending
	int LicenseCheckPollStatus();

	// 0 --> check passed, != 0 --> check failed
	int LicenseCheckResultStatus();

	void LicenseCheckResultDetail(char *str, int maxlen);

#ifdef __cplusplus
}
#endif


#endif // LICENSE_CHECK_H

