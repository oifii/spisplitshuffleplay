/*
 * Copyright (c) 2010-2016 Stephane Poirier
 *
 * stephane.poirier@oifii.org
 *
 * Stephane Poirier
 * 3532 rue Ste-Famille, #3
 * Montreal, QC, H2X 2L1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include "portaudio.h"

#include "WavFile.h"
#include "SoundTouch.h"
using namespace soundtouch;

#include "WavSet.h"

WavSet::WavSet()
{
	SampleRate = 0;
	totalFrames = 0; 
	numChannels = 0;
	numSamples = 0;  
	numBytes = 0;
	pSamples = NULL;

	numFramesPerSegment = -1;
	numSegments = -1;
	numSamplesPerSegment = -1;  
	numBytesPerSegment = -1;
}

WavSet::~WavSet()
{
	if(pSamples) free(pSamples);
}

bool WavSet::ReadWavFile(const char* filename)
{
	/////////////////////
	// open input file...
    //WavInFile* pWavInFile = new WavInFile("testbeat2.wav");
    WavInFile* pWavInFile = new WavInFile(filename);
	if(pWavInFile)
	{
		printf("Begin reading file.\n"); fflush(stdout);
		SampleRate = pWavInFile->getSampleRate();
		totalFrames = (pWavInFile->getLengthMS()/1000.0) * SampleRate; 
		numChannels = pWavInFile->getNumChannels();
		numSamples = totalFrames * numChannels;  
		numBytes = numSamples * sizeof(float);
		pSamples = (float*) malloc( numBytes );
		if( pSamples == NULL )
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
			int num = pWavInFile->read(pSamples, numSamples);
		}

		delete pWavInFile;
		printf("Done!\n"); fflush(stdout);
	}
	return true;
}

bool WavSet::SplitInSegments(double fSecondsPerSegment)
{
	numFramesPerSegment = fSecondsPerSegment * SampleRate;
	if(numFramesPerSegment>totalFrames || fSecondsPerSegment==0.0) 
	{
		//return false;
		//assert(false);
		numFramesPerSegment=totalFrames;
	}
	numSegments = totalFrames/numFramesPerSegment;
	numSamplesPerSegment = numFramesPerSegment * numChannels;  
	numBytesPerSegment = numSamplesPerSegment * sizeof(float);
	return true;
}

float* WavSet::GetPointerToSegmentData(int idSegment)
{
	if(pSamples==NULL || idSegment>(numSegments-1)) return NULL; //error
	return pSamples+idSegment*numSamplesPerSegment;
}
