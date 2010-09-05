#ifndef __WAVE_CREATOR_ABSTRACT_H__
#define __WAVE_CREATOR_ABSTRACT_H__

class CWaveCreatorAbstract
{
public:
	virtual int GetTotalSeconds(int nSampleRate) = 0;
	virtual int GetSample(int nSecond, double * & pBuffer, int nSampleRate, int nBps) = 0;
};

#endif // __WAVE_CREATOR_ABSTRACT_H__