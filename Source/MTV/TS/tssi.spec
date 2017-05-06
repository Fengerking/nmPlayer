program_association_item() {
program_number 16 uimsbf
reserved 3 bslbf
program_map_PID 13 uimsbf
}

program_association_section() {
transport_stream_id 16 uimsbf
reserved 2 bslbf
version_number 5 uimsbf
current_next_indicator 1 bslbf
section_number 8 uimsbf
last_section_number 8 uimsbf
for (i = 0; i < N; i++) {
program_association_item()
}
CRC_32 32 rpchof
}


CA_section() {
reserved 18 bslbf
version_number 5 uimsbf
current_next_indicator 1 bslbf
section_number 8 uimsbf
last_section_number 8 uimsbf
for (i = 0; i < N; i++) {
descriptor()
}
CRC_32 32 rpchof
}


TS_program_map_item() {
stream_type 8 uimsbf
reserved 3 bslbf
elementary_PID 13 uimsbf
reserved 4 bslbf
ES_info_length 12 uimsbf
for (i = 0; i < N2; i++) {
descriptor() ES_info_length
}
}

TS_program_map_section() {
program_number 16 uimsbf
reserved 2 bslbf
version_number 5 uimsbf
current_next_indicator 1 bslbf
section_number 8 uimsbf
last_section_number 8 uimsbf
reserved 3 bslbf
PCR_PID 13 uimsbf
reserved 4 bslbf
program_info_length 12 uimsbf
for (i = 0; i < N; i++) {
descriptor() program_info_length
}
for (i = 0; i < N1; i++) {
TS_program_map_item()
}
CRC_32 32 rpchof
}


service_description_item(){
service_id 16 uimsbf
reserved_future_use 6 bslbf
EIT_schedule_flag 1 bslbf
EIT_present_following_flag 1 bslbf
running_status 3 uimsbf
free_CA_mode 1 bslbf
descriptors_loop_length 12 uimsbf
for (j=0;j<N;j++){
descriptor() descriptors_loop_length
}
}

service_description_section(){
transport_stream_id 16 uimsbf
reserved 2 bslbf
version_number 5 uimsbf
current_next_indicator 1 bslbf
section_number 8 uimsbf
last_section_number 8 uimsbf
original_network_id 16 uimsbf
reserved_future_use 8 bslbf
for (i=0;i<N;i++){
service_description_item()
}
CRC_32 32 rpchof
}



network_information_item(){
transport_stream_id 16 uimsbf
original_network_id 16 uimsbf
reserved_future_use 4 bslbf
transport_descriptors_length 12 uimsbf
for(j=0;j<N;j++){
descriptor() transport_descriptors_length
}
}

network_information_section(){
network_id 16 uimsbf
reserved 2 bslbf
version_number 5 uimsbf
current_next_indicator 1 bslbf
section_number 8 uimsbf
last_section_number 8 uimsbf
reserved_future_use 4 bslbf
network_descriptors_length 12 uimsbf
for(i=0;i<N;i++){
descriptor() network_descriptors_length
}
reserved_future_use 4 bslbf
transport_stream_loop_length 12 uimsbf
for(i=0;i<N;i++){
network_information_item() transport_stream_loop_length
}
CRC_32 32 rpchof
}


event_information_item(){
event_id 16 uimsbf
start_time 40 bslbf
duration 24 uimsbf
running_status 3 uimsbf
free_CA_mode 1 bslbf
descriptors_loop_length 12 uimsbf
for(i=0;i<N;i++){
descriptor() descriptors_loop_length
}
}


event_information_section(){
service_id 16 uimsbf
reserved 2 bslbf
version_number 5 uimsbf
current_next_indicator 1 bslbf
section_number 8 uimsbf
last_section_number 8 uimsbf
transport_stream_id 16 uimsbf
original_network_id 16 uimsbf
segment_last_section_number 8 uimsbf
last_table_id 8 uimsbf
for(i=0;i<N;i++){
event_information_item()
}
CRC_32 32 rpchof
}
