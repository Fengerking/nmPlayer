#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	typedef int (*ActivateCallback)(unsigned char * p_playlist, unsigned long totalsize_playlist, unsigned char * p_content, unsigned long totalsize_content, int type, int * p_drm, void *p_custom_data);
	typedef int (*DecryptCallback)(unsigned char *p_input_buffer, unsigned long input_buffer_size, unsigned char *p_output_buffer, unsigned long *p_output_buffer_size, unsigned char *p_sample_enc_box, unsigned long sample_enc_box_size, unsigned long sample_idx, unsigned long track_id, void *p_custom_date);

	int RegistCallback(ActivateCallback p_activate, DecryptCallback p_callback, void * p_custom_data);

#ifdef __cplusplus
};
#endif