package com.visualon.OSMPUtils;

/**
 * General audio render format info
 */
public class voOSAudioRenderFormat {
	
	    int     SampleRate;  /*!< Sample rate */
	    int     Channels;    /*!< Channel count */
	    int     SampleBits;  /*!< Bits per sample */
	    int     BufferSize;  /*!< Audio render buffer size */
	    
	    
		public voOSAudioRenderFormat(int sampleRate, int channels,
				int sampleBits, int bufferSize) {
			super();
			SampleRate = sampleRate;
			Channels = channels;
			SampleBits = sampleBits;
			BufferSize = bufferSize;
		}
		
		public int getSampleRate() {
			return SampleRate;
		}
		public void setSampleRate(int sampleRate) {
			SampleRate = sampleRate;
		}
		public int getChannels() {
			return Channels;
		}
		public void setChannels(int channels) {
			Channels = channels;
		}
		public int getSampleBits() {
			return SampleBits;
		}
		public void setSampleBits(int sampleBits) {
			SampleBits = sampleBits;
		}
		public int getBufferSize() {
			return BufferSize;
		}
		public void setBufferSize(int bufferSize) {
			BufferSize = bufferSize;
		}


}
