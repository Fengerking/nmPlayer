// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� FFMPEGWRAPPER_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// FFMPEGWRAPPER_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef FFMPEGWRAPPER_EXPORTS
#define FFMPEGWRAPPER_API __declspec(dllexport)
#else
#define FFMPEGWRAPPER_API __declspec(dllimport)
#endif

// �����Ǵ� FFMPEGWrapper.dll ������
class FFMPEGWRAPPER_API CFFMPEGWrapper {
public:
	CFFMPEGWrapper(void);
	// TODO: �ڴ�������ķ�����
};

extern FFMPEGWRAPPER_API int nFFMPEGWrapper;

FFMPEGWRAPPER_API int fnFFMPEGWrapper(void);
