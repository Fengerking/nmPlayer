video_stream_descriptor(){
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
multiple_frame_rate_flag 1 bslbf
frame_rate_code 4 uimsbf
MPEG_1_only_flag 1 bslbf
constrained_parameter_flag 1 bslbf
still_picture_flag 1 bslbf
if (MPEG_1_only_flag == '0'){
profile_and_level_indication 8 uimsbf
chroma_format 2 uimsbf
frame_rate_extension_flag 1 bslbf
reserved 5 bslbf
}
}

audio_stream_descriptor(){
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
free_format_flag 1 bslbf
ID 1 bslbf
layer 2 bslbf
variable_rate_audio_indicator 1 bslbf
reserved 3 bslbf
}

hierarchy_descriptor() {
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
reserved 4 bslbf
hierarchy_type 4 uimsbf
reserved 2 bslbf
hierarchy_layer_index 6 uimsbf
reserved 2 bslbf
hierarchy_embedded_layer_index 6 uimsbf
reserved 2 bslbf
hierarchy_channel 6 uimsbf
}


data_stream_alignment_descriptor() {
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
alignment_type 8 uimsbf
}

target_background_grid_descriptor() {
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
horizontal_size 14 uimsbf
vertical_size 14 uimsbf
aspect_ratio_information 4 uimsbf
}

video_window_descriptor() {
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
horizontal_offset 14 uimsbf
vertical_offset 14 uimsbf
window_priority 4 uimsbf
}


ISO_639_language_item(){
ISO_639_language_code 24 bslbf
audio_type 8 bslbf
}

ISO_639_language_descriptor() {
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
for (i = 0; i < N; i++) {
ISO_639_language_item() length
}
}

system_clock_descriptor() {
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
external_clock_reference_indicator 1 bslbf
reserved 1 bslbf
clock_accuracy_integer 6 uimsbf
clock_accuracy_exponent 3 uimsbf
reserved 5 bslbf
}

multiplex_buffer_utilization_descriptor() {
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
bound_valid_flag 1 bslbf
LTW_offset_lower_bound 15 uimsbf
reserved 1 bslbf
LTW_offset_upper_bound 14 uimsbf
}


maximum_bitrate_descriptor() {
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
reserved 2 bslbf
maximum_bitrate 22 uimsbf
}

private_data_indicator_descriptor() {
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
private_data_indicator 32 uimsbf
}

smoothing_buffer_descriptor () {
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
reserved 2 bslbf
sb_leak_rate 22 uimsbf
reserved 2 bslbf
sb_size 22 uimsbf
}

STD_descriptor () {
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
reserved 7 bslbf
leak_valid_flag 1 bslbf
}

ibp_descriptor() {
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
closed_gop_flag 1 uimsbf
identical_gop_flag 1 uimsbf
max_gop-length 14 uimsbf
}

MPEG-4_video_descriptor ()
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
MPEG-4_visual_profile_and_level 8 uimsbf
}

MPEG-4_audio_descriptor ()
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
MPEG-4_audio_profile_and_level 8 uimsbf
}


SL_descriptor () {
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
ES_ID 16 uimsbf
}


External_ES_ID_descriptor () {
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
External_ES_ID 16 uimsbf
}

service_descriptor(){
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
service_type 8 uimsbf
service_provider_name_length 8 uimsbf
for (i=0;i<N;I++){
service_provider_name 8 uimsbf service_provider_name_length
}
service_name_length 8 uimsbf
for (i=0;i<N;I++){
service_name 8 uimsbf service_name_length
}
}

network_name_descriptor(){
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
for (i=0;i<N;i++){
network_name 8 uimsbf length
}
}


service_list_item(){
service_id 16 uimsbf
service_type 8 uimsbf
}

service_list_descriptor(){
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
for (i=0;i<N;I++){
service_list_item() length
}
}

satellite_delivery_system_descriptor(){
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
frequency 32 bslbf
orbital_position 16 bslbf
west_east_flag 1 bslbf
polarization 2 bslbf
if (modulation_system == "1") {
roll off 2 bslbf
}
else {
"00" 2 bslbf
}
modulation_system 1 bslbf
modulation_type 2 bslbf
symbol_rate 28 bslbf
FEC_inner 4 bslbf
}


stream_identifier_descriptor(){
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
component_tag 8 uimsbf
}

multilingual_bouquet_name_item(){
ISO_639_language_code 24 bslbf
bouquet_name_length 8 uimsbf
for (j=0;j<N;j++){
bouquet_name 8 uimsbf bouquet_name_length
}
}

multilingual_bouquet_name_descriptor(){
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
for (i=0;i<N;i++) {
multilingual_bouquet_name_item() length
}
}


short_event_descriptor(){
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
ISO_639_language_code 24 bslbf
event_name_length 8 uimsbf
for (i=0;i<event_name_length;i++){
event_name 8 uimsbf event_name_length
}
text_length 8 uimsbf
for (i=0;i<text_length;i++){
text 8 uimsbf text_length
}
}


component_descriptor(){
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
reserved_future_use 4 bslbf
stream_content 4 uimsbf
component_type 8 uimsbf
component_tag 8 uimsbf
ISO_639_language_code 24 bslbf
for (i=0;i<N;i++){
text 8 uimsbf length-6
}
}


content_item(){
content_nibble_level_1 4 uimsbf
content_nibble_level_2 4 uimsbf
user_nibble_1 4 uimsbf
user_nibble_2 4 uimsbf
}

content_descriptor(){
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
for (i=0;i<N;i++) {
content_item() length
}
}


parental_rating_item(){
country_code 24 bslbf
rating 8 uimsbf
}


parental_rating_descriptor(){
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
for (i=0;i<N;i++){
parental_rating_item() length
}
}

private_data_specifier_descriptor(){
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
private_data_specifier 32 uimsbf
}

teletext_descriptor_item(){
ISO_639_language_code 24 bslbf
teletext_type 5 uimsbf
teletext_magazine_number 3 uimsbf
teletext_page_number 8 uimsbf
}


teletext_descriptor(){
descriptor_tag 8 uimsbf
descriptor_length 8 uimsbf
for (i=0;i<N;i++){
teletext_descriptor_item() length
}
}

