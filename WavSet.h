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

////////////////////////////////////////////////////////////////
//nakedsoftware.org, spi@oifii.org or stephane.poirier@oifii.org
//
//
//2012june01, creation
//2012june04, added EraseSegment(), spi@oifii.org or stephane.poirier@oifii.org
//2012june04, added save segment, spi@oifii.org or stephane.poirier@oifii.org
//nakedsoftware.org, spi@oifii.org or stephane.poirier@oifii.org
////////////////////////////////////////////////////////////////
#define _WAVSET_H
class WavSet
{
public:
	int SampleRate;
	int totalFrames; 
	int numChannels;
	int numSamples;  
	int numBytes;
	float* pSamples;

	int numFramesPerSegment;
	int numSegments;
	int numSamplesPerSegment;  
	int numBytesPerSegment;

	WavSet();
	~WavSet();
	bool ReadWavFile(const char* filename);
	bool SplitInSegments(double fSecondsPerSegment);
	float* GetPointerToSegmentData(int idSegment);
	bool EraseSegment(int idSegment);
};