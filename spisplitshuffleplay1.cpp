/** @file spisplitshuffleplay.cpp
	stephane.poirier@oifii.org or spi@oifii.org
	@ingroup test_src
	@brief Record input into an array; Save array to a file; Playback recorded
    data. Implemented using the blocking API (Pa_ReadStream(), Pa_WriteStream() )
	@author Phil Burk  http://www.softsynth.com
    @author Ross Bencina rossb@audiomulch.com
*/
/*
 * $Id: spisplitshuffleplay.cpp 1368 2008-03-01 00:38:27Z rossb $
 *
 * This program uses the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however, 
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also 
 * requested that these non-binding requests be included along with the 
 * license above.
 */

#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"

//2012mar17, spi, begin
#include "WavFile.h"
#include "SoundTouch.h"
using namespace soundtouch;
#define BUFF_SIZE	2048
//2012mar17, spi, end

#include <ctime>
#include <iostream>

/* Select sample format. */
#if 1
#define PA_SAMPLE_TYPE  paFloat32
typedef float SAMPLE;
#define SAMPLE_SILENCE  (0.0f)
#define PRINTF_S_FORMAT "%.8f"
#elif 1
#define PA_SAMPLE_TYPE  paInt16
typedef short SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#elif 0
#define PA_SAMPLE_TYPE  paInt8
typedef char SAMPLE;
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
#else
#define PA_SAMPLE_TYPE  paUInt8
typedef unsigned char SAMPLE;
#define SAMPLE_SILENCE  (128)
#define PRINTF_S_FORMAT "%d"
#endif

/*******************************************************************/
int main(int argc, char *argv[]);
int main(int argc, char *argv[])
{
    PaStreamParameters outputParameters;
    PaStream *stream;
    PaError err;

	int SampleRate = 0;
	int totalFrames = 0; 
	int numChannels = 0;
	int numSamples = 0;  
	int numBytes = 0;
	SAMPLE* pLoadedSamples = NULL;
	SAMPLE* pShuffledSamples = NULL;

	///////////////////
	//read in arguments
	///////////////////
	char charBuffer[2048] = {"testbeat2.wav"};
	double fSecondsPerSegment = 0.5;
	int numShuffle = 1;
	if(argc>1)
	{
		//first argument is the filename
		sprintf_s(charBuffer,2048-1,argv[1]);
	}
	if(argc>2)
	{
		//second argument is the segment lenght in seconds
		fSecondsPerSegment = atof(argv[2]);
	}
	if(argc>3)
	{
		//second argument is the segment lenght in seconds
		numShuffle = atoi(argv[3]);
	}


	//////////////////////////////////
	//read a WAV file using soundtouch 
	//////////////////////////////////

	/////////////////////
	// open input file...
    //WavInFile* pWavInFile = new WavInFile("testbeat2.wav");
    WavInFile* pWavInFile = new WavInFile(charBuffer);
	if(pWavInFile)
	{
		SampleRate = pWavInFile->getSampleRate();
		totalFrames = (pWavInFile->getLengthMS()/1000.0) * SampleRate; 
		numChannels = pWavInFile->getNumChannels();
		numSamples = totalFrames * numChannels;  
		numBytes = numSamples * sizeof(SAMPLE);
		pLoadedSamples = (SAMPLE*) malloc( numBytes );
		if( pLoadedSamples == NULL )
		{
			printf("Could not allocate load buffer.\n");
			exit(1);
		}
		//for(int i=0; i<numSamples; i++ ) pLoadedSamples[i] = 0;

		///////////////////////////////////////
		// read samples from the input file ...
		while (pWavInFile->eof() == 0)
		{
			// Read a chunk of samples from the input file
			int num = pWavInFile->read(pLoadedSamples, numSamples);
		}

		delete pWavInFile;
		printf("Done!\n"); fflush(stdout);
	}

	
	////////////////////////////////////
	// play loaded data using port audio 
	////////////////////////////////////
    err = Pa_Initialize();
    if( err != paNoError ) goto error;

	outputParameters.device = Pa_GetDefaultOutputDevice(); // default output device 
	if (outputParameters.device == paNoDevice) 
	{
		fprintf(stderr,"Error: No default output device.\n");
		goto error;
	}
	outputParameters.channelCount = numChannels;
	outputParameters.sampleFormat =  PA_SAMPLE_TYPE;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	printf("Begin playback.\n"); fflush(stdout);
	err = Pa_OpenStream(
				&stream,
				NULL, // no input
				&outputParameters,
				SampleRate,
				BUFF_SIZE/numChannels, //FRAMES_PER_BUFFER,
				paClipOff,      // we won't output out of range samples so don't bother clipping them 
				NULL, // no callback, use blocking API 
				NULL ); // no callback, so no callback userData 
	if( err != paNoError ) goto error;

	if( stream )
	{
		err = Pa_StartStream( stream );
		if( err != paNoError ) goto error;
		printf("Waiting for playback to finish.\n"); fflush(stdout);

		err = Pa_WriteStream( stream, pLoadedSamples, totalFrames );
		if( err != paNoError ) goto error;

		/*
		err = Pa_CloseStream( stream );
		if( err != paNoError ) goto error;
		printf("Done.\n"); fflush(stdout);
		*/
	}
	
	//////////////////////
	// shuffle loaded data 
	//////////////////////
	//int numFramesPerSegment = 0.5 * SampleRate; //0.5 second per segment
	int numFramesPerSegment = fSecondsPerSegment * SampleRate; //0.5 second per segment
	int numSegments = totalFrames/numFramesPerSegment;
	int numSamplesPerSegment = numFramesPerSegment * numChannels;  
	int numBytesPerSegment = numSamplesPerSegment * sizeof(SAMPLE);
	pShuffledSamples = (SAMPLE*) malloc( numBytes );
	if( pShuffledSamples == NULL )
	{
		printf("Could not allocate shuffle buffer.\n");
		exit(1);
	}
	//for(int i=0; i<numSamples; i++ ) pShuffledSamples[i] = 0;

	//generate random number
	srand((unsigned)time(0));
	int random_integer;
	int lowest=1, highest=numSegments;
	int range=(highest-lowest)+1;
	for(int ii=0; ii<numShuffle; ii++)
	{
		for(int indexSegment=0; indexSegment<numSegments; indexSegment++)
		{
			/*
			//copy audio segments in the exact initial order
			memcpy (pShuffledSamples+indexSegment*numSamplesPerSegment, pLoadedSamples+indexSegment*numSamplesPerSegment, numBytesPerSegment);
			*/
			random_integer = lowest+int(range*rand()/(RAND_MAX + 1.0));
			//printf("%d,%d\n",indexSegment, random_integer-1); 
			//copy audio segments in the exact initial order
			memcpy (pShuffledSamples+indexSegment*numSamplesPerSegment, pLoadedSamples+(random_integer-1)*numSamplesPerSegment, numBytesPerSegment);
		}
		//play shuffled audio segments
		if(stream)
		{
			err = Pa_WriteStream( stream, pShuffledSamples, totalFrames );
			if( err != paNoError ) goto error;
		}
	}

	if( stream )
	{
		err = Pa_CloseStream( stream );
		if( err != paNoError ) goto error;
		printf("Done.\n"); fflush(stdout);
	}
	if(pLoadedSamples) free(pLoadedSamples);
	if(pShuffledSamples) free(pShuffledSamples);
	Pa_Terminate();
	printf("Exiting!\n"); fflush(stdout);
	return 0;
	


error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return -1;

}

