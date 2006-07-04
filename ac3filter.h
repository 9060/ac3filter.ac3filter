/*
  AC3Filter class
*/


#ifndef AC3FILTER_H
#define AC3FILTER_H

#include <streams.h>
#include "win32\cpu.h"
#include "sink\sink_dshow.h"
#include "com_dec.h"

#include "rot.h"

class AC3Filter : public CTransformFilter, public IAC3Filter, public ISpecifyPropertyPages
{
protected:
  CPUMeter   cpu;       // CPU usage meter
  COMDecoder dec;       // decoder & processor
  DShowSink  *sink;     // sink
  ROTEntry   rot;       // registred objects table entry

  bool config_autoload; // auto-load configuration files
  int reinit_samples;   // force audio renderer to reinit sound card
                        // the number means number of PCM samples to send

  bool set_input(const CMediaType &mt);
  bool set_input(Speakers in_spk);

  Speakers guess_pcm_output();
  Speakers guess_spdif_output();

  bool process_chunk(const Chunk *chunk);
  void reset();


private:
  AC3Filter(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr);
  ~AC3Filter();

public:
  DECLARE_IUNKNOWN;
  STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
  static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);


  /////////////////////////////////////////////////////////
  // CTransformFilter

  HRESULT Receive(IMediaSample *in);

  HRESULT EndOfStream();
  HRESULT NewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);

  HRESULT StartStreaming();
  HRESULT StopStreaming();

  HRESULT BeginFlush();
  HRESULT EndFlush();

  STDMETHODIMP Stop();
  STDMETHODIMP Pause();
  STDMETHODIMP Run(REFERENCE_TIME tStart);
  STDMETHODIMP JoinFilterGraph(IFilterGraph *pGraph, LPCWSTR pName);

  HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
  HRESULT CheckInputType(const CMediaType *mt);
  HRESULT CheckOutputType(const CMediaType *mt);
  HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
  bool    CheckConnectPin(IPin *pin);
  HRESULT CheckConnect(PIN_DIRECTION dir, IPin *pin);
  HRESULT SetMediaType(PIN_DIRECTION direction, const CMediaType *mt);

  HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties);


  /////////////////////////////////////////////////////////
  // ISpecifyPropertyPages

  STDMETHODIMP GetPages(CAUUID *pPages);

  /////////////////////////////////////////////////////////
  // IAC3Filter

  // Timing
  STDMETHODIMP get_playback_time (vtime_t *time);

  // CPU usage
  STDMETHODIMP get_cpu_usage(double *cpu_usage);

  // Build and environment info
  STDMETHODIMP get_env(char *buf, int size);
};

#endif
