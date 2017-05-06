#ifndef _PARSESPS_H_
#define _PARSESPS_H_

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#else
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif
enum{
	// AVC
	VO_AVCLevel1 		= 10,		/**< Level 1 */
	VO_AVCLevel11		= 11,				/**< Level 1.1 */
	VO_AVCLevel12		= 12,				/**< Level 1.2 */
	VO_AVCLevel13		= 13,				/**< Level 1.3 */
	VO_AVCLevel2		= 20,					/**< Level 2 */
	VO_AVCLevel21		= 21,				/**< Level 2.1 */
	VO_AVCLevel22		= 22,				/**< Level 2.2 */
	VO_AVCLevel3		= 30,					/**< Level 3 */
	VO_AVCLevel31		= 31,				/**< Level 3.1 */
	VO_AVCLevel32		= 32,				/**< Level 3.2 */
	VO_AVCLevel4		= 40,					/**< Level 4 */
	VO_AVCLevel41		= 41,				/**< Level 4.1 */
	VO_AVCLevel42		= 42,				/**< Level 4.2 */
	VO_AVCLevel5		= 50,					/**< Level 5 */
	VO_AVCLevel51		= 51,				/**< Level 5.1 */
};

enum{
	VO_AVCProfileBaseline 		= 66,	/**< Baseline profile */
	VO_AVCProfileMain					= 77,					/**< Main profile */
	VO_AVCProfileExtended			= 88,				/**< Extended profile */
	VO_AVCProfileHigh					= 100,					/**< High profile */
	VO_AVCProfileHigh10				=	110,					/**< High 10 profile */
	VO_AVCProfileHigh422			= 122,					/**< High 4:2:2 profile */
	VO_AVCProfileHigh444			= 144,					/**< High 4:4:4 profile */
};
int GetSizeInfo(unsigned char* buf,int size,int* width,int* height);
int GetH264Profile(unsigned char* buf, int size);
int GetH264Levle(unsigned char* buf, int size);
int GetFrameType(char * buffer, int size);

#ifdef _VONAMESPACE
}
#else
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif

#endif//_PARSESPS_H_