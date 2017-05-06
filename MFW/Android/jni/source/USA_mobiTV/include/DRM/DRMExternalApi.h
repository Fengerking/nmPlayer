#ifndef DRMEXTERNALAPI_H
#define DRMEXTERNALAPI_H

#include <MKBase/Type.h>
#include <MKBase/List.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/*
 * Define MK_DrmKey structure.
 */
typedef struct _MK_DrmKey MK_DrmKey;

struct _MK_DrmKey
{
    MK_U32 KeyId;
    MK_U16 KeyLen;
    MK_U8* Key;
};

/*
 * Defines for default DrmKey ListNode initialization. May be used instead of
 * init function when appropriate.
 */
#define MK_LISTNODE_DRMKEY_INITVAL {0, 0, NULL}
#define MK_LISTNODE_DRMKEY_INIT {MK_LISTNODE_INITVAL, MK_LISTNODE_DRMKEY_INITVAL}

/*
 * Define DrmKey list type basics.
 */
MK_NEW_LIST(DrmKey, MK_DrmKey, DrmKey, MK_LISTNODE_DRMKEY_INIT);
/******************************************************************************/

typedef struct MK_MobiDrmIDs
{
    MK_U8  mUserIDLen;
    MK_U8* mUserID;
    MK_U8  mDeviceIDLen;
    MK_U8* mDeviceID;
    MK_U8  mXvalueIDLen;
    MK_U8* mXvalueID;
} sMK_MobiDrmIDs;

typedef struct MK_MobiDrmMbdcHeaderStruct
{
    MK_U32 mBoxLen;            // length of the whole header
    MK_U32 mBoxType;           // type, is always "mbdc"
    MK_U8  mMajorVersion;
    MK_U8  mMinorVersion;
    MK_U64 mInventoryID;       // also called "AssetID"
    MK_U32 mContentExpiry;     // DRM v1.0, number of seconds from 1/1/1970
    MK_U32 mBroadcastID;       // DRM v1.1
    MK_U16 mEncryptAlgorithm;
    MK_U32 mEncryptOffset;     // DRM v1.0, original offset of the moov box
    MK_U32 mEncryptSize;       // DRM v1.0, original length of the un-encrypted moov box
} sMK_MobiDrmMbdcHeader;

typedef struct MK_MobiDrmLicenseStruct
{
    MK_U32 mBoxLen;            // length of the whole license
    MK_U32 mBoxType;           // type, is always "mbdl"
    MK_U8  mMajorVersion;
    MK_U8  mMinorVersion;
    MK_U32 mLicenseID;
    MK_U64 mInventoryID;       // also called "AssetID"
    MK_U32 mBroadcastID;       // DRM v1.1
    MK_U32 mPackageID;         // DRM v1.1
    MK_U32 mKeyID;             // DRM v1.1
    MK_U32 mLicenseExpiry;     // number of seconds from 1/1/1970
    MK_U32 mLicenseStart;      // number of seconds from 1/1/1970
    sMK_MobiDrmIDs mIDs;
    MK_U32 mContentExpiry;     // DRM v1.1, number of seconds from 1/1/1970
    MK_U32 mMaxPlaybackCount;  // DRM v1.1
    MK_U8  mAutoRenew;         // DRM v1.1
    MK_U8  mDeleteOnExpiry;    // DRM v1.1
    MK_U8  mLicenseType;       // DRM v1.1
    MK_U32 mLicenseKeyCount;   // DRM v1.1
} sMK_MobiDrmLicense;


/**
 * Check if the content file starts with a MobiDRM Header.
 *
 * @param[in]  aBuf   Pointer to buffer with at least the first 8 bytes of the content file
 * @param[in]  aSize  Length of the buffer, must be >= 8 bytes
 * @return  TRUE if the file starts with a MobiDRM Header, otherwise FALSE
 */
MK_Bool MK_MobiDrmContentIsEncrypted(const MK_U8* aBuf, MK_U32 aSize);


/**
 * Parse a MobiDRM Header and perform basic verification.
 *
 * Will verify the values of BoxLength, BoxType and EncryptionAlgorithm.
 * Will also calculate the hash value and compare with the checksum value in the Header.
 *
 * @param[in]   aBuf     Pointer to buffer with the Header
 * @param[in]   aSize    Length of the buffer
 * @param[out]  aHeader  Pointer to struct with the parameters of the Header
 * @return  MK_S_OK on success, otherwise an error code
 */
MK_Result MK_MobiDrmGetContentInfo(const MK_U8* aBuf, MK_U32 aSize, sMK_MobiDrmMbdcHeader* aHeader);


/**
 * Parse a MobiDRM License and perform basic verification.
 *
 * NOTE: The returned list of keys must be freed by the caller by calling MK_MobiDrmKeysDelete().
 * NOTE: The following 3 returned buffers must also be freed by the caller:
 *       - aLicense->mIDs.mUserID
 *       - aLicense->mIDs.mDeviceID
 *       - aLicense->mIDs.mXvalueID
 *
 * Will verify the values of BoxType, Major- & MinorVersion and (if aTime != 0) LicenseStart & -Expiry
 * and (if aIDs != NULL) also verify UID/DID/XID.
 * Will use the Private Key to decrypt the encrypted part of the License to retrieve the AES256 encryption keys
 * and checksum value. Will also calculate the hash value and compare with the checksum value.
 *
 * @param[in]   aBuf         Pointer to buffer with the License
 * @param[in]   aSize        Length of the buffer with the License
 * @param[in]   aPrivKey     Pointer to buffer with the Private Key (serialized with PKCS#8)
 * @param[in]   aPrivKeyLen  Length of the buffer with the Private Key
 * @param[in]   aTime        Current time (number of seconds from 1/1/1970), 0 => LicenseStart & -Expiry not verified
 * @param[in]   aIDs         Pointer to struct with current values of UID/DID/XID, NULL => UID/DID/XID not verified
 * @param[out]  aLicense     Pointer to struct with the parameters of the License
 * @param[out]  aKeys        Pointer to a list that will be filled the AES256 encryption keys
 * @return  MK_S_OK on success, otherwise an error code
 */
MK_Result MK_MobiDrmGetLicenseInfo(const MK_U8* aBuf, MK_U32 aSize, const MK_U8* aPrivKey, MK_U32 aPrivKeyLen, MK_U32 aTime,
                                   sMK_MobiDrmIDs* aIDs, sMK_MobiDrmLicense* aLicense, MK_List* aKeys);


/**
 * Clean up and delete any nodes in the list of keys
 *
 * @param[in]  aList  Pointer to a list with AES256 encryption keys
 * @return  None
 */
void MK_MobiDrmKeysDelete(MK_List* aList);


/**
 * Retrieve the key with specified ID
 *
 * NOTE: The buffer that aKey will point to must NOT be freed by the caller.
 *
 * @param[in]   aList    Pointer to a list with AES256 encryption keys
 * @param[in]   aKeyId   ID of the requested key
 * @param[out]  aKey     Will point at the buffer that contains the requested AES256 encryption key
 * @param[out]  aKeyLen  Will be filled with the length of the buffer that contains the AES256 encryption key
 * @return  MK_S_OK on success, otherwise an error code
 */
MK_Result MK_MobiDrmKeysFind(MK_List* aList, MK_U32 aKeyId, MK_U8** aKey, MK_U16* aKeyLen);


#ifdef __cplusplus
}
#endif

#endif // DRMEXTERNALAPI_H
